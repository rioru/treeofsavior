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
CommanderInfo_createBasicCommander (
    CommanderInfo *commander
) {
    memset (commander, 0, sizeof (CommanderInfo));

    commander->classId = 10001;
    commander->unk2 = 0;
    commander->jobId = 1;
    commander->gender = 1;
    commander->unk3 = 1;
    commander->level = 1;
    commander->head_top = 0x00002710;
    commander->head_middle = 0x00002710;
    commander->itemUnk1 = 0x00002710;
    commander->body_armor = 0x00002710;
    commander->gloves = 0x00002710;
    commander->boots = 0x00002710;
    commander->itemUnk2 = 0x00002710;
    commander->bracelet = 0x00002710;
    commander->weapon = 0x00002710;
    commander->shield = 0x00002710;
    commander->costume = 0x00002710;
    commander->itemUnk3 = 0x00002710;
    commander->itemUnk4 = 0x00002710;
    commander->itemUnk5 = 0x00002710;
    commander->leg_armor = 0x00002710;
    commander->itemUnk6 = 0x00002710;
    commander->itemUnk7 = 0x00002710;
    commander->ring_left = 0x00002710;
    commander->ring_right = 0x00002710;
    commander->necklace = 0x00002710;
    commander->hairType = 2;
    commander->unk5 = 0;
    commander->pose = 0;
    commander->spriteRelated = 1;
    commander->unk6 = 0;
    commander->mapId = 0x551;
    commander->unk7 = 0;
    commander->unk8 = 0;
    commander->unk9 = 0;
    commander->unk10 = 0;
    commander->unk11 = 0;
    commander->unk12 = 0;

    // Fill with dummy stats
    commander->currentXP = 6000;
    commander->maxXP = 12000;
    commander->currentHP = 6000;
    commander->maxHP = 12000;
    commander->currentSP = 6000;
    commander->maxSP = 12000;
    commander->level = 10;
}


void
CommanderInfo_print (
    CommanderInfo *commander
) {
    dbg ("charName = %s", commander->charName);
    dbg ("familyName = %s", commander->familyName);
    // dbg ("unk1 = %d (%x)", commander->unk1, commander->unk1);
    dbg ("accountId = %" PRIu64, commander->accountId, commander->accountId);
    dbg ("classId = %d (%x)", commander->classId, commander->classId);
    dbg ("unk2 = %d (%x)", commander->unk2, commander->unk2);
    dbg ("jobId = %d (%x)", commander->jobId, commander->jobId);
    dbg ("gender = %d (%x)", commander->gender, commander->gender);
    dbg ("unk3 = %d (%x)", commander->unk3, commander->unk3);
    dbg ("level = %d (%x)", commander->level, commander->level);
    dbg ("head_top = %d (%x)", commander->head_top, commander->head_top);
    dbg ("head_middle = %d (%x)", commander->head_middle, commander->head_middle);
    dbg ("itemUnk1 = %d (%x)", commander->itemUnk1, commander->itemUnk1);
    dbg ("body_armor = %d (%x)", commander->body_armor, commander->body_armor);
    dbg ("gloves = %d (%x)", commander->gloves, commander->gloves);
    dbg ("boots = %d (%x)", commander->boots, commander->boots);
    dbg ("itemUnk2 = %d (%x)", commander->itemUnk2, commander->itemUnk2);
    dbg ("bracelet = %d (%x)", commander->bracelet, commander->bracelet);
    dbg ("weapon = %d (%x)", commander->weapon, commander->weapon);
    dbg ("shield = %d (%x)", commander->shield, commander->shield);
    dbg ("costume = %d (%x)", commander->costume, commander->costume);
    dbg ("itemUnk3 = %d (%x)", commander->itemUnk3, commander->itemUnk3);
    dbg ("itemUnk4 = %d (%x)", commander->itemUnk4, commander->itemUnk4);
    dbg ("itemUnk5 = %d (%x)", commander->itemUnk5, commander->itemUnk5);
    dbg ("leg_armor = %d (%x)", commander->leg_armor, commander->leg_armor);
    dbg ("itemUnk6 = %d (%x)", commander->itemUnk6, commander->itemUnk6);
    dbg ("itemUnk7 = %d (%x)", commander->itemUnk7, commander->itemUnk7);
    dbg ("ring_left = %d (%x)", commander->ring_left, commander->ring_left);
    dbg ("ring_right = %d (%x)", commander->ring_right, commander->ring_right);
    dbg ("necklace = %d (%x)", commander->necklace, commander->necklace);
    dbg ("hairType = %d (%x)", commander->hairType, commander->hairType);
    dbg ("unk4 = %d (%x)", commander->unk4, commander->unk4);
    dbg ("unk5 = %d (%x)", commander->unk5, commander->unk5);
    dbg ("pcId = %d (%x)", commander->pcId, commander->pcId);
    dbg ("listPosition = %d (%x)", commander->listPosition, commander->listPosition);
    dbg ("charPosition = %d (%x)", commander->charPosition, commander->charPosition);
    dbg ("unk6 = %d (%x)", commander->unk6, commander->unk6);
    dbg ("mapId = %d (%x)", commander->mapId, commander->mapId);
    dbg ("currentXP = %d (%x)", commander->currentXP, commander->currentXP);
    dbg ("maxXP = %d (%x)", commander->maxXP, commander->maxXP);
    dbg ("unk7 = %d (%x)", commander->unk7, commander->unk7);
    dbg ("pose = %d (%x)", commander->pose, commander->pose);
    dbg ("spriteRelated = %d (%x)", commander->spriteRelated, commander->spriteRelated);
    dbg ("commanderId =%" PRIu64, commander->commanderId, commander->commanderId);
    dbg ("currentHP = %d (%x)", commander->currentHP, commander->currentHP);
    dbg ("maxHP = %d (%x)", commander->maxHP, commander->maxHP);
    dbg ("currentSP = %d (%x)", commander->currentSP, commander->currentSP);
    dbg ("maxSP = %d (%x)", commander->maxSP, commander->maxSP);
    dbg ("unk8 = %d (%x)", commander->unk8, commander->unk8);
    dbg ("unk9 = %d (%x)", commander->unk9, commander->unk9);
    dbg ("unk10 = %d (%x)", commander->unk10, commander->unk10);
    dbg ("unk11 = %d (%x)", commander->unk11, commander->unk11);
    dbg ("unk12 = %d (%x)", commander->unk12, commander->unk12);
}
