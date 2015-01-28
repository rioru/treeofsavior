#include <windows.h>
#include <stdio.h>
#include "Utils/Utils.h"

#include "tableItems.h"
#include "tableMaps.h"

unsigned int dtb_xor_x360 (unsigned int data, unsigned int mask)
{
    ldiv_t v5 = ldiv (data, 0x1F31Du);
    return (0x41A7 * v5.rem - 0xB14 * v5.quot + ((char)-(0x41A7 * v5.rem - 0xB14 * v5.quot < 0) & 0x7FFFFFFF)) & mask;
}

unsigned int getItemIndex (unsigned int itemId, unsigned int size, unsigned int mask)
{
    unsigned int dtbSum = dtb_xor_x360 (itemId, mask);
    if (size <= dtbSum )
        dtbSum += -1 - ((unsigned int) 0xFFF >> 1);
    return dtbSum;
}

/*
Utilitaire pour convertir la sortie d'Ollyplgn vers un fichier
void
outputData (unsigned char *data, int sizeData)
{
    FILE *output = fopen("output.txt", "w+");
    for (int i = 0; i < sizeData; i++)
    {
        fprintf (output, "0x%08X, ", ((unsigned int *)data)[i]);
        if (i % 8 == 7) fprintf(output, "\n");
    }
    fclose(output);
}
*/

int main (int argc, char **argv)
{
    int mapID = 0xD34;
    int itemID = 0x9D89D;

    printf("itemsID = [");
    int nbItem = 0;
    for (int i = itemID; i < itemID + 100; i++) {
        int itemIndex = getItemIndex(i, sizeof_array (tableItems) / 2, 0xFFF);
        unsigned int item = tableItems[2 * itemIndex];
        if (item != 0x4a04110) {
            printf("0x%x", i);
            if (++nbItem >= 20)
                break;
            else
                printf (", ");
        }
    }
    printf("];\n");

    int mapIndex = getItemIndex(mapID, sizeof_array(tableMapId) / 2, 0x1FF);
    unsigned int map = tableMapId[2 * mapIndex];
    printf("idx %x => %x\n", mapIndex, map);

    return 0;
}
