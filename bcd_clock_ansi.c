/* BCD(Binary Coded Decimal) clock, using ANSI.
 *
 * Copyright (C) 2009 Kristian Gunstone
 *
 * For playing with binary.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */
#include <stdio.h>
#include <time.h>

#define ON  "\e[42m "
#define OFF "\e[41m "

static unsigned char
dec_to_bin(unsigned char dec, const unsigned char bits) {
  
  unsigned char offset = bits;
  unsigned char bin = 0;

  while(dec > 0) {
    bin ^= (dec & 1) << offset;
    dec >>= 1;
    offset--;
  }

  return(bin);
}

int main(int argc, char *argv[]) {

  time_t t;
  struct tm *tm;
  unsigned char dt[6], bt[6];

  printf("\e[2J\e[1;1H");

  for(;;) {

    t = time(NULL);
    tm = localtime(&t);

    // BCD split 
    dt[0] = tm->tm_hour % 10;
    dt[1] = (tm->tm_hour - dt[0]) / 10;
    dt[2] = tm->tm_min % 10;
    dt[3] = (tm->tm_min - dt[2]) / 10;
    dt[4] = tm->tm_sec % 10;
    dt[5] = (tm->tm_sec - dt[4]) / 10;

    bt[0] = dec_to_bin(dt[1], 2); 
    bt[1] = dec_to_bin(dt[0], 4); 
    bt[2] = dec_to_bin(dt[3], 3);
    bt[3] = dec_to_bin(dt[2], 4);
    bt[4] = dec_to_bin(dt[5], 3);
    bt[5] = dec_to_bin(dt[4], 4); 

    printf("\e[0;m HHMMSS\n"
           "\e[0;m8 %s\e[0m %s\e[0m %s\e[0m\n"
           "\e[0;m4 %s%s%s%s%s\e[0m\n"
           "\e[0;m2%s%s%s%s%s%s\e[0m\n"
           "\e[0;m1%s%s%s%s%s%s\e[0m\n"
           "\e[0;m %u%u%u%u%u%u\n\e[1;1H",
                              bt[1]>>1&1?ON:OFF,                    bt[3]>>1&1?ON:OFF,                    bt[5]>>1&1?ON:OFF,
                              bt[1]>>2&1?ON:OFF, bt[2]>>1&1?ON:OFF, bt[3]>>2&1?ON:OFF, bt[4]>>1&1?ON:OFF, bt[5]>>2&1?ON:OFF,
           bt[0]>>1&1?ON:OFF, bt[1]>>3&1?ON:OFF, bt[2]>>2&1?ON:OFF, bt[3]>>3&1?ON:OFF, bt[4]>>2&1?ON:OFF, bt[5]>>3&1?ON:OFF,
           bt[0]>>2&1?ON:OFF, bt[1]>>4&1?ON:OFF, bt[2]>>3&1?ON:OFF, bt[3]>>4&1?ON:OFF, bt[4]>>3&1?ON:OFF, bt[5]>>4&1?ON:OFF,
           dt[1], dt[0], dt[3], dt[2], dt[5], dt[4]);

    usleep(1000000);
  }

  return(0);
}
