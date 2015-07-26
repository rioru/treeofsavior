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
    PositionXZ around = event->commander.cPos;
    uint16_t routerId = EventServer_getRouterId (self);

    // Get the clients around
    zlist_t *clientsAround = EventServer_getClientsWithinRange (
        self,
        routerId,
        event->mapId,
        event->socketId,
        &around,
        COMMANDER_RANGE_AROUND
    );

    if (zlist_size (clientsAround) > 0)
    {
        // Build the packet for the clients around
        zmsg_t *msg = zmsg_new ();
        ZoneBuilder_enterPc (&event->commander, msg);

        // Send the packet
        zframe_t *frame = zmsg_first (msg);
        if (!(EventServer_sendToClients (self, clientsAround, zframe_data (frame), zframe_size (frame)))) {
            error ("Failed to send the packet to the clients.");
            return false;
        }

        // Also get information about the people around and them back to the pcId source client
        zlist_t *newPcClient = zlist_new ();
        zlist_append (newPcClient, event->socketId);
        zmsg_t *reply = zmsg_new ();
        for (char *socketId = zlist_first (clientsAround); socketId != NULL; socketId = zlist_next (clientsAround))
        {
            GameSession gameSessionClientAround;
            if (!(EventServer_getGameSessionBySocketId (self, routerId, socketId, &gameSessionClientAround))) {
                warning ("Cannot get the game session of the client around.");
                continue;
            }

            ZoneBuilder_enterPc (&gameSessionClientAround.currentCommander, reply);
        }

        frame = zmsg_first (reply);
        if (!(EventServer_sendToClients (self, newPcClient, zframe_data (frame), zframe_size (frame)))) {
            error ("Failed to send the packet to the clients.");
            return false;
        }
    }

    return true;
}


bool
EventHandler_commanderMove (
    EventServer *self,
    GameEventCommanderMove *event
) {
    PositionXZ around = PositionXYZToXZ (&event->position);

    // Get the clients around
    zlist_t *clientsAround = EventServer_getClientsWithinRange (
        self,
        EventServer_getRouterId (self),
        event->mapId,
        event->socketId,
        &around,
        COMMANDER_RANGE_AROUND
    );

    if (zlist_size (clientsAround) > 0)
    {
        // Build the packet for the clients around
        zmsg_t *msg = zmsg_new ();
        ZoneBuilder_moveDir (
            event->targetPcId,
            &event->position,
            &event->direction,
            event->timestamp,
            msg
        );

        // Send the packet
        zframe_t *frame = zmsg_first (msg);
        if (!(EventServer_sendToClients (self, clientsAround, zframe_data (frame), zframe_size (frame)))) {
            error ("Failed to send the packet to the clients.");
            return false;
        }
    }

    return true;
}


bool
EventHandler_moveStop (
    EventServer *self,
    GameEventMoveStop *event
) {
    PositionXZ around = PositionXYZToXZ (&event->position);

    // Get the clients around
    zlist_t *clientsAround = EventServer_getClientsWithinRange (
        self,
        EventServer_getRouterId (self),
        event->mapId,
        event->socketId,
        &around,
        COMMANDER_RANGE_AROUND
    );

    if (zlist_size (clientsAround) > 0)
    {
        // Build the packet for the clients around
        zmsg_t *msg = zmsg_new ();
        ZoneBuilder_pcMoveStop (
            event->targetPcId,
            &event->position,
            &event->direction,
            event->timestamp,
            msg
        );

        // Send the packet
        zframe_t *frame = zmsg_first (msg);
        if (!(EventServer_sendToClients (self, clientsAround, zframe_data (frame), zframe_size (frame)))) {
            error ("Failed to send the packet to the clients.");
            return false;
        }
    }

    return true;
}


bool
EventHandler_jump (
    EventServer *self,
    GameEventJump *event
) {
    PositionXZ around = PositionXYZToXZ (&event->position);

    // Get the clients around
    zlist_t *clientsAround = EventServer_getClientsWithinRange (
        self,
        EventServer_getRouterId (self),
        event->mapId,
        event->socketId,
        &around,
        COMMANDER_RANGE_AROUND
    );

    if (zlist_size (clientsAround) > 0)
    {
        // Build the packet for the clients around
        zmsg_t *msg = zmsg_new ();
        ZoneBuilder_jump (
            event->targetPcId,
            event->height,
            msg
        );

        // Send the packet
        zframe_t *frame = zmsg_first (msg);
        if (!(EventServer_sendToClients (self, clientsAround, zframe_data (frame), zframe_size (frame)))) {
            error ("Failed to send the packet to the clients.");
            return false;
        }
    }

    return true;
}

bool
EventHandler_restSit (
    EventServer *self,
    GameEventRestSit *event
) {
    PositionXZ around = PositionXYZToXZ (&event->position);

    // Get the clients around
    zlist_t *clientsAround = EventServer_getClientsWithinRange (
        self,
        EventServer_getRouterId (self),
        event->mapId,
        event->socketId,
        &around,
        COMMANDER_RANGE_AROUND
    );

    if (zlist_size (clientsAround) > 0)
    {
        // Build the packet for the clients around
        zmsg_t *msg = zmsg_new ();
        ZoneBuilder_restSit (
            event->targetPcId,
            msg
        );

        // Send the packet
        zframe_t *frame = zmsg_first (msg);
        if (!(EventServer_sendToClients (self, clientsAround, zframe_data (frame), zframe_size (frame)))) {
            error ("Failed to send the packet to the clients.");
            return false;
        }
    }

    return true;
}
