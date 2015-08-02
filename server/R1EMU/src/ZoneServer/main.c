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
 * @brief Entry point of the Servers
 *
 * Entry point of the Servers
 *
 * @license <license placeholder>
 */

// ---------- Includes ------------
#include "ZoneServer/ZoneServer.h"
#include "BarrackServer/BarrackServer.h"
#include "SocialServer/SocialServer.h"
#include "Common/Server/EventServer.h"
#include "Common/Server/ServerFactory.h"

/* Crash handlers */

#ifdef WIN32
LONG WINAPI crashHandler (EXCEPTION_POINTERS *ExceptionInfo)
{
    DWORD exceptionCode = ExceptionInfo->ExceptionRecord->ExceptionCode;
    uintptr_t ip = ExceptionInfo->ContextRecord->Rip;

    die ("Application crashed at %p. Exception code = %x", ip, exceptionCode);

    return EXCEPTION_EXECUTE_HANDLER;
}
#else
#include <ucontext.h>
#include <execinfo.h>

void
print_trace (void)
{
    void *array[20];
    size_t size;
    char **strings;

    size = backtrace (array, sizeof_array (array));
    strings = backtrace_symbols (array, size);

    error ("Obtained %zd stack frames.", size);

    for (size_t i = 0; i < size; i++) {
        error ("Frame %d : %s", i, strings[i]);
    }

    error ("==================================");

    free (strings);
}

void crashHandler (int sig, siginfo_t *siginfo, void *_context)
{
    int exceptionCode = siginfo->si_errno;
    ucontext_t *context = (ucontext_t*) _context;
    uintptr_t ip = context->uc_mcontext.gregs[REG_RIP];

    if (sig == SIGABRT) {
        print_trace ();
    }

    die ("Application crashed at %p. Exception code = %x", ip, exceptionCode);
}
#endif

int main (int argc, char **argv)
{
    ZoneServer *zoneServer = NULL;
    BarrackServer *barrackServer = NULL;
    SocialServer *socialServer = NULL;

    int curArg = 1;

    // === Crash handler ===
    #ifdef WIN32
    SetUnhandledExceptionFilter (crashHandler);
    #else
    struct sigaction sa = {};
    sa.sa_flags = SA_SIGINFO;
    sigemptyset(&sa.sa_mask);
    sa.sa_sigaction = crashHandler;
    sigaction (SIGSEGV, &sa, NULL);
    sigaction (SIGABRT, &sa, NULL);
    #endif

    // === Read the command line arguments ===
    uint16_t routerId = atoi (argv[curArg++]);

    char *routerIp = argv[curArg++];
    int portsCount = atoi (argv[curArg++]);
    int *ports;
    if (!(ports = malloc (sizeof (int) * portsCount))) {
        error ("Cannot allocate the port array.");
        return -1;
    }
    for (int i = 0; i < portsCount; i++) {
        ports[i] = atoi (argv[curArg++]);
    }
    uint16_t workersCount = atoi (argv[curArg++]);
    char *globalServerIp = argv[curArg++];
    int globalServerPort = atoi (argv[curArg++]);
    char *sqlHostname = argv[curArg++];
    char *sqlUsername = argv[curArg++];
    char *sqlPassword = argv[curArg++];
    char *sqlDatabase = argv[curArg++];
    char *redisHostname = argv[curArg++];
    int redisPort = atoi(argv[curArg++]);
    ServerType serverType = atoi(argv[curArg++]);


    // Set a custom output for linux for each servers
    #ifndef WIN32
    dbg_set_output (fopen (zsys_sprintf ("ZoneServer%d_output.txt", routerId), "w+"));
    #else
    // For Windows, change the console title
    switch (serverType) {
        case SERVER_TYPE_BARRACK:
            SetConsoleTitle (zsys_sprintf ("Barrack (%d)", routerId));
        break;

        case SERVER_TYPE_ZONE:
            SetConsoleTitle (zsys_sprintf ("Zone (%d)", routerId));
        break;

        case SERVER_TYPE_SOCIAL:
            SetConsoleTitle (zsys_sprintf ("Social (%d)", routerId));
        break;

        default :
            SetConsoleTitle (zsys_sprintf ("UNKNOWN (%d)", routerId));
        break;
    }
    #endif

    // === Build the Event Server ===
    EventServer *eventServer;
    EventServerStartupInfo eventServerInfo;
    if (!(EventServerStartupInfo_init (&eventServerInfo, routerId, workersCount, redisHostname, redisPort))) {
        error ("Cannot initialize the event server.");
        return -1;
    }
    if (!(eventServer = EventServer_new (&eventServerInfo))) {
        error ("Cannot create the event server.");
        return -1;
    }
    if ((zthread_new ((zthread_detached_fn *) EventServer_start, eventServer)) != 0) {
        error ("Cannot start the event server.");
        return -1;
    }

    // === Build the Zone Server ===
    Server *server;
    if (!(server = ServerFactory_createServer (
        serverType,
        routerId, routerIp,
        portsCount, ports,
        workersCount,
        globalServerIp, globalServerPort,
        sqlHostname, sqlUsername, sqlPassword, sqlDatabase,
        redisHostname, redisPort
    ))) {
        error ("Cannot create a Server.");
        return -1;
    }

    // Initialize the Server
    switch (serverType)
    {
        case SERVER_TYPE_BARRACK:
            // Initialize the Barrack Server
            if ((barrackServer = BarrackServer_new (server))) {

                // Start the Barrack Server
                if (!BarrackServer_start (barrackServer)) {
                    error ("Cannot start the BarrackServer properly.");
                }

                // Unload the Barrack Server properly
                BarrackServer_destroy (&barrackServer);
            }
            else {
                error ("Cannot initialize the BarrackServer properly.");
            }
        break;

        case SERVER_TYPE_ZONE:
            // Initialize the Zone Server
            if ((zoneServer = ZoneServer_new (server))) {

                // Start the Zone Server
                if (!ZoneServer_start (zoneServer)) {
                    error ("Cannot start the Zone Server properly.");
                }

                // Unload the Zone Server properly
                ZoneServer_destroy (&zoneServer);
            }
            else {
                error ("Cannot initialize the Zone Server properly.");
            }
        break;

        case SERVER_TYPE_SOCIAL:
            // Initialize the Social Server
            if ((socialServer = SocialServer_new (server))) {

                // Start the Social Server
                if (!SocialServer_start (socialServer)) {
                    error ("Cannot start the Social Server properly.");
                }

                // Unload the Social Server properly
                SocialServer_destroy (&socialServer);
            }
            else {
                error ("Cannot initialize the Social Server properly.");
            }
        break;

        default :
            error ("Cannot start an unknown serverType.");
        break;
    }

    // Shutdown the CZMQ layer properly
    zsys_shutdown ();

    // Close the custom debug file if necessary
    dbg_close ();

    return 0;
}
