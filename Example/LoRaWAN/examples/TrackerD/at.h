#ifndef _at_h_
#define _at_h_

#include "common.h"

#define AT            "AT"
#define MODEL         "+MODEL"
#define DEUI          "+DEUI"
#define APPEUI        "+APPEUI"
#define APPKEY        "+APPKEY"
#define DADDR         "+DADDR"
#define APPSKEY       "+APPSKEY"
#define NWKSKEY       "+NWKSKEY"
#define ADR           "+ADR"
#define DR            "+DR"
#define TXP           "+TXP"
#define CFG           "+CFG"
#define RESET         "Z"
#define FDR           "+FDR"
#define TDC           "+TDC"
#define MTDC          "+MTDC"
#define ATDC          "+ATDC"
#define LON           "+LON"
#define CHS           "+CHS"
#define CHE           "+CHE"
#define FTIME         "+FTIME"
#define SMOD          "+SMOD"
#define CFM           "+CFM"
#define PDTA          "+PDTA"
#define PDOP          "+PDOP"
#define SLEEP         "+SLEEP"
#define NMEA353       "+NMEA353"
#define NMEA886       "+NMEA886"
#define DWELLT        "+DWELLT"
#define BLEMASK       "+BLEMASK"
#define WiFiMASK      "+WiFiMASK"
#define INTWK         "+INTWK"
#define PNACKMD       "+PNACKMD"
#define SHOWID        "+SHOWID"
#define PT            "+PT"
typedef enum
{
  AT_OK = 0,
  AT_ERROR,
  AT_PARAM_ERROR,
  AT_BUSY_ERROR,
  AT_TEST_PARAM_OVERFLOW,
  AT_RX_ERROR,
  AT_RUNNING
}ATEerror_t;

static const char *const ATError_description[] =
{
  "\r\nOK\r\n",
  "\r\nERROR\r\n",
  "\r\nAT_PARAM_ERROR\r\n",
  "\r\nAT_BUSY_ERROR\r\n",
  "\r\nAT_TEST_PARAM_OVERFLOW\r\n",
  "\r\nAT_RX_ERROR\r\n"
};

struct ATCommand_s 
{
  const char* string;                       /*< command string, after the "AT" */
  const int size_string;                    /*< size of the command string, not including the final \0 */
#if !defined(NO_HELP)
  const char* help_string;                  /*< to be printed when ? after the string */
#endif
  ATEerror_t (*get)(const char *param);     /*< =? after the string to get the current value*/
  ATEerror_t (*set)(const char *param);     /*< = (but not =?\0) after the string to set a value */
  ATEerror_t (*run)(const char *param);     /*< \0 after the string - run the command */
};

uint8_t StrToHex(uint8_t *pbDest, char *pbSrc, uint8_t nLen);

ATEerror_t at_return_error(const char *param);
ATEerror_t at_que(const char *param);
ATEerror_t at_model_get(const char *param);
ATEerror_t at_smod_set(const char *param);
ATEerror_t at_smod_get(const char *param);
ATEerror_t at_reset_run(const char *param);
ATEerror_t at_fdr_run(const char *param);
ATEerror_t at_sleep_run(const char *param);
ATEerror_t at_cfg_run(const char *param);
ATEerror_t at_tdc_get(const char *param);
ATEerror_t at_tdc_set(const char *param);
ATEerror_t at_mtdc_get(const char *param);
ATEerror_t at_mtdc_set(const char *param);
ATEerror_t at_atdc_get(const char *param);
ATEerror_t at_atdc_set(const char *param);
ATEerror_t at_sysmes_run(const char *param);
ATEerror_t at_deui_get(const char *param);
ATEerror_t at_deui_set(const char *param);
ATEerror_t at_appeui_get(const char *param);
ATEerror_t at_appeui_set(const char *param);
ATEerror_t at_appkey_get(const char *param);
ATEerror_t at_appkey_set(const char *param);
ATEerror_t at_daddr_get(const char *param);
ATEerror_t at_daddr_set(const char *param);
ATEerror_t at_nwkskey_get(const char *param);
ATEerror_t at_nwkskey_set(const char *param);
ATEerror_t at_appskey_get(const char *param);
ATEerror_t at_appskey_set(const char *param);
ATEerror_t at_adr_get(const char *param);
ATEerror_t at_adr_set(const char *param);
ATEerror_t at_dr_get(const char *param);
ATEerror_t at_dr_set(const char *param);
ATEerror_t at_txp_get(const char *param);
ATEerror_t at_txp_set(const char *param);
ATEerror_t at_ftime_get(const char *param);
ATEerror_t at_ftime_set(const char *param);
ATEerror_t at_intwk_get(const char *param);
ATEerror_t at_intwk_set(const char *param);
ATEerror_t at_lon_get(const char *param);
ATEerror_t at_lon_set(const char *param);
ATEerror_t at_CHE_get(const char *param);
ATEerror_t at_CHE_set(const char *param);
ATEerror_t at_cfm_get(const char *param);
ATEerror_t at_cfm_set(const char *param);
ATEerror_t at_PDTA_set(const char *param);
ATEerror_t at_NMEA886_get(const char *param);
ATEerror_t at_NMEA886_set(const char *param);
ATEerror_t at_NMEA353_get(const char *param);
ATEerror_t at_NMEA353_set(const char *param);
ATEerror_t at_PDOP_get(const char *param);
ATEerror_t at_PDOP_set(const char *param);
ATEerror_t at_dwelltime_get(const char *param);
ATEerror_t at_dwelltime_set(const char *param);
ATEerror_t at_blemask_get(const char *param);
ATEerror_t at_blemask_set(const char *param);
ATEerror_t at_wifimask_get(const char *param);
ATEerror_t at_wifimask_set(const char *param);
ATEerror_t at_pnackmd_get(const char *param);
ATEerror_t at_pnackmd_set(const char *param);
ATEerror_t at_showid_get(const char *param);
ATEerror_t at_showid_set(const char *param);
ATEerror_t at_pt_get(const char *param);
ATEerror_t at_pt_set(const char *param);
static const struct ATCommand_s ATCommand[] =
{
/**************** AT+MODEL ****************/  
  {
    .string = AT MODEL,
    .size_string = sizeof(MODEL) - 1,
#ifndef NO_HELP
    .help_string = AT MODEL "   : Get module information",
#endif
    .get = at_model_get,
    .set = at_return_error,
    .run = at_return_error,
  },
/**************** ATZ ****************/  
  {
    .string = AT RESET,
    .size_string = sizeof(RESET) - 1,
#ifndef NO_HELP
    .help_string = AT RESET "        : Trig a reset of the MCU",
#endif
    .get = at_return_error,
    .set = at_return_error,
    .run = at_reset_run,
  },
/**************** AT+FDR ****************/  
  {
    .string = AT FDR,
    .size_string = sizeof(FDR) - 1,
#ifndef NO_HELP
    .help_string = AT FDR "     : Reset Parameters to Factory Default",
#endif
    .get = at_return_error,
    .set = at_return_error,
    .run = at_fdr_run,
  },
/**************** AT+SLEEP ****************/  
  {
    .string = AT SLEEP,
    .size_string = sizeof(SLEEP) - 1,
#ifndef NO_HELP
    .help_string = AT SLEEP "     : Trig a sleep of the MCU",
#endif
    .get = at_return_error,
    .set = at_return_error,
    .run = at_sleep_run,
  },  
/**************** AT+DEUI ****************/  
  {
    .string = AT DEUI,
    .size_string = sizeof(DEUI) - 1,
#ifndef NO_HELP
    .help_string = "AT" DEUI "    : Get or Set the Device EUI",
#endif
    .get = at_deui_get,
    .set = at_deui_set,
    .run = at_return_error,
  },
/**************** AT+APPEUI ****************/  
  {
    .string = AT APPEUI,
    .size_string = sizeof(APPEUI) - 1,
#ifndef NO_HELP
    .help_string = "AT" APPEUI "  : Get or Set the Application EUI",
#endif
    .get = at_appeui_get,
    .set = at_appeui_set,
    .run = at_return_error,
  },
/**************** AT+APPKEY ****************/  
  {
    .string = AT APPKEY,
    .size_string = sizeof(APPKEY) - 1,
#ifndef NO_HELP
    .help_string = "AT" APPKEY "  : Get or Set the Application Key",
#endif
    .get = at_appkey_get,
    .set = at_appkey_set,
    .run = at_return_error,
  },
/**************** AT+DADDR ****************/  
  {
    .string = AT DADDR,
    .size_string = sizeof(DADDR) - 1,
#ifndef NO_HELP
    .help_string = "AT" DADDR "   : Get or Set the Device Address",
#endif
    .get = at_daddr_get,
    .set = at_daddr_set,
    .run = at_return_error,
  }, 
/**************** AT+NWKSKEY ****************/  
  {
    .string = AT NWKSKEY,
    .size_string = sizeof( NWKSKEY) - 1,
#ifndef NO_HELP
    .help_string = "AT" NWKSKEY " : Get or Set the Network Session Key",
#endif
    .get = at_nwkskey_get,
    .set = at_nwkskey_set,
    .run = at_return_error,
  },  
/**************** AT+APPSKEY ****************/  
  {
    .string = AT APPSKEY,
    .size_string = sizeof(APPSKEY) - 1,
#ifndef NO_HELP
    .help_string = "AT" APPSKEY " : Get or Set the Application Session Key",
#endif
    .get = at_appskey_get,
    .set = at_appskey_set,
    .run = at_return_error,
  },
/**************** AT+ADR ****************/  
  {
    .string = AT ADR,
    .size_string = sizeof(ADR) - 1,
#ifndef NO_HELP
    .help_string = "AT" ADR "     : Get or Set the Adaptive Data Rate setting.(0: off, 1: on)",
#endif
    .get = at_adr_get,
    .set = at_adr_set,
    .run = at_return_error,
  },
/**************** AT+DR ****************/  
  {
    .string = AT DR,
    .size_string = sizeof(DR) - 1,
#ifndef NO_HELP
    .help_string = "AT" DR "      : Get or Set the Data Rate setting.",
#endif
    .get = at_dr_get,
    .set = at_dr_set,
    .run = at_return_error,
  },
/**************** AT+TXP ****************/  
  {
    .string = AT TXP,
    .size_string = sizeof(TXP) - 1,
#ifndef NO_HELP
    .help_string = "AT" TXP "     : Get or Set the Transmit Power (MAX:0, MIN:5).",
#endif
    .get = at_txp_get,
    .set = at_txp_set,
    .run = at_return_error,
  },
/**************** AT+SMOD ****************/  
  {
    .string = AT SMOD,
    .size_string = sizeof(SMOD) - 1,
#ifndef NO_HELP
    .help_string = AT SMOD "   : Get or set work mode",
#endif
    .get = at_smod_get,
    .set = at_smod_set,
    .run = at_return_error,
  },    
/**************** AT+CFG ****************/  
  {
    .string = AT CFG,
    .size_string = sizeof(CFG) - 1,
#ifndef NO_HELP
    .help_string = AT CFG "     : Print all settings",
#endif
    .get = at_return_error,
    .set = at_return_error,
    .run = at_cfg_run,
  },
/**************** AT+TDC ****************/  
  {
    .string = AT TDC,
    .size_string = sizeof(TDC) - 1,
#ifndef NO_HELP
    .help_string = AT TDC "     : Get or set the application data transmission interval in ms",
#endif
    .get = at_tdc_get,
    .set = at_tdc_set,
    .run = at_return_error,
  },

/**************** AT+MTDC ****************/  
  {
    .string = AT MTDC,
    .size_string = sizeof(MTDC) - 1,
#ifndef NO_HELP
    .help_string = AT MTDC "     : Get or set the application motion state data transmission interval, in milliseconds",
#endif
    .get = at_mtdc_get,
    .set = at_mtdc_set,
    .run = at_return_error,
  },
/**************** AT+ATDC ****************/  
  {
    .string = AT ATDC,
    .size_string = sizeof(ATDC) - 1,
#ifndef NO_HELP
    .help_string = AT ATDC "     : Get or set the application alarm status data transmission interval, in milliseconds",
#endif
    .get = at_atdc_get,
    .set = at_atdc_set,
    .run = at_return_error,
  },  
/**************** AT+FTIME ****************/  
  {
    .string = AT FTIME,
    .size_string = sizeof(FTIME) - 1,
#ifndef NO_HELP
    .help_string = AT FTIME "     : Get or set the application data transmission interval in ms",
#endif
    .get = at_ftime_get,
    .set = at_ftime_set,
    .run = at_return_error,
  },  
/**************** AT+INTWK ****************/  
  {
    .string = AT INTWK,
    .size_string = sizeof(INTWK) - 1,
#ifndef NO_HELP
    .help_string = AT INTWK "     : Enable sport mode",
#endif
    .get = at_intwk_get,
    .set = at_intwk_set,
    .run = at_return_error,
  },  
/**************** AT+LON ****************/  
  {
    .string = AT LON,
    .size_string = sizeof(LON) - 1,
#ifndef NO_HELP
    .help_string = AT LON "     : Enable/Disable LED activity for uplink",
#endif
    .get = at_lon_get,
    .set = at_lon_set,
    .run = at_return_error,
  },
/**************** AT+CHE ****************/  
  {
    .string = AT CHE,
    .size_string = sizeof(CHE) - 1,
#ifndef NO_HELP
    .help_string = AT CHE "     : Enable/Disable LED activity for uplink",
#endif
    .get = at_CHE_get,
    .set = at_CHE_set,
    .run = at_return_error,
  },
/**************** AT+NMEA353 ****************/  
  {
    .string = AT NMEA353,
    .size_string = sizeof(NMEA353) - 1,
#ifndef NO_HELP
    .help_string = AT NMEA353 "     : Enable/Disable LED activity for uplink",
#endif
    .get = at_NMEA353_get,
    .set = at_NMEA353_set,
    .run = at_return_error,
  },
/**************** AT+NMEA886 ****************/  
  {
    .string = AT NMEA886,
    .size_string = sizeof(NMEA886) - 1,
#ifndef NO_HELP
    .help_string = AT NMEA886 "     : Enable/Disable LED activity for uplink",
#endif
    .get = at_NMEA886_get,
    .set = at_NMEA886_set,
    .run = at_return_error,
  },
/**************** AT+CFM ****************/  
  {
    .string = AT CFM,
    .size_string = sizeof(CFM) - 1,
#ifndef NO_HELP
    .help_string = AT CFM "     : Get or Set the confirmation mode (0-1)",
#endif
    .get = at_cfm_get,
    .set = at_cfm_set,
    .run = at_return_error,
  },  
/**************** AT+PNACKMD ****************/  
  {
    .string = AT PNACKMD,
    .size_string = sizeof(PNACKMD) - 1,
#ifndef NO_HELP
    .help_string = AT PNACKMD "     : Enable/Disable Auto Send None-ACK messages",
#endif
    .get = at_pnackmd_get,
    .set = at_pnackmd_set,
    .run = at_return_error,
  },  
/**************** AT+PDOP ****************/  
  {
    .string = AT PDOP,
    .size_string = sizeof(PDOP) - 1,
#ifndef NO_HELP
    .help_string = AT PDOP "     : Enable/Disable LED activity for uplink",
#endif
    .get = at_PDOP_get,
    .set = at_PDOP_set,
    .run = at_return_error,
  }, 
/**************** AT+DWELLT ****************/  
  {
    .string = AT DWELLT,
    .size_string = sizeof(DWELLT) - 1,
#ifndef NO_HELP
    .help_string = AT DWELLT "     : Get or Set UplinkDwellTime",
#endif
    .get = at_dwelltime_get,
    .set = at_dwelltime_set,
    .run = at_return_error,
  }, 

/**************** AT+SHOWID ****************/  
  {
    .string = AT SHOWID,
    .size_string = sizeof(SHOWID) - 1,
#ifndef NO_HELP
    .help_string = AT SHOWID "     : Get or Set SHOWID",
#endif
    .get = at_showid_get,
    .set = at_showid_set,
    .run = at_return_error,
  },   
/**************** AT+BLEMASK ****************/  
  {
    .string = AT BLEMASK,
    .size_string = sizeof(BLEMASK) - 1,
#ifndef NO_HELP
    .help_string = AT BLEMASK "     : Get or Set BLE MASK",
#endif
    .get = at_blemask_get,
    .set = at_blemask_set,
    .run = at_return_error,
  }, 
/**************** AT+WiFiMASK ****************/  
  {
    .string = AT WiFiMASK,
    .size_string = sizeof(WiFiMASK) - 1,
#ifndef NO_HELP
    .help_string = AT WiFiMASK "     : Get or Set WiFi MASK",
#endif
    .get = at_wifimask_get,
    .set = at_wifimask_set,
    .run = at_return_error,
  },    
  
/**************** AT+PT ****************/  
  {
    .string = AT PT,
    .size_string = sizeof(PT) - 1,
#ifndef NO_HELP
    .help_string = AT PT "     : Get or Set Motion Threshold",
#endif
    .get = at_pt_get,
    .set = at_pt_set,
    .run = at_return_error,
  },  
};

ATEerror_t ATInsPro(char* atdata);

#endif
