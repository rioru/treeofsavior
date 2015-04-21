/**
*
*   ██████╗   ██╗ ███████╗ ███╗   ███╗ ██╗   ██╗
*   ██╔══██╗ ███║ ██╔════╝ ████╗ ████║ ██║   ██║
*   ██████╔╝ ╚██║ █████╗   ██╔████╔██║ ██║   ██║
*   ██╔══██╗  ██║ ██╔══╝   ██║╚██╔╝██║ ██║   ██║
*   ██║  ██║  ██║ ███████╗ ██║ ╚═╝ ██║ ╚██████╔╝
*   ╚═╝  ╚═╝  ╚═╝ ╚══════╝ ╚═╝     ╚═╝  ╚═════╝
*
* @file MySQL.h
* @brief MySQL contains all database related functions
*
* @license <license placeholder>
*/

#pragma once

// ---------- Includes ------------
#include "R1EMU.h"

// ---------- Defines -------------
typedef enum MySQLStatus {

    SQL_SUCCESS = 0,
    SQL_ERROR = -1,

}   MySQLStatus;

#define MAX_QUERY_SIZE 1024

#define MYSQL_HOSTNAME_DEFAULT     (char []) {"localhost"}
#define MYSQL_LOGIN_DEFAULT        (char []) {"r1emu"}
#define MYSQL_PASSWORD_DEFAULT     (char []) {"r1emu"}
#define MYSQL_DATABASE_DEFAULT     (char []) {"r1emu"}

// ------ Structure declaration -------
typedef struct MySQL MySQL;

typedef struct MySQLInfo
{
    char *hostname;
    char *login;
    char *password;
    char *database;

}   MySQLInfo;

// ----------- Functions ------------

/**
 * @brief Allocate a new MySQL structure.
 * @return A pointer to an allocated MySQL, or NULL if an error occured.
 */
MySQL *
MySQL_new (
    void
);

/**
 * @brief Initialize an allocated MySQL structure.
 * @param self An allocated MySQL to initialize.
 * @return true on success, false otherwise.
 */
bool
MySQL_init (
    MySQL *self
);

/**
 * @brief : Connect to the MySQL database
 * @param self An allocated MySQL instance
 * @param sqlInfo The information about the SQL database connection to etablish
 * @return true on success, false otherwise
 */
bool
MySQL_connect (
    MySQL *self,
    MySQLInfo *sqlInfo
);

/**
 * @brief
 * @param
 */
MySQLStatus
MySQL_query (
	MySQL *self,
	const char *query,
	...
);


/**
 * @brief
 * @param
 */
void
MySQL_freeResult (
	MySQL *self
);


/**
 * @brief Free an allocated MySQL structure and nullify the content of the pointer
 * @param self A pointer to an allocated MySQL
 */
void
MySQL_destroy (
    MySQL **_self
);

