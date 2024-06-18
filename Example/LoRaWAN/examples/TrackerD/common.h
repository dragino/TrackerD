#ifndef _common_h_
#define _common_h_

#include "EEPROM.h"
#include "Wire.h"
#include "lora.h"
#include "extiButton.h"
#include "extiButtonLS.h"
#include "blescan.h"
#include "wifiscan.h"
#include "driver/rtc_io.h" 
#include "hal/rtc_io_hal.h"

#define Pro_model           "TrackerD "
#define Pro_version         "v1.4.9"
#define Pro_model1          "TrackerD-LS "
#define Pro_version1        "v1.0.3"
#define EEPROM_DATA_SIZE    256
#define EEPROM_KEY_SIZE     82
#define EEPROM_GPSDATA_SIZE 4096

#define BAT_PIN_READ    35
#define BAT_PIN_READ1   34
#define BAT_PIN_LOW     4
#define LED_PIN_RED     13
#define LED_PIN_BLUE    2
#define LED_PIN_GREEN   15

#define LED_PIN_RED1     2
#define LED_PIN_BLUE1    13

#define DEFAULT_MODE                0x00
#define MOVE_DETECT_MODE            0x01
#define COLLISION_DETECT_MODE       0x02
#define CUSTOM_MOVE_DETECT_MODE     0x03
#define GXHT3x_MODE                 0x04

class SYS:public LORA
{
  public:   
    SYS(uint8_t pin_red=LED_PIN_RED,uint8_t pin_blue=LED_PIN_BLUE,uint8_t pin_green=LED_PIN_GREEN);
    ~SYS(void);
    void eeprom_init(void);
    void DATA_CLEAR(void);
    void config_Write(void);
    void config_Read(void);
    void gps_data_Weite(void);
    void gps_data_Read(void);
    void gps_pdta_Read(uint32_t pdta_addr);
    void read_gps_data_on_flash();
    void GPSDATA_CLEAR(void);
    void customize_freq_set(void);
    void Band_information(void);
    uint8_t     gps_data_buff[15];
    char        blemask_data[12]={'0','0','0','0','0','0'};
    char        wifimask_data[12]={'0','0','0','0','0','0'};
    char        buff1[100];
    char        BLEDATA[1000];
    char        BLEDATA1[100];
    char        BLEDATA2[100];
    char        BLEDATA3[100];
    char        BLEDATA4[100];
    char        BLEDATA5[100]; 
    char        databuf[10];
    uint32_t    ble_config[32] = {0};
    uint32_t    wifi_config[32] = {0};
    uint8_t     blemask_flag =0;
    uint8_t     ble_flag =0; 
    uint8_t     TF[1] ={0x14};
    uint8_t     bledata[240]= {0};
    uint8_t     blecount=0;
    uint32_t    tdc = 1200000;     //uint:ms
    uint32_t    mtdc = 300000;     //uint:ms
    uint32_t    atdc = 60000;     //uint:ms
    uint32_t    sys_time = 1200000; //uint:ms
    uint8_t     mod = 0;
    uint8_t     lon = 1;      //1:on  2:0ff
    uint32_t    Positioning_time = 180000;
    uint32_t    cdevaddr = 0;
    uint8_t     atst = 15;
    uint8_t     pedometer = 0;
    uint8_t     ble_gps = 0;
    uint8_t     beep_flag = 0;
    uint32_t    exit_alarm_time = 2000;
    uint32_t    alarm_fprt7_time = 60000;
    uint8_t     frame = 0;
    uint8_t     FDR_flag = 0;
    uint8_t     GPS_flag = 1;
    uint8_t     alarm = 0;
    uint8_t     gps_start = 1;
    uint8_t     gps_alarm = 0;
    uint8_t     alarm_count = 0;
    uint8_t     buzzer_flag = 0;
    uint8_t     alarm_no = 0;
    uint8_t     exti_flag = 1;
    uint8_t     exit_off = 1;
    uint8_t     fall_detection = 0;
    uint8_t     sleep_flag = 0;
    uint8_t     sleep_exti = 0;
    uint8_t     port = 2;
    uint8_t     sensor_mode = 1;
    uint8_t     save_sensor_mode = 1;
    uint8_t     save_mode = 0;
    uint8_t     ble_mod = 0;
    uint8_t     njm = 1;
    uint8_t     save_ble_mode = 0;
    uint16_t    fire_version=0;
    uint16_t    fire_version_write=0;
    uint8_t     channel_single = 1;
    uint32_t    addr_gps_write=0;
    uint32_t    gps_write=0;
    uint32_t    addr_gps_read=0;
    uint32_t    addr_pdta_read=0;
    uint32_t    fre=0;
    uint8_t     frame_ACK =0;
    uint8_t     frame_flag = 0;
    uint8_t     sensor_type;
    uint8_t     loggpsdata_send = 0;
    uint8_t     fr_mode = 0;
    uint8_t     se_mode = 0;
    uint8_t     ic_versino = 2;
    uint8_t     Dwelltime = 1;
    uint8_t     Intwk = 0;
    uint8_t     PNACKmd = 0;
    uint8_t     keep_flag = 0;
    uint8_t     device_flag = 0;
    uint8_t     showid = 0;
    float       pdop_value = 7.0;
    int         data_read = 0;
    bool        gps_work_flag = true;
    bool        collect_sensor_flag = false; 
       
  private: 
};
class Devicet
{
  public: 
    uint8_t sensor_type;
    
    uint32_t firm_ver;
    
    uint8_t freq_band;
    
    uint8_t sub_band;

    uint8_t FLAG = 0;
    
    uint16_t battrey;  

    uint32_t SMODE;
};

#include "at.h"
#include "GPS.h"
#include "GXHT30.h"

class Sensor
{
  public:
    int hum=0;
    int tem=0;
    int bat=0;
    int latitude=0;
    int longitude=0;
    int year_gps = 0;
    int month_gps = 0;
    int day_gps = 0;
    int hour_gps = 0;
    int minute_gps = 0;
    int second_gps = 0;
    float pdop_gps ; 
    int Fix_Status;   
    int16_t x=0;
    int16_t y=0;
    int16_t z=0;
};

extern SYS sys;
extern Sensor sensor;
extern Devicet devicet;

int BatGet(void);
void buzzer(void);
void ABP_Band_information(void);
void ABP_InIt(void);
void Stop_buzzer(void);
void device_strcmp(void);
void gpio_reset(void);
void Device_status();
void button_loop(void);
void I2C_Sent(int addr,uint8_t* buff,uint8_t buff_len);
void I2C_Read(int addr,uint8_t* buff,uint8_t buff_len,int delay_ms);
char *rtrim(char* str);
void StrToHex(uint8_t *pbDest, char *pszSrc, int nLen);
uint16_t string_touint(void);
void Device_status();
#endif
