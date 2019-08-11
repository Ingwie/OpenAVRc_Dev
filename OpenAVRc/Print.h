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

#ifndef Print_h
#define Print_h

#include <inttypes.h>
#include <stdio.h> // for size_t
#include <avr/pgmspace.h>

#define BASE_DEC 10
#define BASE_HEX  16
#define BASE_OCT  8
#ifdef BASE_BIN // Prevent warnings if BIN is previously defined in "iotnx4.h" or similar
#undef BASE_BIN
#endif
#define BASE_BIN 2

// When compiling programs with this class, the following gcc parameters
// dramatically increase performance and memory (RAM) efficiency, typically
// with little or no increase in code size.
//     -felide-constructors
//     -std=c++0x

class __FlashStringHelper;
#define F(string_literal) (reinterpret_cast<const __FlashStringHelper *>(PSTR(string_literal)))

class Print
{
  private:
    size_t printNumber(unsigned long, uint8_t);
  public:
    virtual size_t write(uint8_t) = 0;
    size_t write(const char *str) {
      if (str == NULL) return 0;
      return write((const uint8_t *)str, strlen(str));
    }
    virtual size_t write(const uint8_t *buffer, size_t size);
    size_t write(const char *buffer, size_t size) {
      return write((const uint8_t *)buffer, size);
    }
    size_t print(const __FlashStringHelper *);
    size_t print(const char[]);
    size_t print(char);
    size_t print(unsigned char, int = BASE_DEC);
    size_t print(int, int = BASE_DEC);
    size_t print(unsigned int, int = BASE_DEC);
    size_t print(long, int = BASE_DEC);
    size_t print(unsigned long, int = BASE_DEC);

    size_t println(const __FlashStringHelper *);
    size_t println(const char[]);
    size_t println(char);
    size_t println(unsigned char, int = BASE_DEC);
    size_t println(int, int = BASE_DEC);
    size_t println(unsigned int, int = BASE_DEC);
    size_t println(long, int = BASE_DEC);
    size_t println(unsigned long, int = BASE_DEC);
    size_t println(void);

};

#endif

