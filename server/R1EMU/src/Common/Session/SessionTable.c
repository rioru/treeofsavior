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
#include "SessionTable.h"


// ------ Structure declaration -------
struct SessionTable
{
    /** Hashtable of ClientSession */
    zhash_t *sessions;
};


// ------ Static declaration -------


// ------ Extern function implementation -------

SessionTable *
SessionTable_new (
    void
) {
    SessionTable *self;

    if ((self = calloc (1, sizeof (SessionTable))) == NULL) {
        return NULL;
    }

    if (!SessionTable_init (self)) {
        SessionTable_destroy (&self);
        error ("SessionTable failed to initialize.");
        return NULL;
    }

    return self;
}

bool
SessionTable_init (
    SessionTable *self
) {
    if (!(self->sessions = zhash_new ())) {
        error ("Cannot allocate a new sessions hashtable.");
        return false;
    }

    return true;
}

ClientSession *
SessionTable_lookup (
    SessionTable *self,
    unsigned char *key
) {
    return zhash_lookup (self->sessions, key);
}

ClientSession *
SessionTable_getByAccountId (
    SessionTable *self,
    uint64_t accountId,
    const unsigned char **key
) {
    for (ClientSession *session = zhash_first (self->sessions); session != NULL; session = zhash_next (self->sessions))
    {
        if (session->accountId == accountId) {
            *key = zhash_cursor (self->sessions);
            return session;
        }
    }

    return NULL;
}

ClientSession *
SessionTable_create (
    SessionTable *self,
    unsigned char *sessionKey
) {
    ClientSession *session = ClientSession_new ();
    zhash_insert (self->sessions, sessionKey, session);
    zhash_freefn (self->sessions, sessionKey, (zhash_free_fn *) ClientSession_free);

    return session;
}

void
SessionTable_delete (
    SessionTable *self,
    unsigned char *key
) {
    return zhash_delete (self->sessions, key);
}

void
SessionTable_destroy (
    SessionTable **_self
) {
    SessionTable *self = *_self;

    if (self->sessions) {
        zhash_destroy (&self->sessions);
    }

    free (self);
    *_self = NULL;
}
