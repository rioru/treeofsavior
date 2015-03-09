/**
 *
 *   ██████╗   ██╗ ███████╗ ███╗   ███╗ ██╗   ██╗
 *   ██╔══██╗ ███║ ██╔════╝ ████╗ ████║ ██║   ██║
 *   ██████╔╝ ╚██║ █████╗   ██╔████╔██║ ██║   ██║
 *   ██╔══██╗  ██║ ██╔══╝   ██║╚██╔╝██║ ██║   ██║
 *   ██║  ██║  ██║ ███████╗ ██║ ╚═╝ ██║ ╚██████╔╝
 *   ╚═╝  ╚═╝  ╚═╝ ╚══════╝ ╚═╝     ╚═╝  ╚═════╝
 *
 * @file PacketType.h
 * @brief PacketType enumerates all the packets used in the game between the client and the server
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
    uint32_t PCID;
    uint32_t listPosition;
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
    uint32_t unk8;
    uint32_t unk9;
    uint32_t unk10;
    uint32_t unk11;
    uint32_t unk12;
}   CommanderInfo;

typedef enum COMMANDER_JOB_ID
{
    COMMANDER_JOB_WARRIOR = 1,
    COMMANDER_JOB_ARCHER  = 2,
    COMMANDER_JOB_MAGE    = 3,
    COMMANDER_JOB_CLERIC  = 4

}   COMMANDER_JOB_ID;

typedef enum COMMANDER_CLASS_ID
{
    COMMANDER_CLASS_WARRIOR = 10001,
    COMMANDER_CLASS_ARCHER  = 10006,
    COMMANDER_CLASS_MAGE    = 10003,
    COMMANDER_CLASS_CLERIC  = 10005

}   COMMANDER_JOB_ID;

// ----------- Functions ------------

CommanderInfo
Commander_CreateBasicCommander (
    void
);
