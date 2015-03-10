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
#include "R1EMU.h"


// ------ Structure declaration -------


// ------ Static declaration -------


// ------ Extern function implementation -------
uint64_t zmq_clock_now ()
{
#if defined WIN32

    //  Get the high resolution counter's accuracy.
    LARGE_INTEGER ticksPerSecond;
    QueryPerformanceFrequency (&ticksPerSecond);

    //  What time is it?
    LARGE_INTEGER tick;
    QueryPerformanceCounter (&tick);

    //  Convert the tick number into the number of seconds
    //  since the system was started.
    double ticks_div = ticksPerSecond.QuadPart / 1000000.0;
    return (uint64_t) (tick.QuadPart / ticks_div);

#elif defined HAVE_CLOCK_GETTIME && defined CLOCK_MONOTONIC

    //  Use POSIX clock_gettime function to get precise monotonic time.
    struct timespec tv;
    int rc = clock_gettime (CLOCK_MONOTONIC, &tv);
		// Fix case where system has clock_gettime but CLOCK_MONOTONIC is not supported.
		// This should be a configuration check, but I looked into it and writing an
		// AC_FUNC_CLOCK_MONOTONIC seems beyond my powers.
		if( rc != 0) {
			//  Use POSIX gettimeofday function to get precise time.
			struct timeval tv;
			int rc = gettimeofday (&tv, NULL);
			errno_assert (rc == 0);
			return (tv.tv_sec * (uint64_t) 1000000 + tv.tv_usec);
		}
    return (tv.tv_sec * (uint64_t) 1000000 + tv.tv_nsec / 1000);

#elif defined HAVE_GETHRTIME

    return (gethrtime () / 1000);

#else

    //  Use POSIX gettimeofday function to get precise time.
    struct timeval tv;
    int rc = gettimeofday (&tv, NULL);
    assert (rc == 0);
    return (tv.tv_sec * (uint64_t) 1000000 + tv.tv_usec);

#endif
}


void R1EMU_seed_random ()
{
    #if defined ZMQ_HAVE_WINDOWS
        int pid = (int) GetCurrentProcessId ();
    #else
        int pid = (int) getpid ();
    #endif
    srand ((unsigned int) (zmq_clock_now () + pid));
}

uint32_t R1EMU_generate_random ()
{
    // Compensate for the fact that rand() returns signed integer.
    uint32_t low = (uint32_t) rand ();
    uint32_t high = (uint32_t) rand ();
    high <<= (sizeof (high) * 8 - 1);
    return high | low;
}

uint64_t R1EMU_generate_random64 ()
{
    uint32_t low  = R1EMU_generate_random ();
    uint32_t high = R1EMU_generate_random ();
    high <<= (sizeof (high) * 8 - 1);
    return high | low;
}
