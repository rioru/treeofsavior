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


// ------ Static declaration -------


// ------ Extern function implementation ------

int
MySQL_query (
	SQL *self,
	const char* query,
	...
) {
	va_list	args;
	char	buf[MAX_QUERY_SIZE];

	if (self->handle == NULL)
		return SQL_ERROR;
	MySQL_freeResult(self);
	va_start(args, query);
		vsnprintf (buf, MAX_QUERY_SIZE - 1, query, args);
	va_end(args);
	dbg ("Launching SQL command : %s", buf);
	if (mysql_real_query(self->handle, buf, (unsigned long)strlen(buf)))
		return SQL_ERROR;
	self->result = mysql_store_result(self->handle);
	return SQL_SUCCESS;
}

void
MySQL_freeResult (
	SQL *self
) {
	if (self && self->result)
	{
		mysql_free_result(self->result);
		self->result = NULL;
	}
}
