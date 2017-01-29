/*
   Gamebox
    Copyright (C) 2004-2006  by Malte Marwedel
    m.marwedel AT onlinehome dot de

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

#include <avr/pgmspace.h>

const char game_pts[] PROGMEM = "Pts:";

//Leichtes grün
const char intro1[16] PROGMEM = {0x10,0x10,0x10,0x10,0x10,0x10,0x10,0x10,0x10,
                                 0x10,0x10,0x10,0x10,0x10,0x10,0x10};

uint8_t init_random_done = 0;

void wordtostr(char *s, uint16_t nummer, uint8_t digits, uint8_t strposition) {
s += strposition;
while (digits > 0) {
  digits--;
  *(s+digits) = nummer%10+48;
  nummer = nummer / 10;
}
}

uint8_t max(uint8_t val1, uint8_t val2) {
if (val1 > val2) {
  return val1;
}
return val2;
}

uint8_t min(uint8_t val1, uint8_t val2) {
if (val1 > val2) {
  return val2;
}
return val1;
}
