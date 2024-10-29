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

#pragma once

#include <stdint.h> // Include for standard integer types

// Use descriptive names for better readability and consistency
struct d11rxhdr {
    uint16_t rx_frame_size;        // Actual byte length of the frame data received
    uint16_t pad;                 // Padding
    uint16_t phy_rx_status_0;     // PhyRxStatus 15:0
    uint16_t phy_rx_status_1;     // PhyRxStatus 31:16
    uint16_t phy_rx_status_2;     // PhyRxStatus 47:32
    uint16_t phy_rx_status_3;     // PhyRxStatus 63:48
    uint16_t phy_rx_status_4;     // PhyRxStatus 79:64
    uint16_t phy_rx_status_5;     // PhyRxStatus 95:80
    uint16_t rx_status_1;         // MAC Rx status
    uint16_t rx_status_2;         // Extended MAC Rx status
    uint16_t rx_tsf_time;         // RxTSFTime time of first MAC symbol + M_PHY_PLCPRX_DLY
    uint16_t rx_chan;             // Gain code, channel radio code, and phy type -> looks like chanspec
} __attribute__((packed));

// Macros for RxStatus1
#define D11RXHDR_GET_BCNSENT(rxhdr)      ((rxhdr)->rx_status_1 & RXS_BCNSENT)
#define D11RXHDR_GET_SECKINDX(rxhdr)     (((rxhdr)->rx_status_1 & RXS_SECKINDX_MASK) >> RXS_SECKINDX_SHIFT)

/* ucode RxStatus1: */
#define RXS_BCNSENT             0x8000
#define RXS_SECKINDX_MASK       0x07e0
#define RXS_SECKINDX_SHIFT      5
#define RXS_DECERR              (1 << 4)
#define RXS_DECATMPT            (1 << 3)
/* PAD bytes to make IP data 4 bytes aligned */
#define RXS_PBPRES              (1 << 2)
#define RXS_RESPFRAMETX         (1 << 1)
#define RXS_FCSERR              (1 << 0)

/* ucode RxStatus2: */
#define RXS_AMSDU_MASK          1
#define RXS_AGGTYPE_MASK        0x6
#define RXS_AGGTYPE_SHIFT       1
#define RXS_PHYRXST_VALID       (1 << 8)
#define RXS_RXANT_MASK          0x3
#define RXS_RXANT_SHIFT         12

// Macros for RxChan
#define D11RXHDR_IS_40MHZ(rxhdr)        ((rxhdr)->rx_chan & RXS_CHAN_40)
#define D11RXHDR_IS_5GHZ(rxhdr)        ((rxhdr)->rx_chan & RXS_CHAN_5G)
#define D11RXHDR_GET_CHANNEL(rxhdr)     (((rxhdr)->rx_chan & RXS_CHAN_ID_MASK) >> RXS_CHAN_ID_SHIFT)
#define D11RXHDR_GET_PHYTYPE(rxhdr)     (((rxhdr)->rx_chan & RXS_CHAN_PHYTYPE_MASK) >> RXS_CHAN_PHYTYPE_SHIFT)

/* RxChan */
#define RXS_CHAN_40             0x1000
#define RXS_CHAN_5G             0x0800
#define RXS_CHAN_ID_MASK        0x07f8
#define RXS_CHAN_ID_SHIFT       3
#define RXS_CHAN_PHYTYPE_MASK   0x0007
#define RXS_CHAN_PHYTYPE_SHIFT  0

// Extended receive header
struct wlc_d11rxhdr {
    struct d11rxhdr rxhdr;
    uint32_t tsf_l;
    int8_t rssi;                  // Computed instantaneous RSSI in BMAC
    int8_t rxpwr0;
    int8_t rxpwr1;
    int8_t do_rssi_ma;           // Do per-pkt sampling for per-antenna ma in HIGH
    int8_t rxpwr[4];              // RSSI for supported antennas
} __attribute__((packed));
