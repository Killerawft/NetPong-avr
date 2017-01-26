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

#include "../main.h"


void pong_drawball(byte c)
{
    h_line(ball->posx, ball->posy - 1, BALL_SIZE, 0, c);
    h_line(ball->posx, ball->posy, BALL_SIZE, 0, c);
    h_line(ball->posx, ball->posy + 1, BALL_SIZE, 0, c);
}

static void pong_moveball(u08 play1pos, u08 play2pos) {
s08 tmpdiff;
pong_drawball(0); //Alten Ball l�schen
ball->posx += ball->speedx;
if (ball->posx == 1) { //Linke Wand
  ball->speedx = 1;
}
if (ball->posx == screeny) { //Rechte Wand
  ball->speedx = -1;
}
ball->posy += ball->speedy;
if (ball->posy == 1) { //Obere Wand
  tmpdiff = ball->posx - play2pos; //Links ist negativ, rechts positiv
  if ((tmpdiff >= -1) && (tmpdiff <= 1)) {
    ball->speedy = 1;
  }
  if (tmpdiff == -2) {
    if (ball->speedx == 1) { //Stoppt horizontale Bewegung
      ball->speedy = 1;
      ball->speedx = 0;
    } else
    if (ball->speedx == 0) { //Startet horizontale Bewegung
      ball->speedy = 1;
      ball->speedx = -1;
    }
  }
  if (tmpdiff == 2) {
    if (ball->speedx == -1) { //Stoppt horizontale Bewegung
      ball->speedy = 1;
      ball->speedx = 0;
    } else
    if (ball->speedx == 0) { //Startet horizontale Bewegung
      ball->speedy = 1;
      ball->speedx = 1;
    }
  }
}
if (ball->posy == 14) { //Untere Wand
  tmpdiff = ball->posx - play1pos; //Links ist negaktiv, rechts positiv
  if ((tmpdiff >= -1) && (tmpdiff <= 1)) {
    ball->speedy = -1;
  }
  if (tmpdiff == -2) {
    if (ball->speedx == 1) { //Stoppt horizontale Bewegung
      ball->speedy = -1;
      ball->speedx = 0;
    } else
    if (ball->speedx == 0) { //Startet horizontale Bewegung
      ball->speedy = -1;
      ball->speedx = -1;
    }
  }
  if (tmpdiff == 2) {
    if (ball->speedx == -1) { //Stoppt horizontale Bewegung
      ball->speedy = -1;
      ball->speedx = 0;
    } else
    if (ball->speedx == 0) { //Startet horizontale Bewegung
      ball->speedy = -1;
      ball->speedx = 1;
    }
  }
}
pong_drawball(1); //Neuen Ball zeichnen
}

static void pong_endabs(u08 gameend) {
u08 color = 0x32;
//Spiel wird mit Ausgaben gewonnen/verloren/unentschieden beendet
if (gameend == 1) { //Spieler 1 gewinnt
  
}
if (gameend == 2) { //Spieler 2 gewinnt
  
}

}

static void pong_drawplayers(u08 play1pos, u08 play1pos_o,
                      u08 play2pos, u08 play2pos_o) {
//Zeichnen der Spielerposition
if (play1pos != play1pos_o) { //Wenn LCD Kontakte oben, Player1 ist links
  v_line(PLAYER_1_RANGE, PLAYER_LINE_START, PLAYER_SIZE, 0, 0);  //L�schen an alter Position
  v_line(PLAYER_1_RANGE, PLAYER_LINE_START, PLAYER_SIZE, 0, 1); //Zeichnen an neuer Position
}
if (play2pos != play2pos_o) { //Wenn LCD Kontakte oben, Player2 ist rechts
  v_line(PLAYER_2_RANGE, PLAYER_LINE_START, PLAYER_SIZE, 0, 0);  //L�schen an alter Position
  v_line(PLAYER_2_RANGE, PLAYER_LINE_START, PLAYER_SIZE, 0, 1); //Zeichnen an neuer Position
}
}


void pong_draw_field(void) //Spielfeld malen
{
    rectangle(FIELD_SPACE,0, FIELD_SIZE + FIELD_SPACE, FIELD_SIZE - 1, 0, 1);   //Feld Umrandung
    pong_drawplayers(PLAYER_LINE_START, 0, PLAYER_LINE_START, 0);
//     v_line(PLAYER_1_RANGE, PLAYER_LINE_START, PLAYER_SIZE, 0, 1);               //Spieler 1 zeichnen
//     v_line(PLAYER_2_RANGE, PLAYER_LINE_START, PLAYER_SIZE, 0, 1);               //Spieler 2 zeichnen
    pong_drawball(FIELD_CENTER_X, FIELD_CENTER_Y, 1);                        //Ball in der Mitte Platzieren
}

void draw_start_screen() //Startbildschirm malen
{
    pong_draw_field();

    draw_string("Sp 1", 20,20, 1, 1);
    draw_string("Sp 2", 128 - CTP_X(3),64 - CTP_Y(1), 1, 1);
}

#define pong_virtual_timers 5

void pong_init() {
u08 mode, gameend = 0;
u08 play1pos = PLAYER_LINE_START, play1pos_o = 0;
u08 play2pos = PLAYER_LINE_START, play2pos_o = 0;
u08 timings[pong_virtual_timers] = {0,0,0,0,0};
u08 nun;
u16 points = 0;


draw_start_screen();

while(1)
{
    
}

}


ISR(TIMER0_OVF_vect) //Timer Overflow ca alle 0,001 s
{
    static uint8_t tick = 0;
    
    if (tick >= 1000) //einmal in der Sekunde
    {
        LED1_TOGGLE
        tick = 0;
    }    
        
    
    tick++;
    TCNT0 = 100;                    //Preload 100
}