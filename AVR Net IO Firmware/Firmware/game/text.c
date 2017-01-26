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

/* Diese Funktion schreibt einzelne Buchstaben auf das LED Display
Die Schriftgr��e betr�gt 5x8 Pixel, wobei nur 5x7 auch gezeichnet werden.
Jeder Buchstabe ben�tigt 5 Byte Speicher
Insgesammt werden ungef�hr 465 Byte f�r die Buchstaben verbaucht
Der Funktion kann zus�tzlich die Font Farbe und Transparenz als
Parameter �bergeben werden
Die Schrift basiert auf einer modifizierten Schrift von:
http://overcode.yak.net/12

Zus�tzlich gibt es Funktionen zum anzeigen von Texten und Scrolltexten

Datum der letzten �nderung:
  2005-07-19: Buchstabe "P" war falsch
  2005-07-23: draw_tinynumber(), draw_tinydigit() hinzugef�gt

*/
#include "main.h"
#include "text.h"
#include <string.h>
#include <avr/pgmspace.h>
#include <inttypes.h>


const char characters[] PROGMEM = {
0x00,0x00,0x00,0x00,0x00, //Leerzeichen
0x00,0x00,0x5F,0x00,0x00, //Ausrufezeichen
0x00,0x03,0x00,0x03,0x00,
0x14,0x7F,0x14,0x7F,0x14,
0x24,0x2A,0x7F,0x2A,0x12,
0x26,0x16,0x08,0x34,0x32,
0x4A,0x55,0x21,0x50,0x00,
0x00,0x00,0x03,0x00,0x00,
0x00,0x1C,0x22,0x41,0x00,
0x00,0x41,0x22,0x1C,0x00,
0x22,0x14,0x7F,0x14,0x22,
0x08,0x08,0x3E,0x08,0x08,
0x00,0x00,0x60,0x30,0x00,
0x08,0x08,0x08,0x08,0x08,
0x00,0x00,0x60,0x60,0x00,
0x20,0x10,0x08,0x04,0x02,
0x3E,0x41,0x41,0x41,0x3E,
0x00,0x42,0x7F,0x40,0x00,
0x42,0x61,0x51,0x49,0x46,
0x22,0x41,0x49,0x49,0x36,
0x0C,0x0A,0x49,0x7F,0x48,
0x2F,0x49,0x49,0x49,0x31,
0x3E,0x49,0x49,0x49,0x32,
0x01,0x71,0x09,0x05,0x03,
0x36,0x49,0x49,0x49,0x36,
0x26,0x49,0x49,0x49,0x3E,
0x00,0x00,0x36,0x36,0x00,
0x00,0x00,0x56,0x36,0x00,
0x08,0x14,0x22,0x41,0x00,
0x14,0x14,0x14,0x14,0x14,
0x41,0x22,0x14,0x08,0x00,
0x02,0x01,0x51,0x09,0x06,
0x3E,0x41,0x5D,0x55,0x1E,
0x7E,0x09,0x09,0x09,0x7E, //A
0x7F,0x49,0x49,0x49,0x36,
0x3E,0x41,0x41,0x41,0x22,
0x7F,0x41,0x41,0x41,0x3E,
0x7F,0x49,0x49,0x49,0x41,
0x7F,0x09,0x09,0x09,0x09, //F
0x3E,0x41,0x49,0x49,0x3A,
0x7F,0x08,0x08,0x08,0x7F, //H
0x00,0x41,0x7F,0x41,0x00, //I
0x30,0x40,0x41,0x3F,0x01,
0x7F,0x08,0x08,0x14,0x63,
0x00,0x7F,0x40,0x40,0x40, //L
0x7F,0x04,0x08,0x04,0x7F,
0x7F,0x02,0x0C,0x10,0x7F,
0x3E,0x41,0x41,0x41,0x3E,
0x7F,0x09,0x09,0x09,0x06, //P
0x3E,0x41,0x51,0x61,0x7E,
0x7F,0x09,0x19,0x29,0x46,
0x26,0x49,0x49,0x49,0x32,
0x01,0x01,0x7F,0x01,0x01,
0x3F,0x40,0x40,0x40,0x3F,
0x07,0x18,0x60,0x18,0x07,
0x1F,0x60,0x18,0x60,0x1F,
0x63,0x14,0x08,0x14,0x63,
0x03,0x04,0x78,0x04,0x03,
0x61,0x51,0x49,0x45,0x43,
0x00,0x7F,0x41,0x41,0x00,
0x02,0x04,0x08,0x10,0x20,
0x00,0x41,0x41,0x7F,0x00,
0x04,0x02,0x01,0x02,0x04,
0x40,0x40,0x40,0x40,0x40,
0x00,0x01,0x02,0x00,0x00,
0x30,0x4A,0x4A,0x4A,0x7C,
0x00,0x7F,0x44,0x44,0x38,
0x38,0x44,0x44,0x44,0x00,
0x00,0x38,0x44,0x44,0x7F,
0x38,0x54,0x54,0x54,0x18,
0x08,0x7E,0x09,0x01,0x00,
0x0C,0x52,0x52,0x52,0x3C,
0x7F,0x04,0x04,0x04,0x78,
0x00,0x00,0x7A,0x00,0x00,
0x20,0x40,0x3A,0x00,0x00,
0x7F,0x10,0x28,0x44,0x00,
0x00,0x00,0x7F,0x00,0x00,
0x7C,0x04,0x18,0x04,0x78,
0x00,0x78,0x04,0x04,0x78,
0x38,0x44,0x44,0x44,0x38,
0x7C,0x14,0x24,0x24,0x18,
0x18,0x24,0x24,0x14,0x7C,
0x7C,0x08,0x04,0x04,0x00,
0x48,0x54,0x54,0x54,0x24,
0x00,0x02,0x3F,0x42,0x00,
0x3C,0x40,0x40,0x40,0x3C,
0x1C,0x20,0x40,0x20,0x1C,
0x3C,0x40,0x30,0x40,0x3C,
0x44,0x28,0x10,0x28,0x44,
0x0C,0x50,0x50,0x50,0x3C,
0x44,0x64,0x54,0x4C,0x00,
0x08,0x36,0x41,0x41,0x00,
0x00,0x00,0x7F,0x00,0x00,
0x41,0x41,0x36,0x08,0x00,
0x08,0x04,0x08,0x10,0x08,
};


uint8_t draw_char(uint8_t zeichen, uint8_t posx, uint8_t posy, uint8_t color) 
{
uint8_t nunbyte,charwidth,nunbit;
uint8_t copyedbytes[5];
uint8_t tempx,tempy;

charwidth = 0;
zeichen -= 32;
if (zeichen < 94) { //g�ltiges Zeichen
 memcpy_P(copyedbytes,characters+zeichen*5,5);
 //Automatisches verk�rzen der Buchstaben
 for (nunbyte = 0; nunbyte < 5;nunbyte++) {
   for (nunbit = 0; nunbit < 7;nunbit++) {
     if ((copyedbytes[nunbyte] & (0x01<<nunbit)) != 0) {
       tempx = posx+charwidth;
       tempy = posy+nunbit;
       if ((tempx < screenx) && (tempy < screeny)) {
         if (color == 0) { //transparent
           point_at(tempx, tempy, 0);
         }else{ //nicht transparent
           point_at(tempx, tempy, 1);
         }
       }   //end: check range
     }     //end: there is a bit to write
   }       //end: inner loop
   if (copyedbytes[nunbyte] != 0) {
     charwidth++;
   }       //end: charwidth++
 }         //end: outer loop
}          //end: valid char
return charwidth++;
}          //end: function

void draw_string (char* c, uint8_t posx, uint8_t posy, uint8_t color)
{ // Sendet String an Display
uint8_t i = 0;       //welches Zeichen gezeichnet wird
const uint8_t end = strlen(c); //erspart Rechenzeit!
while (i < end) {
 posx += draw_char(*(c+i), posx+i, posy, color);
  i++;  // Zeiger um 1 erh�hen
}
}

void scrolltext (char* c, uint8_t posy, uint8_t color, uint8_t bcolor, uint8_t waittime){
uint8_t i=0;       //welches Zeichen gezeichnet wird
uint8_t posx;      //wo gezeichnet wird
uint8_t posshift=0;
const uint8_t end = strlen(c); //erspart Rechenzeit!
uint8_t tomove = end;//wie oft geschoben werden muss

//Erstes Zeichnen um die maximale L�nge zu ermitteln, jedoch nicht sichtbar
if ((end < 36) && (end != 0)) { //maximale Textl�nge = 35 Zeichen
  while (i < end) { //Ermittelt die Textl�nge in Pixel
    tomove += draw_char(*(c+i), screenx, 0, 1);
    i++;  // Zeiger um 1 erh�hen
  }
  tomove -= 2; //zwei Pixel weniger schieben
  while (tomove != 0) { //eigentliche Animation
    tomove --;//Schiebe Text um 1 Pixel nach links
    posshift++;
    //alten Text mit Hintergrundfarbe �berzeichnen
    //glcd_clear();
    //Text neu schreiben
    posx = screenx-1;
    i = 0;
    while (i < end) {
      posx += draw_char(*(c+i), posx+i-posshift, posy, color);
      i++;  // Zeiger um 1 erh�hen
    }
    //Warten, damit der Text sichbar ist
    waitms(waittime);
  }
}
}

const u08 tinynumbers[] PROGMEM = {
0x1f,0x11,0x1f, //0
0x00,0x00,0x1f, //1
0x1d,0x15,0x17, //2
0x15,0x15,0x1f, //3
0x07,0x04,0x1f, //4
0x17,0x15,0x1d, //5
0x1f,0x15,0x1d, //6
0x01,0x01,0x1f, //7
0x1f,0x15,0x1f, //8
0x17,0x15,0x7f  //9
};

void draw_tinydigit(uint8_t ziffer, uint8_t posx,uint8_t posy, uint8_t color) {
u08 nun, muster, pixely;
if (ziffer < 10) { //Bereichs�berpr�fung
  for (nun = 0; nun <3; nun++) {
    muster = pgm_read_byte(tinynumbers +ziffer*3+nun);
    for (pixely = 0; pixely < 5; pixely++) {
      if (muster & 1) {
        point_at(posx+nun,posy+pixely,color);
      }
      muster = muster >> 1;
    }
  }
}
}

void draw_tinynumber(uint16_t value, uint8_t posx, uint8_t posy, uint8_t color){
u08 ziffern = 1,zvalue;

if (value >= 10) { ziffern = 2; }
if (value >= 100) { ziffern = 3; }
if (value >= 1000) { ziffern = 4; }
if (value >= 10000) { ziffern = 5; }
while (ziffern > 0) {
  zvalue = value % 10;
  value /= 10;
  draw_tinydigit(zvalue, posx+(ziffern-1)*4, posy, color);
  ziffern--;
}
}
