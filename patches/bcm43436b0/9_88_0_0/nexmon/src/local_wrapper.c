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

#ifndef LOCAL_WRAPPER_C
#define LOCAL_WRAPPER_C

#include <firmware_version.h>
#include <structs.h>
#include <stdarg.h> // Include this if variadic functions are used

// More descriptive macro names
#ifndef WRAPPER_H
    #define DECLARE_DUMMY_VOID { (void)0; } // More standard way to ignore unused parameters
    #define DECLARE_DUMMY_RETURN { (void)0; return 0; }
    #define ATTRIBUTE_DUMMY(CHIPVER, FWVER, ADDR) __attribute__((weak, at(ADDR, "dummy", CHIPVER, FWVER)))
#else
    #define DECLARE_DUMMY_VOID ;
    #define DECLARE_DUMMY_RETURN ;
    #define ATTRIBUTE_DUMMY(CHIPVER, FWVER, ADDR)
#endif


// Undefining macros is good practice
#undef DECLARE_DUMMY_VOID
#undef DECLARE_DUMMY_RETURN
#undef ATTRIBUTE_DUMMY

#endif /* LOCAL_WRAPPER_C */
