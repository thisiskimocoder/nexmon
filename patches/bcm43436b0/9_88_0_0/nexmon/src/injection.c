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

#include <firmware_version.h>
#include <wrapper.h>
#include <structs.h>
#include <patcher.h>
#include <helper.h>
#include <ieee80211_radiotap.h>
#include <sendframe.h>
#include "d11.h"
#include "brcm.h"


#define IEEE80211_RADIOTAP_RATE 1
#define IEEE80211_RADIOTAP_CHANNEL 2 // Define constants for Radiotap fields


int inject_frame(struct wl_info *wl, struct sk_buff *p) {
    int rtap_len = 0;
    struct wlc_info *wlc = wl->wlc;
    int data_rate = 0; // Initialize data_rate
    struct ieee80211_radiotap_iterator iterator;
    struct ieee80211_radiotap_header *rtap_header;

    // More robust Radiotap header length check
    if (p->len < 2) {
        printf("Error: Packet too short for Radiotap header\n");
        return -1;
    }
    rtap_len = p->data[1] << 8 | p->data[0]; // Use both bytes for length

    if (p->len < rtap_len) {
        printf("Error: Invalid Radiotap header length\n");
        return -1;
    }

    rtap_header = (struct ieee80211_radiotap_header *)p->data;

    if (ieee80211_radiotap_iterator_init(&iterator, rtap_header, rtap_len, NULL) != 0) {
        printf("Error initializing Radiotap iterator\n");
        return -1;
    }

    while (ieee80211_radiotap_iterator_next(&iterator) == 0) {
        switch (iterator.this_arg_index) {
            case IEEE80211_RADIOTAP_RATE:
                data_rate = *(uint8_t *)iterator.this_arg; // Correct type cast
                break;
            case IEEE80211_RADIOTAP_CHANNEL:
                // Handle channel information if needed
                break;
            default:
                break; // No need to print "default"
        }
    }

    skb_pull(p, rtap_len);

    // Check for valid data_rate before sending
    if (data_rate == 0) {
        printf("Error: Data rate not found in Radiotap header\n");
        return -1;
    }

    return sendframe(wlc, p, 1, data_rate); // Return the result of sendframe
}

int wl_send_hook(struct hndrte_dev *src, struct hndrte_dev *dev, struct sk_buff *p) {
    struct wl_info *wl = (struct wl_info *)dev->softc;
    struct wlc_info *wlc = wl->wlc;

    // Check for null pointers before dereferencing
    if (!wl || !wlc || !p || !p->data) {
        printf("Error: Invalid arguments to wl_send_hook\n"); // Log the error
        return -1; // Or another appropriate error code
    }

    if (wlc->monitor && p && p->data && ((uint16_t *)p->data)[0] == 0) { // Use uint16_t for clarity
        int ret = inject_frame(wl, p);
        if (ret != 0) {
            printf("Error injecting frame: %d\n", ret); // Log injection errors
        }
        return ret; // Return the result of inject_frame
    } else {
        return wl_send(src, dev, p);
    }
}

// Ensure correct type for address and firmware version
__attribute__((at(0x3E4EC, "", CHIP_VER_BCM43436b0, FW_VER(9, 88, 0, 0)))) // Use FW_VER macro
GenericPatch4(wl_send_hook, wl_send_hook + 1); // Assuming GenericPatch4 is defined elsewhere
