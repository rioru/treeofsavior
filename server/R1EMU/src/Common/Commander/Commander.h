/**
 *
 *   ██████╗   ██╗ ███████╗ ███╗   ███╗ ██╗   ██╗
 *   ██╔══██╗ ███║ ██╔════╝ ████╗ ████║ ██║   ██║
 *   ██████╔╝ ╚██║ █████╗   ██╔████╔██║ ██║   ██║
 *   ██╔══██╗  ██║ ██╔══╝   ██║╚██╔╝██║ ██║   ██║
 *   ██║  ██║  ██║ ███████╗ ██║ ╚═╝ ██║ ╚██████╔╝
 *   ╚═╝  ╚═╝  ╚═╝ ╚══════╝ ╚═╝     ╚═╝  ╚═════╝
 *
 * @file Commander.h
 * @brief Commander contains all character related functions
 *
 * @license <license placeholder>
 */

#pragma once

// ---------- Includes ------------
#include "R1EMU.h"

// ---------- Defines -------------

// ------ Structure declaration -------

/**
 * @brief CommanderInfo is the struct of a commander.
 */
typedef struct CommanderInfo
{
    unsigned char charName[65];
    unsigned char familyName[65];
    unsigned char unk1[6];
    uint64_t accountId;
    uint16_t classId;
    uint16_t unk2;
    uint16_t jobId;
    uint8_t gender;
    unsigned char unk3;
    uint32_t level;
    uint32_t head_top;
    uint32_t head_middle;
    uint32_t itemUnk1;
    uint32_t body_armor;
    uint32_t gloves;
    uint32_t boots;
    uint32_t itemUnk2;
    uint32_t bracelet;
    uint32_t weapon;
    uint32_t shield;
    uint32_t costume;
    uint32_t itemUnk3;
    uint32_t itemUnk4;
    uint32_t itemUnk5;
    uint32_t leg_armor;
    uint32_t itemUnk6;
    uint32_t itemUnk7;
    uint32_t ring_left;
    uint32_t ring_right;
    uint32_t necklace;
    uint8_t hairType;
    unsigned char unk4;
    uint16_t unk5;
    uint32_t pcId;
    uint32_t unk13;
    uint8_t charPosition;
    unsigned char unk6;
    uint16_t mapId;
    uint32_t currentXP;
    uint32_t maxXP;
    uint32_t unk7;
    uint32_t pose;
    uint32_t spriteRelated;
    uint64_t commanderId;
    uint32_t currentHP;
    uint32_t maxHP;
    uint16_t currentSP;
    uint16_t maxSP;
    float cPosX;
    float cPosY;
    uint32_t unk10;
    uint32_t unk11;
    uint32_t unk12;
}   CommanderInfo;

typedef enum CommanderJobId
{
    COMMANDER_JOB_WARRIOR = 1,
    COMMANDER_JOB_ARCHER  = 2,
    COMMANDER_JOB_MAGE    = 3,
    COMMANDER_JOB_CLERIC  = 4

}   CommanderJobId;

typedef enum CommanderClassId
{
    COMMANDER_CLASS_WARRIOR = 10001,
    COMMANDER_CLASS_ARCHER  = 10006,
    COMMANDER_CLASS_MAGE    = 10003,
    COMMANDER_CLASS_CLERIC  = 10005

}   CommanderClassId;


typedef enum CommanderGender
{
    COMMANDER_GENDER_MALE   = 1,
    COMMANDER_GENDER_FEMALE = 2,
    COMMANDER_GENDER_BOTH   = 3,

}   CommanderGender;


typedef enum CommanderHair
{
    COMMANDER_HAIR_ID1  = 1, // http://puu.sh/gt8wV/4531dcbe29.jpg | http://puu.sh/gt8os/a75dfea0d8.jpg
    COMMANDER_HAIR_ID2  = 2, // http://puu.sh/gt8xE/9eaa03d9a4.jpg | http://puu.sh/gt8nO/88a720bd23.jpg
    COMMANDER_HAIR_ID3  = 3, // http://puu.sh/gt8y0/33a7e4275c.jpg | http://puu.sh/gt8nn/e22e910499.jpg
    COMMANDER_HAIR_ID4  = 4, // http://puu.sh/gt8yp/683d434b60.jpg | http://puu.sh/gt8pc/63b723ca4d.jpg
    COMMANDER_HAIR_ID5  = 5, // http://puu.sh/gt8A4/46531d2733.jpg | http://puu.sh/gt8pv/2b456a2625.jpg
    COMMANDER_HAIR_ID6  = 6, // http://puu.sh/gt8As/3de6bbe4be.jpg | http://puu.sh/gt8pU/f1ab1d90eb.jpg
    COMMANDER_HAIR_ID7  = 7, // http://puu.sh/gt8B3/8bde560d58.jpg | http://puu.sh/gt8qk/61f3b7baf0.jpg
    COMMANDER_HAIR_ID8  = 8, // http://puu.sh/gt8Bw/6fe882c20b.jpg | http://puu.sh/gt8rz/9f4f4dae1b.jpg
    COMMANDER_HAIR_ID9  = 9, // http://puu.sh/gt8BV/998b70d932.jpg | http://puu.sh/gt8sv/515b0aeff3.jpg
    COMMANDER_HAIR_ID10 = 10,// http://puu.sh/gt8Ck/387055e450.jpg | http://puu.sh/gt8tc/b5e0f89e90.jpg
}   CommanderHair;

// ----------- Functions ------------

/**
 * @brief Initialize a commander with basic information
 */

void
CommanderInfo_createBasicCommander (
    CommanderInfo *commander
);

/**
 * @brief Dump a CommanderInfo in the console
 */
void
CommanderInfo_print (
    CommanderInfo *commander
);
