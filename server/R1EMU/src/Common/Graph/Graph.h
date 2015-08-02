/**
 *
 *   ██████╗   ██╗ ███████╗ ███╗   ███╗ ██╗   ██╗
 *   ██╔══██╗ ███║ ██╔════╝ ████╗ ████║ ██║   ██║
 *   ██████╔╝ ╚██║ █████╗   ██╔████╔██║ ██║   ██║
 *   ██╔══██╗  ██║ ██╔══╝   ██║╚██╔╝██║ ██║   ██║
 *   ██║  ██║  ██║ ███████╗ ██║ ╚═╝ ██║ ╚██████╔╝
 *   ╚═╝  ╚═╝  ╚═╝ ╚══════╝ ╚═╝     ╚═╝  ╚═════╝
 *
 * @file Graph.h
 * @brief
 *
 *
 *
 * @license <license placeholder>
 */

#pragma once

// ---------- Includes ------------
#include "R1EMU.h"

// ---------- Defines -------------


// ------ Structure declaration -------
// Graph is an opaque structure
typedef struct Graph Graph;

/**
 * @brief GraphNode contains a list of arcs pointing to neighbors
 */
typedef struct GraphNode {
    /** Each GraphNode can be linked to [0..*] GraphArcs **/
    zlist_t *arcs;

    /** HashTable key */
    char *key;

    /** user_data is a user pointer to any data */
    void *user_data;
}   GraphNode;

/**
 * @brief GraphArc is a simple connection between 2 GraphNode
 */
typedef struct GraphArc {
    /** An GraphArc is located between two nodes */
    GraphNode *from, *to;

    /** user_data is a user pointer to any data */
    void *user_data;
}   GraphArc;


// ----------- Functions ------------

/**
 * @brief Allocate a new Graph structure.
 * @return A pointer to an allocated Graph, or NULL if an error occured.
 */
Graph *
Graph_new (
    void
);

/**
 * @brief Initialize an allocated Graph structure.
 * @param self An allocated Graph to initialize.
 * @return true on success, false otherwise.
 */
bool
Graph_init (
    Graph *self
);

/**
 * @brief Allocate a new GraphNode structure.
 * @param hashKey A string to the hashtable key. Must be unique.
 * @param user_data A custom user data
 * @return A pointer to an allocated GraphNode, or NULL if an error occured.
 */
GraphNode *
GraphNode_new (
    char *hashKey,
    void *user_data
);

/**
 * @brief Initialize an allocated GraphNode structure.
 * @param self An allocated GraphNode to initialize.
 * @param hashKey A string to the hashtable key. Must be unique.
 * @param user_data A custom user data
 * @return true on success, false otherwise.
 */
bool
GraphNode_init (
    GraphNode *self,
    char *hashKey,
    void *user_data
);

/**
 * @brief Add an arc between to nodes
 * @param self A pointer to an allocated graph.
 * @param from A source node
 * @param to A destination node
 * @return A new GraphArc
 */
GraphArc *
Graph_addArc (
    Graph *self,
    GraphNode *from,
    GraphNode *to
);

/**
 * @brief Generate a node hash key
 * @param self A pointer to an allocated GraphNode.
 * @return the hashtable key to the given GraphNode
 */
char *
GraphNode_genKey (
    GraphNode *self
);

/**
 * @brief Get a given node in the graph given its key
 * @param self A pointer to an allocated Graph.
 * @param key the node key
 */
GraphNode *
Graph_getNode (
    Graph *self,
    char *key
);

/**
 * @brief Display a graph content in the standard output
 * @param self A pointer to an allocated Graph.
 */
void
Graph_dump (
    Graph *self
);

/**
 * @brief Free an allocated GraphNode structure.
 * @param self A pointer to an allocated GraphNode.
 */
void
GraphNode_free (
    GraphNode *self
);

/**
 * @brief Free an allocated GraphNode structure and nullify the content of the pointer.
 * @param self A pointer to an allocated GraphNode.
 */
void
GraphNode_destroy (
    GraphNode **self
);

/**
 * @brief Free an allocated GraphArc structure.
 * @param self A pointer to an allocated GraphArc.
 */
void
GraphArc_free (
    GraphArc *self
);

/**
 * @brief Free an allocated GraphArc structure and nullify the content of the pointer.
 * @param self A pointer to an allocated GraphArc.
 */
void
GraphArc_destroy (
    GraphArc **self
);

/**
 * @brief Connect 2 nodes together
 * @param self A pointer to an allocated GraphArc.
 * @param from The first node
 * @param to The second node
 * @return true on success, false otherwise
 */
bool
Graph_link (
    Graph *self,
    GraphNode *node1,
    GraphNode *node2
);

/**
 * @brief Disconnect 2 nodes connected
 * @param self A pointer to an allocated GraphArc.
 * @param from The first node
 * @param to The second node
 * @return true on success, false otherwise
 */
bool
Graph_unlink (
    Graph *self,
    GraphNode *node1,
    GraphNode *node2
);

/** @brief Insert a node into the graph hashtable
 * @param self A pointer to an allocated GraphArc.
 * @param node The given node
 * @return true on success, false otherwise
 */
bool
Graph_insertNode (
    Graph *self,
    GraphNode *node
);

/**
 * @brief Get the arc between 2 nodes
 * @param from The source node
 * @param to The destination node
 * @param Return the Arc between the two nodes, or NULL if it doesn't exist
 */
GraphArc *
GraphNode_getArc (
    GraphNode *from,
    GraphNode *to
);

/**
 * @brief Remove the arc between 2 nodes
 * @param from The source node
 * @param to The destination node
 * @param true on success, false otherwise
 */
bool
Graph_destroyArc (
    Graph *self,
    GraphNode *from,
    GraphNode *to
);

/**
 * @brief Free an allocated Graph structure.
 * @param self A pointer to an allocated Graph.
 */
void
Graph_free (
    Graph *self
);

/**
 * @brief Free an allocated Graph structure and nullify the content of the pointer.
 * @param self A pointer to an allocated Graph.
 */
void
Graph_destroy (
    Graph **self
);

