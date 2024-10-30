/***************************************************************************
 *                                                                         *
 *          ###########   ###########   ##########    ##########           *
 *         ############  ############  ############  ############          *
 *         ##            ##            ##   ##   ##  ##        ##          *
 *         ##            ##            ##   ##   ##  ##        ##          *
 *         ###########   ####  ######  ##   ##   ##  ##    ######          *
 *          ###########  ####  #       ##   ##   ##  ##    #    #          *
 *                   ##  ##    ######  ##   ##   ##  ##    #    #          *
 *                   ##  ##    #       ##   ##   ##  ##    #    #          *
 *         ############  ##### ######  ##   ##   ##  ##### ######          *
 *         ###########    ###########  ##   ##   ##   ##########           *
 *                                                                         *
 *            S E C U R E   M O B I L E   N E T W O R K I N G              *
 *                                                                         *
 * This file is part of NexMon.                                            *
 *                                                                         *
 * Copyright (c) 2024 NexMon Team                                          *
 *                                                                         *
 * NexMon is free software: you can redistribute it and/or modify          *
 * it under the terms of the GNU General Public License as published by    *
 * the Free Software Foundation, either version 3 of the License, or       *
 * (at your option) any later version.                                     *
 *                                                                         *
 * NexMon is distributed in the hope that it will be useful,               *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of          *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the           *
 * GNU General Public License for more details.                            *
 *                                                                         *
 * You should have received a copy of the GNU General Public License       *
 * along with NexMon. If not, see <http://www.gnu.org/licenses/>.          *
 *                                                                         *
 **************************************************************************/
 /**************************************************************************

 Notes:
    * Added HEX dump of payload (find packet corruptions)
    * Added metrics for function performance
    * Added debug around most functions
    * Added configurable debug/features switches
    * Added debug levels 'INFO, WARN, and ERROR' for targeted log control
      - Used scoped clock_gettime calls to keep timing specific to each function call
      - Outputs additional details for structured debugging, including hex dumps and SCB info.

 TODO:
    * Implement retry control if hardware of device is unavailable / busy

 Switches:
    - DEBUG_ENABLED: Enables or disables all debug print statements.
    - HEX_DUMP_ENABLED: Enables or disables the hex dump output of the packet data.
    - TIMING_ENABLED: Controls whether timing information for the function execution is recorded and printed.
 */


#pragma NEXMON targetregion "patch"

#include <wrapper.h>            // wrapper definitions for functions that already exist in the firmware
#include <structs.h>            // structures that are used by the code in the firmware
#include <patcher.h>            // macros used to create patches such as BLPatch, BPatch, ...
#include <rates.h>              // rates used to build the ratespec for frame injection
#include <time.h>               // track metrics for function performance

// Configurable options
#define DEBUG_ENABLED 1        // Enable/Disable all debug messages
#define HEX_DUMP_ENABLED 1     // Enable/Disable hex dump output
#define TIMING_ENABLED 1       // Enable/Disable function performance timing

// Below is NOT configurable switches, but debug log levels. (don't change!)
#define INFO 1
#define WARN 2
#define ERROR 3

int debug_level = INFO; // Set initial debug level

#if DEBUG_ENABLED
    #define DEBUG_PRINT(level, fmt, ...) \
        do { if (level >= debug_level) printf(fmt, __VA_ARGS__); } while (0)
#else
    #define DEBUG_PRINT(level, fmt, ...) // No-op
#endif

#if HEX_DUMP_ENABLED
void hex_dump(const void *data, size_t size) {
    const unsigned char *byte = data;
    for (size_t i = 0; i < size; i++) {
        printf("%02x ", byte[i]);
        if ((i + 1) % 16 == 0) printf("\n");
    }
    printf("\n");
}
#else
    #define hex_dump(data, size) // No-op
#endif

char
sendframe(struct wlc_info *wlc, struct sk_buff *p, unsigned int fifo, unsigned int rate)
{
    char ret = 0;

    #if TIMING_ENABLED
        struct timespec start, end;
        clock_gettime(CLOCK_MONOTONIC, &start); // Start timing
    #endif

    // Debug Entry Point
    DEBUG_PRINT(INFO, "Entering %s: wlc=0x%08x, p=0x%08x, fifo=%d, rate=%d\n", __FUNCTION__, wlc, p, fifo, rate);

    // Print sk_buff details
    DEBUG_PRINT(INFO, "Packet Info: len=%d, data=0x%08x, protocol=0x%04x\n", p->len, p->data, p->protocol);

    p->scb = wlc->band->hwrs_scb;

    // Debug SCB Info
    DEBUG_PRINT(INFO, "SCB Info: hwrs_scb=0x%08x, bandtype=%d\n", wlc->band->hwrs_scb, wlc->band->bandtype);

    // Ensure minimum rate for 5G
    if (wlc->band->bandtype == WLC_BAND_5G && rate < RATES_RATE_6M) {
        rate = RATES_RATE_6M;
        DEBUG_PRINT(WARN, "Adjusted rate to 6M for 5G band.\n");
    }

    // Check if hardware is up
    if (wlc->hw->up) {
        // Original send control - Uncomment to enable actual send
        // ret = wlc_sendctl(wlc, p, wlc->active_queue, wlc->band->hwrs_scb, fifo, rate, 0);
        DEBUG_PRINT(INFO, "%s: wlc=0x%08x, p=0x%08x, queue=0x%08x, scb=0x%08x, fifo=%d, rate=%d\n",
                    __FUNCTION__, wlc, p, wlc->active_queue, wlc->band->hwrs_scb, fifo, rate);
    } else {
        DEBUG_PRINT(ERROR, "ERR: wlc is down. Cannot send frame. Status flags: 0x%08x\n", wlc->hw->flags);
        ret = -1;  // Return an error code when hardware is down
    }

    // Hex dump of the packet data to reveal any unexpected patterns or issues
    DEBUG_PRINT(INFO, "Packet Hex Dump:\n");
    hex_dump(p->data, p->len);

    #if TIMING_ENABLED
        // Log function execution time
        clock_gettime(CLOCK_MONOTONIC, &end); // End timing
        long elapsed_ms = (end.tv_sec - start.tv_sec) * 1000 + (end.tv_nsec - start.tv_nsec) / 1000000;
        DEBUG_PRINT(INFO, "Execution time for %s: %ld ms\n", __FUNCTION__, elapsed_ms);
    #endif

    DEBUG_PRINT(INFO, "Exiting %s with return value=%d\n", __FUNCTION__, ret);

    return ret;
}

// Add a patch if needed, depending on where this function needs to be injected.
// Example (replace with correct address and firmware version):
// __attribute__((at(0xYOUR_PATCH_ADDRESS, "", CHIP_VER_YOUR_CHIP, FW_VER(X, X, X, X))))
// GenericPatch4(sendframe, sendframe + 1);
