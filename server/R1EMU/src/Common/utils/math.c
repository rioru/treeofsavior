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
    float x1, float y1,
    float x2, float y2,
    float distance
) {
    float dx = abs(x2 - x1);
    float dy = abs(y2 - y1);

    if (dx > distance) return false;
    if (dy > distance) return false;

    return true;
}

bool
Math_isWithin2DCircleDistance (
    float x1, float y1,
    float x2, float y2,
    float distance
) {
    float dx = abs (x2 - x1);
    float dy = abs (y2 - y1);

    return (dx*dx + dy*dy) < (distance*distance);
}
