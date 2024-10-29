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

#include <firmware_version.h>
#include <wrapper.h>
#include <structs.h>
#include <patcher.h>
#include <helper.h>
#include <capabilities.h>

// Use enum for capabilities for better readability and maintainability
enum NexmonCapabilities {
    NEX_CAP_MONITOR_MODE = 0x01,
    NEX_CAP_MONITOR_MODE_RADIOTAP = 0x02,
    NEX_CAP_FRAME_INJECTION = 0x04
};

// Use a constant instead of a variable
const int capabilities = NEX_CAP_MONITOR_MODE | NEX_CAP_MONITOR_MODE_RADIOTAP | NEX_CAP_FRAME_INJECTION;

// More descriptive comments
// Hook the call to wlc_ucode_write in wlc_ucode_download to potentially modify ucode loading
__attribute__((at(WLC_UCODE_WRITE_BL_HOOK_ADDR, "", CHIP_VER_ALL, FW_VER_ALL)))
BLPatch(wlc_ucode_write_compressed, wlc_ucode_write_compressed);


// Patch hndrte_reclaim_0_end, purpose unclear without more context. Add a comment explaining the purpose.
__attribute__((at(HNDRTE_RECLAIM_0_END_PTR, "", CHIP_VER_ALL, FW_VER_ALL)))
GenericPatch4(hndrte_reclaim_0_end, PATCHSTART);


// Ensure templateram_bin is properly initialized.  Consider using a const if it's read-only.
extern unsigned char templateram_bin[]; // Or  extern const unsigned char templateram_bin[]; if it's read-only


// Relocate template RAM to a different location in ucode
#if TEMPLATERAMSTART_PTR != 0
__attribute__((at(TEMPLATERAMSTART_PTR, "", CHIP_VER_ALL, FW_VER_ALL)))
GenericPatch4(templateram_bin, templateram_bin);
#endif
