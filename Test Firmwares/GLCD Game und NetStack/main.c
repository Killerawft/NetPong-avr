/*----------------------------------------------------------------------------
 Copyright:      Radig Ulrich  mailto: mail@ulrichradig.de
 Author:         Radig Ulrich
 Remarks:        
 known Problems: none
 Version:        24.10.2007
 Description:    Webserver uvm.

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
----------------------------------------------------------------------------*/

#define F_CPU 16000000UL        //Taktfrequenz für delay.h hier definiert und nicht in config.h vom Stack

// Net Stack Bibliotheken
#include "inc/NetStack/config.h"
#include "inc/NetStack/usart.h"
#include "inc/NetStack/enc28j60.h"
#include "inc/NetStack/stack.h"
#include "inc/NetStack/timer.h"
#include "inc/NetStack/cmd.h"
#include "inc/NetStack/base64.h"
 
// //Bibliotheken fürs Ping Pong spiel
// #include "inc/Game/timings.h"
// #include "inc/Game/other.h"
// #include "inc/Game/text.h"
// #include "inc/Game/glcd.h"
// #include "inc/Game/ppong.h"

//Externe Funktionen
#include <avr/io.h>
#include <avr/eeprom.h>
#include <inttypes.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <avr/pgmspace.h>
#include <avr/eeprom.h>
#include <stdlib.h>
#include <math.h>


//----------------------------------------------------------------------------
//Hier startet das Hauptprogramm
int main(void)
{  
	unsigned char reset_counter = 0;
	unsigned long time_update = 0;
	
    unsigned long a;
	
    usart_init(BAUDRATE); // setup the UART
	
	usart_write("\n\rSystem Ready\n\r");
    usart_write("Compiliert am "__DATE__" um "__TIME__"\r\n");
    usart_write("Compiliert mit GCC Version "__VERSION__"\r\n");

	for(a=0;a<1000000;a++){asm("nop");};

	//Applikationen starten
	stack_init();
    
    #if USE_SNMP
    snmp_init();
    #endif
		
	//Ethernetcard Interrupt enable
	ETH_INT_ENABLE;
	
	//Globale Interrupts einschalten
	sei(); 
		
    usart_write("\r\nIP   %1i.%1i.%1i.%1i\r\n", myip[0]     , myip[1]     , myip[2]     , myip[3]);
    usart_write("MASK %1i.%1i.%1i.%1i\r\n", netmask[0]  , netmask[1]  , netmask[2]  , netmask[3]);
    usart_write("GW   %1i.%1i.%1i.%1i\r\n", router_ip[0], router_ip[1], router_ip[2], router_ip[3]);
	    	
	while(1)
	{
	    eth_get_data();
				
        //Terminalcommandos auswerten
		if (usart_status.usart_ready){
            usart_write("\r\n");
			if(extract_cmd(&usart_rx_buffer[0]))
			{
				usart_write("Ready\r\n\r\n");
			}
			else
			{
				usart_write("ERROR\r\n\r\n");
			}
			usart_status.usart_ready =0;
		}
        
        if(ping.result)
        {
            usart_write("Get PONG: %i.%i.%i.%i\r\n",ping.ip1[0],ping.ip1[1],ping.ip1[2],ping.ip1[3]); 
            ping.result = 0;
        }
		
		#if USE_ENC28J60
			//wird jede Sekunde aufgerufen
			if(time != time_update)
			{
				time_update = time;
				if(eth.no_reset)
				{
					reset_counter = 0;
					eth.no_reset = 0;
				}
				else
				{
					if((reset_counter++)>5)
					{
						reset_counter = 0;
						enc_init();
					}
				}
                
			}			
		#endif
    }//while (1)
		
return(0);
}