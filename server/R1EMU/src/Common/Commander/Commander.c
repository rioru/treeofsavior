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


// ------ Extern function implementation ------

void CommanderEquipment_init (
    CommanderEquipment *equipment
) {
    equipment->head_top = 2;
    equipment->head_middle = 2;
    equipment->itemUnk1 = 4;
    equipment->body_armor = 0x81a9d;
    equipment->gloves = 6;
    equipment->boots = 7;
    equipment->itemUnk2 = 0x2710;
    equipment->bracelet = 0x2af8;
    equipment->weapon = 0x3118d;
    equipment->shield = 0x98967c;
    equipment->costume = 4;
    equipment->itemUnk3 = 9;
    equipment->itemUnk4 = 9;
    equipment->itemUnk5 = 4;
    equipment->leg_armor = 0x7f38d;
    equipment->itemUnk6 = 9;
    equipment->itemUnk7 = 9;
    equipment->ring_left = 9;
    equipment->ring_right = 9;
    equipment->necklace = 0xa;
}

void
CommanderInfo_createBasicCommander (
    CommanderInfo *commander
) {
    memset (commander, 0, sizeof (CommanderInfo));

    commander->base.unk1 = SWAP_UINT32 (0x10000000); // ICBT
    commander->base.unk2 = 0; // ICBT
    commander->base.unk3 = SWAP_UINT16 (0x0610); // ICBT
    commander->base.accountId = -1;
    commander->base.classId = 0x2715; // Cleric
    commander->base.unk4 = SWAP_UINT16 (0xCB0F); // ICBT
    commander->base.jobId = SWAP_UINT16 (0xA10F); // Cleric ; ICBT
    commander->base.gender = 2;
    commander->base.unk5 = 0;
    commander->base.level = 1;
    CommanderEquipment_init (&commander->base.equipment);
    commander->base.hairType = 0x10;

    commander->pos.x = 27.0;
    commander->pos.z = 29.0;
}

void
CommanderEquipment_print (
    CommanderEquipment *equipment
) {
    dbg ("head_top = %d (%x)", equipment->head_top, equipment->head_top);
    dbg ("head_middle = %d (%x)", equipment->head_middle, equipment->head_middle);
    dbg ("itemUnk1 = %d (%x)", equipment->itemUnk1, equipment->itemUnk1);
    dbg ("body_armor = %d (%x)", equipment->body_armor, equipment->body_armor);
    dbg ("gloves = %d (%x)", equipment->gloves, equipment->gloves);
    dbg ("boots = %d (%x)", equipment->boots, equipment->boots);
    dbg ("itemUnk2 = %d (%x)", equipment->itemUnk2, equipment->itemUnk2);
    dbg ("bracelet = %d (%x)", equipment->bracelet, equipment->bracelet);
    dbg ("weapon = %d (%x)", equipment->weapon, equipment->weapon);
    dbg ("shield = %d (%x)", equipment->shield, equipment->shield);
    dbg ("costume = %d (%x)", equipment->costume, equipment->costume);
    dbg ("itemUnk3 = %d (%x)", equipment->itemUnk3, equipment->itemUnk3);
    dbg ("itemUnk4 = %d (%x)", equipment->itemUnk4, equipment->itemUnk4);
    dbg ("itemUnk5 = %d (%x)", equipment->itemUnk5, equipment->itemUnk5);
    dbg ("leg_armor = %d (%x)", equipment->leg_armor, equipment->leg_armor);
    dbg ("itemUnk6 = %d (%x)", equipment->itemUnk6, equipment->itemUnk6);
    dbg ("itemUnk7 = %d (%x)", equipment->itemUnk7, equipment->itemUnk7);
    dbg ("ring_left = %d (%x)", equipment->ring_left, equipment->ring_left);
    dbg ("ring_right = %d (%x)", equipment->ring_right, equipment->ring_right);
    dbg ("necklace = %d (%x)", equipment->necklace, equipment->necklace);
}

void
CommanderInfo_print (
    CommanderInfo *commander
) {
    dbg ("commanderName = %s", commander->base.commanderName);
    dbg ("familyName = %s", commander->base.familyName);
    dbg ("accountId = %llu (%llx)", commander->base.accountId, commander->base.accountId);
    dbg ("classId = %d (%x)", commander->base.classId, commander->base.classId);
    dbg ("unk2 = %d (%x)", commander->base.unk2, commander->base.unk2);
    dbg ("jobId = %d (%x)", commander->base.jobId, commander->base.jobId);
    dbg ("gender = %d (%x)", commander->base.gender, commander->base.gender);
    dbg ("unk3 = %d (%x)", commander->base.unk3, commander->base.unk3);
    dbg ("level = %d (%x)", commander->base.level, commander->base.level);
    CommanderEquipment_print (&commander->base.equipment);
    dbg ("hairType = %d (%x)", commander->base.hairType, commander->base.hairType);
    dbg ("unk4 = %d (%x)", commander->base.unk4, commander->base.unk4);
    dbg ("unk5 = %d (%x)", commander->base.unk5, commander->base.unk5);
    dbg ("currentXP = %d (%x)", commander->currentXP, commander->currentXP);
    dbg ("maxXP = %d (%x)", commander->maxXP, commander->maxXP);
    dbg ("spriteId = %lld (%llx)", commander->spriteId, commander->spriteId);
    dbg ("commanderId = %llu (%llx)", commander->commanderId, commander->commanderId);
    dbg ("currentHP = %f (%x)", commander->currentHP, commander->currentHP);
    dbg ("maxHP = %f (%x)", commander->maxHP, commander->maxHP);
    dbg ("currentSP = %f (%x)", commander->currentSP, commander->currentSP);
    dbg ("maxSP = %f (%x)", commander->maxSP, commander->maxSP);
    dbg ("posX = %f (%x)", commander->pos.x, commander->pos.x);
    dbg ("posZ = %f (%x)", commander->pos.z, commander->pos.z);
}
