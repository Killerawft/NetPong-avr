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
            SNMP_Data.Comm_Name[PakPos + i + 1] = NULL;  //String terminieren
            if (i >= (sizeof(SNMP_Data.Comm_Name) - 1)) //Nur 10 Byte für String
                continue;
    } 
                    
    PakPos += length; //Positionspointer auf nächsten Type schieben


    // ------------ PDU TYPE ------------
    SNMP_Data.PDU_Type = RecPak[PakPos];
    PakPos++; //Positionspointer auf nächsten Typen schieben
    
    PakPos++; //Von Paketlänge auf nächsten Typ springen
    
    // ------------ REQUEST ID ------------ 
    PakPos++; //Von Type auf Length springen
    SNMP_Data.RequestID = 0;
    if (RecPak[PakPos] == 2)
    {
        SNMP_Data.RequestID = RecPak[PakPos + 2]; //Low Teil der 16 Bit Zahl
        SNMP_Data.RequestID += (RecPak[PakPos + 1] << 8); //High Teil der 16 Bit Zahl
       PakPos++;
       PakPos++;     
    }else if (RecPak[PakPos] == 1)
    {
        PakPos++; //Da Type immer 1 gleich weiter auf die Daten springen
        SNMP_Data.RequestID = RecPak[PakPos];
    }    
    PakPos++; //Auf den nächsten Datensatzt springen
    // ------------ ERROR ------------ 
    PakPos++; //Von Type auf Length springen
    PakPos++; //Da length immer 1 gleich weiter auf die Daten springen
    SNMP_Data.Error = RecPak[PakPos];
    PakPos++; //Auf nächste Daten springen
    
    
    // ------------ ERROR INDEX ------------
    PakPos++; //Von Type auf Length springen
    PakPos++; //Da length immer 1 gleich weiter auf die Daten springen
    SNMP_Data.ErrorIndex = RecPak[PakPos];         
    
    PakPos++; //Von Daten auf nächsten Type springen
    
    // ------------ VARBIND LIST ------------
    PakPos++; //Von Type auf length springen
    
    if (RecPak[PakPos] > 0) //Es ist etwas in der Varbind List
    {
       volatile Managed_Object *Objects;
       
       Objects = malloc(RecPak[PakPos] + 1); //Speicher reservieren für die einzelnen Objekte
     
       PakPos += 2; //Von List Length auf Varbind objekt length springen
       
       // ------------ VARBIND ------------
       while (RecPak[PakPos] > 0) //Es ist etwas in dem Varbind
       {
           // ------------ OBJECT ID ------------
           PakPos += 2; //Von Varbind objekt length auf OID length springen
           uint8_t OID_length = RecPak[PakPos];
           
           Objects->OID = malloc(OID_length + 1); //Speicher reservieren für die OID

           Objects->OID[0] = 1;
           Objects->OID[1] = 3;

           PakPos++; //Objekt ID beginnt immer mit 1.3. und die Zahlen sind in einem Bytes kodiert           

           for (uint8_t i = 2; i < OID_length - 2; i++)
            Objects->OID[i] = RecPak[PakPos + i];
            
           PakPos += OID_length;
           
           // ------------ VALUE ------------
           Objects->Value_Type = RecPak[PakPos];
           PakPos++; //Auf die Value länge springen
           
           if (RecPak[PakPos] > 0)
               continue; //While schleife beenden
           uint8_t value_length = RecPak[PakPos];
           Objects->Value = malloc(value_length + 1);
           
           for (uint8_t i = 2; i < value_length; i++)
            Objects->Value[i] = RecPak[PakPos + i];   
            
       }//while VARBIND
    
    }//if VARBIND LIST
    
    
    
    

    DEBUG("\n\n\nSNMP Nachricht\nVersion: %i \nComm: %s \nPDU: %i \nReq ID: %i\n Error: %i \nError Idx: %i\n\n", SNMP_Data.Version, SNMP_Data.Comm_Name, SNMP_Data.PDU_Type, SNMP_Data.RequestID, SNMP_Data.Error, SNMP_Data.ErrorIndex);
} 