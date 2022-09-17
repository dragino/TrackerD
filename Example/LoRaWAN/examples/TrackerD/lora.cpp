#include "lora.h"

LORA::LORA()
{}

LORA::~LORA()
{}

void LORA::LORA_SetDEUI( uint8_t *deui)
{
	memcpy(deveui,deui,8);
}

void LORA::LORA_GetDEUI( uint8_t *deui)
{
	memcpy(deui,deveui,8);
}

void LORA::LORA_SetAEUI( uint8_t *aeui)
{
	memcpy(appeui,aeui,8);
}

void LORA::LORA_GetAEUI( uint8_t *aeui)
{
	memcpy(aeui,appeui,8);
}

void LORA::LORA_SetAKEY( uint8_t *akey)
{
	memcpy(appkey,akey,16);
}

void LORA::LORA_GetAKEY( uint8_t *akey)
{
	memcpy(akey,appkey,16);
}

void LORA::LORA_SetAPPSKEY( uint8_t *appskey)
{
	memcpy(appSKey,appskey,16);
}

void LORA::LORA_GetAPPSKEY( uint8_t *appskey)
{
	memcpy(appskey,appSKey,16);
}

void LORA::LORA_SetNWKSKEY( uint8_t *nwkskey)
{
	memcpy(nwkSKey,nwkskey,16);
}

void LORA::LORA_GetNWKSKEY( uint8_t *nwkskey)
{
	memcpy(nwkskey,nwkSKey,16);
}

void LORA::LORA_SetDevaddr(uint32_t daddr)
{
	devaddr = daddr;
}

uint32_t LORA::LORA_GetDevaddr( void)
{
	return devaddr;
}

void LORA::LORA_SetADR(bool _adr)
{
	adr_flag = _adr;
}

bool LORA::LORA_GetADR(void)
{
	return adr_flag;
}
			
void LORA::LORA_SetDR(uint8_t _dr)
{
	dr = _dr;
}

uint8_t LORA::LORA_GetDR(void)
{
	return dr;
}

void LORA::LORA_SetTXP(uint8_t _txp)
{
	txp = _txp;
}

uint8_t LORA::LORA_GetTXP(void)
{
	return txp;
}

void LORA::LORA_EnterSleepMode( void)
{
	uint8_t cmd = 0;
	uint8_t reg_mode = 0x01;
	hal_spi_write(reg_mode,&cmd,1);
}

