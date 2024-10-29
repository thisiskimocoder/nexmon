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

#pragma NEXMON targetregion "patch"

#include <stdint.h>
#include <firmware_version.h>   // definition of firmware version macros
#include <wrapper.h>            // wrapper definitions for functions that already exist in the firmware
#include <patcher.h>            // macros used to craete patches such as BLPatch, BPatch, ...
#include <local_wrapper.h>


struct fp_config {
    uint32_t *target_addr;
    uint32_t data_ptr;
};

// Ensure the size calculation is correct and handles potential alignment issues
uint32_t fp_orig_data[(FP_CONFIG_ORIGEND - FP_CONFIG_ORIGBASE + sizeof(struct fp_config) - 1) / sizeof(struct fp_config)][3] = {0};
size_t fp_orig_data_len = sizeof(fp_orig_data) / sizeof(fp_orig_data[0]);

int fp_apply_patches_hook(void) {
    struct fp_config *fpc = (struct fp_config *)FP_CONFIG_ORIGBASE;

    for (size_t i = 0; i < fp_orig_data_len && (uintptr_t)(fpc + i) < FP_CONFIG_ORIGEND; i++) { // Add bounds check
        fp_orig_data[i][0] = (uint32_t)(uintptr_t)fpc[i].target_addr; // Use uintptr_t for address storage
        if (fpc[i].target_addr) { // Check for null pointer before dereferencing
            fp_orig_data[i][1] = fpc[i].target_addr[0];
            fp_orig_data[i][2] = fpc[i].target_addr[1];
        } else {
            fp_orig_data[i][1] = 0; // Handle null pointer case
            fp_orig_data[i][2] = 0;
        }
    }

    return fp_apply_patches();
}


// Hook call to fp_apply_patches in c_main
__attribute__((at(0x42846, "", CHIP_VER_BCM43436b0, FW_VER_9_88_0_0)))
BPatch(fp_apply_patches, fp_apply_patches_hook);
