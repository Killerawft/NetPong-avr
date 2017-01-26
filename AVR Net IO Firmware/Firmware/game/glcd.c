/*********************************************************************************************
 * Osama's Lab KS0108 GLCD library; a library that facilitates interfacing KS0108 based GLCDs.
 *
 *   Copyright (C) 2010-2012  Osama's Lab
 *
 *   This program is free software: you can redistribute it and/or modify
 *   it under the terms of the GNU Affero General Public License as
 *   published by the Free Software Foundation, either version 3 of the
 *   License.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU Affero General Public License for more details.
 *
 *   You should have received a copy of the GNU Affero General Public License
 *   along with this program.  If not, see <http://www.gnu.org/licenses/>
 ********************************************************************************************/

#include "main.h"

//------------------------
void trigger()
{
    CONTROLPORT |= EN;	//EN high
    _delay_us(E_DELAY);
    CONTROLPORT &= ~EN;	//EN low
    _delay_us(E_DELAY);
}
//------------------------
void glcd_init()
{
    //GLCD Ports definieren
    DATADDR = 0xFF; //Data port direction
    CONTROLDDR = 0xFF; //Control port direction
    
    CONTROLPORT |= RET; //Display Reset auf high
    
    //Display zwei mal resetten
    //Erster Reset
    CONTROLPORT &= ~RET; //Display REset auf Low
    _delay_us(300);
    CONTROLPORT |= RET; //Display Reset auf high
    
    _delay_us(300);
    
    //Zweiter Reset
    CONTROLPORT &= ~RET; //Display REset auf Low
    _delay_us(300);
    CONTROLPORT |= RET; //Display Reset auf high
    
    //GLCD Einschalten
    glcd_on();
    glcd_clear();
}
//----------------------
void glcd_on()
{
    #ifdef CS_ACTIVE_LOW
    CONTROLPORT &= ~CS1;	//Activate both chips
    CONTROLPORT &= ~CS2;
    #else
    CONTROLPORT |= CS1;	//Activate both chips
    CONTROLPORT |= CS2;
    #endif
    CONTROLPORT &= ~DI;	//DI low --> command
    CONTROLPORT &= ~RW;	//RW low --> write
    DATAPORT = 0x3F; 	//ON command
    trigger();
}
//----------------------
void glcd_off()
{
    #ifdef CS_ACTIVE_LOW
    CONTROLPORT &= ~CS1;	//Activate both chips
    CONTROLPORT &= ~CS2;
    #else
    CONTROLPORT |= CS1;	//Activate both chips
    CONTROLPORT |= CS2;
    #endif
    CONTROLPORT &= ~DI;	//DI low --> command
    CONTROLPORT &= ~RW;	//RW low --> write
    DATAPORT = 0x3E; //OFF command
    trigger();
}
//----------------------
void set_start_line(byte line)
{
    CONTROLPORT &= ~DI;			//RS low --> command
    CONTROLPORT &= ~RW;			//RW low --> write
    //Activate both chips
    #ifdef CS_ACTIVE_LOW
    CONTROLPORT &= ~CS1;
    CONTROLPORT &= ~CS2;
    #else
    CONTROLPORT |= CS1;
    CONTROLPORT |= CS2;
    #endif
    DATAPORT = 0xC0 | line;     //Set Start Line command
    trigger();
}
//----------------------
void goto_col(unsigned int x)
{
    byte pattern;
    CONTROLPORT &= ~DI;	//DI low --> command
    CONTROLPORT &= ~RW;	//RW low --> write
    if(x<64)             //left section
    {
        #ifdef CS_ACTIVE_LOW
        CONTROLPORT &= ~CS1;	//select chip 1
        CONTROLPORT |= CS2;	//deselect chip 2
        #else
        CONTROLPORT |= CS1;	//deselect chip 1
        CONTROLPORT &= ~CS2;	//select chip 2
        #endif
        pattern = x;      	//put column address on data port
    }
    else if(x < 128)                 //right section
    {
        #ifdef CS_ACTIVE_LOW
        CONTROLPORT |= CS1;	//deselct chip 1
        CONTROLPORT &= ~CS2;	//select chip 2
        #else
        CONTROLPORT &= ~CS1;	//deselct chip 1
        CONTROLPORT |= CS2;	//select chip 2
        #endif
        pattern = x-64;   //put column address on data port
    }else
    {
        return;
    }
    pattern = (pattern | 0x40 ) & 0x7F;  //Command format
    DATAPORT = pattern;
    trigger();
}
//----------------------
void goto_row(unsigned int y)
{
    byte pattern;
    CONTROLPORT &= ~DI;	//DI low --> command
    CONTROLPORT &= ~RW;	//RW low --> write
    pattern = (y | 0xB8) & 0xBF; //put row address on data port set command
    DATAPORT = pattern;
    trigger();
}
//----------------------
void goto_xy(unsigned int x,unsigned int y)
{
    goto_col(x);
    goto_row(y);
}
//----------------------
void glcd_write(byte b)
{
    CONTROLPORT |= DI;	//DI high --> data
    CONTROLPORT &= ~RW;	//RW low --> write
    DATAPORT = b;    	//put data on data port
    _delay_us(10);
    trigger();
}
//------------------------
void glcd_clrln(byte ln)
{
    int i;
    goto_xy(0,ln);      //At start of line of left side
    goto_xy(64,ln);     //At start of line of right side (Problem)
    #ifdef CS_ACTIVE_LOW
    CONTROLPORT &= ~CS1;
    #else
    CONTROLPORT |= CS1;
    #endif
    for(i=0;i<64;i++)
    glcd_write(0);
}
//-------------------------
void glcd_clear()
{
    int i;
    for(i=0;i<8;i++)
    glcd_clrln(i);
}
//-----------------------
byte glcd_read(byte column)
{
    byte read_data = 0; //Read data here
    DATADDR = 0x00;     //Input
    
    CONTROLPORT |= RW;	//Read
    CONTROLPORT |= DI;	//Data

    if(column<64)
    {
        #ifdef CS_ACTIVE_LOW
        CONTROLPORT &= ~CS1;	//Enable CS1
        CONTROLPORT |= CS2;		//Disable CS2
        #else
        CONTROLPORT |= CS1;		//Enable CS1
        CONTROLPORT &= ~CS2;	//Disable CS2
        #endif
    }
    else
    {
        #ifdef CS_ACTIVE_LOW
        CONTROLPORT &= ~CS2;	//Enable CS2
        CONTROLPORT |= CS1;		//Disable CS1
        #else
        CONTROLPORT |= CS2;		//Enable CS2
        CONTROLPORT &= ~CS1;	//Disable CS1
        #endif
    }
    _delay_us(1);        		//tasu
    CONTROLPORT |= EN;			//Latch RAM data into ouput register
    _delay_us(10);        		//twl + tf
    
    //Dummy read
    CONTROLPORT &= ~EN;			//Low Enable
    _delay_us(20);       		//tf + twl + chineese error
    
    CONTROLPORT |= EN;			//latch data from output register to data bus
    _delay_us(1);        		//tr + td(twh)
    
    read_data = DATAPIN;    	//Input data
    CONTROLPORT &= ~EN;			//Low Enable to remove data from the bus
    _delay_us(1);        		//tdhr
    DATADDR = 0xFF;     //Output again
    return read_data;
}
//-----------------------
void point_at(unsigned int x,unsigned int y,byte color)
{
#if FLIP_SCREEN
    x = screenx - x - 1;
    y = screeny - y - 1;
#endif
    
    byte pattern = 0;
    goto_xy(x,(int)(y/8));
    switch (color)
    {
        case 0:         //Light spot
        pattern = ~(1<<(y%8)) & glcd_read(x);
        break;
        default:         //Dark spot
        pattern = (1<<(y%8)) | glcd_read(x);
        break;
    }
    goto_xy(x,(int)(y/8));
    glcd_write(pattern);
}
//-----------------------
void h_line(unsigned int x,unsigned int y,byte l,byte s,byte c)
{
    int i;
    for(i=x; i<(l+x); i += (byte)(s+1))
    point_at(i,y,c);
}
//-----------------------
void v_line(unsigned int x,unsigned int y,signed int l,byte s,byte c)
{
    unsigned int i;
    for(i=y; i<(y+l); i += (byte)(s+1))
    point_at(x,i,c);
}
//-----------------------
void circle(unsigned int x0,unsigned int y0,unsigned int r,byte s,byte c)
{
    byte i,y,y00;
    y00 = r;            //Point (0,r) is the 1st point
    for(i=0; i<r; i++)
    {
        y = sqrt((int)r*r - (int)i*i);
        point_at((x0+i),(y0+y),c);
        point_at((x0+i),(y0-y),c);
        point_at((x0-i),(y0+y),c);
        point_at((x0-i),(y0-y),c);
        if(abs(y00-y)>1)
        {
            v_line(x0+(i-1),y0+fmin(y00,y)+1,abs(y00-y),s,c);
            v_line(x0+(i-1),y0-fmax(y00,y),abs(y00-y),s,c);
            v_line(x0-(i-1),y0+fmin(y00,y)+1,abs(y00-y),s,c);
            v_line(x0-(i-1),y0-fmax(y00,y),abs(y00-y),s,c);
        }
        y00 = y;
    }
    v_line(x0+(i-1) ,y0,y ,s,c);
    v_line(x0+(i-1) ,y0-y ,y,s,c);
    v_line(x0-(i-1) ,y0,y ,s,c);
    v_line(x0-(i-1) ,y0-y ,y,s,c);
}
//-----------------------
void rectangle(unsigned int x1,unsigned int y1, unsigned int x2,unsigned int y2, byte s,byte c)
{
    h_line(x1,y1,(x2-x1),s,c);
    h_line(x1,y2,(x2-x1),s,c);
    v_line(x1,y1,(y2-y1),s,c);
    v_line(x2,y1,(y2-y1+1),s,c);
}