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

#define WAIT_FOR_PLAYER     0
#define COUNTDOWN           1
#define GAME_RUNNING        2
#define GAME_END            3

//Net Stack
#include <avr/io.h>
#include <avr/eeprom.h>
#include "config.h"
#include "usart.h"
#include "networkcard/enc28j60.h"
#include "stack.h"
#include "timer.h"
#include "cmd.h"
#include "base64.h"

// Game
#include <inttypes.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <avr/pgmspace.h>
#include <stdlib.h>
#include <math.h>
#include "game/glcd.h"
#include "game/timings.h"
#include "game/other.h"
#include "game/text.h"
#include "game/ppong.h"

void UdpParse(void);
void ParseNewPlayer(unsigned char* UdpData);
uint8_t NewPlayer(uint8_t PlayerNr, unsigned char* Name, unsigned long IpAddress);
void DeletePlayer(uint8_t PlayerNr);
void ClearUdp(unsigned char* UdpData, uint8_t DataSize);


uint8_t gamestatus = WAIT_FOR_PLAYER;        //0 Wenn auf Spieler gewartet wird | 1 Wenn das Spiel gestartet werden kann | 2 Wenn das Spiel gerade läuft | 3 Wenn das Spiel beendet ist.

//----------------------------------------------------------------------------
//Hier startet das Hauptprogramm
int main(void)
{  
	unsigned char reset_counter = 0;
	unsigned long time_update = 0;
	
    unsigned long a;
	
    uint8_t Countdwn = 4;
    
    glcd_init();
    //Ladescreen anzeigen
    draw_string("Net_Pong", FIELD_CENTER_X - CTP_X(3), FIELD_CENTER_Y - 10, 1, 0);
    draw_string("Loading...", FIELD_CENTER_X - CTP_X(3) + 2, FIELD_CENTER_Y - 3, 1, 0);
    
    DDRA = 0x70; //Port 6, 5, 4 als Ausgang für LED's
    LED1_OFF
    LED2_OFF
    LED3_OFF
    
    usart_init(BAUDRATE); // setup the UART
	
	usart_write("\n\rSystem Ready\n\r");
    usart_write("Compiliert am "__DATE__" um "__TIME__"\r\n");
    usart_write("Compiliert mit GCC Version "__VERSION__"\r\n");

	for(a=0;a<1000000;a++){asm("nop");};

	//Applikationen starten
	stack_init();
    
	//Ethernetcard Interrupt enable
	ETH_INT_ENABLE;
	
	//Globale Interrupts einschalten
	sei();    
        
    usart_write("\r\nIP   %1i.%1i.%1i.%1i\r\n", myip[0]     , myip[1]     , myip[2]     , myip[3]);
    usart_write("MASK %1i.%1i.%1i.%1i\r\n", netmask[0]  , netmask[1]  , netmask[2]  , netmask[3]);
    usart_write("GW   %1i.%1i.%1i.%1i\r\n", router_ip[0], router_ip[1], router_ip[2], router_ip[3]);
	
    add_udp_app(GAME_PORT, (void(*)(unsigned char))UdpParse);
    
    reset_game();
    
	while(1)
	{
        
        
        //Spiel steuerung
        if (player[0].Name[0] != NULL && player[1].Name[0] != NULL && gamestatus == WAIT_FOR_PLAYER) //Beide Spieler angemeldet, und Spiel läuft nicht -> Spiel kann starten
        {
            draw_string("Warte auf", FIELD_CENTER_X - CTP_X(4) + 2, FIELD_CENTER_Y - CTP_Y(1) - 1, 0, 0); //Warte auf Spieler TExt löschen
            draw_string("Spieler", FIELD_CENTER_X - CTP_X(3) + 2, FIELD_CENTER_Y, 0, 0);
            gamestatus = COUNTDOWN;
            DEBUG("\nGamestatus 1\n");
        }else if (gamestatus == GAME_END) //Spiel beendet, gewinner anzeigen
        {
           draw_string("Gewinner", FIELD_CENTER_X - CTP_X(4), FIELD_CENTER_Y - CTP_Y(1), 1, 0);
           
           if (player[0].points >= WIN_POINTS)
                draw_string(player[0].Name, FIELD_CENTER_X - CTP_X(4), FIELD_CENTER_Y, 1, 0);
           if (player[1].points >= WIN_POINTS)
                draw_string(player[1].Name, FIELD_CENTER_X - CTP_X(4), FIELD_CENTER_Y, 1, 0);
        
           reset_game();           	 
        }                  

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
                
                
                if (gamestatus == COUNTDOWN && Countdwn > 0) //Countdown ausgeben
                {
                    draw_tinynumber(Countdwn, FIELD_CENTER_X, FIELD_CENTER_Y, 0); //Zahl löschen
                    Countdwn--;
                    draw_tinynumber(Countdwn, FIELD_CENTER_X, FIELD_CENTER_Y, 1); //Zahl schreiben
                    
                }else if (gamestatus == COUNTDOWN && Countdwn == 0) //Countdown löschen und in Spielmodus wechseln
                {
                    draw_tinynumber(Countdwn, FIELD_CENTER_X, FIELD_CENTER_Y, 0); //Zahl löschen
                    //strcpy(eth_buffer[UDP_DATA_START], "Go");

                    //pong_drawball(1);
                    ball.posx = FIELD_CENTER_X;
                    ball.posy = FIELD_CENTER_Y;
                    
                    ball.speedx = 1; //Ball in eine Richtung bewegen
                    ball.speedy = -1;
                    gamestatus = GAME_RUNNING; //Spiel starten
                    DEBUG("\nGamestatus 2\n");                   
                }

                //TestConnection(0); //Verbindung zu den Spielern testen
                //TestConnection(1);	                      
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
    
    DEBUG("\n\n\nNeues Paket der Länge %i", DataSize);
    //print_ip(eth_buffer[IP_OFFSET]);
    
    LED2_TOGGLE //Datenempfang signalisieren
    
    if ((UdpData = malloc(DataSize + 1)) == NULL)
    {
        DEBUG("Empfangene Paket zu lang. Nicht genügend Speicher vorhanden");
        return;   
    }        
    	
    //Daten Sichern bevor der eth_buffer von woanders überschrieben wird  
    uint8_t i = 0;
    
    for (uint8_t i = 0; i < DataSize; i++)
        UdpData[i] = eth_buffer[UDP_DATA_START + i];

    DEBUG("\nUDP Daten: %s\n\n", UdpData);
    
    //Daten Analysieren
   if (UdpData[0] == 'N')   //Neuer Spieler
      ParseNewPlayer(UdpData);
   else if (UdpData[0] == 'S') //Befehl von einem Spieler
   {
       uint8_t PlayerNr = UdpData[1] - 48;
       DataCnt = CNTRL_HEAD;
       
       if (UdpData[DataCnt] == 'M' && player[PlayerNr].Name[0] != NULL) //Move Befehl wenn Spieler vorhanden
       {        
           DataCnt++;
              
           if (((UdpData[DataCnt] == 'R' && PlayerNr == 0) || (UdpData[DataCnt] == 'L' && PlayerNr == 1)) && player[PlayerNr].posy < screeny - PLAYER_SIZE - 1) //Spieler 1 nach Rechts bewegen aus seiner Sicht also nach oben
               player[PlayerNr].posy++;
           else if (((UdpData[DataCnt] == 'L' && PlayerNr == 0) || (UdpData[DataCnt] == 'R' && PlayerNr == 1)) && player[PlayerNr].posy > 1)               	
               player[PlayerNr].posy--;    
           
           DEBUG(" Spieler %i auf Position %i\n", PlayerNr + 1, player[PlayerNr].posy);                 
       }       
     
   }else if (UdpData[0] == 'C') //Verbindungsabfrage Antwort von Spieler
   {
       player[UdpData[1] - 48].connected = 1; //Verbindung wieder hergestellt
       player[UdpData[1] - 48].retry = 0;
   }else if (UdpData[0] == 'D') //Verbindung zu Spieler trennen
   {
       DeletePlayer(UdpData[1] - 48);
   }    
    
    ClearUdp(UdpData, DataSize); //Speicher nullen
    free(UdpData); //Speicherplatz wieder frei geben
}

void ParseNewPlayer(unsigned char* UdpData) //Daten für neuen SPieler parsen und versuchen Hinzuzufügen. Die UDP Response wird ebenfalls gesendet
{
  uint8_t DataCnt = 0;
  uint8_t PlayerNr = UdpData[1] - 48;
  unsigned char Name[NAME_SIZE + 1];
  unsigned long IpAddress;
  
  DEBUG("Neuen Spieler hinzufügen\n");
  
  if (PlayerNr > 1 || PlayerNr < 0) //Falsche Spielernummer
  return;
  DEBUG("Player Nr: %i\n", PlayerNr);
  
  //Namen entnehmen
  uint8_t i = 0;
  
  while (UdpData[CNTRL_HEAD + i] != ';')
  {
      Name[i] = UdpData[CNTRL_HEAD + i];
      i++;
      if (i >= NAME_SIZE)
      	continue;
  }
  Name[i] = NULL; //Name String abschließen
  
  DEBUG("Name: %s\n", Name);
  
  DataCnt = i + CNTRL_HEAD + 1; //Zähler auf aktuelle Position verschieben, da i gleich noch neu gebraucht wird

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
        
    DEBUG("\nNeuer Spieler Nr: %i  Name: %s  IP: ", PlayerNr, Name);
    print_ip(IpAddress);
    strcpy(player[PlayerNr].Name, Name);
    player[PlayerNr].IpAddress = IpAddress;
    
    draw_string(player[PlayerNr].Name, PlayerNr * (screenx - FIELD_SPACE + 2), CTP_Y(1) + 3, 1, 0);
    
    if (PlayerNr == 0)
        LED1_ON
    else
        LED3_ON
    	 
    return 1;
}

void DeletePlayer(uint8_t PlayerNr)
{
    if (PlayerNr > 1 || PlayerNr < 0) //Ungültige Nummer
    	return;
    
    draw_string(player[PlayerNr].Name, PlayerNr * (screenx - FIELD_SPACE + 2), CTP_Y(1) + 3, 0, 0); //Spielernamen löschen
    
    DEBUG("\n\nDelete IP: ");
    print_ip(player[PlayerNr].IpAddress);
    
    if(player[PlayerNr].IpAddress != IP(0,0,0,0))
    {
       strcpy(eth_buffer[UDP_DATA_START], "Disc"); //Disconnect an gelöschten Spieler senden, damit er das mitbekommt
       create_new_udp_packet(4, GAME_PORT, GAME_PORT, player[PlayerNr].IpAddress); 
    }
      
    player[PlayerNr].posy = PLAYER_LINE_START;
    pong_drawplayers(); //Spieler an neuer Position zeichnen, danach erst alte Position löschen, sonst wird der alte Balken nicht gelöscht
    player[PlayerNr].posyo = PLAYER_LINE_START;
    player[PlayerNr].points = 0;
    player[PlayerNr].IpAddress = IP(0,0,0,0);
    player[PlayerNr].connected = 0;
    player[PlayerNr].retry = 0;
    
    for (uint8_t i = 0; i < NAME_SIZE; i++)
        player[PlayerNr].Name[i] = NULL;
          
    gamestatus = 0; //Es wird wieder auf Spieler gewartet
    
    

    if (PlayerNr == 0)
        LED1_OFF
    else
        LED3_OFF
}

void TestConnection(uint8_t PlayerNr) //Verbindung zu Spieler testen
{
    if (player[PlayerNr].Name[PlayerNr] != NULL && player[PlayerNr].connected == 1) //Verbindung testen Paket senden
    {
        player[PlayerNr].connected = 0; //Verbindung wird als Unterbrochen angenommen
        player[PlayerNr].retry = 1; //Erster Versuch
                        
        strcpy(eth_buffer[UDP_DATA_START], "CON");
        create_new_udp_packet(3, GAME_PORT, GAME_PORT, player[PlayerNr].IpAddress);
    }else if (player[PlayerNr].Name[PlayerNr] != NULL && player[PlayerNr].connected == 0 && player[PlayerNr].retry <= RETRYS) //Es kann noch versucht werden weiter zu senden
    {
        player[PlayerNr].retry++;
        strcpy(eth_buffer[UDP_DATA_START], "CON");
        create_new_udp_packet(3, GAME_PORT, GAME_PORT, player[PlayerNr].IpAddress);
    }else if (player[PlayerNr].Name[PlayerNr] != NULL && player[PlayerNr].connected == 0 && player[PlayerNr].retry > RETRYS)  //Verbindung abgebrochen
    {
        DEBUG("\n\nVerbindung zu Spieler %i verloren\n", PlayerNr + 1);
        DeletePlayer(0);
        DeletePlayer(1);
        gamestatus = 1;  //Spiel abbrechen
    }
}

void ClearUdp(unsigned char* UdpData, uint8_t DataSize) //Zum nullen der Empfangspuffer Sicherung
{
    for (uint8_t i = 0; i < DataSize; i++)
        UdpData[i] = NULL;
}

void print_ip(unsigned long ip)
{
    unsigned char bytes[4];
    bytes[0] = ip & 0xFF;
    bytes[1] = (ip >> 8) & 0xFF;
    bytes[2] = (ip >> 16) & 0xFF;
    bytes[3] = (ip >> 24) & 0xFF;
    usart_write("%i.%i.%i.%i\n", bytes[0], bytes[1], bytes[2], bytes[3]);
}