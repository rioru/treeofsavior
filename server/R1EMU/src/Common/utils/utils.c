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
#include "utils.h"


// ------ Structure declaration -------


// ------ Static declaration -------


// ------ Extern function implementation -------

int
str_getline (
    char *str,
    char *dest,
    int maxlen,
    int pos
) {
	int len = strlen (str);
	int i;
	int bufpos = 0;
	char c;

	if (pos >= len) {
        return -1;
	}

	for (i = pos ; i < len; i++)
	{
		if (bufpos >= maxlen)
			return i;

		c = str[i];
		dest[bufpos++] = str[i];

		if (c == '\n')
		{
			dest[--bufpos] = '\0';
			return i + 1;
		}
	}

	dest[bufpos] = '\0';
	return len + 1;
}

void *dumpToMem (
    char *dump,
    void *memory,
    size_t *memSize
) {
    char buffer[1000];
    char localMem[1000000];
    size_t dumpPos = 0;
    size_t memPos = 0;
    unsigned char *memoryBytes = (unsigned char *) memory;
    bool isLocal;

    if (memory == NULL) {
        memory = localMem;
        memoryBytes = localMem;
        isLocal = true;
    }

    while ((dumpPos = str_getline (dump, buffer, sizeof(buffer) - 1, dumpPos)) != -1)
    {
        char *strPos;
        if ((strPos = strstr (buffer, "dbgBuffer]  ")) != NULL) {
            strPos += strlen ("dbgBuffer]  ");
            char *end = strstr (strPos, " | ");
            *end = 0;

            while (strPos < end) {
                char *newPos;
                unsigned char octet = strtol (strPos, &newPos, 16);
                if (strPos == newPos)
                    break;
                memoryBytes[memPos++] = octet;
                strPos = newPos;
            }
        }
    }

    if (!isLocal) {
        if (*memSize != memPos) {
            dbg ("Warning : memSize != bytes written (%lu != %lu).", (long unsigned) *memSize, (long unsigned) memPos);
        }
    } else {
        memory = malloc (memPos);
        memcpy (memory, localMem, memPos);
        *memSize = memPos;
    }

    return memory;
}
