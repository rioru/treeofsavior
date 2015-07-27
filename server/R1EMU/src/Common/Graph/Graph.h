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
typedef struct Graph Graph;
typedef struct GraphArc GraphArc;
typedef struct GraphVertex GraphVertex;

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
 * @brief Allocate a new GraphVertex structure.
 * @param user_data A custom user data
 * @return A pointer to an allocated GraphVertex, or NULL if an error occured.
 */
GraphVertex *
GraphVertex_new (
    void *user_data
);

/**
 * @brief Initialize an allocated GraphVertex structure.
 * @param self An allocated GraphVertex to initialize.
 * @param user_data A custom user data
 * @return true on success, false otherwise.
 */
bool
GraphVertex_init (
    GraphVertex *self,
    void *user_data
);

/**
 * @brief Add an arc between to vertices
 * @param self A pointer to an allocated graph.
 * @param from A source vertex
 * @param to A destination vertex
 * @return A new GraphArc
 */
GraphArc *
Graph_addArc (
    Graph *self,
    GraphVertex *from,
    GraphVertex *to
);

/**
 * @brief Generate a vertex hash key
 * @param self A pointer to an allocated GraphVertex.
 * @return the hashtable key to the given GraphVertex
 */
char *
GraphVertex_genKey (
    GraphVertex *self
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
 * @brief Free an allocated GraphVertex structure.
 * @param self A pointer to an allocated GraphVertex.
 */
void
GraphVertex_free (
    GraphVertex *self
);

/**
 * @brief Free an allocated GraphVertex structure and nullify the content of the pointer.
 * @param self A pointer to an allocated GraphVertex.
 */
void
GraphVertex_destroy (
    GraphVertex **self
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

