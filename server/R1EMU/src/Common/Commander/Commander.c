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
#include "Commander.h"


// ------ Structure declaration -------


// ------ Static declaration -------


// ------ Extern declaration -------

CommanderInfo
Commander_CreateBasicCommander (
    void
) {
    CommanderInfo commander;

    commander.classId = 10001;
    commander.jobId = 1;
    commander.gender = 1;
    commander.unk3 = 1;
    commander.level = 1;
    commander.head_top = 0x00002710;
    commander.head_middle = 0x00002710;
    commander.itemUnk1 = 0x00002710;
    commander.body_armor = 0x00002710;
    commander.gloves = 0x00002710;
    commander.boots = 0x00002710;
    commander.itemUnk2 = 0x00002710;
    commander.bracelet = 0x00002710;
    commander.weapon = 0x00002710;
    commander.shield = 0x00002710;
    commander.costume = 0x00002710;
    commander.itemUnk3 = 0x00002710;
    commander.itemUnk4 = 0x00002710;
    commander.itemUnk5 = 0x00002710;
    commander.leg_armor = 0x00002710;
    commander.itemUnk6 = 0x00002710;
    commander.itemUnk7 = 0x00002710;
    commander.ring_left = 0x00002710;
    commander.ring_right = 0x00002710;
    commander.necklace = 0x00002710;
    commander.hairType = 2;
    commander.mapId = 0x551;

    return commander;
}
