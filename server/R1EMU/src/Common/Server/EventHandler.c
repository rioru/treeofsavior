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
    if (!(EventServer_updateClientPosition (self, event->socketId, &event->commander, &event->commander.cPos, event->mapId, &clientsAround))) {
        error ("Cannot update player %s position.", event->socketId);
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
    PositionXZ position = PositionXYZToXZ (&event->position);
    bool status = true;
    zlist_t *clientsAround = NULL;
    zmsg_t *msg = NULL;

    // Update client position and get the clients around
    if (!(EventServer_updateClientPosition (self, event->socketId, &event->commander, &position, event->mapId, &clientsAround))) {
        error ("Cannot update player %s position.", event->socketId);
        status = false;
        goto cleanup;
    }

    if (zlist_size (clientsAround) > 0)
    {
        // Build the packet for the clients around
        msg = zmsg_new ();
        ZoneBuilder_moveDir (
            event->commander.pcId,
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
    PositionXZ position = PositionXYZToXZ (&event->position);
    bool status = true;
    zmsg_t *msg = NULL;
    zlist_t *clientsAround = NULL;

    // Update client position and get the clients around
    if (!(EventServer_updateClientPosition (self, event->socketId, &event->commander, &position, event->mapId, &clientsAround))) {
        error ("Cannot update player %s position.", event->socketId);
        status = false;
        goto cleanup;
    }

    if (zlist_size (clientsAround) > 0)
    {
        // Build the packet for the clients around
        msg = zmsg_new ();
        ZoneBuilder_pcMoveStop (
            event->commander.pcId,
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
    PositionXZ position = PositionXYZToXZ (&event->position);
    bool status = true;
    zmsg_t *msg = NULL;
    zlist_t *clientsAround = NULL;

    // Update client position and get the clients around
    if (!(EventServer_updateClientPosition (self, event->socketId, &event->commander, &position, event->mapId, &clientsAround))) {
        error ("Cannot update player %s position.", event->socketId);
        status = false;
        goto cleanup;
    }

    if (zlist_size (clientsAround) > 0)
    {
        // Build the packet for the clients around
        msg = zmsg_new ();
        ZoneBuilder_jump (
            event->commander.pcId,
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
    if (!EventServer_getClientsAround (self, event->socketId, &clientsAround)) {
        error ("Cannot get clients within range");
        status = false;
        goto cleanup;
    }

    if (zlist_size (clientsAround) > 0)
    {
        // Build the packet for the clients around
        msg = zmsg_new ();
        ZoneBuilder_restSit (
            event->targetPcId,
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
