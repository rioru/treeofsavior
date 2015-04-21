/**
 *
 *   ██████╗   ██╗ ███████╗ ███╗   ███╗ ██╗   ██╗
 *   ██╔══██╗ ███║ ██╔════╝ ████╗ ████║ ██║   ██║
 *   ██████╔╝ ╚██║ █████╗   ██╔████╔██║ ██║   ██║
 *   ██╔══██╗  ██║ ██╔══╝   ██║╚██╔╝██║ ██║   ██║
 *   ██║  ██║  ██║ ███████╗ ██║ ╚═╝ ██║ ╚██████╔╝
 *   ╚═╝  ╚═╝  ╚═╝ ╚══════╝ ╚═╝     ╚═╝  ╚═════╝
 *
 * @file GameSession.h
 * @brief
 *
 *
 *
 * @license <license placeholder>
 */

// ---------- Includes ------------
#include "GameSession.h"

GameSession *
GameSession_new (
    void
) {
    GameSession *self;

    if ((self = calloc (1, sizeof (GameSession))) == NULL) {
        return NULL;
    }

    if (!GameSession_init (self)) {
        GameSession_destroy (&self);
        error ("GameSession failed to initialize.");
        return NULL;
    }

    return self;
}

bool
GameSession_init (
    GameSession *self
) {
    memset (self, 0, sizeof (GameSession));
    return true;
}

void
GameSession_print (
    GameSession *self
) {
    dbg ("==== GameSession %p ====", self);
    dbg ("charactersBarrackCount = %u", self->charactersBarrackCount);
    dbg ("currentCommanderId = 0x%llX", self->currentCommanderId);
    dbg ("currentPcId = 0x%X", self->currentPcId);
    CommanderInfo_print (&self->currentCommander);
}

void
GameSession_destroy (
    GameSession **_self
) {
    GameSession_free (*_self);
    *_self = NULL;
}

void
GameSession_free (
    GameSession *self
) {
    free (self);
}
