/**
 *
 *   ██████╗   ██╗ ███████╗ ███╗   ███╗ ██╗   ██╗
 *   ██╔══██╗ ███║ ██╔════╝ ████╗ ████║ ██║   ██║
 *   ██████╔╝ ╚██║ █████╗   ██╔████╔██║ ██║   ██║
 *   ██╔══██╗  ██║ ██╔══╝   ██║╚██╔╝██║ ██║   ██║
 *   ██║  ██║  ██║ ███████╗ ██║ ╚═╝ ██║ ╚██████╔╝
 *   ╚═╝  ╚═╝  ╚═╝ ╚══════╝ ╚═╝     ╚═╝  ╚═════╝
 *
 * @file math.h
 * @brief
 *
 *
 *
 * @license <license placeholder>
 */

#pragma once

// ---------- Includes ------------
#include "R1EMU.h"
#include "position.h"

// ---------- Defines -------------
#define square(x) ((x)*(x))


// ------ Structure declaration -------

// ----------- Functions ------------
/**
 * @brief Check if 2 points are within a given Manhattan distance
 * @param pos1 : position of the first point
 * @param pos2 : position of the second point
 * @param distance : Distance being checked
 * @return true on success, false otherwise
 * References : http://en.wikipedia.org/wiki/Taxicab_geometry#Circles_in_Taxicab_geometry
 */
bool
Math_isWithin2DManhattanDistance (
    Position2D *pos1,
    Position2D *pos2,
    float distance
);

/**
 * @brief Check if 2 points are within a given euclidian distance
 * @param x1 y1 : position of the first point
 * @param x2 y2 : position of the second point
 * @param range : Distance being checked
 * @return true on success, false otherwise
 * References : http://en.wikipedia.org/wiki/Euclidean_distance
 */
bool
Math_isWithin2DCircleDistance (
    Position2D *pos1,
    Position2D *pos2,
    float distance
);
