/*----------------------------------------------------------------------------
 Copyright:      Stefan Krupop  mailto: mail@stefankrupop.de
 Author:         Stefan Krupop
 Remarks:        
 known Problems: none
 Version:        17.01.2009
 Description:    Implementation des ArtNet-Protokolls für DMX-Übertragung über Ethernet

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

#if USE_ARTNET
#ifndef _ARTNETCLIENT_H
	#define _ARTNETCLIENT_H

	//#define ARTNET_DEBUG usart_write
	#define ARTNET_DEBUG(...)

	#include <avr/io.h>
	#include <avr/pgmspace.h>
	#include "stack.h"
	#include "usart.h"

	#define ARTNET_SUBNET_EEPROM_STORE    	50
	#define ARTNET_INUNIVERSE_EEPROM_STORE	51
	#define ARTNET_OUTUNIVERSE_EEPROM_STORE	52
	#define ARTNET_PORT_EEPROM_STORE    	54
	#define ARTNET_NETCONFIG_EEPROM_STORE   56
	#define ARTNET_SHORTNAME_EEPROM_STORE	60
	#define ARTNET_LONGNAME_EEPROM_STORE    78

	void artnet_init(void);
	void artnet_sendPollReply(void);
	void artnet_main(void);
	void artnet_get(unsigned char);
	void artnet_tick(void);
	
#endif
#endif