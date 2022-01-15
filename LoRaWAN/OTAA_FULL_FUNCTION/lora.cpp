#include "lora.h"

void LORA_RxData(uint8_t *AppData, uint8_t AppData_Len)
{
	bool store_flag = false;
	switch (AppData[0] & 0xff)
	{
	case 0x01:
	{
		if (AppData_Len == 4) //---->AT+TDC
		{
			uint32_t ServerSetTDC = (AppData[1] << 16 | AppData[2] << 8 | AppData[3]); // S
			if (ServerSetTDC < 6)
			{
				Serial.println("Invalid parameter");
			}
			else
			{
				store_flag = true;
				sys.tdc = ServerSetTDC * 1000;
			}
		}
	}
	break;
	case 0x04:
	{
		if (AppData_Len == 2 && AppData[1] == 0xFF) //---->ATZ
		{
			ESP.restart();
		}
	}
	break;
	default:
		Serial.println("Unknown instruction");
		break;
	}

	if (store_flag == true)
	{
		sys.config_Set();
	}
}
