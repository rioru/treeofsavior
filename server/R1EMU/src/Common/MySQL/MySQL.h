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
#define SQL_ERROR -1
#define SQL_SUCCESS 0
#define MAX_QUERY_SIZE 1024

// ------ Structure declaration -------

typedef struct	SQL
{
	MYSQL		*handle;
	MYSQL_RES	*result;
}				SQL;

// ----------- Functions ------------

int
MySQL_query (
	SQL *self,
	const char *query,
	...
);

void
MySQL_freeResult (
	SQL *self
);
