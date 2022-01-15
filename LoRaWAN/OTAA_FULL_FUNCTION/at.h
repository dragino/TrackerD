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
#define CFG           "+CFG"
#define RESET         "Z"
#define FDR           "+FDR"
#define SCWIFI        "+SCWIFI"
#define TDC           "+TDC"
#define SYSMES        "+SYSMES"

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


ATEerror_t at_return_error(const char *param);
ATEerror_t at_que(const char *param);
ATEerror_t at_model_get(const char *param);
ATEerror_t at_reset_run(const char *param);
ATEerror_t at_fdr_run(const char *param);
ATEerror_t at_cfg_run(const char *param);
ATEerror_t at_scwifi_run(const char *param);
ATEerror_t at_tdc_get(const char *param);
ATEerror_t at_tdc_set(const char *param);
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
/**************** AT+SCWIFI ****************/  
  {
    .string = AT SCWIFI,
    .size_string = sizeof(SCWIFI) - 1,
#ifndef NO_HELP
    .help_string = AT SCWIFI "  : Scan WIFI signal ",
#endif
    .get = at_return_error,
    .set = at_return_error,
    .run = at_scwifi_run,
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
/**************** AT+SYSMES ****************/  
  {
    .string = AT SYSMES,
    .size_string = sizeof(SYSMES) - 1,
#ifndef NO_HELP
    .help_string = AT SYSMES "  : Get system information",
#endif
    .get = at_return_error,
    .set = at_return_error,
    .run = at_sysmes_run,
  },
};

ATEerror_t ATInsPro(char* atdata);

#endif
