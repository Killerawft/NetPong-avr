/*
 * glcd.h
 *
 * Created: 22.01.2017 01:31:45
 *  Author: Rene
 */ 


#ifndef GLCD_H_
#define GLCD_H_

#include <avr/pgmspace.h>
#include <util/delay.h>
#include <stdlib.h>
#include <math.h>

typedef unsigned char byte;

//----------------------
#define E_DELAY 9

#define DATAPORT PORTC
#define DATADDR DDRC
#define DATAPIN PINC

#define CONTROLPORT	PORTD
#define CONTROLDDR DDRD

#define RET (1<<7)
#define CS2 (1<<6)
#define CS1 (1<<5)
#define EN  (1<<4)
#define RW  (1<<3)
#define DI  (1<<2)

//#define CS_ACTIVE_LOW   1   //Define this if your GLCD CS
                              //is active low (refer to datasheet)

#define screenx 128
#define screeny 64

void trigger();
void glcd_on();
void glcd_off();
void set_start_line(byte line);
void goto_col(unsigned int x);
void goto_row(unsigned int y);
void goto_xy(unsigned int x,unsigned int y);
void glcd_write(byte b);
void glcd_clrln(byte ln);
void glcd_clear();
byte glcd_read(byte column);
void point_at(unsigned int x,unsigned int y,byte color);
void glcd_init();


#endif /* GLCD_H_ */