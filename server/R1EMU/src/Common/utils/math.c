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
#include "math.h"


// ------ Structure declaration -------


// ------ Static declaration -------


// ------ Extern function implementation -------

bool
Math_isWithin2DManhattanDistance (
    Position2D *pos1,
    Position2D *pos2,
    float distance
) {
    float dx = abs (pos2->x - pos1->x);
    float dz = abs (pos2->z - pos1->z);

    return (dx < distance && dz < distance);
}

bool
Math_isWithin2DCircleDistance (
    Position2D *pos1,
    Position2D *pos2,
    float distance
) {
    float dx = abs (pos2->x - pos1->x);
    float dz = abs (pos2->z - pos1->z);

    return (square(dx) + square(dz)) < (square(distance));
}
