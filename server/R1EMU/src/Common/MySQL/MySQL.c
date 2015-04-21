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
#include "MySQL.h"


// ------ Structure declaration -------
struct MySQL {
	MYSQL		*handle;
	MYSQL_RES	*result;
};

// ------ Static declaration -------


// ------ Extern function implementation ------

MySQL *
MySQL_new (
    void
) {
    MySQL *self;

    if ((self = calloc (1, sizeof (MySQL))) == NULL) {
        return NULL;
    }

    if (!MySQL_init (self)) {
        MySQL_destroy (&self);
        error ("MySQL failed to initialize.");
        return NULL;
    }

    return self;
}

bool
MySQL_init (
    MySQL *self
) {
    self->result = NULL;
    self->handle = mysql_init (NULL);

    return true;
}

bool
MySQL_connect (
    MySQL *self,
    MySQLInfo *sqlInfo
) {
    info ("Connecting to the MySQL Server (%s@%s -> %s)...", sqlInfo->login, sqlInfo->hostname, sqlInfo->database);

    if (!mysql_real_connect (self->handle, sqlInfo->hostname, sqlInfo->login, sqlInfo->password, sqlInfo->database, 0, NULL, 0)) {
        error ("Could not connect to the '%s' database (%s@%s. (mysql_errno = %d)",
            sqlInfo->database, sqlInfo->login, sqlInfo->hostname, mysql_errno (self->handle));
        return false;
    }

    info ("Connected to the MySQL Server !");
    return true;
}

MySQLStatus
MySQL_query (
	MySQL *self,
	const char* query,
	...
) {
	va_list	args;
	char buf[MAX_QUERY_SIZE];

	if (self->handle == NULL) {
		return SQL_ERROR;
	}

	MySQL_freeResult(self);

	va_start (args, query);
		vsnprintf (buf, MAX_QUERY_SIZE - 1, query, args);
	va_end (args);

	dbg ("Launching SQL command : %s", buf);

	if (mysql_real_query (self->handle, buf, (unsigned long) strlen (buf))) {
		return SQL_ERROR;
	}

	self->result = mysql_store_result (self->handle);

	return SQL_SUCCESS;
}

void
MySQL_freeResult (
	MySQL *self
) {
	if (self && self->result) {
		mysql_free_result (self->result);
		self->result = NULL;
	}
}


void
MySQL_destroy (
    MySQL **_self
) {
    MySQL *self = *_self;

    free (self);
    *_self = NULL;
}
