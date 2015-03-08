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
#define PACKET_TYPES_MAX_INDEX      (SI_FROM_GLOBAL + 1)


// ------ Structure declaration -------

/**
 * @brief PacketType enumerates all the packets used in the game between the client and the server
 */
typedef enum PacketType
{
	/** ===================================================================== */
	/** ======================== Barrack -> Client  ========================= */
	/** ===================================================================== */

	/** ========== Account ========== */
	/** Description : UNKNOWN */
	BC_ACCOUNT_PROP = 0x004D,                    // Size: 0
	/** Description : Change the family name */
	BC_BARRACKNAME_CHANGE = 0x0017,              // Size: 75
	/** Description : Login and logout the account */
	BC_LOGINOK = 0x000D,                         // Size: 37
	BC_LOGOUTOK = 0x000E,                        // Size: 6

	/** ========== Commander ========== */
	/** Description : Create a new commander */
	BC_COMMANDER_CREATE = 0x0010,                // Size: 318
	/** Description : Delete one or all commanders of the barrack */
	BC_COMMANDER_DESTROY = 0x0011,               // Size: 7
	/** Description : List all the existing commanders */
	BC_COMMANDER_LIST = 0x000F,                  // Size: 0
	/** Description : Makes the commander jump */
	BC_JUMP = 0x0056,                            // Size: 19

	/** ========== IES ========== */
	/** Description : UNKNOWN */
	BC_IES_MODIFY_INFO = 0x0047,                 // Size: 0
	BC_IES_MODIFY_LIST = 0x0048,                 // Size: 0
	BC_IES_REVISION_DELETE = 0x004A,             // Size: 0

	/** ========== Chat ========== */
	/** Description : Communicate in the barrack */
	BC_CHAT = 0x0053,                            // Size: 0

	/** ========== Server ========== */
	/** Description : Describes an entry of a social server */
	BC_SERVER_ENTRY = 0x0057,                    // Size: 18
	/** Description : Describes the information needed for a single map instance */
	BC_SINGLE_INFO = 0x0013,                     // Size: 337
	/** Description : Start the loading screen */
	BC_START_GAMEOK = 0x0012,                    // Size: 60

	/** ========== UNKNOWN ========== */
	BC_NORMAL = 0x004F,                          // Size: 0
	BC_ECHO = 0x0015,                            // Size: 26
	BC_MYPAGE_MAP = 0x0016,                      // Size: 0
	BC_MESSAGE = 0x0014,                         // Size: 0


	/** ===================================================================== */
	/** ========================== Client -> Game =========================== */
	/** ===================================================================== */

	/** ========== Status ========== */
	/** Description : UNKNOWN */
	BG_ALIVE_PING = 0x0389,                      // Size: 8

	/** ========== Commander ========== */
	/** Description : UNKNOWN */
	BG_COMMANDER_DESTROY = 0x038B,               // Size: 83
	BG_BAN_COMMANDER = 0x038D,                   // Size: 14

	/** ========== Zone ========== */
	/** Description : UNKNOWN */
	BG_CREATE_ZONE = 0x0393,                     // Size: 8
	BG_ENTER_ZONE = 0x038A,                      // Size: 196

	/** ========== IES ========== */
	/** Description : UNKNOWN */
	BG_IES_HIS_DEL = 0x0391,                     // Size: 0
	BG_IES_MODIFY_INFO = 0x038F,                 // Size: 0

	/** ========== UNKNOWN ========== */
	BG_NORMAL = 0x0398,                          // Size: 0
	BG_START = 0x0385,                           // Size: 8


	/** ===================================================================== */
	/** ========================= Client -> Barrack ========================= */
	/** ===================================================================== */

	/** ========== Account ========== */
	/** Description : Request a login by login / password */
	CB_LOGIN = 0x0003,                           // Size: 53
	/** Description : Request a login by a nexon passport */
	CB_LOGIN_BY_PASSPORT = 0x0004,               // Size: 531
	/** Description : Request a logout */
	CB_LOGOUT = 0x0005,                          // Size: 10
	/** Description : Request the information about the barrack */
	CB_START_BARRACK = 0x0006,                   // Size: 11
	/** Description : Request a family name change */
	CB_BARRACKNAME_CHANGE = 0x000A,              // Size: 74
	/** Description : Sends information about the current barrack */
	CB_CURRENT_BARRACK = 0x004E,                 // Size: 39
	/** Description : Request to buy a new barrack */
	CB_BUY_THEMA = 0x004C,                       // Size: 18

	/** ========== Commander ========== */
	/** Description : Requests to create, destroy, move, jump or pose a commander */
	CB_COMMANDER_CREATE = 0x0007,                // Size: 91
	CB_COMMANDER_DESTROY = 0x0008,               // Size: 11
	CB_COMMANDER_MOVE = 0x000B,                  // Size: 31
	CB_JUMP = 0x0055,                            // Size: 19
	CB_POSE = 0x0050,                            // Size: 15
	/** Description : Request to delete a pet */
	CB_DELETE_PET = 0x0059,                      // Size: 18
	/** Description : UNKNOWN */
	CB_PET_PC = 0x0058,                          // Size: 26

	/** ========== Server ========== */
	/** Description : Sends when the button is pressed, requests the loading screen */
	CB_START_GAME = 0x0009,                      // Size: 13

	/** ========== IES ========== */
	/** Description : UNKNOWN */
	CB_IES_MODIFY_INFO = 0x0046,                 // Size: 0
	CB_IES_REVISION_DELETE = 0x0049,             // Size: 0

	/** ========== Visit ========== */
	/** Description : Visits someone else's barrack */
	CB_VISIT = 0x004B,                           // Size: 74

	/** ========== UNKNOWN ========== */
	CB_ECHO = 0x000C,                            // Size: 30
	CB_ECHO_NORMAL = 0x0054,                     // Size: 0
	CB_PLACE_CMD = 0x0051,                       // Size: 46
	CB_CHAT = 0x0052,                            // Size: 0

	/** ===================================================================== */
	/** ========================= Client -> Social ========================== */
	/** ===================================================================== */

	/** ========== Server ========== */
	CS_LOGIN = 0x0B55,                           // Size: 48
	CS_NORMAL_GAME_START = 0x0B6C,               // Size: 6

	/** ========== Chat ========== */
	CS_CHAT = 0x0B60,                            // Size: 0
	CS_CHAT_HISTORY = 0x0B64,                    // Size: 22
	CS_CHAT_READ = 0x0B65,                       // Size: 30

	/** ========== GroupChat ========== */
	CS_CREATE_GROUP_CHAT = 0x0B61,               // Size: 6
	CS_GROUP_CHAT_INVITE = 0x0B62,               // Size: 78
	CS_REFRESH_GROUP_CHAT = 0x0B63,              // Size: 6
	CS_REMOVE_GROUP_MEMBER = 0x0B66,             // Size: 14

	/** ========== Friend  ========== */
	CS_FRIEND_CMD = 0x0B5E,                      // Size: 24
	CS_FRIEND_SET_ADDINFO = 0x0B5F,              // Size: 160
	CS_REQ_ADD_FRIEND = 0x0B5C,                  // Size: 70
	CS_REQ_BLOCK_FRIEND = 0x0B5D,                // Size: 70

	/** ========== Market ========== */
	CS_REQ_MARKET_LIST = 0x0B5A,                 // Size: 158
	CS_REQ_MY_SELL_LIST = 0x0B5B,                // Size: 10

	/** ========== PC ==========	 */
	CS_PC_INTERACTION = 0x0B67,                  // Size: 24
	CS_PC_INTERACTION_HISTORY = 0x0B69,          // Size: 32
	CS_REQ_RELATED_PC_SESSION = 0x0B68,          // Size: 16
	CS_REDIS_SKILLPOINT = 0x0B6A,                // Size: 22

	/** ========== Party ========== */
	CS_PARTY_CLIENT_INFO_SEND = 0x0B6B,          // Size: 0

	/** ========== PVP ========== */
	CS_REQUEST_PVP_RANKING = 0x0B6E,             // Size: 83
	CS_PVP_COMMAND = 0x0B6D,                     // Size: 18

	/** =========== SNS =========== */
	CS_REGISTER_SNS_ID = 0x0B58,                 // Size: 16
	CS_REQ_SNS_PC_INFO = 0x0B59,                 // Size: 0


	/** ===================================================================== */
	/** ========================== Client -> Zone =========================== */
	/** ===================================================================== */

	/** =========== Party =========== */
	CZ_ACCEPT_PARTY_QUEST = 0x0D8B,              // Size: 20
	CZ_REQUEST_SOME_PARTY = 0x0D84,              // Size: 34

	/** =========== Barrack =========== */
	CZ_VISIT_BARRACK = 0x0D4B,                   // Size: 74

	/** =========== Equipment =========== */
	CZ_ACHIEVE_EQUIP = 0x0CB6,                   // Size: 18

	/** =========== Skills =========== */
	CZ_ACTIVE_ABILITY = 0x0C35,                  // Size: 15
	CZ_BUFF_REMOVE = 0x0BE7,                     // Size: 14
	CZ_CANCEL_TRANSFORM_SKILL = 0x0D1B,          // Size: 10
	CZ_CAST_CONTROL_SHOT = 0x0CDC,               // Size: 10
	CZ_SKILL_CANCEL = 0x0C16,                    // Size: 11
	CZ_SKILL_CANCEL_SCRIPT = 0x0CB1,             // Size: 10
	CZ_SKILL_CELL_LIST = 0x0D5A,                 // Size: 0
	CZ_SKILL_GROUND = 0x0C14,                    // Size: 56
	CZ_SKILL_JUMP_REQ = 0x0C0B,                  // Size: 30
	CZ_SKILL_SELF = 0x0C15,                      // Size: 35
	CZ_SKILL_TARGET = 0x0C12,                    // Size: 19
	CZ_SKILL_TARGET_ANI = 0x0C13,                // Size: 23
	CZ_SKILL_TOOL_GROUND_POS = 0x0D5B,           // Size: 22
	CZ_SKILL_USE_HEIGHT = 0x0D8A,                // Size: 14
	CZ_HARDCODED_SKILL = 0x0D15,                 // Size: 34
	CZ_SPC_SKILL_POS = 0x0D4C,                   // Size: 22
	CZ_SUMMON_COMMAND = 0x0D6D,                  // Size: 18

	/** =========== UI =========== */
	CZ_ADD_HELP = 0x0D55,                        // Size: 14
	CZ_CLICK_TRIGGER = 0x0C07,                   // Size: 15
	CZ_UI_EVENT = 0x0C50,                        // Size: 0

	/** =========== Economy =========== */
	CZ_ADD_SELLMODE_ITEM = 0x0D32,               // Size: 30
	CZ_DELETE_SELLMODE_ITEM = 0x0D33,            // Size: 18
	CZ_BUY_PROPERTYSHOP_ITEM = 0x0D89,           // Size: 0
	CZ_COMMON_SHOP_LIST = 0x0C78,                // Size: 10
	CZ_EXCHANGE_ACCEPT = 0x0C67,                 // Size: 10
	CZ_EXCHANGE_AGREE = 0x0C6D,                  // Size: 10
	CZ_EXCHANGE_CANCEL = 0x0C71,                 // Size: 10
	CZ_EXCHANGE_DECLINE = 0x0C68,                // Size: 10
	CZ_EXCHANGE_FINALAGREE = 0x0C6F,             // Size: 10
	CZ_EXCHANGE_OFFER = 0x0C6B,                  // Size: 30
	CZ_EXCHANGE_REQUEST = 0x0C64,                // Size: 14
	CZ_ON_ITEMBUY_MODE = 0x0D39,                 // Size: 0
	CZ_ON_SELLITEM_MODE = 0x0D34,                // Size: 14
	CZ_OPEN_AUTOSELLER = 0x0D80,                 // Size: 20
	CZ_REGISTER_AUTOSELLER = 0x0D7F,             // Size: 0
	CZ_SOLD_ITEM = 0x0CDA,                       // Size: 19
	CZ_REQ_MARKET_BUY = 0x0D79,                  // Size: 0
	CZ_REQ_MARKET_REGISTER = 0x0D78,             // Size: 27
	CZ_REQ_CANCEL_MARKET_ITEM = 0x0D7C,          // Size: 18
	CZ_NPC_AUCTION_CMD = 0x0D67,                 // Size: 30

	/** =========== Server =========== */
	CZ_BACKTO_ORIGINAL_SERVER = 0x0C8F,          // Size: 12
	CZ_CONNECT = 0x0BB9,                         // Size: 54
	CZ_COMPLETE_PRELOAD = 0x0D53,                // Size: 14
	CZ_GAME_READY = 0x0BFD,                      // Size: 10
	CZ_LOGOUT = 0x0BFF,                          // Size: 10

	/** =========== Channel =========== */
	CZ_CHANGE_CHANNEL = 0x0D87,                  // Size: 11
	CZ_REQ_CHANNEL_TRAFFICS = 0x0D88,            // Size: 12

	/** =========== Configs =========== */
	CZ_CHANGE_CONFIG = 0x0CB8,                   // Size: 18
	CZ_CHANGE_CONFIG_STR = 0x0CB9,               // Size: 34

	/** =========== Commander =========== */
	CZ_CHANGE_HEAD = 0x0D4E,                     // Size: 14
	CZ_HEAD_ROTATE = 0x0C19,                     // Size: 18
	CZ_JUMP = 0x0C0A,                            // Size: 11
	CZ_POSE = 0x0C1B,                            // Size: 34
	CZ_REST_SIT = 0x0C0E,                        // Size: 10
	CZ_RESURRECT = 0x0BFA,                       // Size: 11
	CZ_VEHICLE_RIDE = 0x0D6E,                    // Size: 15
	CZ_ROTATE = 0x0C18,                          // Size: 18
	CZ_REQ_CHANGEJOB = 0x0D4D,                   // Size: 14

	/** =========== Battle =========== */
	CZ_CLIENT_ATTACK = 0x0CDF,                   // Size: 15
	CZ_CLIENT_DAMAGE = 0x0CDE,                   // Size: 14
	CZ_CLIENT_HIT_LIST = 0x0D3E,                 // Size: 0
	CZ_COUNTER_ATTACK = 0x0D1E,                  // Size: 14
	CZ_DYNAMIC_CASTING_END = 0x0CB0,             // Size: 19
	CZ_DYNAMIC_CASTING_START = 0x0CAF,           // Size: 23
	CZ_EXP_UP_BY_MONSTER = 0x0C3A,               // Size: 18
	CZ_GUARD = 0x0C9F,                           // Size: 19

	/** =========== Crafting  =========== */
	CZ_CREATE_ARROW_CRAFT = 0x0D4F,              // Size: 14
	CZ_PUZZLE_CRAFT = 0x0D81,                    // Size: 0
	CZ_REQ_RECIPE = 0x0C84,                      // Size: 0

	/** =========== Chat =========== */
	CZ_CHAT = 0x0C1C,                            // Size: 0
	CZ_CHAT_LOG = 0x0C1D,                        // Size: 0
	CZ_CHAT_MACRO = 0x0CC2,                      // Size: 146
	CZ_SHOUT = 0x0C1E,                           // Size: 0

	/** =========== Commands =========== */
	CZ_CUSTOM_COMMAND = 0x0CC0,                  // Size: 22
	CZ_CUSTOM_SCP = 0x0D12,                      // Size: 14
	CZ_FORMATION_CMD = 0x0D7E,                   // Size: 24

	/** =========== Direction =========== */
	CZ_DIRECTION_MOVE_STATE = 0x0D5D,            // Size: 0
	CZ_DIRECTION_PROCESS = 0x0D5C,               // Size: 18

	/** =========== Dialog =========== */
	CZ_DIALOG_ACK = 0x0C42,                      // Size: 14
	CZ_DIALOG_SELECT = 0x0C43,                   // Size: 11
	CZ_DIALOG_STRINGINPUT = 0x0C44,              // Size: 138
	CZ_DIALOG_TX = 0x0C83,                       // Size: 0

	/** =========== Wiki =========== */
	CZ_GET_WIKI_REWARD = 0x0CF0,                 // Size: 11
	CZ_REQ_WIKI_CATEGORY_RANK_PAGE_INFO = 0x0D6F,// Size: 78
	CZ_REQ_WIKI_PROP_RANK = 0x0CEF,              // Size: 16
	CZ_REQ_WIKI_RANK = 0x0CEC,                   // Size: 15
	CZ_WIKI_GET = 0x0C93,                        // Size: 14
	CZ_WIKI_RECIPE_UPDATE = 0x0C94,              // Size: 10

	/** =========== HackShield =========== */
	CZ_HACKSHIELD_BUFFER = 0x0D8F,               // Size: 612

	/** =========== Item =========== */
	CZ_GIVEITEM_TO_DUMMYPC = 0x0CA9,             // Size: 22
	CZ_HARDCODED_ITEM = 0x0D1A,                  // Size: 22
	CZ_ITEM_BUY = 0x0C24,                        // Size: 0
	CZ_ITEM_DELETE = 0x0C20,                     // Size: 0
	CZ_ITEM_DROP = 0x0C1F,                       // Size: 22
	CZ_ITEM_DROP_TO_OBJECT = 0x0D26,             // Size: 26
	CZ_ITEM_EQUIP = 0x0C26,                      // Size: 19
	CZ_ITEM_SELL = 0x0C25,                       // Size: 0
	CZ_ITEM_UNEQUIP = 0x0C27,                    // Size: 11
	CZ_ITEM_USE = 0x0C21,                        // Size: 22
	CZ_ITEM_USE_TO_GROUND = 0x0C23,              // Size: 30
	CZ_ITEM_USE_TO_ITEM = 0x0C22,                // Size: 306
	CZ_REQ_ITEM_GET = 0x0C9D,                    // Size: 1

	/** =========== IES =========== */
	CZ_IES_MODIFY_INFO = 0x0CD3,                 // Size: 0
	CZ_IES_REVISION_DELETE = 0x0CD6,             // Size: 0

	/** =========== Keyboard =========== */
	CZ_KEYBOARD_BEAT = 0x0D43,                   // Size: 10
	CZ_KEYBOARD_MOVE = 0x0C08,                   // Size: 41

	/** =========== Party =========== */
	CZ_PARTY_INVITE_ACCEPT = 0x0D75,             // Size: 75
	CZ_PARTY_INVITE_CANCEL = 0x0D76,             // Size: 75
	CZ_PARTY_PROP_CHANGE = 0x0D77,               // Size: 17

	/** =========== Map =========== */
	CZ_GET_MAP_REVEAL_ACHIEVE = 0x0CD2,          // Size: 10
	CZ_MAP_REVEAL_INFO = 0x0CCA,                 // Size: 146
	CZ_MAP_SEARCH_INFO = 0x0CCB,                 // Size: 55

	/** =========== Page =========== */
	CZ_GET_TARGET_GUESTPAGE = 0x0D31,            // Size: 14
	CZ_GUESTPAGE_COMMENT_ADD = 0x0D2D,           // Size: 278
	CZ_GET_TARGET_MYPAGE = 0x0D2E,               // Size: 14
	CZ_MYPAGE_COMMENT_ADD = 0x0D2B,              // Size: 278
	CZ_MYPAGE_COMMENT_DELETE = 0x0D2C,           // Size: 18
	CZ_ON_MYPAGE_MODE = 0x0D2F,                  // Size: 14

	/** =========== Movements =========== */
	CZ_EXPECTED_STOP_POS = 0x0C09,               // Size: 31
	CZ_MOVEMENT_INFO = 0x0C11,                   // Size: 23
	CZ_MOVE_BARRACK = 0x0C00,                    // Size: 10
	CZ_MOVE_PATH = 0x0C0C,                       // Size: 27
	CZ_MOVE_PATH_END = 0x0D41,                   // Size: 10
	CZ_MOVE_STOP = 0x0C0D,                       // Size: 35
	CZ_MOVE_ZONE_OK = 0x0BBC,                    // Size: 10
	CZ_ON_AIR = 0x0C0F,                          // Size: 10
	CZ_ON_GROUND = 0x0C10,                       // Size: 10

	/** =========== Object =========== */
	CZ_CONTROL_OBJECT_ROTATE = 0x0D6C,           // Size: 22
	CZ_OBJECT_MOVE = 0x0D6B,                     // Size: 27
	CZ_OBJ_RECORD_POS = 0x0D7D,                  // Size: 0
	CZ_S_OBJ_VALUE_C = 0x0C8A,                   // Size: 24

	/** =========== Quickslot =========== */
	CZ_QUICKSLOT_LIST = 0x0CC6,                  // Size: 0

	/** =========== Quests =========== */
	CZ_G_QUEST_CHECK = 0x0D28,                   // Size: 14
	CZ_QUEST_CHECK_SAVE = 0x0CE2,                // Size: 50
	CZ_QUEST_NPC_STATE_CHECK = 0x0CD0,           // Size: 14

	/** =========== Spray =========== */
	CZ_SPRAY_DRAW_INFO = 0x0CE4,                 // Size: 0
	CZ_SPRAY_REQ_INFO = 0x0CE3,                  // Size: 14

	/** =========== MGame =========== */
	CZ_MGAME_JOIN_CMD = 0x0D54,                  // Size: 46
	CZ_REQ_MGAME_CHAT = 0x0D73,                  // Size: 0
	CZ_REQ_MGAME_VIEW = 0x0D72,                  // Size: 18

	/** =========== Social =========== */
	CZ_RESET_SOCIAL_MODE = 0x0D30,               // Size: 10

	/** =========== Pet =========== */
	CZ_PET_EQUIP = 0x0D82,                       // Size: 30

	/** =========== UNKNOWN & MISC =========== */
	CZ_CLIENT_DIRECT = 0x0D1F,                   // Size: 30
	CZ_FLEE_OBSTACLE = 0x0CF9,                   // Size: 26
	CZ_HELP_READ_TYPE = 0x0D40,                  // Size: 18
	CZ_HOLD = 0x0C17,                            // Size: 11
	CZ_INTE_WARP = 0x0BE8,                       // Size: 14
	CZ_JUNGTAN_TOGGLE = 0x0C37,                  // Size: 16
	CZ_LOG = 0x0D0A,                             // Size: 0
	CZ_MOVEHIT_SCP = 0x0D44,                     // Size: 22
	CZ_PING = 0x0D8C,                            // Size: 10
	CZ_PROPERTY_COMPARE = 0x0D61,                // Size: 15
	CZ_REQ_ACHIEVE_RANK_PAGE_INFO = 0x0D70,      // Size: 78
	CZ_REQ_CABINET_LIST = 0x0D7A,                // Size: 18
	CZ_REQ_GET_CABINET_ITEM = 0x0D7B,            // Size: 20
	CZ_REQ_DUMMYPC_INFO = 0x0D4A,                // Size: 14
	CZ_REQ_MINITEXT = 0x0D50,                    // Size: 266
	CZ_REQ_NORMAL_TX = 0x0C8B,                   // Size: 29
	CZ_REQ_NORMAL_TX_NUMARG = 0x0C90,            // Size: 0
	CZ_REQ_TX_ITEM = 0x0CF1,                     // Size: 0
	CZ_REVEAL_NPC_STATE = 0x0D86,                // Size: 14
	CZ_REWARD_CMD = 0x0D60,                      // Size: 14
	CZ_RUN_BGEVENT = 0x0D23,                     // Size: 74
	CZ_SAVE_INFO = 0x0CA6,                       // Size: 0
	CZ_STOP_ALLPC = 0x0D52,                      // Size: 10
	CZ_STOP_TIMEACTION = 0x0D49,                 // Size: 10
	CZ_SWAP_INVITEM = 0x0C28,                    // Size: 18
	CZ_TOURNAMENT_GIFT = 0x0D74,                 // Size: 18
	CZ_WAREHOUSE_CMD = 0x0CDB,                   // Size: 27


	/** ===================================================================== */
	/** ========================== Zone -> Client =========================== */
	/** ===================================================================== */

	/** =========== Skills =========== */
	ZC_ABILITY_LIST = 0x0C34,                    // Size: 0
	ZC_ACHIEVE_POINT = 0x0CB5,                   // Size: 18
	ZC_ACHIEVE_POINT_LIST = 0x0CB4,              // Size: 0
	ZC_ACTIVE_ABILITY = 0x0C36,                  // Size: 11
	ZC_ADD_SKILL_EFFECT = 0x0D24,                // Size: 18
	ZC_BUFF_ADD = 0x0BE3,                        // Size: 0
	ZC_BUFF_CLEAR = 0x0BE6,                      // Size: 11
	ZC_BUFF_LIST = 0x0BE2,                       // Size: 0
	ZC_BUFF_REMOVE = 0x0BE5,                     // Size: 12
	ZC_BUFF_UPDATE = 0x0BE4,                     // Size: 0
	ZC_CAST_TARGET = 0x0D0D,                     // Size: 14
	ZC_COOLDOWN_CHANGED = 0x0C75,                // Size: 22
	ZC_COOLDOWN_LIST = 0x0C74,                   // Size: 0
	ZC_COOLDOWN_RATE = 0x0CC8,                   // Size: 22
	ZC_SKILLMAP_LIST = 0x0CA8,                   // Size: 0
	ZC_SKILL_ADD = 0x0C33,                       // Size: 0
	ZC_SKILL_CAST = 0x0BD9,                      // Size: 38
	ZC_SKILL_CAST_CANCEL = 0x0BD8,               // Size: 10
	ZC_SKILL_DISABLE = 0x0BDB,                   // Size: 15
	ZC_SKILL_FORCE_GROUND = 0x0BE0,              // Size: 0
	ZC_SKILL_FORCE_TARGET = 0x0BDF,              // Size: 0
	ZC_SKILL_HIT_INFO = 0x0BE1,                  // Size: 0
	ZC_ORDER_SKILL_JUMP = 0x0BCE,                // Size: 10
	ZC_SKILL_JUMP = 0x0BCF,                      // Size: 38
	ZC_SKILL_LIST = 0x0C32,                      // Size: 0
	ZC_SKILL_MELEE_GROUND = 0x0BDE,              // Size: 0
	ZC_SKILL_MELEE_TARGET = 0x0BDD,              // Size: 0
	ZC_SKILL_RANGE_CIRCLE = 0x0D07,              // Size: 28
	ZC_SKILL_RANGE_DBG = 0x0D04,                 // Size: 58
	ZC_SKILL_RANGE_FAN = 0x0D05,                 // Size: 40
	ZC_SKILL_RANGE_SQUARE = 0x0D06,              // Size: 40
	ZC_SKILL_READY = 0x0BDA,                     // Size: 46
	ZC_SKILL_USE_CANCEL = 0x0BDC,                // Size: 10
	ZC_HEAL_INFO = 0x0BF4,                       // Size: 30
	ZC_HARDCODED_SKILL = 0x0D14,                 // Size: 26


	/** =========== Equipment =========== */
	ZC_ACHIEVE_EQUIP = 0x0CB7,                   // Size: 22
	ZC_CHANGE_EQUIP_DURABILITY = 0x0C82,         // Size: 11
	ZC_EQUIP_ITEM_REMOVE = 0x0CD8,               // Size: 18

	/** =========== PKS =========== */
	ZC_ACTION_PKS = 0x0D19,                      // Size: 35

	/** =========== Addon =========== */
	ZC_ADDON_MSG = 0x0C4F,                       // Size: 0

	/** =========== Actor =========== */
	ZC_ADD_HP = 0x0BD7,                          // Size: 18
	ZC_ADD_STAMINA = 0x0CA2,                     // Size: 10
	ZC_BORN = 0x0CB3,                            // Size: 10
	ZC_CANCEL_DEADEVENT = 0x0D18,                // Size: 10
	ZC_ENTER_DUMMYPC = 0x0BC0,                   // Size: 292
	ZC_ENTER_PC = 0x0BBE,                        // Size: 316
	ZC_FLY = 0x0CFF,                             // Size: 18
	ZC_FLY_MATH = 0x0D00,                        // Size: 22
	ZC_GUARD = 0x0CA0,                           // Size: 19
	/** -- Set the position of the actor-- */
	ZC_SET_POS = 0x0BD0,                         // Size: 22
	/** --  */
	ZC_HEAD_ROTATE = 0x0BEB,                     // Size: 18
	ZC_STANCE_CHANGE = 0x0BD6,                   // Size: 14
	ZC_ROTATE = 0x0BE9,                          // Size: 19
	ZC_ROTATE_RESERVED = 0x0BEA,                 // Size: 18
	ZC_QUICK_ROTATE = 0x0BED,                    // Size: 18

	/** =========== Monster =========== */
	ZC_MONSTER_DIST = 0x0CF3,                    // Size: 0
	ZC_MONSTER_LIFETIME = 0x0CE7,                // Size: 14
	ZC_MONSTER_PROPERTY = 0x0CFD,                // Size: 0
	ZC_MONSTER_SDR_CHANGED = 0x0C98,             // Size: 11
	ZC_MON_STAMINA = 0x0D11,                     // Size: 22
	ZC_UPDATED_MONSTERAPPEARANCE = 0x0BD3,       // Size: 0
	ZC_ENTER_MONSTER = 0x0BBF,                   // Size: 0

	/** =========== Commander =========== */
	ZC_COMMANDER_LOADER_INFO = 0x0C8C,           // Size: 0
	/** -- Dead -- */
	ZC_DEAD = 0x0BEF,                            // Size: 0
	ZC_RESURRECT = 0x0BF0,                       // Size: 14
	ZC_RESURRECT_DIALOG = 0x0BF2,                // Size: 7
	ZC_RESURRECT_HERE_ACK = 0x0BFC,              // Size: 7
	ZC_RESURRECT_SAVE_POINT_ACK = 0x0BFB,        // Size: 7
	/** -- Exp -- */
	ZC_EXP_UP = 0x0C38,                          // Size: 14
	ZC_EXP_UP_BY_MONSTER = 0x0C39,               // Size: 18
	ZC_GIVE_EXP_TO_PC = 0x0CBD,                  // Size: 42
	ZC_JOB_EXP_UP = 0x0D0F,                      // Size: 10
	ZC_MAX_EXP_CHANGED = 0x0C3D,                 // Size: 18
	ZC_PC_LEVELUP = 0x0C3B,                      // Size: 11
	/** -- */
	ZC_JOB_PTS = 0x0D10,                         // Size: 10
	ZC_PC = 0x0D09,                              // Size: 0
	ZC_PC_ATKSTATE = 0x0D3F,                     // Size: 11
	ZC_PC_PROP_UPDATE = 0x0CDD,                  // Size: 9
	ZC_PC_STAT_AVG = 0x0C3C,                     // Size: 30
	ZC_UPDATED_DUMMYPC = 0x0BC1,                 // Size: 250
	ZC_UPDATED_PCAPPEARANCE = 0x0BD2,            // Size: 250
	ZC_UPDATE_ALL_STATUS = 0x0C5F,               // Size: 22
	ZC_UPDATE_MHP = 0x0C41,                      // Size: 14
	ZC_UPDATE_MSHIELD = 0x0D02,                  // Size: 12
	ZC_UPDATE_SHIELD = 0x0D01,                   // Size: 12
	ZC_UPDATE_SP = 0x0C3F,                       // Size: 15
	/** -- Pose */
	ZC_REST_SIT = 0x0BCB,                        // Size: 11
	ZC_POSE = 0x0BEE,                            // Size: 34
	ZC_MYPC_ENTER = 0x0CA4,                      // Size: 18
	ZC_STAMINA = 0x0CA1,                         // Size: 10

	/** =========== NPC =========== */
	ZC_NPC_STATE_LIST = 0x0CCF,                  // Size: 0
	ZC_SET_NPC_STATE = 0x0CCE,                   // Size: 18

	/** =========== Attach / Detach =========== */
	ZC_ATTACH_BY_KNOCKBACK = 0x0D6A,             // Size: 38
	ZC_ATTACH_TO_OBJ = 0x0D56,                   // Size: 48
	ZC_DETACH_FROM_OBJ = 0x0D57,                 // Size: 14

	/** =========== Server relation =========== */
	ZC_BACKTO_ORIGINAL_SERVER = 0x0C8E,          // Size: 8
	ZC_CONNECT_FAILED = 0x0BBD,                  // Size: 0
	ZC_CONNECT_OK = 0x0BBA,                      // Size: 0
	ZC_LOGOUT_OK = 0x0C02,                       // Size: 6
	ZC_START_GAME = 0x0C05,                      // Size: 26
	ZC_START_INFO = 0x0D0E,                      // Size: 0
	ZC_TIME_FACTOR = 0x0C7A,                     // Size: 10
	ZC_LOGIN_TIME = 0x0CBC,                      // Size: 14

	/** =========== Battle =========== */
	ZC_CAUTION_DAMAGE_INFO = 0x0BF6,             // Size: 15
	ZC_CAUTION_DAMAGE_RELEASE = 0x0BF7,          // Size: 10
	ZC_HIT_INFO = 0x0BF3,                        // Size: 60

	/** =========== Log =========== */
	ZC_BEGIN_KILL_LOG = 0x0D64,                  // Size: 6
	ZC_END_KILL_LOG = 0x0D65,                    // Size: 6
	ZC_CLEAR_KILL_LOG = 0x0D66,                  // Size: 6

	/** =========== Animation =========== */
	ZC_CHANGE_ANI = 0x0C56,                      // Size: 44
	ZC_STD_ANIM = 0x0C59,                        // Size: 11
	ZC_FIX_ANIM = 0x0C57,                        // Size: 14
	ZC_MOVE_ANIM = 0x0C58,                       // Size: 12
	ZC_BGMODEL_ANIM_INFO = 0x0D69,               // Size: 15

	/** =========== Camera =========== */
	ZC_CHANGE_CAMERA = 0x0C97,                   // Size: 31
	ZC_CHANGE_CAMERA_ZOOM = 0x0C9A,              // Size: 34
	ZC_RESET_VIEW = 0x0C04,                      // Size: 6
	ZC_VIEW_FOCUS = 0x0D13,                      // Size: 24

	/** =========== Relation =========== */
	ZC_CHANGE_RELATION = 0x0BF1,                 // Size: 11

	/** =========== Collision damage =========== */
	ZC_COLL_DAMAGE = 0x0D42,                     // Size: 11

	/** =========== Economy =========== */
	ZC_COMMON_SHOP_LIST = 0x0C79,                // Size: 8
	ZC_EXCHANGE_AGREE_ACK = 0x0C6E,              // Size: 7
	ZC_EXCHANGE_CANCEL_ACK = 0x0C72,             // Size: 6
	ZC_EXCHANGE_DECLINE_ACK = 0x0C69,            // Size: 6
	ZC_EXCHANGE_FINALAGREE_ACK = 0x0C70,         // Size: 7
	ZC_EXCHANGE_OFFER_ACK = 0x0C6C,              // Size: 0
	ZC_EXCHANGE_REQUEST_ACK = 0x0C65,            // Size: 72
	ZC_EXCHANGE_REQUEST_RECEIVED = 0x0C66,       // Size: 71
	ZC_EXCHANGE_START = 0x0C6A,                  // Size: 71
	ZC_EXCHANGE_SUCCESS = 0x0C73,                // Size: 6
	ZC_SOLD_ITEM_LIST = 0x0CD9,                  // Size: 0

	/** =========== UI =========== */
	ZC_CREATE_LAYERBOX = 0x0CAB,                 // Size: 38
	ZC_CREATE_SCROLLLOCKBOX = 0x0CAD,            // Size: 38
	ZC_CUSTOM_DIALOG = 0x0C85,                   // Size: 47
	ZC_DIALOG_CLOSE = 0x0C4A,                    // Size: 6
	ZC_DIALOG_COMMON_TRADE = 0x0C4C,             // Size: 39
	ZC_DIALOG_ITEM_SELECT = 0x0C49,              // Size: 0
	ZC_DIALOG_NEXT = 0x0C47,                     // Size: 0
	ZC_DIALOG_NUMBERRANGE = 0x0C4D,              // Size: 0
	ZC_DIALOG_OK = 0x0C46,                       // Size: 0
	ZC_DIALOG_SELECT = 0x0C48,                   // Size: 0
	ZC_DIALOG_STRINGINPUT = 0x0C4E,              // Size: 0
	ZC_DIALOG_TRADE = 0x0C4B,                    // Size: 39
	ZC_REMOVE_SCROLLLOCKBOX = 0x0CAE,            // Size: 10

	/** =========== Chat =========== */
	ZC_CHAT = 0x0BD4,                            // Size: 0
	ZC_CHAT_MACRO_LIST = 0x0CC3,                 // Size: 0
	ZC_CHAT_WITH_TEXTCODE = 0x0BD5,              // Size: 14
	ZC_EMOTICON = 0x0CF5,                        // Size: 18
	ZC_SHOUT = 0x0C62,                           // Size: 0
	ZC_SHOUT_FAILED = 0x0C63,                    // Size: 7
	ZC_SHOW_EMOTICON = 0x0CF6,                   // Size: 18
	ZC_SYSTEM_MSG = 0x0CE0,                      // Size: 0
	ZC_WORLD_MSG = 0x0CBA,                       // Size: 43
	ZC_SHARED_MSG = 0x0CEE,                      // Size: 10

	/** =========== Items =========== */
	ZC_ENTER_ITEM = 0x0BC2,                      // Size: 103
	ZC_ENABLE_SHOW_ITEM_GET = 0x0CBB,            // Size: 7
	ZC_ITEM_ADD = 0x0C2D,                        // Size: 0
	ZC_ITEM_DROPABLE = 0x0D25,                   // Size: 10
	ZC_ITEM_EQUIP_LIST = 0x0C2C,                 // Size: 0
	ZC_ITEM_GET = 0x0C9E,                        // Size: 18
	ZC_ITEM_INVENTORY_INDEX_LIST = 0x0C2B,       // Size: 0
	ZC_ITEM_INVENTORY_LIST = 0x0C2A,             // Size: 0
	ZC_ITEM_REMOVE = 0x0C2E,                     // Size: 20
	ZC_ITEM_USE = 0x0C2F,                        // Size: 14
	ZC_ITEM_USE_TO_GROUND = 0x0C30,              // Size: 22

	/** =========== IES =========== */
	ZC_IES_MODIFY_INFO = 0x0CD4,                 // Size: 0
	ZC_IES_MODIFY_LIST = 0x0CD5,                 // Size: 0
	ZC_IES_REVISION_DELETE = 0x0CD7,             // Size: 0

	/** =========== Party =========== */
	ZC_FOUND_PARTY_LIST = 0x0D83,                // Size: 0
	ZC_PARTY_CHAT = 0x0C80,                      // Size: 0
	ZC_PARTY_DESTROY = 0x0C7D,                   // Size: 15
	ZC_PARTY_ENTER = 0x0C7B,                     // Size: 0
	ZC_PARTY_INFO = 0x0C7E,                      // Size: 0
	ZC_PARTY_INST_INFO = 0x0C81,                 // Size: 0
	ZC_PARTY_LIST = 0x0C7F,                      // Size: 0
	ZC_PARTY_OUT = 0x0C7C,                       // Size: 24

	/** =========== Movements =========== */
	ZC_COUNTER_MOVE = 0x0D1D,                    // Size: 10
	ZC_EXPECTED_STOPPOS = 0x0BC7,                // Size: 35
	ZC_MOVE_BARRACK = 0x0C01,                    // Size: 6
	ZC_MOVE_DIR = 0x0BC6,                        // Size: 40
	ZC_MOVE_IGNORE_COLLISION = 0x0C99,           // Size: 30
	ZC_MOVE_PATH = 0x0BC4,                       // Size: 42
	ZC_MOVE_PATH_MATH = 0x0D2A,                  // Size: 30
	ZC_MOVE_POS = 0x0BC5,                        // Size: 42
	ZC_MOVE_SPEED = 0x0BC9,                      // Size: 18
	ZC_MOVE_STOP = 0x0BCA,                       // Size: 23
	ZC_MOVE_ZONE = 0x0BBB,                       // Size: 7
	ZC_MOVE_ZONE_OK = 0x0BFE,                    // Size: 85
	ZC_PC_MOVE_STOP = 0x0D51,                    // Size: 35
	ZC_JUMP = 0x0BCC,                            // Size: 19
	ZC_JUMP_DIR = 0x0BCD,                        // Size: 34
	ZC_FORCE_MOVE = 0x0D16,                      // Size: 30
	ZC_HOLD_MOVE_PATH = 0x0CFA,                  // Size: 11
	ZC_MOTIONBLUR = 0x0D0B,                      // Size: 11
	ZC_WARP_MOVE = 0x0D29,                       // Size: 30
	ZC_RUN_FROM = 0x0D58,                        // Size: 14
	ZC_FSM_MOVE = 0x0CE1,                        // Size: 0

	/** =========== Single zone =========== */
	ZC_MOVE_SINGLE_ZONE = 0x0C8D,                // Size: 18

	/** =========== Quickslot =========== */
	ZC_QUICK_SLOT_LIST = 0x0C31,                 // Size: 0
	ZC_QUICKSLOT_REGISTER = 0x0CC5,              // Size: 46

	/** =========== Play events =========== */
	ZC_PLAY_ALARMSOUND = 0x0C5B,                 // Size: 83
	ZC_PLAY_ANI = 0x0C55,                        // Size: 24
	ZC_PLAY_EXP_TEXT = 0x0C5D,                   // Size: 14
	ZC_PLAY_FORCE = 0x0D0C,                      // Size: 78
	ZC_PLAY_MUSICQUEUE = 0x0C53,                 // Size: 14
	ZC_PLAY_NAVI_EFFECT = 0x0C5E,                // Size: 150
	ZC_PLAY_SKILL_ANI = 0x0C9B,                  // Size: 82
	ZC_PLAY_SKILL_CAST_ANI = 0x0C9C,             // Size: 30
	ZC_PLAY_SOUND = 0x0C51,                      // Size: 15
	ZC_STOP_ALARMSOUND = 0x0C5C,                 // Size: 10
	ZC_STOP_MUSICQUEUE = 0x0C54,                 // Size: 14
	ZC_STOP_SOUND = 0x0C52,                      // Size: 14

	/** =========== UI =========== */
	ZC_UI_INFO_LIST = 0x0CCC,                    // Size: 0
	ZC_UI_OPEN = 0x0C95,                         // Size: 39

	/** =========== Spray =========== */
	ZC_SPRAY_DRAW_INFO = 0x0CE6,                 // Size: 0
	ZC_SPRAY_ID = 0x0CE5,                        // Size: 18
	ZC_SPRAY_LIKE_LIST = 0x0CE8,                 // Size: 0

	/** =========== Help =========== */
	ZC_HELP_ADD = 0x0D3D,                        // Size: 11
	ZC_HELP_LIST = 0x0D3C,                       // Size: 0

	/** =========== Layer =========== */
	ZC_LAYER_INFO = 0x0CC1,                      // Size: 10
	ZC_LAYER_PC_LIST = 0x0CBE,                   // Size: 0
	ZC_LAYER_PC_SOBJ_PROP = 0x0CBF,              // Size: 0

	/** =========== Map =========== */
	ZC_MAP_REVEAL_LIST = 0x0CC9,                 // Size: 0
	ZC_SHOW_MAP = 0x0CF8,                        // Size: 0

	/** =========== Hackshield =========== */
	ZC_HACKSHIELD_BUFFER = 0x0D8E,               // Size: 608

	/** =========== Page =========== */
	ZC_MYPAGE_MAP = 0x0D35,                      // Size: 0
	ZC_GUESTPAGE_MAP = 0x0D36,                   // Size: 0
	ZC_ON_MYPAGE_MODE = 0x0D37,                  // Size: 0

	/** =========== Knock =========== */
	ZC_KNOCKBACK_INFO = 0x0BF8,                  // Size: 66
	ZC_KNOCKDOWN_INFO = 0x0BF9,                  // Size: 67

	/** =========== Session =========== */
	ZC_SESSION_OBJECTS = 0x0C86,                 // Size: 0
	ZC_SESSION_OBJ_ADD = 0x0C87,                 // Size: 0
	ZC_SESSION_OBJ_REMOVE = 0x0C88,              // Size: 10
	ZC_SESSION_OBJ_TIME = 0x0C89,                // Size: 14

	/** =========== Sync =========== */
	ZC_SYNC_START = 0x0D45,                      // Size: 10
	ZC_SYNC_END = 0x0D46,                        // Size: 14
	ZC_SYNC_EXEC = 0x0D47,                       // Size: 10
	ZC_SYNC_EXEC_BY_SKILL_TIME = 0x0D48,         // Size: 18

	/** =========== Gamepad =========== */
	ZC_VIBRATE = 0x0D1C,                         // Size: 30

	/** =========== Wiki =========== */
	ZC_WIKI_ADD = 0x0C92,                        // Size: 22
	ZC_WIKI_BOOL_PROP_UPDATE = 0x0CEB,           // Size: 12
	ZC_WIKI_COUNT_UPDATE = 0x0CE9,               // Size: 19
	ZC_WIKI_INT_PROP_UPDATE = 0x0CEA,            // Size: 15
	ZC_WIKI_LIST = 0x0C91,                       // Size: 0
	ZC_WIKI_RANK_LIST = 0x0CED,                  // Size: 0

	/** =========== Commands =========== */
	ZC_EXEC_CLIENT_SCP = 0x0CCD,                 // Size: 0
	ZC_GM_ORDER = 0x0CA3,                        // Size: 10

	/** =========== UNKNOWN & MISC =========== */
	ZC_CHECK_INVINDEX = 0x0C29,                  // Size: 30
	ZC_DIRECTION_APC = 0x0D68,                   // Size: 20
	ZC_DUMP_PROPERTY = 0x0C61,                   // Size: 0
	ZC_ENABLE_CONTROL = 0x0C96,                  // Size: 11
	ZC_ENTER_HOOK = 0x0CFB,                      // Size: 10
	ZC_FACTION = 0x0D63,                         // Size: 14
	ZC_FILE_MOVE = 0x0BD1,                       // Size: 46
	ZC_GD_RANK = 0x0D22,                         // Size: 10
	ZC_GROUND_EFFECT = 0x0CFE,                   // Size: 50
	ZC_HSKILL_CONTROL = 0x0D17,                  // Size: 22
	ZC_LEAVE = 0x0BC3,                           // Size: 12
	ZC_LEAVE_HOOK = 0x0CFC,                      // Size: 10
	ZC_LEAVE_TRIGGER = 0x0CB2,                   // Size: 6
	ZC_LOOKAT_OBJ = 0x0D59,                      // Size: 14
	ZC_MESSAGE = 0x0C03,                         // Size: 0
	ZC_MSPD = 0x0BC8,                            // Size: 14
	ZC_NORMAL = 0x0D27,                          // Size: 0
	ZC_ON_BUYITEM_MODE = 0x0D3A,                 // Size: 0
	ZC_OPTION_LIST = 0x0CA7,                     // Size: 0
	ZC_OVERHEAT_CHANGED = 0x0C76,                // Size: 26
	ZC_OWNER = 0x0D21,                           // Size: 14
	ZC_PING = 0x0D8D,                            // Size: 10
	ZC_PROPERTY_COMPARE = 0x0D62,                // Size: 0
	ZC_QUIET = 0x0C06,                           // Size: 7
	ZC_RANK_ACHIEVE_ADD = 0x0CD1,                // Size: 14
	ZC_RESET_BOX = 0x0CAC,                       // Size: 11
	ZC_RESET_SKILL_FORCEID = 0x0CF4,             // Size: 10
	ZC_RESET_SOCIAL_MODE = 0x0D38,               // Size: 10
	ZC_RESTORATION = 0x0C40,                     // Size: 12
	ZC_RULLET_LIST = 0x0CC4,                     // Size: 0
	ZC_SAVE_INFO = 0x0CA5,                       // Size: 6
	ZC_SET_LAYER = 0x0CAA,                       // Size: 10
	ZC_SHOW_GROUND_ITEM_MARK = 0x0D3B,           // Size: 30
	ZC_SHOW_MODEL = 0x0D03,                      // Size: 15
	ZC_SPC_TRIGGER_EXEC = 0x0D71,                // Size: 30
	ZC_TARGET_ROTATE = 0x0BEC,                   // Size: 18
	ZC_TEAMID = 0x0D08,                          // Size: 11
	ZC_TEST_AGENT = 0x0C77,                      // Size: 18
	ZC_TEST_DBG = 0x0CF2,                        // Size: 0
	ZC_TEXT = 0x0C3E,                            // Size: 0
	ZC_TO_ALL_CLIENT = 0x0D5E,                   // Size: 0
	ZC_TO_CLIENT = 0x0D5F,                       // Size: 0
	ZC_TO_SOMEWHERE_CLIENT = 0x0D85,             // Size: 0
	ZC_TREASUREMARK_BY_MAP = 0x0CF7,             // Size: 0
	ZC_TREASUREMARK_LIST_MAP = 0x00B3,           // Size: 0
	ZC_TRICK_PACKET = 0x0CC7,                    // Size: 0
	ZC_CLIENT_DIRECT = 0x0D20,                   // Size: 30


	/** ===================================================================== */
	/** ============================ Client -> H ============================ */
	/** ===================================================================== */
	CH_DAEMON_MODIFY = 0x0177,                   // Size: 1065
	CH_DELETE_FILE_ITEM = 0x016E,                // Size: 262
	CH_FORCE_STOP_DAEMON = 0x0173,               // Size: 20
	CH_LOGIN = 0x016F,                           // Size: 48
	CH_MESSAGE = 0x017A,                         // Size: 272
	CH_PROPERTY_TABLE = 0x0161,                  // Size: 14
	CH_PROPERTY_TABLE_BLOCK = 0x0163,            // Size: 14
	CH_SET_AUTOMATICALLY_RUNNING = 0x0178,       // Size: 7
	CH_SET_HUB_RUNNING = 0x017B,                 // Size: 7
	CH_START_DAEMON = 0x0171,                    // Size: 20
	CH_STATE_TABLE = 0x0165,                     // Size: 14
	CH_STOP_DAEMON = 0x0172,                     // Size: 20
	CH_STOP_DAEMON_TABLE = 0x0168,               // Size: 14
	CH_UPDATE_FILE = 0x0176,                     // Size: 275
	CH_UPDATE_SVN = 0x017E,                      // Size: 24
	CH_UPLOADED_FILE_TABLE = 0x016B,             // Size: 14
	CH_UPLOAD_FILE = 0x0174,                     // Size: 274
	CH_UPLOAD_FILE_BLOCK = 0x0175,               // Size: 4114



	/** ===================================================================== */
	/** ========================= Game -> Barrack =========================== */
	/** ===================================================================== */

	GB_ADD_SERVER_ZONE = 0x0394,                 // Size: 13
	GB_BAN_COMMANDER = 0x038E,                   // Size: 14
	GB_BAN_LIST = 0x038C,                        // Size: 0
	GB_CHANNEL_TRAFFIC = 0x0397,                 // Size: 0
	GB_IES_HIS_DEL = 0x0392,                     // Size: 0
	GB_IES_MODIFY_INFO = 0x0390,                 // Size: 0
	GB_NORMAL = 0x0399,                          // Size: 0
	GB_SEND_ALIVE_SERVERS = 0x0388,              // Size: 0
	GB_UPDATE_ALL_WORLD = 0x0396,                // Size: 0
	GB_UPDATE_ZONE = 0x0395,                     // Size: 0
	GB_ZONE_START = 0x0386,                      // Size: 8
	GB_ZONE_STOP = 0x0387,                       // Size: 8



	/** ===================================================================== */
	/** ========================== Game -> Social =========================== */
	/** ===================================================================== */

	GS_NORMAL = 0x0A2C,                          // Size: 0
	GS_TO_INTEG = 0x0A2E,                        // Size: 0


	/** ===================================================================== */
	/** ============================ Game -> Zone =========================== */
	/** ===================================================================== */

	GZ_ADD_SERVER_ZONE = 0x00B9,                 // Size: 13
	GZ_ALL_ZONE = 0x00C5,                        // Size: 0
	GZ_APPTIME = 0x00CC,                         // Size: 10
	GZ_BAN_COMMANDER = 0x00A9,                   // Size: 18
	GZ_BAN_LIST = 0x007A,                        // Size: 0
	GZ_CHANGE_NAME = 0x00C1,                     // Size: 79
	GZ_CHANNEL_TRAFFIC = 0x00BF,                 // Size: 0
	GZ_CHAT_STOP = 0x0082,                       // Size: 18
	GZ_CHAT_STOP_TIME = 0x0084,                  // Size: 83
	GZ_COMMANDER_INFO = 0x007C,                  // Size: 22
	GZ_COM_KICK = 0x0079,                        // Size: 79
	GZ_CREATE_FIELDBOSS = 0x00A1,                // Size: 28
	GZ_CREATE_ROOM = 0x0098,                     // Size: 23
	GZ_CREATE_ZONE = 0x00B7,                     // Size: 19
	GZ_DELETE_DAILY_ACHIEVE = 0x00B0,            // Size: 0
	GZ_DESTROY_COMMANDER = 0x00A8,               // Size: 26
	GZ_DESTROY_ROOM = 0x00A5,                    // Size: 10
	GZ_FOUND_PARTYID_LIST = 0x0096,              // Size: 0
	GZ_GIFTITEM = 0x00B4,                        // Size: 26
	GZ_GLOBALSERVER_STATE = 0x008B,              // Size: 7
	GZ_IES_HIS_DEL = 0x00AF,                     // Size: 0
	GZ_IES_MODIFY_INFO = 0x00AD,                 // Size: 0
	GZ_MOVE_ZONE = 0x0068,                       // Size: 32
	GZ_MSG = 0x006B,                             // Size: 0
	GZ_NORMAL = 0x00C8,                          // Size: 0
	GZ_PARTY_CHAT = 0x0093,                      // Size: 0
	GZ_PARTY_DESTROY = 0x0090,                   // Size: 15
	GZ_PARTY_ENTER = 0x008D,                     // Size: 279
	GZ_PARTY_INFO = 0x0091,                      // Size: 23
	GZ_PARTY_MEMBERLIST = 0x0095,                // Size: 0
	GZ_PARTY_OUT = 0x008F,                       // Size: 88
	GZ_PC_INFO = 0x007D,                         // Size: 22
	GZ_QUIET = 0x0071,                           // Size: 7
	GZ_RECALL_ALLIANCE = 0x009C,                 // Size: 29
	GZ_RECALL_FAIL_ALLIANCE = 0x009E,            // Size: 10
	GZ_RELOAD_MISSION_MAP = 0x009F,              // Size: 20
	GZ_REQ_CHANNEL_TRAFFIC = 0x00BD,             // Size: 14
	GZ_SCHEDULE_UPDATE = 0x00B2,                 // Size: 10
	GZ_SEND_ALIVE_SERVERS = 0x0085,              // Size: 0
	GZ_SHOUT = 0x006D,                           // Size: 0
	GZ_START = 0x0066,                           // Size: 8
	GZ_START_MISSION = 0x0099,                   // Size: 0
	GZ_STOP = 0x0067,                            // Size: 8
	GZ_TEXT = 0x0073,                            // Size: 0
	GZ_TO_ALL_CLIENT = 0x00CA,                   // Size: 0
	GZ_TO_CLIENT = 0x00CB,                       // Size: 0
	GZ_TO_PC = 0x00CE,                           // Size: 0
	GZ_UPDATED_NOTICE = 0x0080,                  // Size: 6
	GZ_UPDATE_ALLWORLD = 0x00BB,                 // Size: 0
	GZ_UPDATE_ZONE = 0x00BA,                     // Size: 0
	GZ_USER_LIST = 0x0088,                       // Size: 6
	GZ_USER_SESSION_INFO = 0x00C9,               // Size: 22
	GZ_VAR = 0x00B6,                             // Size: 0
	GZ_W = 0x0075,                               // Size: 18
	GZ_WHO = 0x006F,                             // Size: 26
	GZ_WORLD_MSG = 0x00AB,                       // Size: 43
	GZ_ZTZ = 0x00C3,                             // Size: 0

	HC_COMMAND_STATE = 0x015F,                   // Size: 63
	HC_LOG = 0x0160,                             // Size: 1034
	HC_LOGIN = 0x0170,                           // Size: 16
	HC_PROPERTY_TABLE_BLOCK = 0x0164,            // Size: 4112
	HC_PROPERTY_TABLE_DESC = 0x0162,             // Size: 23
	HC_SET_AUTOMATICALLY_RUNNING = 0x0179,       // Size: 7
	HC_SET_HUB_RUNNING = 0x017C,                 // Size: 7
	HC_STATE_TABLE_BLOCK = 0x0167,               // Size: 4112
	HC_STATE_TABLE_DESC = 0x0166,                // Size: 23
	HC_STOP_DAEMON_TABLE_BLOCK = 0x016A,         // Size: 4112
	HC_STOP_DAEMON_TABLE_DESC = 0x0169,          // Size: 17
	HC_SYSTEMLOG = 0x017D,                       // Size: 1034
	HC_UPDATE_PROPERTY = 0x017F,                 // Size: 1034
	HC_UPLOADED_FILE_TABLE_BLOCK = 0x016D,       // Size: 4112
	HC_UPLOADED_FILE_TABLE_DESC = 0x016C,        // Size: 17

	HN_AUTH = 0x01C4,                            // Size: 7
	HN_COMMAND_FORCE_STOP_DAEMON = 0x01CA,       // Size: 16
	HN_COMMAND_MESSAGE = 0x01CD,                 // Size: 272
	HN_COMMAND_START_DAEMON = 0x01C8,            // Size: 1040
	HN_COMMAND_STOP_DAEMON = 0x01C9,             // Size: 16
	HN_COMMAND_UPDATE_FILE = 0x01CB,             // Size: 15
	HN_COMMAND_UPDATE_SVN = 0x01CC,              // Size: 20
	HN_DAEMON_PROPERTYS = 0x01C5,                // Size: 0

	HU_AUTH = 0x0192,                            // Size: 7
	HU_DOWNLOAD_FILE_BLOCK = 0x0197,             // Size: 4106
	HU_DOWNLOAD_FILE_EXPLAIN = 0x0195,           // Size: 266

	IS_NORMAL = 0x0FA4,                          // Size: 0
	IS_START = 0x0FA1,                           // Size: 6
	IS_TO_CLIENT = 0x0FA6,                       // Size: 0
	IS_TO_GLOBAL = 0x0FA7,                       // Size: 0
	IS_TO_ZONE = 0x0FA8,                         // Size: 0

	NH_AUTH = 0x01C3,                            // Size: 10
	NH_COMMAND_STATE = 0x01C7,                   // Size: 63
	NH_UPDATE_STATE = 0x01C6,                    // Size: 0

	RZ_CONNECT = 0x01F5,                         // Size: 44
	RZ_NORMAL = 0x01F6,                          // Size: 0

	SC_FROM_INTEGRATE = 0x0B57,                  // Size: 0
	SC_NORMAL = 0x0B56,                          // Size: 0

	SG_ALIVE_PING = 0x0A2A,                      // Size: 6
	SG_FROM_INTEG = 0x0A2D,                      // Size: 0
	SG_NORMAL = 0x0A2B,                          // Size: 0
	SG_START = 0x0A29,                           // Size: 10

	SI_ALIVE_PING = 0x0FA3,                      // Size: 6
	SI_FROM_GLOBAL = 0x0FA9,                     // Size: 0
	SI_NORMAL = 0x0FA5,                          // Size: 0
	SI_START = 0x0FA2,                           // Size: 12

	SZ_FROM_INTEG = 0x0AF5,                      // Size: 0
	SZ_NORMAL = 0x0AF4,                          // Size: 0
	SZ_START = 0x0AF2,                           // Size: 8

	UH_AUTH = 0x0191,                            // Size: 14
	UH_COMMAND_STATE = 0x0193,                   // Size: 266
	UH_DOWNLOAD_FILE_BLOCK = 0x0196,             // Size: 6
	UH_START_DOWNLOAD_FILE = 0x0194,             // Size: 6


	ZG_ALL_CLIENT = 0x00C6,                      // Size: 0
	ZG_ALL_ZONE = 0x00C4,                        // Size: 0
	ZG_CHANGE_NAME = 0x00C0,                     // Size: 79
	ZG_CHANNEL_TRAFFIC = 0x00BE,                 // Size: 0
	ZG_CHAT_STOP = 0x0081,                       // Size: 83
	ZG_CHAT_STOP_TIME = 0x0083,                  // Size: 79
	ZG_CHECK_DOUBLE_LOGIN = 0x00A7,              // Size: 26
	ZG_COMMANDER_ALIVE_PING = 0x008A,            // Size: 14
	ZG_COMMANDER_INFO = 0x007B,                  // Size: 28
	ZG_COM_CALL = 0x0077,                        // Size: 97
	ZG_COM_GO = 0x0076,                          // Size: 78
	ZG_COM_KICK = 0x0078,                        // Size: 144
	ZG_CREATE_FIELDBOSS = 0x00A2,                // Size: 14
	ZG_CREATE_ROOM = 0x0097,                     // Size: 23
	ZG_CREATE_ZONE = 0x00B8,                     // Size: 15
	ZG_DESTROY_FIELDBOSS = 0x00A3,               // Size: 10
	ZG_DESTROY_MISSION_MAP = 0x009A,             // Size: 18
	ZG_DESTROY_ROOM = 0x00A4,                    // Size: 18
	ZG_GIFTITEM = 0x00B3,                        // Size: 26
	ZG_GLOBAL_CHEAT_CMD = 0x00B1,                // Size: 0
	ZG_IES_HIS_DEL = 0x00AE,                     // Size: 0
	ZG_IES_MODIFY_INFO = 0x00AC,                 // Size: 0
	ZG_LOGOUT = 0x0087,                          // Size: 15
	ZG_MOVE_ZONE = 0x0069,                       // Size: 38
	ZG_NORMAL = 0x00C7,                          // Size: 0
	ZG_PARTY_CHAT = 0x0092,                      // Size: 0
	ZG_PARTY_INVITE_ACCEPT = 0x008C,             // Size: 279
	ZG_PARTY_OUT = 0x008E,                       // Size: 88
	ZG_PC_INFO = 0x007E,                         // Size: 28
	ZG_QUIET = 0x0070,                           // Size: 7
	ZG_RECALL_ALLIANCE = 0x009B,                 // Size: 60
	ZG_RECALL_FAIL_ALLIANCE = 0x009D,            // Size: 10
	ZG_REGISTER_SESSION = 0x006A,                // Size: 188
	ZG_RELOAD_MISSION_MAP = 0x00A0,              // Size: 19
	ZG_REQ_CREATE_ZONE = 0x00BC,                 // Size: 8
	ZG_SERVER_ALIVE_PING = 0x0086,               // Size: 8
	ZG_SHOUT = 0x006C,                           // Size: 0
	ZG_START = 0x0065,                           // Size: 0
	ZG_TEXT = 0x0072,                            // Size: 0
	ZG_TO_PC = 0x00CD,                           // Size: 0
	ZG_UPDATED_NOTICE = 0x007F,                  // Size: 6
	ZG_UPDATE_COMMANDER_STATE = 0x0094,          // Size: 100
	ZG_VAR = 0x00B5,                             // Size: 0
	ZG_W = 0x0074,                               // Size: 18
	ZG_WHO = 0x006E,                             // Size: 83
	ZG_WORLD_MSG = 0x00AA,                       // Size: 43
	ZG_ZTZ = 0x00C2,                             // Size: 0
	ZR_NORMAL = 0x01F7,                          // Size: 0
	ZS_NORMAL = 0x0AF3,                          // Size: 0
	ZS_START = 0x0AF1,                           // Size: 8
	ZS_TO_INTEG = 0x0AF6,                        // Size: 0

}   PacketType;



typedef struct PacketTypeEntry
{
    int value;
    int size;
    char *string;

}   PacketTypeEntry;

/**
 * @brief PacketTypeInfo enumerates all the packets used in the game between the client and the server.
 *        It gives more info than PacketType (packet size and string)
 *        Its purpose is to give information during runtime execution, contrary to PacketType that is used during the compilation.
 */
typedef struct PacketTypeInfo
{
    PacketTypeEntry packets [PACKET_TYPES_MAX_INDEX];
}   PacketTypeInfo;


// Global packet types
extern const PacketTypeInfo packetTypeInfo;

// ----------- Functions ------------
