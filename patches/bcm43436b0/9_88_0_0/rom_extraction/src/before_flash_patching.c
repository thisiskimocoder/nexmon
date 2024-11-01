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

#include <firmware_version.h>   // definition of firmware version macros
#include <wrapper.h>            // wrapper definitions for functions that already exist in the firmware
#include <patcher.h>            // macros used to craete patches such as BLPatch, BPatch, ...
#include <local_wrapper.h>


struct fp_config {
	unsigned int *target_addr;
	unsigned int data_ptr;
};

unsigned int fp_orig_data[(FP_CONFIG_ORIGEND - FP_CONFIG_ORIGBASE) / sizeof(struct fp_config)][3] = { 0 };
unsigned int fp_orig_data_len = (FP_CONFIG_ORIGEND - FP_CONFIG_ORIGBASE) / sizeof(struct fp_config);

int
fp_apply_patches_hook(void)
{
	struct fp_config *fpc = (struct fp_config *) FP_CONFIG_ORIGBASE;
	int i;

	for (i = 0; i < fp_orig_data_len; i++) {
		fp_orig_data[i][0] = (unsigned int) (fpc)->target_addr;
		fp_orig_data[i][1] = ((fpc)->target_addr)[0];
		fp_orig_data[i][2] = ((fpc)->target_addr)[1];
		fpc++;
	}

	return fp_apply_patches();
}
// Hook call to fp_apply_patches in c_main
__attribute__((at(0x42846, "", CHIP_VER_BCM43436b0, FW_VER_9_88_0_0)))
BPatch(fp_apply_patches, fp_apply_patches_hook);
