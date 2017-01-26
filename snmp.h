/*
 * snmp_fluke.h
 *
 * Created: 10.12.2016 16:44:13
 *  Author: Rene
 */ 


#ifndef SNMP_H_
#define SNMP_H_

#define SNMP_REQ_PORT       161
#define SNMP_TRAP_PORT      162

//BER Complex Types
#define BER_INTEGER          0x02
#define BER_STRING           0x04
#define BER_NULL             0x05
#define BER_OID              0x06
#define BER_SEQUENCE         0x30
#define BER_GET_REQUEST      0xA0
#define BER_GET_NEXT_REQUEST 0xA1
#define BER_GET_RESPONSE     0xA2
#define BER_SET_REQUEST      0xA3

//Positionen der Parameter in der SNMP Message
#define SNMP_VERSION        0
#define SNMP_COMM_STR       1
#define SNMP_REQUEST_ID     2
#define SNMP_ERROR          3
#define SNMP_ERROR_INDEX    4
#define SNMP_OID            5


typedef struct 
{
    volatile uint8_t       Version;
    volatile unsigned char Comm_Name[10];

    volatile uint8_t       PDU_Type; 
    volatile uint16_t      RequestID;
    volatile uint8_t       Error;
    volatile uint8_t       ErrorIndex;
    volatile uint8_t      *VarBind1; 
} SNMP_DATAGRAM;

typedef struct 
{
   volatile uint8_t *OID;
   volatile uint8_t Value_Type;
   volatile unsigned char *Value; 
} Managed_Object;

void snmp_init(void);
void snmp_udp_rec(unsigned char Index);

#endif /* SNMP_H_ */