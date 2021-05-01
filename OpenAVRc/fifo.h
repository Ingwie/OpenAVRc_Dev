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


#ifndef _FIFO_H_
#define _FIFO_H_

template <int8_t N>
class Fifo
{
public:
 Fifo():
  w_idx(0),
  r_idx(0)
 {
 }

 void push(uint8_t byte)
 {
  uint8_t next = (w_idx == (N-1))? (w_idx+1) : 0;
  //uint8_t next = (w_idx+1) & (N-1); //optimised for N is power of 2
  if (next != r_idx)
   {
    buffer[w_idx] = byte;
    w_idx = next;
   }
 }

 uint8_t pop() // call isEmpty() before use, no control here (Faster code)
 {
  uint8_t byte = buffer[r_idx];
  r_idx = (r_idx == (N-1))? (r_idx+1) : 0;
  //r_idx = (r_idx+1) & (N-1); //optimised for N is power of 2
  return byte;
 }

 uint8_t isEmpty()
 {
  return (r_idx == w_idx);
 }

 uint8_t isFull()
 {
  uint8_t next = (w_idx == (N-1))? (w_idx+1) : 0;
  //uint8_t next = (w_idx+1) & (N-1); //optimised for N is power of 2
  return (next == r_idx);
 }

 void flush()
 {
  while (!isEmpty()) {};
 }

 uint8_t buffer[N];
 volatile uint8_t w_idx;
 volatile uint8_t r_idx;
};

#endif // _FIFO_H_
