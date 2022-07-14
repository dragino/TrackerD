#ifndef _lora_h_
#define _lora_h_

#include <lmic.h>
#include <hal/hal.h>
#include "Arduino.h"

#define ADR_ENABLE  1

class LORA
{
    public:
                    LORA();
                    ~LORA();

        uint32_t    netid;
        uint8_t     appSKey[16]={0};
        uint8_t     nwkSKey[16]={0};
        uint32_t    devaddr = 0;
        
        void        LORA_SetDEUI( uint8_t *deui);
        void        LORA_GetDEUI( uint8_t *deui);

        void        LORA_SetAEUI( uint8_t *aeui);
        void        LORA_GetAEUI( uint8_t *aeui);

        void        LORA_SetAKEY( uint8_t *akey);
        void        LORA_GetAKEY( uint8_t *akey);

        void        LORA_SetAPPSKEY( uint8_t *appskey);
        void        LORA_GetAPPSKEY( uint8_t *appskey);

        void        LORA_SetNWKSKEY( uint8_t *nwkskey);
        void        LORA_GetNWKSKEY( uint8_t *nwkskey);

        void        LORA_SetDevaddr(uint32_t daddr);
        uint32_t    LORA_GetDevaddr( void);

        void        LORA_SetADR(bool _adr);
        bool        LORA_GetADR(void);

        void        LORA_SetDR(uint8_t _dr);
        uint8_t     LORA_GetDR(void);
        
        void        LORA_SetTXP(uint8_t _txp);
        uint8_t     LORA_GetTXP(void);

        void        LORA_EnterSleepMode( void);
        
    private:
        uint8_t     deveui[8]={0};   //lsb
        uint8_t     appeui[8]={0};   //lsb
        uint8_t     appkey[16]={0};  //msb               
        bool        adr_flag;
        uint8_t     dr;
        uint8_t     txp;
};


#endif
