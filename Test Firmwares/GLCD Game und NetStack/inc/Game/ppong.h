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

#ifndef PPONG_H
 #define PPONG_H


#define BALL_SIZE           3                                       //Radius des Balls
#define PLAYER_SIZE         10                                      //Breite des Spielerbalkens, muss durch 2 teilbar sein
#define FIELD_SIZE          64                                      //Feldgröße -> Feld ist immer rechteckig
#define FIELD_SPACE         32                                      //Bereich neben dem Feld

#define PLAYER_1_RANGE      FIELD_SPACE + 2                         //Bereich für Linken Spielerbalken
#define PLAYER_2_RANGE      FIELD_SPACE + FIELD_SIZE - 2            //Bereich für Rechten Spielerbalken
#define PLAYER_LINE_START   (FIELD_SIZE / 2) - (PLAYER_SIZE / 2)    //Startpunkt des Spielerbalkens auf Y Achse
#define FIELD_CENTER_X      FIELD_SPACE + (FIELD_SIZE / 2)          //X Mittelpunkt des Feldes
#define FIELD_CENTER_Y      (FIELD_SIZE / 2)                        //Y Mittelpunkt des Feldes

#include "text.h"
#include <avr/pgmspace.h>


void pong_start();
// void pong_shotstart(struct _pong_shotstruct *ball, uint8_t playpos,uint8_t playerno);
// uint8_t pong_shotdown(struct _pong_shotstruct *ball, uint8_t playpos,uint8_t playerno);
// void pong_moveshot(struct _pong_shotstruct *ball, struct _pong_shotstruct *ball2);
// void pong_moveball(struct _pong_ballstruct *ball, uint8_t play1pos, uint8_t play2pos);
// void pong_endabs(uint8_t gameend);
// void pong_drawplayers(uint8_t play1pos, uint8_t play1pos_o,uint8_t play2pos, uint8_t play2pos_o);
// void pong_drawball(unsigned int x,unsigned int y, unsigned char c);
// void pong_draw_field(void);
// void draw_start_screen();


#endif
