/**
 *
 *   ██████╗   ██╗ ███████╗ ███╗   ███╗ ██╗   ██╗
 *   ██╔══██╗ ███║ ██╔════╝ ████╗ ████║ ██║   ██║
 *   ██████╔╝ ╚██║ █████╗   ██╔████╔██║ ██║   ██║
 *   ██╔══██╗  ██║ ██╔══╝   ██║╚██╔╝██║ ██║   ██║
 *   ██║  ██║  ██║ ███████╗ ██║ ╚═╝ ██║ ╚██████╔╝
 *   ╚═╝  ╚═╝  ╚═╝ ╚══════╝ ╚═╝     ╚═╝  ╚═════╝
 *
 * @file AccountSession.h
 * @brief
 *
 *
 *
 * @license <license placeholder>
 */

// ---------- Includes ------------
#include "AccountSession.h"

AccountSession *
AccountSession_new (
    uint8_t *accountLogin,
    uint8_t *socketId,
    AccountSessionPrivileges accountPrivilege
) {
    AccountSession *self;

    if ((self = calloc (1, sizeof (AccountSession))) == NULL) {
        return NULL;
    }

    if (!AccountSession_init (self, accountLogin, socketId, accountPrivilege)) {
        AccountSession_destroy (&self);
        error ("AccountSession failed to initialize.");
        return NULL;
    }

    return self;
}

bool
AccountSession_init (
    AccountSession *self,
    uint8_t *accountLogin,
    uint8_t *socketId,
    AccountSessionPrivileges accountPrivilege
) {
    memset (self, 0, sizeof (AccountSession));

    memcpy (self->accountLogin, accountLogin, sizeof (self->accountLogin));
    memcpy (self->socketId, socketId, sizeof (self->socketId));
    self->accountPrivilege = accountPrivilege;

    return true;
}

void
AccountSession_print (
    AccountSession *self
) {
    dbg ("==== AccountSession %p ====", self);
}

void
AccountSession_destroy (
    AccountSession **_self
) {
    AccountSession_free (*_self);
    *_self = NULL;
}

void
AccountSession_free (
    AccountSession *self
) {
    free (self);
}
