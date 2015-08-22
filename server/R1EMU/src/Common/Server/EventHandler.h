/**
 *
 *   ██████╗   ██╗ ███████╗ ███╗   ███╗ ██╗   ██╗
 *   ██╔══██╗ ███║ ██╔════╝ ████╗ ████║ ██║   ██║
 *   ██████╔╝ ╚██║ █████╗   ██╔████╔██║ ██║   ██║
 *   ██╔══██╗  ██║ ██╔══╝   ██║╚██╔╝██║ ██║   ██║
 *   ██║  ██║  ██║ ███████╗ ██║ ╚═╝ ██║ ╚██████╔╝
 *   ╚═╝  ╚═╝  ╚═╝ ╚══════╝ ╚═╝     ╚═╝  ╚═════╝
 *
 * @file EventHandler.h
 * @brief
 *
 *
 *
 * @license <license placeholder>
 */

#pragma once

// ---------- Includes ------------
#include "R1EMU.h"
#include "EventServer.h"

// ---------- Defines -------------
#define COMMANDER_RANGE_AROUND 250.0


// ------ Structure declaration -------
typedef struct {
    uint16_t mapId;
    char sessionKey [SOCKET_SESSION_ID_SIZE];
    CommanderInfo commanderInfo;
    PositionXYZ position;
    PositionXZ direction;
    float timestamp;
} GameEventCommanderMove;

typedef struct {
    uint16_t mapId;
    char sessionKey [SOCKET_SESSION_ID_SIZE];
    CommanderInfo commanderInfo;
    PositionXYZ position;
    PositionXZ direction;
    float timestamp;
} GameEventMoveStop;

typedef struct {
    uint32_t pcId;
    char sessionKey [SOCKET_SESSION_ID_SIZE];
} GameEventRestSit;

typedef struct {
    uint16_t mapId;
    char sessionKey [SOCKET_SESSION_ID_SIZE];
    CommanderInfo commanderInfo;
    float height;
} GameEventJump;

typedef struct {
    uint16_t mapId;
    char sessionKey [SOCKET_SESSION_ID_SIZE];
    CommanderInfo commanderInfo;
} GameEventPcEnter;


// ----------- Functions ------------

bool
EventHandler_commanderMove (
    EventServer *self,
    GameEventCommanderMove *eventData
);

bool
EventHandler_moveStop (
    EventServer *self,
    GameEventMoveStop *eventData
);

bool
EventHandler_restSit (
    EventServer *self,
    GameEventRestSit *eventData
);

bool
EventHandler_enterPc (
    EventServer *self,
    GameEventPcEnter *event
);

bool
EventHandler_jump (
    EventServer *self,
    GameEventJump *event
);
