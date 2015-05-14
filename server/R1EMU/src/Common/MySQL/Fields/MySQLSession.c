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
#include "MySQLSession.h"

void
MySQL_flushSession (
    MySQL *self,
    Session *session
) {
    MYSQL_ROW count;

    // Flush the commander
    if (MySQL_query (self, "SELECT count(*) FROM commander WHERE commander_id = %u", session->game.currentCommanderId)) {
        error ("SQL Error : %s" , mysql_error (self->handle));
    }
    else {
        count = mysql_fetch_row (self->result);

        if (atoi (count[0]) == 0)
        {
            // Insert a new Commander
            if (MySQL_query (self,
                "INSERT INTO commander ("
                "`commander_id`, `account_id`, `commander_num`, `name`, `family_name`, `class`, "
                "`gender`, `level`, `head_top`, `head_middle`, `necklace`, `body_armor`, `leg_armor`, "
                "`gloves`, `shoes`, `weapon`, `shield`, `costume`, `ring`, `bracelet_left`, "
                "`bracelet_right`, `hair_type`, `last_map`, `last_x`, `last_y`, `current_xp`, `pose`, "
                "`current_hp`, `current_sp`) VALUES ("
                "%u, %lu, %d, '%s', '%s', %d, "
                "%d, %d, %d, %d, %d, %d, %d, "
                "%d, %d, %d, %d, %d, %d, %d, "
                "%d, %d, %d, %d, %d, %d, %d, "
                "%d, %d)",
                session->game.currentCommander.commanderId,
                session->game.currentCommander.accountId,
                session->game.currentCommander.charPosition,
                session->game.currentCommander.charName,
                session->game.currentCommander.familyName,
                session->game.currentCommander.classId,
                session->game.currentCommander.gender,
                session->game.currentCommander.level,
                session->game.currentCommander.head_top,
                session->game.currentCommander.head_middle,
                session->game.currentCommander.necklace,
                session->game.currentCommander.body_armor,
                session->game.currentCommander.leg_armor,
                session->game.currentCommander.gloves,
                session->game.currentCommander.boots,
                session->game.currentCommander.weapon,
                session->game.currentCommander.shield,
                session->game.currentCommander.costume,
                session->game.currentCommander.bracelet,
                session->game.currentCommander.ring_left,
                session->game.currentCommander.ring_right,
                session->game.currentCommander.hairType,
                session->game.currentCommander.mapId,
                session->game.currentCommander.cPosX,
                session->game.currentCommander.cPosY,
                session->game.currentCommander.currentXP,
                session->game.currentCommander.pose,
                session->game.currentCommander.currentHP,
                session->game.currentCommander.currentSP
            )) {
                error ("SQL Error : %s" , mysql_error (self->handle));
            }
            dbg ("Inserting a new commander");
        }
        else {
            // Update the commander
            dbg ("Updating the commander");
        }
    }
}
