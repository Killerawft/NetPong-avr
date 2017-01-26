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

/* Interne Funktionen, die als static deklariert sind:
static void pong_shotstart(struct pong_shotstruct *ball, u08 playpos,
                           u08 playerno);
static u08 pong_shotdown(struct pong_shotstruct *ball, u08 playpos,
                         u08 playerno);
static void pong_moveshot(struct pong_shotstruct *ball,
                          struct pong_shotstruct *ball2);
static u08 pong_ai_precalcball(struct pong_ballstruct *ball);
static s08 pong_ai_analyze(struct pong_ballstruct *ball,
             struct pong_shotstruct *shot1,
             struct pong_shotstruct *shot2, u08 analyzepos, u08 player2pos);
static u08 pong_ai(struct pong_ballstruct *ball, struct pong_shotstruct *shot1,
          struct pong_shotstruct *shot2, u08 play1pos, u08 play2pos, u08 mode);
static void pong_moveball(struct pong_ballstruct *ball, u08 play1pos,
                          u08 play2pos);
static void pong_endabs(u08 gameend);
static void pong_drawplayers(u08 play1pos, u08 play1pos_o,
                             u08 play2pos, u08 play2pos_o);
static void pong_drawselectmenu(u08 mode);
static u08 pong_selectmode(void);
*/

void pong_start();

#endif
