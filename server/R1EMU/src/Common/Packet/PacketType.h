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
#define PACKET_TYPES_MAX_INDEX      (PACKET_TYPE_COUNT + 1)


// ------ Structure declaration -------

typedef enum PacketTypeBarrackNormal {

    BC_NORMAL_COMMANDER_MOVE_OK = 0x2,
    BC_NORMAL_UNKNOWN_1 = 0x4,
    BC_NORMAL_ZONE_TRAFFIC = 0xB,
    BC_NORMAL_PET_INFORMATION = 0x8

}   PacketTypeBarrackNormal;

/**
 * @brief PacketType enumerates all the packets used in the game between the client and the server
 */
typedef enum PacketType
{
    CB_LOGIN = 3, // Size = 49
    CB_LOGIN_BY_PASSPORT = 4, // Size = 527
    CB_LOGOUT = 5, // Size = 10
    CB_START_GAME = 9, // Size = 13
    CB_START_BARRACK = 6, // Size = 11
    CB_COMMANDER_CREATE = 7, // Size = 91
    CB_COMMANDER_DESTROY = 8, // Size = 11
    CB_ECHO = 12, // Size = 30
    CB_BARRACKNAME_CHANGE = 10, // Size = 74
    CB_COMMANDER_MOVE = 11, // Size = 31
    BC_LOGINOK = 13, // Size = 37
    BC_LOGOUTOK = 14, // Size = 6
    BC_START_GAMEOK = 18, // Size = 60
    BC_SINGLE_INFO = 19, // Size = 337
    BC_COMMANDER_LIST = 15, // Size = 0
    BC_COMMANDER_CREATE = 16, // Size = 318
    BC_COMMANDER_DESTROY = 17, // Size = 7
    BC_MESSAGE = 20, // Size = 0
    BC_ECHO = 21, // Size = 26
    CB_IES_MODIFY_INFO = 70, // Size = 0
    BC_IES_MODIFY_INFO = 71, // Size = 0
    BC_IES_MODIFY_LIST = 72, // Size = 0
    CB_IES_REVISION_DELETE = 73, // Size = 0
    BC_IES_REVISION_DELETE = 74, // Size = 0
    BC_MYPAGE_MAP = 22, // Size = 0
    BC_BARRACKNAME_CHANGE = 23, // Size = 75
    CB_VISIT = 75, // Size = 74
    CB_BUY_THEMA = 76, // Size = 18
    BC_ACCOUNT_PROP = 77, // Size = 0
    CB_CURRENT_BARRACK = 78, // Size = 39
    BC_NORMAL = 79, // Size = 0
    CB_POSE = 80, // Size = 15
    CB_PLACE_CMD = 81, // Size = 46
    CB_CHAT = 82, // Size = 0
    BC_CHAT = 83, // Size = 0
    CB_ECHO_NORMAL = 84, // Size = 0
    CB_JUMP = 85, // Size = 19
    BC_JUMP = 86, // Size = 19
    BC_SERVER_ENTRY = 87, // Size = 18
    CB_PET_PC = 88, // Size = 26
    CB_DELETE_PET = 89, // Size = 18
    CS_LOGIN = 2901, // Size = 48
    SC_NORMAL = 2902, // Size = 0
    SC_FROM_INTEGRATE = 2903, // Size = 0
    CS_REGISTER_SNS_ID = 2904, // Size = 16
    CS_REQ_SNS_PC_INFO = 2905, // Size = 0
    CS_REQ_MARKET_LIST = 2906, // Size = 158
    CS_REQ_MY_SELL_LIST = 2907, // Size = 10
    CS_REQ_ADD_FRIEND = 2908, // Size = 70
    CS_REQ_BLOCK_FRIEND = 2909, // Size = 70
    CS_FRIEND_CMD = 2910, // Size = 24
    CS_FRIEND_SET_ADDINFO = 2911, // Size = 160
    CS_CHAT = 2912, // Size = 0
    CS_CREATE_GROUP_CHAT = 2913, // Size = 6
    CS_GROUP_CHAT_INVITE = 2914, // Size = 78
    CS_REFRESH_GROUP_CHAT = 2915, // Size = 6
    CS_CHAT_HISTORY = 2916, // Size = 22
    CS_CHAT_READ = 2917, // Size = 30
    CS_REMOVE_GROUP_MEMBER = 2918, // Size = 14
    CS_PC_INTERACTION = 2919, // Size = 24
    CS_PC_INTERACTION_HISTORY = 2921, // Size = 32
    CS_REQ_RELATED_PC_SESSION = 2920, // Size = 16
    CS_REDIS_SKILLPOINT = 2922, // Size = 22
    CS_PARTY_CLIENT_INFO_SEND = 2923, // Size = 0
    CS_NORMAL_GAME_START = 2924, // Size = 6
    CS_PVP_COMMAND = 2925, // Size = 18
    CS_REQUEST_PVP_RANKING = 2926, // Size = 83
    CZ_CONNECT = 3001, // Size = 55
    CZ_GAME_READY = 3069, // Size = 10
    CZ_KEYBOARD_MOVE = 3080, // Size = 41
    CZ_EXPECTED_STOP_POS = 3081, // Size = 31
    CZ_MOVE_PATH = 3084, // Size = 27
    CZ_MOVE_STOP = 3085, // Size = 35
    CZ_JUMP = 3082, // Size = 11
    CZ_REST_SIT = 3086, // Size = 10
    CZ_ON_AIR = 3087, // Size = 10
    CZ_ON_GROUND = 3088, // Size = 10
    CZ_SKILL_JUMP_REQ = 3083, // Size = 30
    CZ_LOGOUT = 3071, // Size = 11
    CZ_MOVE_BARRACK = 3072, // Size = 11
    CZ_MOVE_ZONE_OK = 3004, // Size = 10
    CZ_MOVEMENT_INFO = 3089, // Size = 23
    CZ_SKILL_TARGET = 3090, // Size = 19
    CZ_SKILL_TARGET_ANI = 3091, // Size = 23
    CZ_SKILL_GROUND = 3092, // Size = 57
    CZ_SKILL_SELF = 3093, // Size = 35
    CZ_SKILL_CANCEL = 3094, // Size = 11
    CZ_HOLD = 3095, // Size = 11
    CZ_SHOUT = 3102, // Size = 0
    CZ_CHAT = 3100, // Size = 0
    CZ_CHAT_LOG = 3101, // Size = 0
    CZ_ITEM_USE = 3105, // Size = 22
    CZ_ITEM_USE_TO_ITEM = 3106, // Size = 30
    CZ_ITEM_USE_TO_GROUND = 3107, // Size = 30
    CZ_ITEM_DROP = 3103, // Size = 22
    CZ_ITEM_EQUIP = 3110, // Size = 19
    CZ_ITEM_UNEQUIP = 3111, // Size = 11
    ZC_CHECK_INVINDEX = 3112, // Size = 30
    CZ_ITEM_DELETE = 3104, // Size = 0
    CZ_ITEM_BUY = 3108, // Size = 0
    CZ_ITEM_SELL = 3109, // Size = 0
    CZ_DIALOG_ACK = 3138, // Size = 14
    CZ_DIALOG_SELECT = 3139, // Size = 11
    CZ_DIALOG_STRINGINPUT = 3140, // Size = 138
    CZ_MOVE_CAMP = 3159, // Size = 18
    CZ_CAMPINFO = 3160, // Size = 18
    ZC_CAMPINFO = 3161, // Size = 18
    CZ_CLICK_TRIGGER = 3079, // Size = 15
    CZ_ROTATE = 3096, // Size = 18
    CZ_HEAD_ROTATE = 3097, // Size = 18
    CZ_TARGET_ROTATE = 3098, // Size = 18
    CZ_POSE = 3099, // Size = 34
    ZC_ENTER_PC = 3006, // Size = 317
    ZC_ENTER_MONSTER = 3007, // Size = 0
    ZC_ENTER_DUMMYPC = 3008, // Size = 292
    ZC_UPDATED_DUMMYPC = 3009, // Size = 250
    ZC_ENTER_ITEM = 3010, // Size = 103
    ZC_LEAVE = 3011, // Size = 12
    ZC_MOVE_PATH = 3012, // Size = 42
    ZC_MOVE_POS = 3013, // Size = 42
    ZC_MSPD = 3016, // Size = 14
    ZC_MOVE_SPEED = 3017, // Size = 18
    ZC_MOVE_DIR = 3014, // Size = 40
    ZC_EXPECTED_STOPPOS = 3015, // Size = 35
    ZC_MOVE_STOP = 3018, // Size = 23
    ZC_REST_SIT = 3019, // Size = 11
    ZC_JUMP = 3020, // Size = 19
    ZC_JUMP_DIR = 3021, // Size = 34
    ZC_ORDER_SKILL_JUMP = 3022, // Size = 10
    ZC_SKILL_JUMP = 3023, // Size = 38
    ZC_SET_POS = 3024, // Size = 22
    ZC_FILE_MOVE = 3025, // Size = 46
    ZC_MESSAGE = 3075, // Size = 0
    ZC_CONNECT_OK = 3002, // Size = 0
    ZC_CONNECT_FAILED = 3005, // Size = 0
    ZC_START_GAME = 3077, // Size = 26
    ZC_MOVE_ZONE = 3003, // Size = 7
    ZC_MOVE_BARRACK = 3073, // Size = 6
    ZC_MOVE_ZONE_OK = 3070, // Size = 85
    ZC_DEAD = 3055, // Size = 0
    ZC_RESURRECT = 3056, // Size = 14
    ZC_RESURRECT_DIALOG = 3058, // Size = 7
    CZ_RESURRECT = 3066, // Size = 11
    ZC_RESURRECT_SAVE_POINT_ACK = 3067, // Size = 7
    ZC_RESURRECT_HERE_ACK = 3068, // Size = 7
    ZC_UPDATED_PCAPPEARANCE = 3026, // Size = 250
    ZC_UPDATED_MONSTERAPPEARANCE = 3027, // Size = 0
    ZC_ADD_HP = 3031, // Size = 18
    ZC_UPDATE_SP = 3135, // Size = 15
    ZC_UPDATE_MHP = 3137, // Size = 12
    ZC_EXP_UP = 3128, // Size = 14
    ZC_EXP_UP_BY_MONSTER = 3129, // Size = 18
    CZ_EXP_UP_BY_MONSTER = 3130, // Size = 18
    ZC_PC_LEVELUP = 3131, // Size = 14
    ZC_PC_STAT_AVG = 3132, // Size = 30
    ZC_MAX_EXP_CHANGED = 3133, // Size = 18
    ZC_UPDATE_ALL_STATUS = 3170, // Size = 22
    ZC_CHANGE_RELATION = 3057, // Size = 11
    ZC_QUICK_SLOT_LIST = 3120, // Size = 0
    ZC_SKILL_LIST = 3121, // Size = 0
    ZC_SKILL_ADD = 3122, // Size = 0
    ZC_SKILL_CAST_CANCEL = 3032, // Size = 10
    ZC_SKILL_CAST = 3033, // Size = 38
    ZC_SKILL_READY = 3034, // Size = 46
    ZC_SKILL_USE_CANCEL = 3036, // Size = 10
    ZC_SKILL_DISABLE = 3035, // Size = 15
    ZC_SKILL_MELEE_TARGET = 3037, // Size = 0
    ZC_SKILL_FORCE_TARGET = 3039, // Size = 0
    ZC_SKILL_MELEE_GROUND = 3038, // Size = 0
    ZC_SKILL_FORCE_GROUND = 3040, // Size = 0
    ZC_SKILL_HIT_INFO = 3041, // Size = 0
    ZC_ABILITY_LIST = 3123, // Size = 0
    CZ_ACTIVE_ABILITY = 3124, // Size = 15
    ZC_ACTIVE_ABILITY = 3125, // Size = 11
    CZ_DISPEL_DEBUFF_TOGGLE = 3126, // Size = 14
    CZ_JUNGTAN_TOGGLE = 3127, // Size = 16
    ZC_BUFF_LIST = 3042, // Size = 0
    ZC_BUFF_ADD = 3043, // Size = 0
    ZC_BUFF_UPDATE = 3044, // Size = 0
    ZC_BUFF_REMOVE = 3045, // Size = 13
    ZC_BUFF_CLEAR = 3046, // Size = 11
    CZ_BUFF_REMOVE = 3047, // Size = 14
    CZ_INTE_WARP = 3048, // Size = 14
    ZC_HIT_INFO = 3059, // Size = 60
    ZC_HEAL_INFO = 3060, // Size = 30
    ZC_CAUTION_DAMAGE_INFO = 3062, // Size = 15
    ZC_CAUTION_DAMAGE_RELEASE = 3063, // Size = 10
    ZC_KNOCKBACK_INFO = 3064, // Size = 70
    ZC_KNOCKDOWN_INFO = 3065, // Size = 71
    ZC_CHAT = 3028, // Size = 0
    ZC_CHAT_WITH_TEXTCODE = 3029, // Size = 14
    ZC_SHOUT = 3173, // Size = 0
    ZC_SHOUT_FAILED = 3174, // Size = 7
    ZC_TEXT = 3134, // Size = 0
    ZC_QUIET = 3078, // Size = 7
    ZC_DIALOG_CLOSE = 3146, // Size = 6
    ZC_DIALOG_OK = 3142, // Size = 0
    ZC_DIALOG_NEXT = 3143, // Size = 0
    ZC_DIALOG_SELECT = 3144, // Size = 0
    ZC_DIALOG_TRADE = 3147, // Size = 39
    ZC_DIALOG_COMMON_TRADE = 3148, // Size = 39
    ZC_DIALOG_ITEM_SELECT = 3145, // Size = 0
    ZC_DIALOG_NUMBERRANGE = 3149, // Size = 0
    ZC_DIALOG_STRINGINPUT = 3150, // Size = 0
    ZC_STANCE_CHANGE = 3030, // Size = 14
    ZC_ITEM_ADD = 3116, // Size = 0
    ZC_ITEM_INVENTORY_LIST = 3113, // Size = 0
    ZC_ITEM_INVENTORY_INDEX_LIST = 3114, // Size = 0
    ZC_ITEM_EQUIP_LIST = 3115, // Size = 0
    ZC_ITEM_REMOVE = 3117, // Size = 20
    ZC_ITEM_USE = 3118, // Size = 14
    ZC_ITEM_USE_TO_GROUND = 3119, // Size = 22
    ZC_RESET_VIEW = 3076, // Size = 6
    ZC_RESTORATION = 3136, // Size = 12
    ZC_ROTATE = 3049, // Size = 20
    ZC_ROTATE_RESERVED = 3050, // Size = 18
    ZC_HEAD_ROTATE = 3051, // Size = 18
    ZC_TARGET_ROTATE = 3052, // Size = 18
    ZC_QUICK_ROTATE = 3053, // Size = 18
    ZC_POSE = 3054, // Size = 34
    ZC_DUMP_PROPERTY = 3172, // Size = 0
    ZC_OBJECT_PROPERTY = 3171, // Size = 0
    ZC_ADDON_MSG = 3151, // Size = 0
    CZ_UI_EVENT = 3152, // Size = 0
    ZC_LOGOUT_OK = 3074, // Size = 6
    ZC_PLAY_SOUND = 3153, // Size = 15
    ZC_STOP_SOUND = 3154, // Size = 14
    ZC_PLAY_MUSICQUEUE = 3155, // Size = 14
    ZC_STOP_MUSICQUEUE = 3156, // Size = 14
    ZC_PLAY_ANI = 3157, // Size = 24
    ZC_CHANGE_ANI = 3158, // Size = 44
    ZC_PLAY_ALARMSOUND = 3166, // Size = 83
    ZC_STOP_ALARMSOUND = 3167, // Size = 10
    ZC_PLAY_EXP_TEXT = 3168, // Size = 14
    ZC_PLAY_NAVI_EFFECT = 3169, // Size = 150
    CZ_EXCHANGE_REQUEST = 3175, // Size = 14
    ZC_EXCHANGE_REQUEST_ACK = 3176, // Size = 72
    ZC_EXCHANGE_REQUEST_RECEIVED = 3177, // Size = 71
    CZ_EXCHANGE_ACCEPT = 3178, // Size = 10
    CZ_EXCHANGE_DECLINE = 3179, // Size = 10
    ZC_EXCHANGE_DECLINE_ACK = 3180, // Size = 6
    ZC_EXCHANGE_START = 3181, // Size = 71
    CZ_EXCHANGE_OFFER = 3182, // Size = 30
    ZC_EXCHANGE_OFFER_ACK = 3183, // Size = 0
    CZ_EXCHANGE_AGREE = 3184, // Size = 10
    ZC_EXCHANGE_AGREE_ACK = 3185, // Size = 7
    CZ_EXCHANGE_FINALAGREE = 3186, // Size = 10
    ZC_EXCHANGE_FINALAGREE_ACK = 3187, // Size = 7
    CZ_EXCHANGE_CANCEL = 3188, // Size = 10
    ZC_EXCHANGE_CANCEL_ACK = 3189, // Size = 6
    ZC_EXCHANGE_SUCCESS = 3190, // Size = 6
    ZC_COOLDOWN_LIST = 3191, // Size = 0
    ZC_COOLDOWN_CHANGED = 3192, // Size = 22
    ZC_OVERHEAT_CHANGED = 3193, // Size = 26
    ZC_TEST_AGENT = 3194, // Size = 18
    CZ_COMMON_SHOP_LIST = 3195, // Size = 10
    ZC_COMMON_SHOP_LIST = 3196, // Size = 8
    ZC_TIME_FACTOR = 3197, // Size = 10
    ZC_PARTY_ENTER = 3198, // Size = 0
    ZC_PARTY_OUT = 3199, // Size = 24
    ZC_PARTY_DESTROY = 3200, // Size = 15
    ZC_PARTY_INFO = 3201, // Size = 0
    ZC_PARTY_LIST = 3202, // Size = 0
    ZC_PARTY_CHAT = 3203, // Size = 0
    ZC_PARTY_INST_INFO = 3204, // Size = 0
    ZC_CHANGE_EQUIP_DURABILITY = 3205, // Size = 11
    CZ_DIALOG_TX = 3206, // Size = 0
    CZ_REQ_RECIPE = 3207, // Size = 0
    ZC_CUSTOM_DIALOG = 3208, // Size = 75
    ZC_SESSION_OBJECTS = 3209, // Size = 0
    ZC_SESSION_OBJ_ADD = 3210, // Size = 0
    ZC_SESSION_OBJ_REMOVE = 3211, // Size = 10
    ZC_SESSION_OBJ_TIME = 3212, // Size = 14
    CZ_S_OBJ_VALUE_C = 3213, // Size = 24
    CZ_REQ_NORMAL_TX = 3214, // Size = 29
    ZC_COMMANDER_LOADER_INFO = 3215, // Size = 0
    ZC_MOVE_SINGLE_ZONE = 3216, // Size = 18
    ZC_BACKTO_ORIGINAL_SERVER = 3217, // Size = 8
    CZ_BACKTO_ORIGINAL_SERVER = 3218, // Size = 12
    CZ_REQ_NORMAL_TX_NUMARG = 3219, // Size = 0
    ZC_WIKI_LIST = 3220, // Size = 0
    ZC_WIKI_ADD = 3221, // Size = 22
    CZ_WIKI_GET = 3222, // Size = 14
    CZ_WIKI_RECIPE_UPDATE = 3223, // Size = 10
    ZC_UI_OPEN = 3224, // Size = 39
    ZC_ENABLE_CONTROL = 3225, // Size = 11
    ZC_CHANGE_CAMERA = 3226, // Size = 31
    ZC_MONSTER_SDR_CHANGED = 3227, // Size = 11
    ZC_MOVE_IGNORE_COLLISION = 3228, // Size = 30
    ZC_CHANGE_CAMERA_ZOOM = 3229, // Size = 34
    ZC_PLAY_SKILL_ANI = 3230, // Size = 82
    ZC_PLAY_SKILL_CAST_ANI = 3231, // Size = 30
    CZ_REQ_ITEM_GET = 3232, // Size = 14
    ZC_ITEM_GET = 3233, // Size = 18
    CZ_GUARD = 3234, // Size = 19
    ZC_GUARD = 3235, // Size = 19
    ZC_STAMINA = 3236, // Size = 10
    ZC_ADD_STAMINA = 3237, // Size = 10
    ZC_GM_ORDER = 3238, // Size = 10
    ZC_MYPC_ENTER = 3239, // Size = 18
    ZC_LOCK_KEY = 3240, // Size = 75
    ZC_SAVE_INFO = 3241, // Size = 6
    CZ_SAVE_INFO = 3242, // Size = 0
    ZC_OPTION_LIST = 3243, // Size = 0
    ZC_SKILLMAP_LIST = 3244, // Size = 0
    CZ_GIVEITEM_TO_DUMMYPC = 3245, // Size = 22
    ZC_SET_LAYER = 3246, // Size = 10
    ZC_CREATE_LAYERBOX = 3247, // Size = 38
    ZC_RESET_BOX = 3248, // Size = 11
    ZC_CREATE_SCROLLLOCKBOX = 3249, // Size = 38
    ZC_REMOVE_SCROLLLOCKBOX = 3250, // Size = 10
    CZ_DYNAMIC_CASTING_START = 3251, // Size = 23
    CZ_DYNAMIC_CASTING_END = 3252, // Size = 19
    CZ_SKILL_CANCEL_SCRIPT = 3253, // Size = 10
    ZC_LEAVE_TRIGGER = 3254, // Size = 6
    ZC_BORN = 3255, // Size = 10
    ZC_ACHIEVE_POINT_LIST = 3256, // Size = 0
    ZC_ACHIEVE_POINT = 3257, // Size = 18
    CZ_ACHIEVE_EQUIP = 3258, // Size = 18
    ZC_ACHIEVE_EQUIP = 3259, // Size = 22
    CZ_CHANGE_CONFIG = 3260, // Size = 18
    CZ_CHANGE_CONFIG_STR = 3261, // Size = 34
    ZC_WORLD_MSG = 3262, // Size = 43
    ZC_ENABLE_SHOW_ITEM_GET = 3263, // Size = 7
    ZC_LOGIN_TIME = 3264, // Size = 14
    ZC_GIVE_EXP_TO_PC = 3265, // Size = 42
    ZC_LAYER_PC_LIST = 3266, // Size = 0
    ZC_LAYER_PC_SOBJ_PROP = 3267, // Size = 0
    CZ_CUSTOM_COMMAND = 3268, // Size = 26
    CZ_ADD_HELP = 3421, // Size = 14
    ZC_LAYER_INFO = 3269, // Size = 10
    CZ_CHAT_MACRO = 3270, // Size = 146
    ZC_CHAT_MACRO_LIST = 3271, // Size = 0
    ZC_RULLET_LIST = 3272, // Size = 0
    ZC_QUICKSLOT_REGISTER = 3273, // Size = 46
    CZ_QUICKSLOT_LIST = 3274, // Size = 0
    CZ_DOUBLE_ITEM_EQUIP = 3275, // Size = 28
    ZC_TRICK_PACKET = 3276, // Size = 0
    ZC_COOLDOWN_RATE = 3277, // Size = 22
    ZC_MAP_REVEAL_LIST = 3278, // Size = 0
    CZ_MAP_REVEAL_INFO = 3279, // Size = 146
    CZ_MAP_SEARCH_INFO = 3280, // Size = 55
    ZC_UI_INFO_LIST = 3281, // Size = 0
    ZC_EXEC_CLIENT_SCP = 3282, // Size = 0
    ZC_SET_NPC_STATE = 3283, // Size = 18
    ZC_NPC_STATE_LIST = 3284, // Size = 0
    CZ_QUEST_NPC_STATE_CHECK = 3285, // Size = 14
    ZC_RANK_ACHIEVE_ADD = 3286, // Size = 14
    CZ_GET_MAP_REVEAL_ACHIEVE = 3287, // Size = 10
    CZ_IES_MODIFY_INFO = 3288, // Size = 0
    ZC_IES_MODIFY_INFO = 3289, // Size = 0
    ZC_IES_MODIFY_LIST = 3290, // Size = 0
    CZ_IES_REVISION_DELETE = 3291, // Size = 0
    ZC_IES_REVISION_DELETE = 3292, // Size = 0
    ZC_EQUIP_ITEM_REMOVE = 3293, // Size = 18
    ZC_SOLD_ITEM_LIST = 3294, // Size = 0
    CZ_SOLD_ITEM = 3295, // Size = 19
    CZ_WAREHOUSE_CMD = 3296, // Size = 27
    CZ_SWAP_ETC_INV_CHANGE_INDEX = 3297, // Size = 35
    CZ_CAST_CONTROL_SHOT = 3298, // Size = 10
    ZC_PC_PROP_UPDATE = 3299, // Size = 9
    CZ_CLIENT_DAMAGE = 3300, // Size = 14
    CZ_CLIENT_ATTACK = 3301, // Size = 15
    ZC_SYSTEM_MSG = 3302, // Size = 0
    ZC_FSM_MOVE = 3303, // Size = 0
    CZ_QUEST_CHECK_SAVE = 3304, // Size = 50
    CZ_SPRAY_REQ_INFO = 3305, // Size = 14
    CZ_SPRAY_DRAW_INFO = 3306, // Size = 0
    ZC_SPRAY_ID = 3307, // Size = 18
    ZC_SPRAY_DRAW_INFO = 3308, // Size = 0
    ZC_MONSTER_LIFETIME = 3309, // Size = 14
    ZC_SPRAY_LIKE_LIST = 3310, // Size = 0
    ZC_WIKI_COUNT_UPDATE = 3311, // Size = 19
    ZC_WIKI_INT_PROP_UPDATE = 3312, // Size = 15
    ZC_WIKI_BOOL_PROP_UPDATE = 3313, // Size = 12
    CZ_REQ_WIKI_RANK = 3314, // Size = 15
    ZC_WIKI_RANK_LIST = 3315, // Size = 0
    ZC_SHARED_MSG = 3316, // Size = 10
    CZ_REQ_WIKI_PROP_RANK = 3317, // Size = 16
    CZ_REQ_TX_ITEM = 3319, // Size = 0
    ZC_TEST_DBG = 3320, // Size = 0
    ZC_MONSTER_DIST = 3321, // Size = 0
    ZC_RESET_SKILL_FORCEID = 3322, // Size = 10
    ZC_EMOTICON = 3323, // Size = 18
    ZC_SHOW_EMOTICON = 3324, // Size = 18
    ZC_TREASUREMARK_BY_MAP = 3325, // Size = 0
    ZC_SHOW_MAP = 3326, // Size = 0
    ZC_TREASUREMARK_LIST_MAP = 197, // Size = 0
    ZC_FIX_ANIM = 3162, // Size = 14
    ZC_MOVE_ANIM = 3163, // Size = 12
    CZ_FLEE_OBSTACLE = 3327, // Size = 26
    ZC_HOLD_MOVE_PATH = 3328, // Size = 11
    ZC_ENTER_HOOK = 3329, // Size = 10
    ZC_LEAVE_HOOK = 3330, // Size = 10
    ZC_MONSTER_PROPERTY = 3331, // Size = 0
    ZC_GROUND_EFFECT = 3332, // Size = 50
    ZC_FLY = 3333, // Size = 18
    ZC_FLY_MATH = 3334, // Size = 22
    ZC_FLY_HEIGHT = 3335, // Size = 14
    ZC_UPDATE_SHIELD = 3336, // Size = 12
    ZC_UPDATE_MSHIELD = 3337, // Size = 12
    ZC_SHOW_MODEL = 3338, // Size = 15
    ZC_SKILL_RANGE_DBG = 3339, // Size = 58
    ZC_SKILL_RANGE_FAN = 3340, // Size = 40
    ZC_SKILL_RANGE_SQUARE = 3341, // Size = 40
    ZC_SKILL_RANGE_CIRCLE = 3342, // Size = 28
    ZC_SKILL_RANGE_DONUTS = 3343, // Size = 32
    ZC_TEAMID = 3344, // Size = 11
    ZC_PC = 3345, // Size = 0
    CZ_LOG = 3346, // Size = 0
    ZC_MOTIONBLUR = 3347, // Size = 11
    ZC_PLAY_FORCE = 3348, // Size = 78
    ZC_CAST_TARGET = 3349, // Size = 14
    ZC_START_INFO = 3350, // Size = 0
    ZC_JOB_EXP_UP = 3351, // Size = 10
    ZC_JOB_PTS = 3352, // Size = 10
    ZC_MON_STAMINA = 3353, // Size = 22
    CZ_CUSTOM_SCP = 3354, // Size = 14
    ZC_VIEW_FOCUS = 3355, // Size = 24
    ZC_HARDCODED_SKILL = 3356, // Size = 26
    CZ_HARDCODED_SKILL = 3357, // Size = 34
    ZC_FORCE_MOVE = 3358, // Size = 30
    ZC_HSKILL_CONTROL = 3359, // Size = 22
    ZC_CANCEL_DEADEVENT = 3360, // Size = 10
    ZC_ACTION_PKS = 3361, // Size = 35
    CZ_HARDCODED_ITEM = 3362, // Size = 22
    CZ_BRIQUET = 3364, // Size = 30
    CZ_CANCEL_TRANSFORM_SKILL = 3363, // Size = 10
    ZC_VIBRATE = 3365, // Size = 30
    ZC_COUNTER_MOVE = 3366, // Size = 10
    CZ_COUNTER_ATTACK = 3367, // Size = 14
    CZ_CLIENT_DIRECT = 3368, // Size = 30
    ZC_CLIENT_DIRECT = 3369, // Size = 30
    ZC_OWNER = 3370, // Size = 14
    ZC_GD_RANK = 3371, // Size = 10
    CZ_RUN_BGEVENT = 3372, // Size = 74
    ZC_ADD_SKILL_EFFECT = 3373, // Size = 18
    ZC_ITEM_DROPABLE = 3374, // Size = 10
    CZ_ITEM_DROP_TO_OBJECT = 3375, // Size = 26
    ZC_NORMAL = 3376, // Size = 0
    CZ_G_QUEST_CHECK = 3377, // Size = 14
    ZC_MOVE_PATH_MATH = 3378, // Size = 30
    ZC_SHOW_GROUND_ITEM_MARK = 3395, // Size = 30
    ZC_HELP_LIST = 3396, // Size = 0
    ZC_HELP_ADD = 3397, // Size = 11
    ZC_STD_ANIM = 3164, // Size = 11
    CZ_CLIENT_HIT_LIST = 3398, // Size = 0
    ZC_PC_ATKSTATE = 3399, // Size = 11
    CZ_HELP_READ_TYPE = 3400, // Size = 18
    CZ_MOVE_PATH_END = 3401, // Size = 10
    ZC_COLL_DAMAGE = 3402, // Size = 11
    CZ_KEYBOARD_BEAT = 3403, // Size = 10
    CZ_MOVEHIT_SCP = 3404, // Size = 22
    ZC_SYNC_START = 3405, // Size = 10
    ZC_SYNC_END = 3406, // Size = 14
    ZC_SYNC_EXEC = 3407, // Size = 10
    ZC_SYNC_EXEC_BY_SKILL_TIME = 3408, // Size = 18
    CZ_STOP_TIMEACTION = 3409, // Size = 10
    CZ_REQ_DUMMYPC_INFO = 3410, // Size = 18
    CZ_VISIT_BARRACK = 3411, // Size = 74
    CZ_SPC_SKILL_POS = 3412, // Size = 22
    CZ_REQ_CHANGEJOB = 3413, // Size = 14
    CZ_REQ_MINITEXT = 3416, // Size = 266
    ZC_PC_MOVE_STOP = 3417, // Size = 35
    CZ_SKILL_TOOL_GROUND_POS = 3427, // Size = 22
    CZ_CHANGE_HEAD = 3414, // Size = 14
    CZ_CREATE_ARROW_CRAFT = 3415, // Size = 14
    CZ_MYPAGE_COMMENT_ADD = 3379, // Size = 278
    CZ_MYPAGE_COMMENT_DELETE = 3380, // Size = 18
    CZ_GET_TARGET_MYPAGE = 3382, // Size = 14
    CZ_ON_MYPAGE_MODE = 3383, // Size = 14
    CZ_RESET_SOCIAL_MODE = 3384, // Size = 10
    CZ_GUESTPAGE_COMMENT_ADD = 3381, // Size = 278
    CZ_GET_TARGET_GUESTPAGE = 3385, // Size = 14
    CZ_ADD_SELLMODE_ITEM = 3386, // Size = 30
    CZ_DELETE_SELLMODE_ITEM = 3387, // Size = 18
    CZ_ON_SELLITEM_MODE = 3388, // Size = 14
    CZ_ON_ITEMBUY_MODE = 3393, // Size = 0
    ZC_MYPAGE_MAP = 3389, // Size = 0
    ZC_GUESTPAGE_MAP = 3390, // Size = 0
    ZC_ON_MYPAGE_MODE = 3391, // Size = 0
    ZC_RESET_SOCIAL_MODE = 3392, // Size = 10
    ZC_ON_BUYITEM_MODE = 3394, // Size = 0
    CZ_STOP_ALLPC = 3418, // Size = 10
    CZ_COMPLETE_PRELOAD = 3419, // Size = 14
    CZ_MGAME_JOIN_CMD = 3420, // Size = 46
    ZC_ATTACH_TO_OBJ = 3422, // Size = 49
    ZC_DETACH_FROM_OBJ = 3423, // Size = 14
    ZC_RUN_FROM = 3424, // Size = 14
    ZC_LOOKAT_OBJ = 3425, // Size = 14
    CZ_SKILL_CELL_LIST = 3426, // Size = 0
    CZ_DIRECTION_PROCESS = 3428, // Size = 18
    CZ_DIRECTION_MOVE_STATE = 3429, // Size = 0
    ZC_TO_ALL_CLIENT = 3430, // Size = 0
    ZC_TO_CLIENT = 3431, // Size = 0
    CZ_REWARD_CMD = 3432, // Size = 14
    CZ_PROPERTY_COMPARE = 3433, // Size = 15
    ZC_PROPERTY_COMPARE = 3434, // Size = 0
    ZC_FACTION = 3435, // Size = 14
    ZC_BEGIN_KILL_LOG = 3436, // Size = 6
    ZC_END_KILL_LOG = 3437, // Size = 6
    ZC_CLEAR_KILL_LOG = 3438, // Size = 6
    CZ_NPC_AUCTION_CMD = 3439, // Size = 30
    ZC_DIRECTION_APC = 3440, // Size = 22
    ZC_BGMODEL_ANIM_INFO = 3441, // Size = 15
    ZC_ATTACH_BY_KNOCKBACK = 3442, // Size = 38
    CZ_OBJECT_MOVE = 3443, // Size = 30
    CZ_CONTROL_OBJECT_ROTATE = 3444, // Size = 22
    CZ_SUMMON_COMMAND = 3445, // Size = 18
    CZ_VEHICLE_RIDE = 3446, // Size = 15
    CZ_REQ_WIKI_CATEGORY_RANK_PAGE_INFO = 3447, // Size = 78
    CZ_REQ_ACHIEVE_RANK_PAGE_INFO = 3448, // Size = 78
    ZC_SPC_TRIGGER_EXEC = 3449, // Size = 30
    CZ_REQ_MGAME_VIEW = 3450, // Size = 18
    CZ_REQ_MGAME_CHAT = 3451, // Size = 0
    CZ_TOURNAMENT_GIFT = 3452, // Size = 18
    CZ_PARTY_INVITE_ACCEPT = 3453, // Size = 75
    CZ_PARTY_INVITE_CANCEL = 3454, // Size = 75
    CZ_PARTY_PROP_CHANGE = 3455, // Size = 145
    CZ_REQ_MARKET_REGISTER = 3456, // Size = 27
    CZ_REQ_MARKET_BUY = 3457, // Size = 0
    CZ_REQ_CABINET_LIST = 3458, // Size = 10
    CZ_REQ_GET_CABINET_ITEM = 3459, // Size = 26
    CZ_REQ_CANCEL_MARKET_ITEM = 3460, // Size = 18
    CZ_OBJ_RECORD_POS = 3461, // Size = 0
    CZ_FORMATION_CMD = 3462, // Size = 32
    CZ_REGISTER_AUTOSELLER = 3463, // Size = 0
    CZ_OPEN_AUTOSELLER = 3464, // Size = 34
    CZ_BUY_AUTOSELLER_ITEMS = 3465, // Size = 0
    CZ_SELL_MY_AUTOSELLER_ITEMS = 3466, // Size = 0
    CZ_PUZZLE_CRAFT = 3467, // Size = 0
    CZ_GET_WIKI_REWARD = 3318, // Size = 11
    CZ_PET_EQUIP = 3468, // Size = 30
    ZC_FOUND_PARTY_LIST = 3469, // Size = 0
    CZ_REQUEST_SOME_PARTY = 3470, // Size = 86
    ZC_TO_SOMEWHERE_CLIENT = 3471, // Size = 0
    CZ_REVEAL_NPC_STATE = 3472, // Size = 14
    CZ_CHANGE_CHANNEL = 3473, // Size = 11
    CZ_REQ_CHANNEL_TRAFFICS = 3474, // Size = 12
    CZ_BUY_PROPERTYSHOP_ITEM = 3475, // Size = 0
    CZ_SKILL_USE_HEIGHT = 3476, // Size = 14
    CZ_ACCEPT_PARTY_QUEST = 3477, // Size = 20
    CZ_ACCEPT_PARTY_MISSION = 3478, // Size = 20
    CZ_PING = 3479, // Size = 10
    ZC_PING = 3480, // Size = 10
    ZC_HACKSHIELD_BUFFER = 3481, // Size = 608
    CZ_HACKSHIELD_BUFFER = 3482, // Size = 612
    CZ_CHANGE_TITLE = 3483, // Size = 74
    CZ_PC_COMMENT_CHANGE = 3484, // Size = 0
    CZ_AUTTOSELLER_BUYER_CLOSE = 3485, // Size = 18
    CZ_REQ_ITEM_LIST = 3486, // Size = 11
    CZ_HIT_MISSILE = 3487, // Size = 14
    CZ_I_NEED_PARTY = 3488, // Size = 23

    PACKET_TYPE_COUNT

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
