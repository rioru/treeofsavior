/**
 *
 *   ██████╗   ██╗ ███████╗ ███╗   ███╗ ██╗   ██╗
 *   ██╔══██╗ ███║ ██╔════╝ ████╗ ████║ ██║   ██║
 *   ██████╔╝ ╚██║ █████╗   ██╔████╔██║ ██║   ██║
 *   ██╔══██╗  ██║ ██╔══╝   ██║╚██╔╝██║ ██║   ██║
 *   ██║  ██║  ██║ ███████╗ ██║ ╚═╝ ██║ ╚██████╔╝
 *   ╚═╝  ╚═╝  ╚═╝ ╚══════╝ ╚═╝     ╚═╝  ╚═════╝
 *
 * @license <license placeholder>
 */


// ---------- Includes ------------
#include "EventHandler.h"
#include "ZoneServer/ZoneHandler/ZoneBuilder.h"

// ------ Structure declaration -------


// ------ Static declaration -------


// ------ Extern function implementation -------

bool
EventHandler_enterPc (
    EventServer *self,
    GameEventPcEnter *event
) {
    bool status = true;
    zlist_t *clientsAround = NULL;

    // Update client position
    if (!(EventServer_updateClientPosition (
        self, event->mapId, event->sessionKey,
        &event->commanderInfo, &event->commanderInfo.pos, &clientsAround))
    ) {
        error ("Cannot update player %s position.", event->sessionKey);
        status = false;
        goto cleanup;
    }

cleanup:
    return status;
}


bool
EventHandler_commanderMove (
    EventServer *self,
    GameEventCommanderMove *event
) {
    bool status = true;
    zlist_t *clientsAround = NULL;
    zmsg_t *msg = NULL;

    // Update client position and get the clients around
    if (!(EventServer_updateClientPosition (
        self, event->mapId, event->sessionKey,
        &event->commanderInfo, &event->position, &clientsAround))
    ) {
        error ("Cannot update player %s position.", event->sessionKey);
        status = false;
        goto cleanup;
    }

    if (zlist_size (clientsAround) > 0)
    {
        // Build the packet for the clients around
        msg = zmsg_new ();
        ZoneBuilder_moveDir (
            event->commanderInfo.pcId,
            &event->position,
            &event->direction,
            event->timestamp,
            msg
        );

        // Send the packet
        zframe_t *frame = zmsg_first (msg);
        if (!(EventServer_sendToClients (self, clientsAround, zframe_data (frame), zframe_size (frame)))) {
            error ("Failed to send the packet to the clients.");
            status = false;
            goto cleanup;
        }
    }

cleanup:
    zlist_destroy (&clientsAround);
    zmsg_destroy (&msg);
    return status;
}

bool
EventHandler_moveStop (
    EventServer *self,
    GameEventMoveStop *event
) {
    bool status = true;
    zmsg_t *msg = NULL;
    zlist_t *clientsAround = NULL;

    // Update client position and get the clients around
    if (!(EventServer_updateClientPosition (
        self, event->mapId, event->sessionKey,
        &event->commanderInfo, &event->position, &clientsAround))
    ) {
        error ("Cannot update player %s position.", event->sessionKey);
        status = false;
        goto cleanup;
    }

    if (zlist_size (clientsAround) > 0)
    {
        // Build the packet for the clients around
        msg = zmsg_new ();
        ZoneBuilder_pcMoveStop (
            event->commanderInfo.pcId,
            &event->position,
            &event->direction,
            event->timestamp,
            msg
        );

        // Send the packet
        zframe_t *frame = zmsg_first (msg);
        if (!(EventServer_sendToClients (self, clientsAround, zframe_data (frame), zframe_size (frame)))) {
            error ("Failed to send the packet to the clients.");
            status = false;
            goto cleanup;
        }
    }

cleanup:
    zmsg_destroy (&msg);
    return status;
}


bool
EventHandler_jump (
    EventServer *self,
    GameEventJump *event
) {
    bool status = true;
    zmsg_t *msg = NULL;
    zlist_t *clientsAround = NULL;

    // Update client position and get the clients around
    if (!(EventServer_updateClientPosition (
        self, event->mapId, event->sessionKey,
        &event->commanderInfo, &event->commanderInfo.pos, &clientsAround))
    ) {
        error ("Cannot update player %s position.", event->sessionKey);
        status = false;
        goto cleanup;
    }

    if (zlist_size (clientsAround) > 0)
    {
        // Build the packet for the clients around
        msg = zmsg_new ();
        ZoneBuilder_jump (
            event->commanderInfo.pcId,
            event->height,
            msg
        );

        // Send the packet
        zframe_t *frame = zmsg_first (msg);
        if (!(EventServer_sendToClients (self, clientsAround, zframe_data (frame), zframe_size (frame)))) {
            error ("Failed to send the packet to the clients.");
            status = false;
            goto cleanup;
        }
    }

cleanup:
    zmsg_destroy (&msg);
    return status;
}

bool
EventHandler_restSit (
    EventServer *self,
    GameEventRestSit *event
) {
    bool status = true;
    zmsg_t *msg = NULL;
    zlist_t *clientsAround = NULL;

    // Get the clients around
    if (!EventServer_getClientsAround (self, event->sessionKey, &clientsAround)) {
        error ("Cannot get clients within range");
        status = false;
        goto cleanup;
    }

    if (zlist_size (clientsAround) > 0)
    {
        // Build the packet for the clients around
        msg = zmsg_new ();
        ZoneBuilder_restSit (
            event->pcId,
            msg
        );

        // Send the packet
        zframe_t *frame = zmsg_first (msg);
        if (!(EventServer_sendToClients (self, clientsAround, zframe_data (frame), zframe_size (frame)))) {
            error ("Failed to send the packet to the clients.");
            status = false;
            goto cleanup;
        }
    }

cleanup:
    zmsg_destroy (&msg);
    return status;
}
