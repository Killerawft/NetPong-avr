/*
 * GLCD Test.c
 *
 * Created: 21.01.2017 02:15:33
 * Author : Rene
 */ 

#define F_CPU 16000000

#define BALL_SIZE           3                                       //Radius des Balls
#define PLAYER_SIZE         10                                      //Breite des Spielerbalkens, muss durch 2 teilbar sein
#define FIELD_SIZE          64                                      //Feldgröße -> Feld ist immer rechteckig
#define FIELD_SPACE         32                                      //Bereich neben dem Feld
#define PLAYER_LEFT_RANGE   FIELD_SPACE + 2                         //Bereich für Linken Spielerbalken
#define PLAYER_RIGHT_RANGE  FIELD_SPACE + FIELD_SIZE - 2            //Bereich für Rechten Spielerbalken
#define PLAYER_LINE_START   (FIELD_SIZE / 2) - (PLAYER_SIZE / 2)    //Startpunkt des Spielerbalkens

#include <avr/io.h>
#include <util/delay.h>
#include "glcd.h"


void ball(unsigned int x,unsigned int y, byte c)
 {
    h_line(x, y - 1, BALL_SIZE, 0, c);
    h_line(x, y, BALL_SIZE, 0, c);
    h_line(x, y + 1, BALL_SIZE, 0, c);
 }

int main(void)
{
    DDRA = 0xF0; // Pin 4 bis 7 auf Ausgang

    PORTA = 0xF0 ;

    glcd_init();
    
    glcd_puts("Ping Pong",20,3,0,1,0);

    _delay_ms(1000);
    glcd_clear();
    rectangle(FIELD_SPACE,0, FIELD_SIZE + FIELD_SPACE, FIELD_SIZE - 1, 0, 1);
    v_line(PLAYER_LEFT_RANGE, PLAYER_LINE_START, PLAYER_SIZE, 0, 1);
    v_line(PLAYER_RIGHT_RANGE, PLAYER_LINE_START, PLAYER_SIZE, 0, 1);
   
   
    
    while (1) 
    {
       for (uint8_t i = PLAYER_LEFT_RANGE + 1; i < PLAYER_RIGHT_RANGE - 2; i++)
       {
           ball( i, 32, 1);
           _delay_ms(30);
           ball( i, 32, 0);
       }
        
       for (uint8_t i = PLAYER_RIGHT_RANGE - 3; i > PLAYER_LEFT_RANGE + 2; i--)
       {
           ball( i, 32, 1);
           _delay_ms(30);
           ball( i, 32, 0);
       }
      
    }
}

