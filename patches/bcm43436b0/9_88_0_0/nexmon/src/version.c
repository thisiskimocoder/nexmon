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
 * Copyright (c) 2020 NexMon Team                                          *
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
#include <patcher.h>

// Combine version, date, and time into a single struct for better organization
struct FirmwareInfo {
    char version[64]; // Increased size to accommodate longer strings
    char date[16];
    char time[16];
};

// Initialize the struct with version, date, and time information
// Use a macro to concatenate strings to avoid potential buffer overflows
#define VERSION_STRING(x) #x
#define STRINGIFY(x) VERSION_STRING(x)
struct FirmwareInfo firmwareInfo = {
    .version = "9.88.0.0 (nexmon.org: " STRINGIFY(GIT_VERSION) "-" STRINGIFY(BUILD_NUMBER) ")",
    .date = __DATE__,
    .time = __TIME__
};


// Use a single patch for the version string to reduce redundancy
__attribute__((at(0x9FA0, "", CHIP_VER_BCM43436b0, FW_VER_9_88_0_0)))
GenericPatch4(version_patch1, firmwareInfo.version);

// Apply other version patches conditionally based on whether they are necessary
#ifdef APPLY_VERSION_PATCH_2
__attribute__((at(0x41C30, "", CHIP_VER_BCM43436b0, FW_VER_9_88_0_0)))
GenericPatch4(version_patch2, firmwareInfo.version);
#endif

#ifdef APPLY_VERSION_PATCH_3
__attribute__((at(0x4282C, "", CHIP_VER_BCM43436b0, FW_VER_9_88_0_0)))
GenericPatch4(version_patch3, firmwareInfo.version);
#endif

// Apply patches for date and time using the struct
__attribute__((at(0x9FAC, "", CHIP_VER_BCM43436b0, FW_VER_9_88_0_0)))
GenericPatch4(date_patch, firmwareInfo.date);

__attribute__((at(0x9F9C, "", CHIP_VER_BCM43436b0, FW_VER_9_88_0_0)))
GenericPatch4(time_patch, firmwareInfo.time);
