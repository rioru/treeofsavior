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

void
CommanderEquipment_init (
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
Commander_init (
    Commander *commander
) {
    memset (commander, 0, sizeof (*commander));

    commander->unk1 = SWAP_UINT32 (0x322C202D); // ICBT
    commander->unk2 = 0x34; // ICBT
    commander->unk3 = SWAP_UINT16 (0x2C20); // ICBT
    commander->accountId = -1;
    commander->classId = 0x2715; // Cleric
    commander->unk4 = SWAP_UINT16 (0x2220); // ICBT
    commander->jobId = SWAP_UINT16 (0xA10F); // Cleric ; ICBT
    commander->gender = 2;
    commander->unk5 = 0x4A; // ICBT
    commander->level = 1;
    CommanderEquipment_init (&commander->equipment);
    commander->hairType = 0x10;
    commander->unk6 = SWAP_UINT16 (0x0020); // ICBT
}

void
CommanderInfo_init (
    CommanderInfo *commanderInfo
) {
    memset (commanderInfo, 0, sizeof (*commanderInfo));

    Commander_init (&commanderInfo->base);

    commanderInfo->pos = PositionXYZ_decl (27.0, 30.0, 29.0);
    commanderInfo->currentXP = 0;
    commanderInfo->maxXP = 0xC; // ICBT
    commanderInfo->zoneServerId = ZoneServerId_decl (SWAP_UINT32 (0xEE250000), SWAP_UINT32 (0x3C010000));
    commanderInfo->zoneServerId2 = commanderInfo->zoneServerId + 1;
    commanderInfo->commanderId = -1;
    commanderInfo->currentHP = 238;
    commanderInfo->maxHP = 238;
    commanderInfo->currentSP = 105;
    commanderInfo->maxSP = 105;
    commanderInfo->currentStamina = 25000;
    commanderInfo->maxStamina = 25000;
    commanderInfo->unk6 = SWAP_UINT16 (0x0020);
    commanderInfo->unk7 = SWAP_UINT16 (0x5910); // ICBT
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
Commander_print (
    Commander *commander
) {
    dbg ("commanderName = %s", commander->commanderName);
    dbg ("familyName = %s", commander->familyName);
    dbg ("unk1 = %d (%x)", commander->unk1, commander->unk1);
    dbg ("unk2 = %d (%x)", commander->unk2, commander->unk2);
    dbg ("unk3 = %d (%x)", commander->unk3, commander->unk3);
    dbg ("accountId = %llu (%llx)", commander->accountId, commander->accountId);
    dbg ("classId = %d (%x)", commander->classId, commander->classId);
    dbg ("unk4 = %d (%x)", commander->unk4, commander->unk4);
    dbg ("jobId = %d (%x)", commander->jobId, commander->jobId);
    dbg ("gender = %d (%x)", commander->gender, commander->gender);
    dbg ("unk5 = %d (%x)", commander->unk5, commander->unk5);
    dbg ("level = %d (%x)", commander->level, commander->level);
    CommanderEquipment_print (&commander->equipment);
    dbg ("hairType = %d (%x)", commander->hairType, commander->hairType);
    dbg ("unk6 = %d (%x)", commander->unk6, commander->unk6);
}

void
CommanderInfo_print (
    CommanderInfo *commanderInfo
) {
    Commander_print (&commanderInfo->base);
    dbg ("posX = %f %f %f (%x %x %x)",
         commanderInfo->pos.x, commanderInfo->pos.y, commanderInfo->pos.z,
         commanderInfo->pos.x, commanderInfo->pos.y, commanderInfo->pos.z);
    dbg ("currentXP = %d (%x)", commanderInfo->currentXP, commanderInfo->currentXP);
    dbg ("maxXP = %d (%x)", commanderInfo->maxXP, commanderInfo->maxXP);
    dbg ("commanderId = %d (%x)", commanderInfo->commanderId, commanderInfo->commanderId);
    dbg ("zoneServerId = %llu (%llx)", commanderInfo->zoneServerId, commanderInfo->zoneServerId);
    dbg ("zoneServerId2 = %llu (%llx)", commanderInfo->zoneServerId2, commanderInfo->zoneServerId2);
    dbg ("currentHP = %d (%x)", commanderInfo->currentHP, commanderInfo->currentHP);
    dbg ("maxHP = %d (%x)", commanderInfo->maxHP, commanderInfo->maxHP);
    dbg ("currentSP = %d (%x)", commanderInfo->currentSP, commanderInfo->currentSP);
    dbg ("maxSP = %d (%x)", commanderInfo->maxSP, commanderInfo->maxSP);
    dbg ("currentStamina = %d (%x)", commanderInfo->currentStamina, commanderInfo->currentStamina);
    dbg ("maxStamina = %d (%x)", commanderInfo->maxStamina, commanderInfo->maxStamina);
    dbg ("unk6 = %d (%x)", commanderInfo->unk6, commanderInfo->unk6);
    dbg ("unk7 = %d (%x)", commanderInfo->unk7, commanderInfo->unk7);
}
