/*
 * snmp_fluke.c
 *
 * Created: 10.12.2016 16:44:29
 *  Author: Rene
 */ 

#include <avr/pgmspace.h>
#include <stdlib.h>
#include "snmp.h"
#include "stack.h"
#include "usart.h"

void snmp_init(void) //Funktionen initialisieren und Funktion bei UDP Port registrieren
{
    add_udp_app(SNMP_REQ_PORT, (void(*)(unsigned char))snmp_udp_rec);
}

void snmp_udp_rec(unsigned char Index) //Bei Paketempfang auf SNMP_REQ_PORT wird diese Funktion aufgerufen
{

    unsigned char *RecPak; //Sicherungskopie der empfangenen SNMP Nachricht
    uint8_t SeqSize = eth_buffer[UDP_DATA_START + 1]; //Länge der SNMP Nachricht
    uint8_t PakPos = 0; //Position im empfangenen Paket
    uint8_t PDU_Type = 0; //Der gefundene PDU Type wird zwischengespeichert
    
    //usart_write("%s", &eth_buffer[UDP_DATA_START]);
    
    //return;
    
    
    if ((uint8_t)eth_buffer[UDP_DATA_START] != BER_SEQUENCE) //Prüfen ob angekommenes Paket eine erwartete Sequenz ist
        return;
    
    RecPak = malloc(SeqSize + 1); //Speicher reservieren um da das angekommene Paket zu sichern

    for (uint8_t i = 0; i < SeqSize; i++)
        RecPak[i] = eth_buffer[UDP_DATA_START + 2 + i];

    
    volatile SNMP_DATAGRAM SNMP_Data;
    
    // ------------ VERSION ------------
    PakPos++; //Datentyp überspringen
    PakPos++; //Datenlänge überspringen
                
    SNMP_Data.Version = RecPak[PakPos];
                
    PakPos++; //Positionspointer auf Community String schieben


    // ------------ COMMUNITY STRING ------------
    PakPos++; //Von Type auf Length springen
    uint8_t length = RecPak[PakPos];
    PakPos++; //Auf die Daten springen
    for (uint8_t i = 0; i < length; i++)
    {   
            SNMP_Data.Comm_Name[i] = RecPak[PakPos + i]; //Daten in String kopieren
    } 
    SNMP_Data.Comm_Name[length] = NULL;                  
    PakPos += length; //Positionspointer auf nächsten Type schieben


    // ------------ PDU TYPE ------------
    PDU_Type = RecPak[PakPos];
    PakPos++; //Positionspointer auf Request ID schieben
    
    
    // ------------ REQUEST ID ------------
    PakPos++; //Von Type auf Length springen
    SNMP_Data.RequestID = 0;
    if (RecPak[PakPos] == 2)
    {
       PakPos++;
       SNMP_Data.RequestID |= (RecPak[PakPos] << 8);
       PakPos++;
       SNMP_Data.RequestID = RecPak[PakPos];
    }else if (RecPak[PakPos] == 1)
    {
        PakPos++; //Da Type immer 1 gleich weiter auf die Daten springen
        SNMP_Data.RequestID = RecPak[PakPos];
    }    
    PakPos++; //Auf den nächsten Datensatzt springen
    // ------------ ERROR ------------ 
    PakPos++; //Von Type auf Length springen
    PakPos++; //Da Type immer 1 gleich weiter auf die Daten springen
    SNMP_Data.Error = RecPak[PakPos];
    
    // ------------ ERROR INDEX ------------
    PakPos++; //Von Type auf Length springen
    PakPos++; //Da Type immer 1 gleich weiter auf die Daten springen
    SNMP_Data.ErrorIndex = RecPak[PakPos];         
    
        
    
    
    usart_write("Version: %i \nComm: %s \nPDU: %i \nReq ID: %i\n Error: %i \nError Idx: %i \n\n\n", SNMP_Data.Version, SNMP_Data.Comm_Name, PDU_Type, SNMP_Data.RequestID, SNMP_Data.Error, SNMP_Data.ErrorIndex);
}