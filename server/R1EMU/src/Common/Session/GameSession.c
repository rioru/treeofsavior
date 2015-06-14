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
    CommanderInfo *commanderInfo
) {
    GameSession *self;

    if ((self = calloc (1, sizeof (GameSession))) == NULL) {
        return NULL;
    }

    if (!GameSession_init (self, commanderInfo)) {
        GameSession_destroy (&self);
        error ("GameSession failed to initialize.");
        return NULL;
    }

    return self;
}

bool
GameSession_init (
    GameSession *self,
    CommanderInfo *commanderInfo
) {
    memset (self, 0, sizeof (GameSession));
    memcpy (&self->currentCommander, commanderInfo, sizeof (CommanderInfo));
    return true;
}

void
GameSession_print (
    GameSession *self
) {
    dbg ("==== GameSession %p ====", self);
    dbg ("charactersBarrackCount = %u", self->charactersBarrackCount);
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
