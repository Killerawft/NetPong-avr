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

#include "../../inc/Game/ppong.h"

struct _pong_ballstruct{
int8_t speedx;
int8_t speedy;
uint8_t posx;
uint8_t posy;
} pong_ballstruct;

struct _pong_shotstruct{
int8_t movey;
uint8_t posx;
uint8_t posy;
} pong_shotstruct;

void pong_shotstart(struct _pong_shotstruct *ball, uint8_t playpos, uint8_t playerno) {
if (ball->movey == 0) {
  ball->posx = playpos;
  if (playerno == 1) {
    ball->posy = 14;
    ball->movey = -1;
  }
  if (playerno == 2) {
    ball->posy = 1;
    ball->movey = 1;
  }
  //Zeichenen des Schuss
  circle(ball->posx, ball->posy, BALL_SIZE, 0, 1);
}
}

uint8_t pong_shotdown(struct _pong_shotstruct *ball, uint8_t playpos, uint8_t playerno) {
int8_t tmpdiff;
if (ball->movey) { //Wenn sich der Ball noch bewegt
  if (((ball->posy == 0) && (playerno == 2)) || //Und an der richtigen Stelle
     ((ball->posy == 15) && (playerno == 1))) {
    tmpdiff = ball->posx - playpos; //Links ist negativ, rechts positiv
    if ((tmpdiff >= -1) && (tmpdiff <= 1)) { //Wenn getroffen
      return playerno;
    }
  }
}
return 0;
}

void pong_moveshot(struct _pong_shotstruct *ball, struct _pong_shotstruct *ball2){
//Entfernen des roten Punktes an bisheriger Position
if (ball->movey != 0) {
  circle(ball->posx,ball->posy, BALL_SIZE, 0, 0);
}
//Bewegen des Balls
if ((ball->posy == 0) || (ball->posy == 15)) { //Wenn am Ende, Schluss stoppen
  ball->movey = 0;
}
if (ball->movey != 0) { //Wenn er sich noch bewegt
  ball->posy += ball->movey; //Neue Position
  //An neuer Position roten Punkt zeichnen
  circle(ball->posx,ball->posy, BALL_SIZE, 0, 1);
}
//Gegenseitiges Vernichten beider Schüsse, wenn auf gleicher Position und noch
//eher in der Mitte des Spielfeldes (noch nicht direkt vor dem Spieler)
if ((ball->posx == ball2->posx) && (ball->posy == ball2->posy) &&
    (ball->movey != 0) && (ball2->movey != 0) &&
    (ball->posx > 1) && (ball->posx < 14)) {
  ball->movey = 0;
  ball2->movey = 0;
  circle(ball->posx,ball->posy, BALL_SIZE, 0, 0);
}
}

void pong_drawball(unsigned int x,unsigned int y, unsigned char c)
{
    h_line(x, y - 1, BALL_SIZE, 0, c);
    h_line(x, y, BALL_SIZE, 0, c);
    h_line(x, y + 1, BALL_SIZE, 0, c);
}

void pong_moveball(struct _pong_ballstruct *ball, uint8_t play1pos, uint8_t play2pos) {
int8_t tmpdiff;
pong_drawball(ball->posx,ball->posy, 0); //Alten Ball löschen
ball->posx += ball->speedx;
if (ball->posx == 0) { //Linke Wand
  ball->speedx = 1;
}
if (ball->posx == 15) { //Rechte Wand
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
pong_drawball(ball->posx,ball->posy, 1); //Neuen Ball zeichnen
}

const char pong_won[] PROGMEM = "Win ";
const char pong_lost[] PROGMEM = "Lost";
const char pong_both[] PROGMEM = "Tie ";
const char pong_Test[] PROGMEM = "HALLO";

void pong_endabs(uint8_t gameend) {
uint8_t color = 0x32;
//Spiel wird mit Ausgaben gewonnen/verloren/unentschieden beendet
load_text(pong_both);
if (gameend == 1) {
  load_text(pong_lost);
  color = 0x03;
}
if (gameend == 2) {
  load_text(pong_won);
  color = 0x30;
}
scrolltext(4,color,0,100);
}

void pong_drawplayers(uint8_t play1pos, uint8_t play1pos_o,
                      uint8_t play2pos, uint8_t play2pos_o) {
//Zeichen der Spielerposition
if (play1pos != play1pos_o) { //Player1 ist unten, menschlicher Spieler
  v_line(PLAYER_1_RANGE, PLAYER_LINE_START, PLAYER_SIZE, 0, 0);  //Löschen an alter Position
  v_line(PLAYER_1_RANGE, PLAYER_LINE_START, PLAYER_SIZE, 0, 1); //Zeichnen an neuer Position
}
if (play2pos != play2pos_o) { //Player2 ist oben, AI Spieler
  v_line(PLAYER_2_RANGE, PLAYER_LINE_START, PLAYER_SIZE, 0, 0);  //Löschen an alter Position
  v_line(PLAYER_2_RANGE, PLAYER_LINE_START, PLAYER_SIZE, 0, 1); //Zeichnen an neuer Position
}
}

void pong_draw_field(void) //Spielfeld malen
{
    rectangle(FIELD_SPACE,0, FIELD_SIZE + FIELD_SPACE, FIELD_SIZE - 1, 0, 1);   //Feld Umrandung
    v_line(PLAYER_1_RANGE, PLAYER_LINE_START, PLAYER_SIZE, 0, 1);               //Spieler 1 zeichnen
    v_line(PLAYER_2_RANGE, PLAYER_LINE_START, PLAYER_SIZE, 0, 1);               //Spieler 2 zeichnen
    pong_drawball(FIELD_CENTER_X, FIELD_CENTER_Y, 1);                        //Ball in der Mitte Platzieren
}

void draw_start_screen() //Startbildschirm malen
{
    pong_draw_field();
    
    load_text(pong_Test);
    draw_string(64,32, 1);
}

#define pong_virtual_timers 5

void pong_start() {
uint8_t mode, gameend = 0;
uint8_t play1pos = 8, play1pos_o = 0;
uint8_t play2pos = 8, play2pos_o = 0;
uint8_t timings[pong_virtual_timers] = {0,0,0,0,0};
uint8_t nun;
uint16_t points = 0;
struct _pong_ballstruct ball = {1,1,0,7};
struct _pong_shotstruct shot1 = {0,0,0}, shot2 = {0,0,0};


draw_start_screen();

while(1)
{
    
}

// Zufallsgenerator initialisieren
// init_random();
// Timer1 wird für das Timing verwendet, 31,25KHZ Takt
// TCNT1 = 0; //Reset Timer
// TCCR1B = (1<<CS12); //Prescaler: 256
// while (gameend == 0) {
//   //Setzen der Spieler 1 Position
//   //play1pos = 7 + userin.x/18;
//   //play1pos = min(play1pos,14);
//   //play1pos = max(play1pos,1);
//   //Timings erzeugen
//   if (TCNT1 > (F_CPU/51282)) { //Wenn 5ms vergangen sind; (8MHZ/51282)=156
//     TCNT1 = 0;
//     for (nun = 0; nun < pong_virtual_timers; nun++) {
//       if (timings[nun] != 0xff) {
//         timings[nun]++;
//       }
//     }
//   }
//   //Ball Position berechnen
//   if (timings[0] >= 30) { //Also nach 150ms
//     timings[0] = 0;
//     pong_moveball(&ball, play1pos, play2pos);
//   }
//   }
//   //Erster Schuss
//   if (timings[2] >= 60) { //Also nach 300ms
//     timings[2] = 0;
//     pong_moveshot(&shot1,&shot2);
//   }
//   //Zweiter Schuss
//   if (timings[3] >= 60) { //Also nach 300ms
//     timings[3] = 0;
//     pong_moveshot(&shot2,&shot1);
//   }
//   //Punkte zählen
//   if (timings[4] >= 200) {//Jede Sekunde ein Punkt
//     timings[4] = 0;
//     points++;
//   }
//   //Schuss starten
//   if ((mode > 1) && userin_press()) {
//     pong_shotstart(&shot1,play1pos,1);
//     timings[2] = 0;
//   }
//   //Ball herausgesprungen überprüfen
//   if (ball.posy == 0) { gameend = 2; }  //Player 2 hat verloren
//   if (ball.posy == 15) { gameend = 1; } //Player 1 hat verloren
//   //Schuss abbekommen überprüfen
//   gameend |= pong_shotdown(&shot2,play1pos,1);
//   gameend |= pong_shotdown(&shot1,play2pos,2);
//   //Game Abbruch
//   pong_drawplayers(play1pos, play1pos_o, play2pos, play2pos_o);
//   play1pos_o = play1pos;
//   play2pos_o = play2pos;
// }
// TCCR1B = 0; //Stopp Timer1
// waitms(1000);
// rectangle(0,1,16,14,0,0);
// if (mode == 1) {
//   draw_gamepoints(points, PONG_ID);
// } else {
//   pong_endabs(gameend);
// }
}