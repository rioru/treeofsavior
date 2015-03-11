/**
 *
 *   ██████╗   ██╗ ███████╗ ███╗   ███╗ ██╗   ██╗
 *   ██╔══██╗ ███║ ██╔════╝ ████╗ ████║ ██║   ██║
 *   ██████╔╝ ╚██║ █████╗   ██╔████╔██║ ██║   ██║
 *   ██╔══██╗  ██║ ██╔══╝   ██║╚██╔╝██║ ██║   ██║
 *   ██║  ██║  ██║ ███████╗ ██║ ╚═╝ ██║ ╚██████╔╝
 *   ╚═╝  ╚═╝  ╚═╝ ╚══════╝ ╚═╝     ╚═╝  ╚═════╝
 *
 * @file ClientSession.h
 * @brief
 *
 *
 *
 * @license <license placeholder>
 */

// ---------- Includes ------------
#include "ClientSession.h"


ClientSession *
ClientSession_new (
    void
) {
    ClientSession *self;

    if ((self = calloc (1, sizeof (ClientSession))) == NULL) {
        return NULL;
    }

    if (!ClientSession_init (self)) {
        ClientSession_destroy (&self);
        error ("ClientSession failed to initialize.");
        return NULL;
    }

    return self;
}


bool
ClientSession_init (
    ClientSession *self
) {
    return true;
}


void
ClientSession_print (
    ClientSession *self
) {
    dbg ("==== Session %p ====", self);
    dbg ("familyName = %s", self->familyName);
    dbg ("charactersBarrackCount = %u", self->charactersBarrackCount);
    dbg ("accountId = 0x%llX", self->accountId);
    dbg ("currentCommanderId = 0x%llX", self->currentCommanderId);
    dbg ("currentPcId = %u", self->currentPcId);
}

void
ClientSession_destroy (
    ClientSession **_self
) {
    ClientSession *self = *_self;

    free (self);
    *_self = NULL;
}
