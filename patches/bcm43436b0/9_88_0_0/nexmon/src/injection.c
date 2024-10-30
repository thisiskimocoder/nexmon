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

// Debugging configuration
#define VERBOSE_DEBUG 1  // Set to 1 for detailed debug output; 0 for minimal output
#define HEX_DUMP 1        // Set to 1 to enable hex dump; 0 to disable
#define RETRY_ENABLED 1
#if RETRY_ENABLED
#define RETRY_LIMIT 3
#endif

void print_hex(const char *desc, const void *addr, int len) {
    if (HEX_DUMP) {
        printf("%s: ", desc);
        const unsigned char *p = addr;
        for (int i = 0; i < len; i++) {
            printf("%02X ", p[i]);
        }
        printf("\n");
    }
}

int inject_frame(struct wl_info *wl, sk_buff *p) {
    int rtap_len = 0;
    struct wlc_info *wlc = wl->wlc;
    int data_rate = 0;

    struct ieee80211_radiotap_iterator iterator;
    struct ieee80211_radiotap_header *rtap_header;

    rtap_len = *((char *)(p->data + 2));
    rtap_header = (struct ieee80211_radiotap_header *) p->data;

    if (VERBOSE_DEBUG) {
        printf("RTAP length: %d\n", rtap_len);
        print_hex("RTAP header", rtap_header, rtap_len);
    }

    int ret = ieee80211_radiotap_iterator_init(&iterator, rtap_header, rtap_len, 0);

    while (!ret) {
        ret = ieee80211_radiotap_iterator_next(&iterator);
        if (ret) {
            continue;
        }
        switch (iterator.this_arg_index) {
            case IEEE80211_RADIOTAP_RATE:
                data_rate = (*iterator.this_arg);
                printf("Data rate: %d\n", data_rate);
                break;
            case IEEE80211_RADIOTAP_CHANNEL:
                int channel_freq = iterator.this_arg[0] | (iterator.this_arg[1] << 8);
                printf("Channel (freq): %d\n", channel_freq);
                break;
            default:
                printf("Unhandled field index: %d\n", iterator.this_arg_index);
                break;
        }
    }

    skb_pull(p, rtap_len);

    int retries = RETRY_ENABLED ? RETRY_LIMIT : 1;
    for (int attempt = 0; attempt < retries; ++attempt) {
        int success = sendframe(wlc, p, 1, data_rate);
        if (success == 0) {
            printf("Frame sent successfully on attempt %d\n", attempt + 1);
            break;  // Exit if sendframe succeeds
        }
        if (attempt == retries - 1) {
            printf("Send failed after %d attempts, error code: %d\n", retries, success);
        }
    }

    return 0;
}

int wl_send_hook(struct hndrte_dev *src, struct hndrte_dev *dev, struct sk_buff *p) {
    struct wl_info *wl = (struct wl_info *) dev->softc;
    struct wlc_info *wlc = wl->wlc;

    if (wlc->monitor && p != 0 && p->data != 0 && ((short *) p->data)[0] == 0) {
        return inject_frame(wl, p);
    } else {
        return wl_send(src, dev, p);
    }
}

__attribute__((at(0x3E4EC, "", CHIP_VER_BCM43436b0, FW_VER_9_88_0_0)))
GenericPatch4(wl_send_hook, wl_send_hook + 1);
