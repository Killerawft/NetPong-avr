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

#include <avr/io.h>
#include <avr/eeprom.h>
#include "config.h"
#include "usart.h"
#include "networkcard/enc28j60.h"
#include "stack.h"
#include "timer.h"
#include "cmd.h"
#include "base64.h"

//#include "NetParse.h"

// Externe Funktionen
#include <inttypes.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <avr/pgmspace.h>
#include <stdlib.h>
#include <math.h>

//Um zu wissen für was uint8_t u.s.w. steht siehe inttypes.h
typedef uint16_t u16;
typedef int16_t  s16;
typedef uint32_t u32;
typedef int32_t  s32;

#include "game/glcd.h"
#include "game/timings.h"
#include "game/other.h"
#include "game/text.h"
#include "game/ppong.h"

void UdpParse(void);
void ParseNewPlayer(unsigned char* UdpData, uint8_t DataCnt);
uint8_t NewPlayer(uint8_t PlayerNr, unsigned char* Name, unsigned long IpAddress);
void DeletePlayer(uint8_t PlayerNr);

// void DisplayData()
// {
//     eth_buffer[UDP_DATA_END_VAR + 1] = NULL; //Empfangene Daten terminieren für Draw_String
//     
//     draw_string(&eth_buffer[UDP_DATA_START],10, 10, 1, 0);
//     
//     create_new_udp_packet(UDP_DATA_END_VAR - UDP_DATA_START, 2222, 55056, IP(192,168,1,51));
//     LED2_TOGGLE
// }

//----------------------------------------------------------------------------
//Hier startet das Hauptprogramm
int main(void)
{  
	unsigned char reset_counter = 0;
	unsigned long time_update = 0;
	
    unsigned long a;
	
    DDRA = 0x70; //Port 6, 5, 4 als Ausgang für LED's
    LED1_ON
    
    usart_init(BAUDRATE); // setup the UART
	
	usart_write("\n\rSystem Ready\n\r");
    usart_write("Compiliert am "__DATE__" um "__TIME__"\r\n");
    usart_write("Compiliert mit GCC Version "__VERSION__"\r\n");

	for(a=0;a<1000000;a++){asm("nop");};

	//Applikationen starten
	stack_init();
    
    glcd_init();
    
    FrameTimerInit();
    
	//Ethernetcard Interrupt enable
	ETH_INT_ENABLE;
	
	//Globale Interrupts einschalten
	sei(); 
		
    //FrameTimerStart();   
        
    usart_write("\r\nIP   %1i.%1i.%1i.%1i\r\n", myip[0]     , myip[1]     , myip[2]     , myip[3]);
    usart_write("MASK %1i.%1i.%1i.%1i\r\n", netmask[0]  , netmask[1]  , netmask[2]  , netmask[3]);
    usart_write("GW   %1i.%1i.%1i.%1i\r\n", router_ip[0], router_ip[1], router_ip[2], router_ip[3]);
	
    add_udp_app(GAME_PORT, (void(*)(unsigned char))UdpParse);
    
    ball.posx = FIELD_CENTER_X;
    ball.posy = FIELD_CENTER_Y;
    ball.speedx = 0;
    ball.speedy = 0;
    
    player[0].posy = PLAYER_LINE_START;
    player[0].posyo = 0;
    player[0].IpAddress = IP(0,0,0,0);
    
    for (uint8_t i = 0; i < NAME_SIZE; i++)
        player[0].Name[i] = NULL;
    
    player[1].posy = PLAYER_LINE_START;
    player[1].posyo = 0;
    player[1].IpAddress = IP(0,0,0,0);
    
    for (uint8_t i = 0; i < NAME_SIZE; i++)
        player[1].Name[i] = NULL;
     
    draw_start_screen();
    
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

void UdpParse(void)
{
    unsigned char* UdpData; //Daten von UDP Paket
    uint8_t DataCnt = 0;
    uint8_t DataSize = UDP_DATA_END_VAR - UDP_DATA_START;   //Länge der Nutzdaten ist das Erste Byte der Daten
    
    DEBUG("\n\n\nNeues Paket der Länge %i\n", DataSize);

    
    if ((UdpData = malloc(DataSize + 1)) == NULL)
    {
        DEBUG("Empfangene Paket zu lang. Nicht genügend Speicher vorhanden");
        return;   
    }        
    	
    //Daten Sichern bevor der eth_buffer von woanders überschrieben wird  
    uint8_t i = 0;
    
    for (uint8_t i = 0; i < DataSize; i++)
        UdpData[i] = eth_buffer[UDP_DATA_START + i];
    
    DEBUG("UDP Daten: %s\n\n", UdpData);
    
    //Daten Analysieren
   if (UdpData[0] == 'N')   //Neuer Spieler
      ParseNewPlayer(UdpData, DataCnt);
    
    free(UdpData); //Speicherplatz wieder frei geben
}

void ParseNewPlayer(unsigned char* UdpData, uint8_t DataCnt) //Daten für neuen SPieler parsen und versuchen Hinzuzufügen. Die UDP Response wird ebenfalls gesendet
{
  uint8_t PlayerNr = UdpData[1] - 48;
  unsigned char Name[NAME_SIZE + 1];
  unsigned long IpAddress;
  
  DEBUG("Neuen Spieler hinzufügen\n");
  
  if (PlayerNr > 1 || PlayerNr < 0) //Falsche Spielernummer
  return;
  DEBUG("Player Nr: %i\n", PlayerNr);
  
  //Namen entnehmen
  uint8_t i = 0;
  
  while (UdpData[3 + i] != ';')// && i < NAME_SIZE)
  {
      Name[i] = UdpData[3 + i];
      i++;
  }
  Name[i] = NULL; //Name String abschließen
  
  DEBUG("Name: %s\n", Name);
  
  DataCnt = i + 3 + 1; //Zähler auf aktuelle Position verschieben, da i gleich noch neu gebraucht wird

  //IP Adresse auslesen und umwandeln
  char IpStr[4];
  uint8_t IpPart[4];
  
  for (uint8_t j = 0; j < 4; j++) //einzelne IP Teile auslesen
  {
      i = 0;
      
      while (UdpData[DataCnt + i] != '.' && i < 3)
      {
          IpStr[i] = UdpData[DataCnt + i];
          i++;
      }
      
      IpStr[i] = NULL;
      IpPart[j] = atoi(IpStr);
      DataCnt = DataCnt + i + 1;
  }
  
  DEBUG ("IP: %i.%i.%i.%i\n", IpPart[0], IpPart[1], IpPart[2], IpPart[3]);
  IpAddress = IP(IpPart[0], IpPart[1], IpPart[2], IpPart[3]);
  
  if(NewPlayer(PlayerNr, Name, IpAddress) == 1)
  {
      strcpy(&eth_buffer[UDP_DATA_START], "NP_OK");
      create_new_udp_packet(5, GAME_PORT, GAME_PORT, IpAddress);
  }else
  {
      strcpy(&eth_buffer[UDP_DATA_START], "NP_NOK");
      create_new_udp_packet(6, GAME_PORT, GAME_PORT, IpAddress);
  }  
}

uint8_t NewPlayer(uint8_t PlayerNr, unsigned char* Name, unsigned long IpAddress)
{
    if (player[PlayerNr].Name[0] != NULL) //Spieler bereits vorhanden
    {
        DEBUG("Spieler bereits vorhanden\n");
        return -1;
    }
        
    DEBUG("\nNeuer Spieler Nr: %i  Name: %s  IP: %i", PlayerNr, Name, IpAddress);
    strcpy(player[PlayerNr].Name, Name);
    player[PlayerNr].IpAddress = IpAddress;
    
    return 1;
}

void DeletePlayer(uint8_t PlayerNr)
{
    if (PlayerNr > 1 || PlayerNr < 0) //Ungültige Nummer
    	return;
    
    player[PlayerNr].posy = 0;
    player[PlayerNr].IpAddress = IP(0,0,0,0);
    
    for (uint8_t i = 0; i < NAME_SIZE; i++)
        player[PlayerNr].Name[i] = NULL;
}