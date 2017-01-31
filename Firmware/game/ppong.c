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

#include "ppong.h"

void pong_drawball(byte c)
{
    h_line(ball.posx, ball.posy - 1, BALL_SIZE, 0, c);
    h_line(ball.posx, ball.posy, BALL_SIZE, 0, c);
    h_line(ball.posx, ball.posy + 1, BALL_SIZE, 0, c);
}

void pong_moveball(void) 
{

    pong_drawball(0); //Alten Ball löschen

    if (ball.posx == PLAYER_1_RANGE + BALL_HITBX && ball.posy < player[0].posy + PLAYER_SIZE + BALL_HITBX && ball.posy > player[0].posy - BALL_HITBX - 1) //Treffen auf Spielerbalken 1
    {
        DEBUG("Spielerbalken 1 getroffen\n");
        ball.speedx = ball.speedx * (-1);
        
    }else if (ball.posx == PLAYER_2_RANGE - BALL_SIZE && ball.posy < player[1].posy + PLAYER_SIZE + BALL_HITBX && ball.posy > player[1].posy - BALL_HITBX - 1) //Treffen auf Spielerbalken 2
    {
        DEBUG("Spielerbalken 2 getroffen\n");
        ball.speedx = ball.speedx * (-1); 
         
    }else if (ball.posx == FIELD_SPACE + BALL_HITBX) { //Linke Wand vom Spieler 1
        DEBUG("Linke Wand getroffen\n");       
        ball.speedx = ball.speedx * (-1);
  
        draw_tinynumber(player[1].points, POINTS_X2, POINTS_Y, 0);
        player[1].points++;                       //Punkt für Spieler 2
        draw_tinynumber(player[1].points, POINTS_X2, POINTS_Y, 1);
        
    }else if (ball.posx == FIELD_SPACE + FIELD_SIZE - BALL_SIZE) { //Rechte Wand vom Spieler 2
        DEBUG("Rechte Wand getroffen\n");    
        ball.speedx = ball.speedx * (-1);

        draw_tinynumber(player[0].points, POINTS_X1, POINTS_Y, 0);
        player[0].points++;             //Punkt für Spieler 1
        draw_tinynumber(player[0].points, POINTS_X1, POINTS_Y, 1);
        
    }else if (ball.posy ==  1 + BALL_HITBX) { //Obere Wand
        DEBUG("Obere Wand getroffen\n");
        ball.speedy = ball.speedy * (-1);
        
    }else if (ball.posy == screeny - BALL_SIZE) { //Untere Wand
        DEBUG("Untere Wand getroffen\n");
        ball.speedy = ball.speedy * (-1);
              
    }

    
    ball.posx += ball.speedx * BALL_SPEED;
    ball.posy += ball.speedy * BALL_SPEED;
    pong_drawball(1); //Neuen Ball zeichnen
}

void pong_drawplayers(void) 
{
    //Zeichnen der Spielerposition
    if (player[0].posy != player[0].posyo) { //Wenn LCD Kontakte oben, Player1 ist links
        v_line(PLAYER_1_RANGE, player[0].posyo, PLAYER_SIZE, 0, 0);  //Löschen an alter Position
        v_line(PLAYER_1_RANGE, player[0].posy, PLAYER_SIZE, 0, 1); //Zeichnen an neuer Position
        player[0].posyo = player[0].posy;
    }

    if (player[1].posy != player[1].posyo) { //Wenn LCD Kontakte oben, Player2 ist rechts
        v_line(PLAYER_2_RANGE, player[1].posyo, PLAYER_SIZE, 0, 0);  //Löschen an alter Position
        v_line(PLAYER_2_RANGE, player[1].posy, PLAYER_SIZE, 0, 1); //Zeichnen an neuer Position
        player[1].posyo = player[1].posy;
    }
}


void draw_start_screen() //Startbildschirm malen
{   
    pong_drawplayers();
    
    draw_string("Sp1", 0, 0, 1, 0);
    draw_string("Sp2", screenx - FIELD_SPACE + 2, 0, 1, 0);
    
    draw_tinynumber(player[0].points, POINTS_X1, POINTS_Y, 1);
    draw_tinynumber(player[1].points, POINTS_X2, POINTS_Y, 1);
    
    draw_string("Warte auf", FIELD_CENTER_X - CTP_X(4) + 2, FIELD_CENTER_Y - CTP_Y(1) - 1, 1, 0);
    draw_string("Spieler", FIELD_CENTER_X - CTP_X(3) + 2, FIELD_CENTER_Y, 1, 0);
    
    rectangle(FIELD_SPACE,0, FIELD_SIZE + FIELD_SPACE, FIELD_SIZE - 1, 0, 1);   //Feld Umrandung
}

void reset_game(void)
{
    glcd_clear();
    
    ball.posx = FIELD_CENTER_X;
    ball.posy = FIELD_CENTER_Y;
    ball.speedx = 0;
    ball.speedy = 0;
    
    DeletePlayer(0);
    DeletePlayer(1);
            
    draw_start_screen();
}