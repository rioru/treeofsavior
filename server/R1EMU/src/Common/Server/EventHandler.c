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
    bool status = true;
    zmsg_t *msg = NULL;
    zlist_t *clientsAround = NULL;

    // Get the clients around
    if (!(clientsAround = EventServer_getClientsWithinRange (
        self,
        routerId,
        event->mapId,
        event->socketId,
        &around,
        COMMANDER_RANGE_AROUND
    ))) {
        error ("Cannot get clients within range");
        status = false;
        goto cleanup;
    }

    if (zlist_size (clientsAround) > 0)
    {
        // Build the packet for the clients around
        msg = zmsg_new ();
        ZoneBuilder_enterPc (&event->commander, msg);

        // Send the packet
        zframe_t *frame = zmsg_first (msg);
        if (!(EventServer_sendToClients (self, clientsAround, zframe_data (frame), zframe_size (frame)))) {
            error ("Failed to send the packet to the clients.");
            status = false;
            goto cleanup;
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
                status = false;
                goto cleanup;
            }

            ZoneBuilder_enterPc (&gameSessionClientAround.currentCommander, reply);
        }

        frame = zmsg_first (reply);
        if (!(EventServer_sendToClients (self, newPcClient, zframe_data (frame), zframe_size (frame)))) {
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
EventHandler_commanderMove (
    EventServer *self,
    GameEventCommanderMove *event
) {
    PositionXZ around = PositionXYZToXZ (&event->position);
    bool status = true;
    zmsg_t *msg = NULL;
    zlist_t *clientsAround = NULL;

    // Get the clients around
    if (!(clientsAround = EventServer_getClientsWithinRange (
        self,
        EventServer_getRouterId (self),
        event->mapId, event->socketId, &around,
        COMMANDER_RANGE_AROUND
    ))) {
        error ("Cannot get clients within range");
        status = false;
        goto cleanup;
    }

    // Get the registred clients around
    GraphNode *currentClientNode = EventServer_getClientNode (self, event->socketId);

    // Mark the nodes as unvisited
    for (GraphArc *neighbourArc = zlist_first (currentClientNode->arcs);
        neighbourArc != NULL;
        neighbourArc = zlist_next (currentClientNode->arcs)
    ) {
        GraphNode *neighbourNode = neighbourArc->to;
        GraphNodeClient *neighbourClient = neighbourNode->user_data;
        neighbourClient->around = false;
    }

    if (zlist_size (clientsAround) > 0)
    {
        // Check in the list of current clients around if the current player entered in their zone
        for (uint8_t *socketIdClientAround = zlist_first (clientsAround);
             socketIdClientAround != NULL;
             socketIdClientAround = zlist_next (clientsAround)
        ) {
            GraphNode *clientAroundNode;

            if (!(clientAroundNode = EventServer_getClientNode (self, socketIdClientAround))) {
                error ("Cannot get the neighbour node %s.", socketIdClientAround);
                status = false;
                goto cleanup;
            }

            GraphNodeClient *neighbourClient = clientAroundNode->user_data;
            if (!(GraphNode_isLinked (currentClientNode, clientAroundNode))) {
                // currentClientNode isn't linked yet with its neighbour
                // It means that the current client has just entered in the neighbour client zone !
                // Connect them together, and warn neighbour client of the arrival of a new client
                zmsg_t *pcEnterMsg = zmsg_new ();
                ZoneBuilder_enterPc (&event->commander, pcEnterMsg);
                EventServer_linkClients (self, currentClientNode, clientAroundNode);
            }
            neighbourClient->around = true;
        }

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

        zmsg_destroy (&msg);
    }

cleanup:
    zmsg_destroy (&msg);
    return status;
}


bool
EventHandler_moveStop (
    EventServer *self,
    GameEventMoveStop *event
) {
    PositionXZ around = PositionXYZToXZ (&event->position);
    bool status = true;
    zmsg_t *msg = NULL;
    zlist_t *clientsAround = NULL;

    // Get the clients around
    if (!(clientsAround = EventServer_getClientsWithinRange (
        self,
        EventServer_getRouterId (self),
        event->mapId,
        event->socketId,
        &around,
        COMMANDER_RANGE_AROUND
    ))) {
        error ("Cannot get clients within range");
        status = false;
        goto cleanup;
    }

    if (zlist_size (clientsAround) > 0)
    {
        // Build the packet for the clients around
        msg = zmsg_new ();
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
    PositionXZ around = PositionXYZToXZ (&event->position);
    bool status = true;
    zmsg_t *msg = NULL;
    zlist_t *clientsAround = NULL;

    // Get the clients around
    if (!(clientsAround = EventServer_getClientsWithinRange (
        self,
        EventServer_getRouterId (self),
        event->mapId,
        event->socketId,
        &around,
        COMMANDER_RANGE_AROUND
    ))) {
        error ("Cannot get clients within range");
        status = false;
        goto cleanup;
    }

    if (zlist_size (clientsAround) > 0)
    {
        // Build the packet for the clients around
        msg = zmsg_new ();
        ZoneBuilder_jump (
            event->targetPcId,
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
    PositionXZ around = PositionXYZToXZ (&event->position);
    bool status = true;
    zmsg_t *msg = NULL;
    zlist_t *clientsAround = NULL;

    // Get the clients around
    if (!(clientsAround = EventServer_getClientsWithinRange (
        self,
        EventServer_getRouterId (self),
        event->mapId,
        event->socketId,
        &around,
        COMMANDER_RANGE_AROUND
    ))) {
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
