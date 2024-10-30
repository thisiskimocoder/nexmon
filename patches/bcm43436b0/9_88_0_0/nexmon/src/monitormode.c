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
 * Copyright (c) 2016 NexMon Team                                          *
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
/***************************************************************************
 *
 * changelog
 *
 *    * Added more debug
 *    * Added enhanced logging mechanism
 *    * Added packet filtering
 *    * The should_monitor_packet() function determines whether a packet should be processed
 *      based on its type (currently set to monitor only management frames).
 *      This feature helps reduce unnecessary processing and logging of irrelevant packets.
 *
 *    * Added packet detail logging
 *      - The log_packet_details() function logs details about the packet,
 *        including its length and optionally a hex dump of the packet data if #define ENABLE_HEX_DUMP is active.
 *
 *      - hex dump: The hex dump functionality is controlled via a define switch to provide a
 *        visual representation of the packet data.
 *
 *    * Added statistics tracking feature
 *      Packet Count Tracking: Two static integers (packet_count and filtered_packet_count)
 *      keep track of the total packets received and how many were filtered out.
 *    * Added timestamp logging:
 *      - The log_current_timestamp() function logs
 *        the current timestamp using ctime to convert it into a human-readable format.
 *        the timestamp option is integrated into monitor hook;
 *
 *      - The call to log_current_timestamp() is added in the wl_monitor_hook() function
 *        to log the time whenever a packet is processed.
 *
 */

#pragma NEXMON targetregion "patch"

#include <firmware_version.h>
#include <wrapper.h>  // wrapper definitions for functions that already exist in the firmware
#include <structs.h>  // structures that are used by the code in the firmware
#include <patcher.h>
#include <helper.h>
#include <brcm.h>
#include <d11.h>
#include <ieee80211_radiotap.h>

#define MONITOR_DISABLED  0
#define MONITOR_IEEE80211 1
#define MONITOR_RADIOTAP  2
#define MONITOR_LOG_ONLY  3
#define MONITOR_DROP_FRM  4
#define MONITOR_IPV4_UDP  5

#define LOG_LEVEL_ERROR   1
#define LOG_LEVEL_WARNING 2
#define LOG_LEVEL_INFO    3
#define LOG_LEVEL_DEBUG   4

#define ENABLE_TIMESTAMP  // Define this to enable timestamp logging
#define ENABLE_HEX_DUMP   // Define this to enable hex dumping of packet data

int current_log_level = LOG_LEVEL_DEBUG; // Set desired log level
static int packet_count = 0;
static int filtered_packet_count = 0;

// Logging function
void log_message(int level, const char *format, ...) {
    if (level <= current_log_level) {
        va_list args;
        va_start(args, format);
        vprintf(format, args);
        va_end(args);
    }
}

// Function to determine if a packet should be monitored
int should_monitor_packet(struct sk_buff *p) {
    struct ieee80211_hdr *hdr = (struct ieee80211_hdr *)p->data;
    return (hdr->frame_control & IEEE80211_FCTL_FTYPE) == IEEE80211_FTYPE_MGMT; // Example: Monitor only management frames
}

// Function to log the current timestamp
void log_current_timestamp() {
    time_t now;
    time(&now);
    #ifdef ENABLE_TIMESTAMP
    log_message(LOG_LEVEL_INFO, "Packet received at %s", ctime(&now)); // Log the time the packet was received
    #endif
}

// Function to log packet details
void log_packet_details(struct sk_buff *p) {
    log_message(LOG_LEVEL_DEBUG, "Packet length: %d\n", p->len);
    // Add hex dump if needed (based on define)
    #ifdef ENABLE_HEX_DUMP
    print_hex("Packet Data", p->data, p->len);
    #endif
}

void wl_monitor_radiotap(struct wl_info *wl, struct wl_rxsts *sts, struct sk_buff *p) {
    // Allocate memory for the new packet, with error checking
    struct sk_buff *p_new = pkt_buf_get_skb(wl->wlc->osh, p->len + sizeof(struct nexmon_radiotap_header));
    if (!p_new) {
        log_message(LOG_LEVEL_ERROR, "Failed to allocate memory for new packet\n");
        return; // Early exit on error
    }

    struct nexmon_radiotap_header *frame = (struct nexmon_radiotap_header *) p_new->data;
    struct tsf tsf;

    // Read the TSF from the hardware
    if (wlc_bmac_read_tsf(wl->wlc_hw, &tsf.tsf_l, &tsf.tsf_h) != 0) {
        log_message(LOG_LEVEL_ERROR, "Failed to read TSF\n");
        pkt_buf_free_skb(wl->wlc->osh, p_new, 0); // Free the allocated packet buffer
        return; // Early exit on error
    }

    frame->header.it_version = 0;
    frame->header.it_pad = 0;
    frame->header.it_len = sizeof(struct nexmon_radiotap_header);
    frame->header.it_present =
          (1 << IEEE80211_RADIOTAP_TSFT)
        | (1 << IEEE80211_RADIOTAP_FLAGS)
        | (1 << IEEE80211_RADIOTAP_CHANNEL)
        | (1 << IEEE80211_RADIOTAP_DBM_ANTSIGNAL);

    frame->tsf.tsf_l = tsf.tsf_l;
    frame->tsf.tsf_h = tsf.tsf_h;
    frame->flags = IEEE80211_RADIOTAP_F_FCS;
    frame->chan_freq = wlc_phy_channel2freq(CHSPEC_CHANNEL(sts->chanspec));
    frame->chan_flags = 0;
    frame->dbm_antsignal = sts->signal;

    // Check if data copying is successful
    if (p->len < 6) {
        log_message(LOG_LEVEL_ERROR, "Insufficient packet length for data copy\n");
        pkt_buf_free_skb(wl->wlc->osh, p_new, 0); // Free the allocated packet buffer
        return; // Early exit on error
    }

    memcpy(p_new->data + sizeof(struct nexmon_radiotap_header), p->data + 6, p->len - 6);
    p_new->len -= 6;

    // Transmit the new packet
    if (wl->wlc->wlcif_list->next) {
        wl->wlc->wlcif_list->wlif->dev->chained->funcs->xmit(wl->wlc->wlcif_list->wlif->dev, wl->wlc->wlcif_list->wlif->dev->chained, p_new);
    } else {
        wl->dev->chained->funcs->xmit(wl->dev, wl->dev->chained, p_new);
    }
}

void wl_monitor_hook(struct wl_info *wl, struct wl_rxsts *sts, struct sk_buff *p) {
    packet_count++;

    if (!should_monitor_packet(p)) {
        filtered_packet_count++;
        log_message(LOG_LEVEL_DEBUG, "Packet filtered out\n");
        return; // Skip processing this packet
    }

    log_current_timestamp(); // Log the current timestamp
    log_packet_details(p); // Log packet details

    int monitor = wl->wlc->monitor & 0xFF;
    switch (monitor) {
        case MONITOR_RADIOTAP:
            wl_monitor_radiotap(wl, sts, p);
            break;

        case MONITOR_IEEE80211:
            wl_monitor(wl, sts, p);
            break;

        case MONITOR_LOG_ONLY:
            log_message(LOG_LEVEL_INFO, "Frame received\n");
            break;

        case MONITOR_DROP_FRM:
            break;

        case MONITOR_IPV4_UDP:
            log_message(LOG_LEVEL_WARNING, "UDP tunneling not implemented\n");
            break;
    }
}

// Function to log statistics
void log_statistics() {
    log_message(LOG_LEVEL_INFO, "Total packets received: %d, Filtered packets: %d\n", packet_count, filtered_packet_count);
}

__attribute__((at(0x8778A6, "flashpatch", CHIP_VER_BCM43436b0, FW_VER_ALL)))
__attribute__((naked))
void bw_wl_monitor_hook(void) {
    asm("b.w wl_monitor_hook\n");
}

void wlc_monitor_hook(struct wlc_info *wlc, void *wrxh, void *p, void *wlcif) {
    pkt_buf_free_skb(wlc->osh, p, 0);
}

__attribute__((at(0x875A5A, "flashpatch", CHIP_VER_BCM43436b0, FW_VER_ALL)))
__attribute__((naked))
void bw_wlc_monitor_hook(void) {
    asm("b.w wlc_monitor_hook\n");
}
