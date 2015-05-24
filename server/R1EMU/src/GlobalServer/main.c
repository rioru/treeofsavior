/**
 *
 *   ██████╗   ██╗ ███████╗ ███╗   ███╗ ██╗   ██╗
 *   ██╔══██╗ ███║ ██╔════╝ ████╗ ████║ ██║   ██║
 *   ██████╔╝ ╚██║ █████╗   ██╔████╔██║ ██║   ██║
 *   ██╔══██╗  ██║ ██╔══╝   ██║╚██╔╝██║ ██║   ██║
 *   ██║  ██║  ██║ ███████╗ ██║ ╚═╝ ██║ ╚██████╔╝
 *   ╚═╝  ╚═╝  ╚═╝ ╚══════╝ ╚═╝     ╚═╝  ╚═════╝
 *
 * @file main.c
 * @brief Entry point of the Global Server
 *
 * Entry point of the Global Server
 *
 * @license <license placeholder>
 */

#include "Common/Server/ServerFactory.h"
#include "GlobalServer/GlobalServer.h"

bool testZlib () {
    #include "Common/utils/zlib.h"
    ZlibPacket zlibPacket[3];

    size_t memSize;
    void *memory = dumpToMem (
        "[11:36:22][           ToSClient:                     dbgBuffer]  29 0C FF FF FF FF 60 01 0F 00 00 00 8D FA 50 01 | ).....`.......P.\n"
        "[11:36:22][           ToSClient:                     dbgBuffer]  63 B8 C1 C9 C0 C6 90 58 B4 EF 21 03 C3 E2 F9 8C | c......X..!.....\n"
        "[11:36:22][           ToSClient:                     dbgBuffer]  0C 8C 0C 10 E0 3C 87 81 21 A7 34 95 81 59 00 C4 | .....<..!.4..Y..\n"
        "[11:36:22][           ToSClient:                     dbgBuffer]  0F 38 02 52 65 A1 F7 EF 0E 44 15 3B 44 11 83 65 | .8.Re....D.;D..e\n"
        "[11:36:22][           ToSClient:                     dbgBuffer]  29 03 83 A9 AE 81 09 44 95 1C 58 95 53 E5 3D A8 | )......D..X.S.=.\n"
        "[11:36:22][           ToSClient:                     dbgBuffer]  2A 26 A8 2A 53 A0 2A 46 06 0E A8 59 92 60 55 0C | *&.*S.*F...Y.`U.\n"
        "[11:36:22][           ToSClient:                     dbgBuffer]  1C A7 6F 43 CC 62 85 AA B2 00 AA 62 E0 67 70 06 | ..oC.b.....b.gp.\n"
        "[11:36:22][           ToSClient:                     dbgBuffer]  9A 95 90 E5 F6 EF 3A C8 C6 1C 86 37 B7 50 DD E5 | ......:....7.P..\n"
        "[11:36:22][           ToSClient:                     dbgBuffer]  04 74 17 13 83 21 D4 AC 74 B0 59 6F 9A AE DC 80 | .t...!..t.Yo....\n"
        "[11:36:22][           ToSClient:                     dbgBuffer]  A8 62 86 9A 65 0E 34 2B 23 31 2F 1D 64 D6 2B 37 | .b..e.4+#1/.d.+7\n"
        "[11:36:22][           ToSClient:                     dbgBuffer]  69 88 8D 26 4E 57 51 6D 34 03 AA F2 07 7A 18 A2 | i..&NWQm4....z..\n"
        "[11:36:22][           ToSClient:                     dbgBuffer]  EA 2F D8 46 06 86 D3 57 50 CD 72 04 DA C8 C0 B0 | ./.F...WP.r.....\n"
        "[11:36:22][           ToSClient:                     dbgBuffer]  C1 11 E2 47 29 B0 59 C7 18 77 40 55 B1 41 6D 34 | ...G).Y..w@U.Am4\n"
        "[11:36:22][           ToSClient:                     dbgBuffer]  01 B9 9E E1 0A 23 44 D5 EA DD BC 40 D7 33 30 6C | .....#D....@.30l\n"
        "[11:36:22][           ToSClient:                     dbgBuffer]  87 AA 32 80 3A EC A7 0A 23 43 28 C4 46 A0 AC CC | ..2.:...#C(.F...\n"
        "[11:36:22][           ToSClient:                     dbgBuffer]  6B 90 1F 19 18 BE 5E 46 F5 63 E2 2B A0 20 43 83 | k.....^F.c.+. C.\n"
        "[11:36:22][           ToSClient:                     dbgBuffer]  3D D0 AC 9A 49 9E 89 60 1B 0B 19 BF A0 A9 32 06 | =...I..`......2.\n"
        "[11:36:22][           ToSClient:                     dbgBuffer]  DB B8 02 64 23 D0 8F 71 10 3F 32 7C 86 AA 82 C5 | ...d#..q.?2|....\n"
        "[11:36:22][           ToSClient:                     dbgBuffer]  90 11 50 D5 25 B8 1F A3 A1 AA 3E A1 A9 32 04 99 | ..P.%.....>..2..\n"
        "[11:36:22][           ToSClient:                     dbgBuffer]  B5 A0 C7 1D 62 56 AF 20 33 83 04 83 1F 23 3B D0 | ....bV. 3....#;.\n"
        "[11:36:22][           ToSClient:                     dbgBuffer]  71 25 48 69 A2 53 18 E4 AE 52 C6 5F FC 0C 1F 82 | q%Hi.S...R._....\n"
        "[11:36:22][           ToSClient:                     dbgBuffer]  5D 7B 04 80 5A 1C 7A 81 64 83 C3 42 71 90 0C 00 | ]{..Z.z.d..Bq...\n"
      , NULL, &memSize
    );

    if (!(ZlibPacket_decompress (&zlibPacket[0], (char *) memory + 16, 0x150))) {
        error ("TEST FAILED.");
        return false;
    }
    buffer_print (zlibPacket[0].buffer, zlibPacket[0].header.size, ">");

    if (!(ZlibPacket_compress (&zlibPacket[1], zlibPacket[0].buffer, zlibPacket[0].header.size))) {
        error ("TEST FAILED.");
        return false;
    }

    if (!(ZlibPacket_decompress (&zlibPacket[2], zlibPacket[1].buffer, zlibPacket[0].header.size))) {
        error ("TEST FAILED.");
        return false;
    }

    if (memcmp (zlibPacket[0].buffer, zlibPacket[2].buffer, zlibPacket[0].header.size) != 0) {
        error ("TEST FAILED.");
        return false;
    }

    info ("Test success !");
    return true;
}

int main (int argc, char **argv)
{
    if (!testZlib ()) {
        // return 0;
    }

    GlobalServer *globalServer;

    // Force the initialization of the CZMQ layer here.
    if (!(zsys_init ())) {
        error ("Cannot init CZMQ.");
        goto cleanup;
    }

    // Get the configuration file
    char *confFilePath;
    if (argc >= 2) {
        confFilePath = argv[1];
    } else {
        confFilePath = DEFAULT_SERVER_CONF_PATH;
    }

    GlobalServerStartupInfo info;
    if (!(GlobalServerStartupInfo_init (&info, confFilePath))) {
        error ("Cannot initialize GlobalServer init information. (%s)", confFilePath);
        goto cleanup;
    }

    // Initialize the Global Server
    if (!(globalServer = GlobalServer_new (&info))) {
        error ("Cannot initialize the GlobalServer properly.");
        goto cleanup;
    }

    // Flush the Redis server
    if (!(GlobalServer_flushRedis (globalServer))) {
        error ("Cannot flush the Redis server properly.");
        goto cleanup;
    }

    // Start the Global Server
    else if (!GlobalServer_start (globalServer)) {
        error ("Cannot start the GlobalServer properly.");
        goto cleanup;
    }

cleanup:

    // Unload the Global Server properly
    GlobalServer_destroy (&globalServer);

    // Shutdown the CZMQ layer properly
    zsys_shutdown ();

    info ("Press any key to exit...");
    getc (stdout);

    return 0;
}
