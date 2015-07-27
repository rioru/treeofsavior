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
#include "Graph.h"


// ------ Structure declaration -------
/**
 * @brief GraphVertex contains a list of arcs pointing to neighbors
 */
struct GraphVertex {
    /** Each GraphVertex can be linked to [0..*] GraphArcs **/
    zlist_t *arcs;

    /** HashTable key */
    char *key;

    /** user_data is a user pointer to any data */
    void *user_data;
};

/**
 * @brief GraphArc is a simple connection between 2 GraphVertex
 */
struct GraphArc {
    /** An GraphArc is located between two vertices */
    GraphVertex *from, *to;

    /** user_data is a user pointer to any data */
    void *user_data;
};

/**
 * @brief Graph is a list of unsorted vertices
 */
struct Graph {
    /** A graph is a hashtable of GraphVertex */
    zhash_t *vertices;
};

// ------ Static declaration -------
/**
 * @brief Allocate a new GraphArc structure.
 * @param from A source vertex
 * @param to A destination vertex
 * @return A pointer to an allocated GraphArc, or NULL if an error occured.
 */
static GraphArc *
GraphArc_new (
    GraphVertex *from,
    GraphVertex *to
);

/**
 * @brief Initialize an allocated GraphArc structure.
 * @param self An allocated GraphArc to initialize.
 * @param from A source vertex
 * @param to A destination vertex
 * @return true on success, false otherwise.
 */
static bool
GraphArc_init (
    GraphArc *self,
    GraphVertex *from,
    GraphVertex *to
);

// ------ Extern function implementation -------

Graph *
Graph_new (
    void
) {
    Graph *self;

    if ((self = calloc (1, sizeof (Graph))) == NULL) {
        return NULL;
    }

    if (!Graph_init (self)) {
        Graph_destroy (&self);
        error ("Graph failed to initialize.");
        return NULL;
    }

    return self;
}

bool
Graph_init (
    Graph *self
) {
    if (!(self->vertices = zhash_new ())) {
        error ("Cannot allocate a new hashtable for vertices.");
        return false;
    }

    return true;
}

static GraphArc *
GraphArc_new (
    GraphVertex *from,
    GraphVertex *to
) {
    GraphArc *self;

    if ((self = calloc (1, sizeof (GraphArc))) == NULL) {
        return NULL;
    }

    if (!GraphArc_init (self, from, to)) {
        GraphArc_destroy (&self);
        error ("GraphArc failed to initialize.");
        return NULL;
    }

    return self;
}

static bool
GraphArc_init (
    GraphArc *self,
    GraphVertex *from,
    GraphVertex *to
) {
    self->from = from;
    self->to = to;
    self->user_data = NULL;

    // Add the current arc to the arcs list : from -> self
    zlist_append (self->from->arcs, self);

    return true;
}

GraphVertex *
GraphVertex_new (
    void *user_data
) {
    GraphVertex *self;

    if ((self = calloc (1, sizeof (GraphVertex))) == NULL) {
        return NULL;
    }

    if (!GraphVertex_init (self, user_data)) {
        GraphVertex_destroy (&self);
        error ("GraphVertex failed to initialize.");
        return NULL;
    }

    return self;
}

bool
GraphVertex_init (
    GraphVertex *self,
    void *user_data
) {
    self->user_data = user_data;

    if (!(self->arcs = zlist_new ())) {
        error ("Cannot allocate a new zlist for arcs.");
        return false;
    }

    if (!(self->key = GraphVertex_genKey (self))) {
        error ("Cannot generated a vertex key.");
        return false;
    }
    info ("key = %s", self->key);

    return true;
}

GraphArc *
Graph_addArc (
    Graph *self,
    GraphVertex *from,
    GraphVertex *to
) {
    GraphArc *arc = NULL;

    if (!(arc = GraphArc_new (from, to))) {
        error ("Cannot create new arc.");
        return NULL;
    }

    if (!zhash_lookup (self->vertices, from->key)) {
        zhash_insert (self->vertices, from->key, from);
    }

    if (!zhash_lookup (self->vertices, to->key)) {
        zhash_insert (self->vertices, to->key, to);
    }

    return arc;
}

char *
GraphVertex_genKey (
    GraphVertex *self
) {
    char *key = NULL;
    size_t keySize = (sizeof (GraphVertex *) * 2) + 1;

    if (!(key = calloc (1, keySize))) {
        error ("Cannot allocate a vertex key.");
        return NULL;
    }

    // Key = pointer
    snprintf (key, keySize, "%p", self);

    return key;
}

void
Graph_dump (
    Graph *self
) {
    int loop_counter = 0;

    for (GraphVertex *vertex = zhash_first (self->vertices); vertex != NULL; vertex = zhash_next (self->vertices)) {
        dbg ("- Arcs of vertex %c :", 'A' + loop_counter++);
        for (GraphArc *arc = zlist_first (vertex->arcs); arc != NULL; arc = zlist_next (vertex->arcs)) {
            dbg ("   %s -> %s", arc->from->key, arc->to->key);
        }
    }
}

void
GraphVertex_free (
    GraphVertex *self
) {
    // TODO
}

void
GraphVertex_destroy (
    GraphVertex **_self
) {
    GraphVertex *self = *_self;

    if (self) {
        GraphVertex_free (self);
        free (self);
    }

    *_self = NULL;
}

void
GraphArc_free (
    GraphArc *self
) {
    // TODO
}

void
GraphArc_destroy (
    GraphArc **_self
) {
    GraphArc *self = *_self;

    if (self) {
        GraphArc_free (self);
        free (self);
    }

    *_self = NULL;
}

void
Graph_free (
    Graph *self
) {
    // TODO
}

void
Graph_destroy (
    Graph **_self
) {
    Graph *self = *_self;

    if (self) {
        Graph_free (self);
        free (self);
    }

    *_self = NULL;
}
