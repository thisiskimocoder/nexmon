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

#include <firmware_version.h>   // definition of firmware version macros
#include <patcher.h>            // macros used to craete patches such as BLPatch, BPatch, ...

char version[] = "9.88.0.0 (nexmon.org: " GIT_VERSION "-" BUILD_NUMBER ")";
char date[] = __DATE__;
char time[] = __TIME__;

__attribute__((at(0x9FA0, "", CHIP_VER_BCM43436b0, FW_VER_9_88_0_0)))
GenericPatch4(version_patch1, version);
__attribute__((at(0x41C30, "", CHIP_VER_BCM43436b0, FW_VER_9_88_0_0)))
GenericPatch4(version_patch2, version);
__attribute__((at(0x4282C, "", CHIP_VER_BCM43436b0, FW_VER_9_88_0_0)))
GenericPatch4(version_patch3, version);

__attribute__((at(0x9FAC, "", CHIP_VER_BCM43436b0, FW_VER_9_88_0_0)))
GenericPatch4(date_patch, date);

__attribute__((at(0x9F9C, "", CHIP_VER_BCM43436b0, FW_VER_9_88_0_0)))
GenericPatch4(time_patch, time);