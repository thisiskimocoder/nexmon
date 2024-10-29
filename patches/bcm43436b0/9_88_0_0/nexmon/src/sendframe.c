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

#pragma NEXMON targetregion "patch"

#include <wrapper.h>            // wrapper definitions for functions that already exist in the firmware
#include <structs.h>            // structures that are used by the code in the firmware
#include <patcher.h>            // macros used to craete patches such as BLPatch, BPatch, ...
#include <rates.h>              // rates used to build the ratespec for frame injection


char sendframe(struct wlc_info *wlc, struct sk_buff *p, unsigned int fifo, unsigned int rate) {
    if (!wlc || !p) {  // Check for null pointers
        return -1; // Indicate an error
    }

    p->scb = wlc->band->hwrs_scb;

    // Check wlc->band for null pointer before dereferencing
    if (wlc->band && wlc->band->bandtype == WLC_BAND_5G && rate < RATES_RATE_6M) {
        rate = RATES_RATE_6M;
    }

    char ret;
    if (wlc->hw && wlc->hw->up) { // Check wlc->hw for null pointer
        ret = wlc_sendctl(wlc, p, wlc->active_queue, wlc->band->hwrs_scb, fifo, rate, 0);
    } else {
        // Consider logging the error using a dedicated logging mechanism if available
        ret = wlc_sendctl(wlc, p, wlc->active_queue, wlc->band->hwrs_scb, fifo, rate, 1);
        printf("ERR: wlc down\n"); // Or use a more appropriate logging function
    }
    return ret;
}

// Add a patch if needed, depending on where this function needs to be injected.
// Example (replace with correct address and firmware version):
// __attribute__((at(0xYOUR_PATCH_ADDRESS, "", CHIP_VER_YOUR_CHIP, FW_VER(X, X, X, X))))
// GenericPatch4(sendframe, sendframe + 1);
