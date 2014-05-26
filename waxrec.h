#ifndef __WAXREC_H
#define __WAXREC_H

#include <sys/types.h>

#ifdef __cplusplus
extern "C" {
#endif

/* WAX structures */
#define MAX_SAMPLES 32
typedef struct
{
    unsigned long long timestamp;
    unsigned short sampleIndex;
    short x, y, z;
} WaxSample;
typedef struct
{
    unsigned long long timestamp;
    unsigned short deviceId;
    unsigned short sequenceId;
    unsigned char sampleCount;
    WaxSample samples[MAX_SAMPLES];
} WaxPacket;

/**
 * Read SLIP-encoded packet bytes in to buffer from the device opened on fd.
 * Returns number of bytes recieved.
 */
size_t WAX_slipread(int fd, void *buffer, size_t len);

/**
 * Parses a binary wax packet from the given buffer of length len
 */
WaxPacket * WAX_parseWaxPacket(const void *buffer, size_t len, unsigned long long now);

/**
 * Opens a given serial port
 */
int WAX_openport(const char *file, char writeable, int timeout);

/**
 * Return the current time
 */
unsigned long long WAX_TicksNow();

/**
 * Parse SLIP-encoded packets, log or convert to UDP packets
 */
int waxrec(const char *infile, const char *host, const char *initString, const char *logfile, char tee, char dump, char timetag, char sendOnly, const char *stompHost, const char *stompAddress, const char *stompUser, const char *stompPassword, int writeFromUdp, int timeformat, char convertToOsc, int format, char ignoreInvalid, const char *waitPrefix, int waitTimeout);

/**
 * Sends a command string to the connected device
 */
void WAX_sendcommand(int fd, const char * string);

#ifdef __cplusplus
}
#endif

#endif
