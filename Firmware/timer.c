/*----------------------------------------------------------------------------
 Copyright:      Radig Ulrich  mailto: mail@ulrichradig.de
 Author:         Radig Ulrich
 Remarks:
 known Problems: none
 Version:        24.10.2007
 Description:    Timer Routinen

 Dieses Programm ist freie Software. Sie können es unter den Bedingungen der 
 GNU General Public License, wie von der Free Software Foundation veröffentlicht, 
 weitergeben und/oder modifizieren, entweder gemäß Version 2 der Lizenz oder 
 (nach Ihrer Option) jeder späteren Version. 

 Die Veröffentlichung dieses Programms erfolgt in der Hoffnung, 
 daß es Ihnen von Nutzen sein wird, aber OHNE IRGENDEINE GARANTIE, 
 sogar ohne die implizite Garantie der MARKTREIFE oder der VERWENDBARKEIT 
 FÜR EINEN BESTIMMTEN ZWECK. Details finden Sie in der GNU General Public License. 

 Sie sollten eine Kopie der GNU General Public License zusammen mit diesem 
 Programm erhalten haben. 
 Falls nicht, schreiben Sie an die Free Software Foundation, 
 Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307, USA. 
------------------------------------------------------------------------------*/

#include "config.h"
#include <avr/io.h>
#include <avr/interrupt.h>
#include "usart.h"
#include "stack.h"
//#include "ntp.h"
#include "cmd.h"
//#include "dhcpc.h"
#include "timer.h"

#if USE_ARTNET
	#include "artnet.h"
#endif

volatile unsigned long time;
volatile unsigned long time_watchdog = 0;

//----------------------------------------------------------------------------
//Diese Routine startet und inizialisiert den Timer
void timer_init (void)
{
	#if EXTCLOCK==1
		#if defined (__AVR_ATmega644__)
			//Asynchroner Modus ein, Oszillator an TOSC1 und TOSC2 aktiv
			ASSR |= (1<<AS2);
			TCCR2B = 0x05;
			while(ASSR & 0x11);
			//Capture/Compare-Interrupt aktiv
			TIMSK2 |= (1<<OCIE2A);
		#else
			//Asynchroner Modus ein, Oszillator an TOSC1 und TOSC2 aktiv
			ASSR  = (1<<AS2);
			//CTC-Modus an (Clear Timer on Compare Match)
			TCCR2 = (1<<WGM21);			
			//dieser Wert ergibt eine Sekunde Periodendauer
			OCR2  = 31;	
			//lösche Prescaler 2				
			SFIE  = (1<<PSR2);			
			//Starte Timer 2 mit Prescaler gleich 1/1024
			TCCR2 |= (1<<CS22)|(1<<CS21)|(1<<CS20); 
			while(ASSR & 0x07);
			//Capture/Compare-Interrupt aktiv
			TIMSK = (1<<OCIE2);			
		#endif
	#else
			TCCR1B |= (1<<WGM12) | (0<<CS10 | 0<<CS11 | 1<<CS12);
			TCNT1 = 0;
			OCR1A = (F_CPU / 1024) - 1;
			TIMSK1 |= (1 << OCIE1A);
            
            //ca. alle 16ms Überlauf
            TCCR0B |= (1<<WGM02) | (1<<CS00 | 0<<CS01 | 1<<CS02);
            TCNT0 = 0;
            OCR0A = 250;
            TIMSK0 |= (1<<OCIE0A);
            
	#endif
return;
};

//----------------------------------------------------------------------------
//Timer Interrupt
#if EXTCLOCK==1
	#if defined (__AVR_ATmega644__)
    ISR (TIMER2_COMPA_vect)
	#else
    ISR (TIMER2_COMP_vect)
	#endif
#else
	ISR (TIMER1_COMPA_vect)
#endif
{
	//tick 1 second
	time++;
    if((stack_watchdog++) > WTT)  //emergency reset of the stack
    {
        RESET();
	}
    eth.timer = 1;
	
	#if USE_NTP
	ntp_timer--;
	#endif //USE_NTP
	
	#if USE_DHCP
	if ( dhcp_lease > 0 ) dhcp_lease--;
    if ( gp_timer   > 0 ) gp_timer--;
    #endif //USE_DHCP
	
	#if USE_ARTNET
	artnet_tick();
	#endif //USE_ARTNET
}

ISR (TIMER0_COMPA_vect)
{
    static uint8_t tick;
    
    
    if (tick == PLAYER_SPEED || tick == BALL_SPEED) //Der Spielbalken ist doppel so schnell wie der Ball
        pong_drawplayers(); //Spielbalken malen
    if (tick == BALL_SPEED && gamestatus == 2) //Spiel läuft
        pong_moveball();
    
    
    if ((player[0].points >= WIN_POINTS || player[1].points >= WIN_POINTS) && gamestatus == 2) //Das Spiel wurde gewonnen
        gamestatus = 3;
        
    tick++;
    
    if (tick > BALL_SPEED)
    	tick = 0;
}
