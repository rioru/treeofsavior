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
    if (MySQL_query (self, "SELECT count(*) FROM commander WHERE commander_id = %u", session->game.currentCommander.commanderId)) {
        error ("SQL Error : %s" , mysql_error (self->handle));
    }
    else {
        count = mysql_fetch_row (self->result);

        if (atoi (count[0]) == 0)
        {
            // Insert a new Commander
            if (MySQL_query (self,
                "INSERT INTO commander ("
                "`commander_id`, `account_id`, `name`, `family_name`, `class`, "
                "`gender`, `level`, `head_top`, `head_middle`, `necklace`, `body_armor`, `leg_armor`, "
                "`gloves`, `shoes`, `weapon`, `shield`, `costume`, `ring`, `bracelet_left`, "
                "`bracelet_right`, `hair_type`, `last_x`, `last_y`, `current_xp`, "
                "`current_hp`, `current_sp`) VALUES ("
                "%u, %lu, '%s', '%s', %d, "
                "%d, %d, %d, %d, %d, %d, %d, "
                "%d, %d, %d, %d, %d, %d, %d, "
                "%d, %d, %d, %d, %d, "
                "%d, %d)",
                session->game.currentCommander.commanderId,
                session->game.currentCommander.base.accountId,
                session->game.currentCommander.base.commanderName,
                session->game.currentCommander.base.familyName,
                session->game.currentCommander.base.classId,
                session->game.currentCommander.base.gender,
                session->game.currentCommander.base.level,
                session->game.currentCommander.base.equipment.head_top,
                session->game.currentCommander.base.equipment.head_middle,
                session->game.currentCommander.base.equipment.necklace,
                session->game.currentCommander.base.equipment.body_armor,
                session->game.currentCommander.base.equipment.leg_armor,
                session->game.currentCommander.base.equipment.gloves,
                session->game.currentCommander.base.equipment.boots,
                session->game.currentCommander.base.equipment.weapon,
                session->game.currentCommander.base.equipment.shield,
                session->game.currentCommander.base.equipment.costume,
                session->game.currentCommander.base.equipment.bracelet,
                session->game.currentCommander.base.equipment.ring_left,
                session->game.currentCommander.base.equipment.ring_right,
                session->game.currentCommander.base.hairType,
                session->game.currentCommander.pos.x,
                session->game.currentCommander.pos.z,
                session->game.currentCommander.currentXP,
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
