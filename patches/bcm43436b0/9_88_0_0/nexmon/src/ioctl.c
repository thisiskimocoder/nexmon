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
#include <debug.h>              // contains macros to access the debug hardware
#include <wrapper.h>            // wrapper definitions for functions that already exist in the firmware
#include <structs.h>            // structures that are used by the code in the firmware
#include <helper.h>             // useful helper functions
#include <patcher.h>            // macros used to craete patches such as BLPatch, BPatch, ...
#include <rates.h>              // rates used to build the ratespec for frame injection
#include <nexioctls.h>          // ioctls added in the nexmon patch
#include <capabilities.h>       // capabilities included in a nexmon patch
#include <sendframe.h>          // sendframe functionality
#include <version.h>            // version information
#include <argprintf.h>          // allows to execute argprintf to print into the arg buffer

int wlc_ioctl_hook(struct wlc_info *wlc, int cmd, char *arg, int len, void *wlc_if) {
    argprintf_init(arg, len);
    int ret = IOCTL_ERROR;

    switch (cmd) {
        case NEX_GET_CONSOLE: {
            struct hnd_debug *hnd_debug = (struct hnd_debug *)hnd_debug_info_get();
            // Check hnd_debug and console for null pointers
            if (hnd_debug && hnd_debug->console && len > 0) {
                size_t copy_len = min((size_t)len, strlen(hnd_debug->console->buf)); // Prevent buffer overflow
                memcpy(arg, hnd_debug->console->buf, copy_len);
                ret = IOCTL_SUCCESS;
            } else {
                ret = IOCTL_ERROR; // Indicate error if pointers are invalid
            }
            break;
        }

        case NEX_GET_CAPABILITIES:
            if (len >= sizeof(capabilities)) { // Ensure sufficient buffer size
                memcpy(arg, &capabilities, sizeof(capabilities));
                ret = IOCTL_SUCCESS;
            }
            break;

        case NEX_WRITE_TO_CONSOLE:
            if (len > 0) {
                if (arg[len - 1] != '\0') { // Check if already null-terminated
                    arg[len - 1] = '\0'; // Ensure null termination to prevent potential issues
                }
                printf("ioctl: %s\n", arg);
                ret = IOCTL_SUCCESS;
            }
            break;

        case 0x603: // read from memory - **DANGER: This is extremely risky!**
            // **Implement robust safety checks here before proceeding.**
            // This needs proper bounds checking and validation of the user-supplied address.
            // Without these checks, this is a major security vulnerability.
            if (len > 0 && *(char **)arg != NULL) { // Basic null check, but insufficient
                memcpy(arg, *(char **)arg, len);
                ret = IOCTL_SUCCESS;
            }
            break;

        default:
            ret = wlc_ioctl(wlc, cmd, arg, len, wlc_if);
    }

    return ret;
}

// Use FW_VER macro for consistency
__attribute__((at(0x4CA30, "", CHIP_VER_BCM43436b0, FW_VER(9, 88, 0, 0))))
GenericPatch4(wlc_ioctl_hook, wlc_ioctl_hook + 1);

