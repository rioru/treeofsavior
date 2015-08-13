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

    commander->unk1 = SWAP_UINT32 (0x10000000); // ICBT3
    commander->unk2 = 0; // ICBT3
    commander->unk3 = SWAP_UINT16 (0x0610); // ICBT3
    commander->accountId = -1;
    commander->classId = 0x2715; // Cleric
    commander->unk4 = SWAP_UINT16 (0xCB0F); // ICBT3
    commander->jobId = SWAP_UINT16 (0xA10F); // Cleric ; ICBT3
    commander->gender = 2;
    commander->unk5 = 0;
    commander->level = 1;
    CommanderEquipment_init (&commander->equipment);
    commander->hairType = 0x10;
    commander->unk6 = 0;
    commander->unk7 = 0;

    commander->pos.x = 27.0;
    commander->pos.z = 29.0;

    /*
    commander->unk13 = 0;
    commander->charPosition = 0;
    commander->unk6 = 0;
    commander->mapId = 0x3fd;
    commander->spriteId = 0;
    commander->spriteIdRelated = 0x41000000;
    commander->unk7 = 0;
    commander->pos.x = 27.0;
    commander->pos.z = 29.0;

    // Fill with dummy stats
    commander->currentXP = 12000;
    commander->maxXP = 12000;
    commander->currentHP = 12000;
    commander->maxHP = 12000;
    commander->currentSP = 12000;
    commander->maxSP = 12000;
    commander->level = 10;

    commander->unk10 = commander->currentHP;
    */
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
    dbg ("commanderName = %s", commander->commanderName);
    dbg ("familyName = %s", commander->familyName);
    // dbg ("unk1 = %d (%x)", commander->unk1, commander->unk1);
    dbg ("accountId = %llu (%llx)", commander->accountId, commander->accountId);
    dbg ("classId = %d (%x)", commander->classId, commander->classId);
    dbg ("unk2 = %d (%x)", commander->unk2, commander->unk2);
    dbg ("jobId = %d (%x)", commander->jobId, commander->jobId);
    dbg ("gender = %d (%x)", commander->gender, commander->gender);
    dbg ("unk3 = %d (%x)", commander->unk3, commander->unk3);
    dbg ("level = %d (%x)", commander->level, commander->level);
    CommanderEquipment_print (&commander->equipment);
    dbg ("hairType = %d (%x)", commander->hairType, commander->hairType);
    dbg ("unk4 = %d (%x)", commander->unk4, commander->unk4);
    dbg ("unk5 = %d (%x)", commander->unk5, commander->unk5);
    dbg ("pcId = %d (%x)", commander->pcId, commander->pcId);
    dbg ("unk13 = %d (%x)", commander->unk13, commander->unk13);
    dbg ("charPosition = %d (%x)", commander->charPosition, commander->charPosition);
    dbg ("unk6 = %d (%x)", commander->unk6, commander->unk6);
    dbg ("mapId = %d (%x)", commander->mapId, commander->mapId);
    dbg ("currentXP = %d (%x)", commander->currentXP, commander->currentXP);
    dbg ("maxXP = %d (%x)", commander->maxXP, commander->maxXP);
    dbg ("unk7 = %d (%x)", commander->unk7, commander->unk7);
    dbg ("spriteId = %d (%x)", commander->spriteId, commander->spriteId);
    dbg ("spriteIdRelated = %d (%x)", commander->spriteIdRelated, commander->spriteIdRelated);
    dbg ("commanderId = %llu (%llx)", commander->commanderId, commander->commanderId);
    dbg ("currentHP = %f (%x)", commander->currentHP, commander->currentHP);
    dbg ("maxHP = %f (%x)", commander->maxHP, commander->maxHP);
    dbg ("currentSP = %f (%x)", commander->currentSP, commander->currentSP);
    dbg ("maxSP = %f (%x)", commander->maxSP, commander->maxSP);
    dbg ("posX = %f (%x)", commander->pos.x, commander->pos.x);
    dbg ("posZ = %f (%x)", commander->pos.z, commander->pos.z);
    dbg ("unk10 = %d (%x)", commander->unk10, commander->unk10);
    dbg ("unk11 = %d (%x)", commander->unk11, commander->unk11);
    dbg ("unk12 = %d (%x)", commander->unk12, commander->unk12);
}
