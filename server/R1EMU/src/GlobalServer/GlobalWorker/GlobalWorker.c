/**
 *
 *   ██████╗   ██╗ ███████╗ ███╗   ███╗ ██╗   ██╗
 *   ██╔══██╗ ███║ ██╔════╝ ████╗ ████║ ██║   ██║
 *   ██████╔╝ ╚██║ █████╗   ██╔████╔██║ ██║   ██║
 *   ██╔══██╗  ██║ ██╔══╝   ██║╚██╔╝██║ ██║   ██║
 *   ██║  ██║  ██║ ███████╗ ██║ ╚═╝ ██║ ╚██████╔╝
 *   ╚═╝  ╚═╝  ╚═╝ ╚══════╝ ╚═╝     ╚═╝  ╚═════╝
 *
 * @file GlobalWorker.h
 * @brief
 *
 *
 *
 * @license <license placeholder>
 */

// ---------- Includes ------------
#include "GlobalWorker.h"
#include "GlobalServer/GlobalServer.h"


// ------ Structure declaration -------


// ------ Static declaration -------

/**
 * @brief Handle a PING request from any entity.
 * @return a zframe_t containing the PONG. Never returns NULL.
 */
static zframe_t *
GlobalWorker_handlePingPacket (
    void
);


// ------ Extern function implementation -------

GlobalWorker *
GlobalWorker_new (
    int workerId
) {
    GlobalWorker *self;

    if ((self = calloc (1, sizeof (GlobalWorker))) == NULL) {
        return NULL;
    }

    if (!GlobalWorker_init (self, workerId)) {
        GlobalWorker_destroy (&self);
        error ("GlobalWorker failed to initialize.");
        return NULL;
    }

    return self;
}


bool
GlobalWorker_init (
    GlobalWorker *self,
    int workerId
) {
    self->workerId = workerId;

    return true;
}


static zframe_t *
GlobalWorker_handlePingPacket (
    void
) {
    return zframe_new (PACKET_HEADER (GLOBAL_SERVER_PONG), sizeof (GLOBAL_SERVER_PONG));
}


void
GlobalWorker_handleRequest (
    GlobalWorker *self,
    zmsg_t *msg
) {
    // Extract the request
    zframe_t *identityFrame = zmsg_pop (msg); (void) identityFrame;
    zframe_t *emptyFrame = zmsg_pop (msg); (void) emptyFrame;
    zframe_t *headerFrame = zmsg_pop (msg);
    zframe_t *requestAnswer = NULL;

    GlobalServerRecvHeader header = *((GlobalServerRecvHeader *) zframe_data (headerFrame));

    // Handle the request
    switch (header) {
        case GLOBAL_SERVER_PING:
            requestAnswer = GlobalWorker_handlePingPacket ();
        break;

        default:
            error ("Packet type %d not handled.", header);
        break;
    }

    // Rebuild the answer message
    zmsg_add (msg, identityFrame);
    zmsg_add (msg, emptyFrame);
    zmsg_add (msg, requestAnswer);

    // Clean up
    zframe_destroy (&headerFrame);
}


void *
GlobalWorker_worker (
    void *arg
) {
    zframe_t *readyFrame;
    zmsg_t *msg;

    GlobalWorker * self = (GlobalWorker *) arg;

    // Create and connect a socket to the backend
    if (!(self->worker = zsock_new (ZMQ_REQ))
    ||  zsock_connect (self->worker, GLOBAL_SERVER_BACKEND_ENDPOINT) == -1
    ) {
        error ("Global worker ID = %d cannot connect to the backend socket.", self->workerId);
        return NULL;
    }

    // Tell to the broker we're ready for work
    if (!(readyFrame = zframe_new (PACKET_HEADER (GLOBAL_SERVER_WORKER_READY), sizeof (GLOBAL_SERVER_WORKER_READY)))
    ||  zframe_send (&readyFrame, self->worker, 0) == -1
    ) {
        error ("Global worker ID = %d cannot send a correct GLOBAL_WORKER_READY state.", self->workerId);
        return NULL;
    }

    dbg ("Global worker ID %d is running and waiting for messages.", self->workerId);

    while (true) {

        // Process messages as they arrive
        if (!(msg = zmsg_recv (self->worker))) {
            dbg ("Global worker ID = %d stops working.", self->workerId);
            break; // Interrupted
        }

        // No message should be with less than 3 frames
        if (zmsg_size (msg) < 3) {
            error ("A malformed message has been received.");
            zmsg_destroy (&msg);
            continue;
        }

        // Handle the request
        GlobalWorker_handleRequest (self, msg);

        // Reply back to the sender
        if (zmsg_send (&msg, self->worker) != 0) {
            warning ("Global worker ID = %d failed to send a message to the backend.", self->workerId);
        }
    }

    // Cleanup
    zsock_destroy (&self->worker);

    dbg ("Global worker ID = %d exits.", self->workerId);
    return NULL;
}

void
GlobalWorker_destroy (
    GlobalWorker **_self
) {
    GlobalWorker *self = *_self;

    free (self);
    *_self = NULL;
}
