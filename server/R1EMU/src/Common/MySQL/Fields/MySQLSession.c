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
                session->game.currentCommander.accountId,
                session->game.currentCommander.commanderName,
                session->game.currentCommander.familyName,
                session->game.currentCommander.classId,
                session->game.currentCommander.gender,
                session->game.currentCommander.level,
                session->game.currentCommander.equipment.head_top,
                session->game.currentCommander.equipment.head_middle,
                session->game.currentCommander.equipment.necklace,
                session->game.currentCommander.equipment.body_armor,
                session->game.currentCommander.equipment.leg_armor,
                session->game.currentCommander.equipment.gloves,
                session->game.currentCommander.equipment.boots,
                session->game.currentCommander.equipment.weapon,
                session->game.currentCommander.equipment.shield,
                session->game.currentCommander.equipment.costume,
                session->game.currentCommander.equipment.bracelet,
                session->game.currentCommander.equipment.ring_left,
                session->game.currentCommander.equipment.ring_right,
                session->game.currentCommander.hairType,
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
