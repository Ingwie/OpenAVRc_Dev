/*
**************************************************************************
*                                                                        *
*                 ____                ___ _   _____                      *
*                / __ \___  ___ ___  / _ | | / / _ \____                 *
*               / /_/ / _ \/ -_) _ \/ __ | |/ / , _/ __/                 *
*               \____/ .__/\__/_//_/_/ |_|___/_/|_|\__/                  *
*                   /_/                                                  *
*                                                                        *
*              This file is part of the OpenAVRc project.                *
*                                                                        *
*                         Based on code(s) named :                       *
*             OpenTx - https://github.com/opentx/opentx                  *
*             Deviation - https://www.deviationtx.com/                   *
*                                                                        *
*                Only AVR code here for visibility ;-)                   *
*                                                                        *
*   OpenAVRc is free software: you can redistribute it and/or modify     *
*   it under the terms of the GNU General Public License as published by *
*   the Free Software Foundation, either version 2 of the License, or    *
*   (at your option) any later version.                                  *
*                                                                        *
*   OpenAVRc is distributed in the hope that it will be useful,          *
*   but WITHOUT ANY WARRANTY; without even the implied warranty of       *
*   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the        *
*   GNU General Public License for more details.                         *
*                                                                        *
*       License GPLv2: http://www.gnu.org/licenses/gpl-2.0.html          *
*                                                                        *
**************************************************************************
*/


/*  This file could be moded */
/*   From the original one   */
/*  Thanks to Roland Riegel  */
/*   Original header above   */

/*
 * Copyright (c) 2006-2012 by Roland Riegel <feedback@roland-riegel.de>
 *
 * This file is free software; you can redistribute it and/or modify
 * it under the terms of either the GNU General Public License version 2
 * or the GNU Lesser General Public License version 2.1, both as
 * published by the Free Software Foundation.
 */

#include "byteordering.h"

/**
 * \addtogroup byteordering
 *
 * Architecture-dependent handling of byte-ordering.
 *
 * @{
 */
/**
 * \file
 * Byte-order handling implementation (license: GPLv2 or LGPLv2.1)
 *
 * \author Roland Riegel
 */

#if DOXYGEN || SWAP_NEEDED

/**
 * \internal
 * Swaps the bytes of a 16-bit integer.
 *
 * \param[in] i A 16-bit integer which to swap.
 * \returns The swapped 16-bit integer.
 */
uint16_t swap16(uint16_t i)
{
    return SWAP16(i);
}

/**
 * \internal
 * Swaps the bytes of a 32-bit integer.
 *
 * \param[in] i A 32-bit integer which to swap.
 * \returns The swapped 32-bit integer.
 */
uint32_t swap32(uint32_t i)
{
    return SWAP32(i);
}

#endif

/**
 * Reads a 16-bit integer from memory in little-endian byte order.
 *
 * \param[in] p Pointer from where to read the integer.
 * \returns The 16-bit integer read from memory.
 */
uint16_t read16(const uint8_t* p)
{
    return (((uint16_t) p[1]) << 8) |
           (((uint16_t) p[0]) << 0);
}

/**
 * Reads a 32-bit integer from memory in little-endian byte order.
 *
 * \param[in] p Pointer from where to read the integer.
 * \returns The 32-bit integer read from memory.
 */
uint32_t read32(const uint8_t* p)
{
    return (((uint32_t) p[3]) << 24) |
           (((uint32_t) p[2]) << 16) |
           (((uint32_t) p[1]) <<  8) |
           (((uint32_t) p[0]) <<  0);
}

/**
 * Writes a 16-bit integer into memory in little-endian byte order.
 *
 * \param[in] p Pointer where to write the integer to.
 * \param[in] i The 16-bit integer to write.
 */
void write16(uint8_t* p, uint16_t i)
{
    p[1] = (uint8_t) ((i & 0xff00) >> 8);
    p[0] = (uint8_t) ((i & 0x00ff) >> 0);
}

/**
 * Writes a 32-bit integer into memory in little-endian byte order.
 *
 * \param[in] p Pointer where to write the integer to.
 * \param[in] i The 32-bit integer to write.
 */
void write32(uint8_t* p, uint32_t i)
{
    p[3] = (uint8_t) ((i & 0xff000000) >> 24);
    p[2] = (uint8_t) ((i & 0x00ff0000) >> 16);
    p[1] = (uint8_t) ((i & 0x0000ff00) >>  8);
    p[0] = (uint8_t) ((i & 0x000000ff) >>  0);
}

/**
 * @}
 */

