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

    commander->classId = 10005;
    commander->unk2 = 0;
    commander->jobId = 4;
    commander->gender = 2;
    commander->unk3 = 1;
    commander->level = 1;
    commander->head_top = 2;
    commander->head_middle = 2;
    commander->itemUnk1 = 4;
    commander->body_armor = 0x81a9d;
    commander->gloves = 6;
    commander->boots = 7;
    commander->itemUnk2 = 0x2710;
    commander->bracelet = 0x2af8;
    commander->weapon = 0x3118d;
    commander->shield = 0x98967c;
    commander->costume = 4;
    commander->itemUnk3 = 9;
    commander->itemUnk4 = 9;
    commander->itemUnk5 = 4;
    commander->leg_armor = 0x7f38d;
    commander->itemUnk6 = 9;
    commander->itemUnk7 = 9;
    commander->ring_left = 9;
    commander->ring_right = 9;
    commander->necklace = 0xa;
    commander->hairType = 51;
    commander->unk4 = 0;
    commander->unk5 = 0;
    commander->pcId = 0x604;
    commander->listPosition = 0x19f74;
    commander->charPosition = 1;
    commander->unk6 = 0;
    commander->mapId = 0x3fd;
    commander->spriteRelated = 0x41000000;
    commander->unk7 = 0;
    commander->pose = 0x1eefe0a0;
    commander->cPosX = 27.0;
    commander->cPosY = 29.0;
    commander->unk12 = 0;

    // Fill with dummy stats
    commander->currentXP = 6000;
    commander->maxXP = 12000;
    commander->currentHP = 6000;
    commander->maxHP = 12000;
    commander->currentSP = 6000;
    commander->maxSP = 12000;
    commander->level = 10;

    commander->unk10 = commander->currentHP;
    commander->unk11 = commander->maxHP;
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
    dbg ("commanderId = %" PRIu64, commander->commanderId, commander->commanderId);
    dbg ("currentHP = %f (%x)", commander->currentHP, commander->currentHP);
    dbg ("maxHP = %f (%x)", commander->maxHP, commander->maxHP);
    dbg ("currentSP = %f (%x)", commander->currentSP, commander->currentSP);
    dbg ("maxSP = %f (%x)", commander->maxSP, commander->maxSP);
    dbg ("cPosX = %f (%x)", commander->cPosX, commander->cPosX);
    dbg ("cPosY = %f (%x)", commander->cPosY, commander->cPosY);
    dbg ("unk10 = %d (%x)", commander->unk10, commander->unk10);
    dbg ("unk11 = %d (%x)", commander->unk11, commander->unk11);
    dbg ("unk12 = %d (%x)", commander->unk12, commander->unk12);
}
