# About this repository
This repository is the place to find the software/hardware resources for Dragino TrackerD product.

TrackerD is a LoRaWAN Tracker, it supports the following features / sensors:
* GPS
* WiFi
* BLE (Bluetooth Low Energy)
* Temperature
* Humidity
* Motion detection
<img width="300" alt="TrackerD enclosure" src="https://www.dragino.com/media/k2/galleries/165/LBT1_10.JPG">


#  Software Example Info
# TrackerD includes three axes, temperature and humidity, Bluetooth, WiFi, gps and LORA
# Development Status
  * Latest Development Release ：TrackerD V1.0

# Hardware Info

## Components

* SOC : [Espressif ESP32 PICO D4](https://www.espressif.com/sites/default/files/documentation/esp32-pico-d4_datasheet_en.pdf)
  * CPU : 2 x Xtensa® dual-core 32-bit LX6
  * Bluetooth: Bluetooth V4.2 BR/EDR and Bluetooth LE
  * WiFi : 802.11 b/g/n (802.11n up to 150 Mbps)
  * Integrated SPI flash : 4 MB
  * ROM : 448 KB
  * SRAM: 520 KB
* GPS : [Quectel L70](https://www.quectel.com/wp-content/uploads/pdfupload/Quectel_L70_GPS_Specification_V2.3.pdf) or [Quectel L76K](https://www.quectel.com/wp-content/uploads/2021/10/Quectel_L76K_Hardware_Design_V1.0.pdf)
* LoRa Transceiver : HopeRF RFM92W/93W (Cf [Semtech SX1272/SX1273](https://semtech.com/products/wireless-rf/lora-core/sx1272#download-resources) as it seems Semtech docs are more up to date)
* Accelerometer: [ST Microelectronics LIS3DH](https://www.st.com/resource/en/datasheet/lis3dh.pdf) (Also contains temperature sensor)
* Humidity / temperature sensor : [GXCAS Technology GXHT3X](https://taoic.oss-cn-hangzhou.aliyuncs.com/8676/product/STD_1623205250000.pdf) (May be compatible with [Sensirion SHT30](https://www.sensirion.com/fileadmin/user_upload/customers/sensirion/Dokumente/2_Humidity_Sensors/Datasheets/Sensirion_Humidity_Sensors_SHT3x_Datasheet_digital.pdf) ?)
* USB-TTL interface (depending on the HW):
  * [CH340N](http://wch-ic.com/downloads/CH340DS1_PDF.html)
  * [CH9102F](http://wch-ic.com/downloads/CH9102DS1_PDF.html)
* Battery charger: [Consonnance CN3052A](http://www.consonance-elec.com/pdf/datasheet/DSE-CN3052A.pdf)

### Other datasheeets
* [Espressif ESP32 Technical reference manual](https://www.espressif.com/sites/default/files/documentation/esp32_technical_reference_manual_en.pdf)


## System Structure

## Pin Mapping

### ESP32-PICO-D4

| Name        | No. | Type | Function                  | Subsystem        | Internal name / Usage               |
| -           | -   | -    | -                         | -                | -                                   |
| VDDA        | 1   | P    | Analog power supply       | -                | +3v3                                |
| LNA_IN      | 2   | I/O  | RF input and output       | -                | Antenna                             |
| VDDA3P3     | 3   | P    | Analog power supply       | -                | +3v3                                |
| VDDA3P3     | 4   | P    | Analog power supply       | -                | +3v3                                |
| SENSOR_VP   | 5   | I    | SENSOR_VP                 | Monitoring       | ADC Capacitor                       |
| SENSOR_CAPP | 6   | I    | SENSOR_CAPP               | Monitoring       | ADC Capacitor                       |
| SENSOR_CAPN | 7   | I    | SENSOR_CAPN               | Monitoring       | ADC Capacitor                       |
| SENSOR_VN   | 8   | I    | SENSOR_VN                 | Monitoring       | ADC Capacitor                       |
| EN          | 9   | I    | 1; power on, 0; power off | Buttons          | CPU Reset button                    |
| IO34        | 10  | I    | ADC1_CH6                  | Monitoring       | PA2 - Battery voltage ADC           |
| IO35        | 11  | I    | ADC1_CH7                  | Monitoring       | PA0 - Battery voltage ADC           |
| IO32        | 12  | I/O  | GPIO32                    | RFM9xW           | DIO2                                |
| IO33        | 13  | I/O  | GPIO33                    | RFM9xW           | DIO1                                |
| IO25        | 14  | I/O  | GPIO25                    | GPS              | GPIO_RESET - Reset                  |
| IO26        | 15  | I/O  | GPIO26                    | RFM9xW           | DIO0                                |
| IO27        | 16  | I/O  | GPIO27                    | RFM9xW           | MOSI                                |
| IO14        | 17  | I/O  | GPIO14                    | Accelerometer    | INT                                 |
| IO12        | 18  | I/O  | GPIO12                    | GPS              | VCC Enable                          |
| VDD3P3_RTC  | 19  | P    | Power supply for RTC IO   | -                | +3v3                                |
| IO13        | 20  | I/O  | GPIO13                    | Leds             | LED1                                |
| IO15        | 21  | I/O  | GPIO15                    | Leds             | LED3                                |
| IO2         | 22  | I/O  | GPIO2                     | Leds             | LED2                                |
| IO0         | 23  | I/O  | GPIO0                     | Buttons          | Boot: 0 = Download mode, 1 = normal |
| IO4         | 24  | I/O  | GPIO4                     | GPS              | GPIO_STANDBY - Standby              |
| IO16        | 25  | I/O  | GPIO16                    | -                | N/C                                 |
| VDD_SDIO    | 26  | P    | Output power supply.      | -                | N/C                                 |
| IO17        | 27  | I/O  | GPIO17                    | -                | N/C                                 |
| SD2         | 28  | I/O  | U1RXD                     | GPS              | TXD_MODULE - TXD                    |
| SD3         | 29  | I/O  | U1TXD                     | GPS              | RXD_MODULE - RXD                    |
| CMD         | 30  | I/O  | GPIO11                    | -                | N/C                                 |
| CLK         | 31  | I/O  | GPIO6                     | -                | N/C                                 |
| SD0         | 32  | I/O  | GPIO7                     | -                | N/C                                 |
| SD1         | 33  | I/O  | GPIO8                     | -                | N/C                                 |
| IO5         | 34  | I/O  | GPIO5                     | RFM9xW           | SCK                                 |
| IO18        | 35  | I/O  | GPIO18                    | RFM9xW           | NSS                                 |
| IO23        | 36  | I/O  | GPIO23                    | RFM9xW           | RESET                               |
| VDD3P3_CPU  | 37  | P    | Power supply for CPU IO   | -                | +3v3                                |
| IO19        | 38  | I/O  | GPIO19                    | RFM9xW           | MISO                                |
| IO22        | 39  | I/O  | GPIO22                    | Accel / Hum-Temp | SCL                                 |
| U0RXD       | 40  | I/O  | U0RXD                     | Firmware upgrade | RXD - Download mode                 |
| U0TXD       | 41  | I/O  | U0TXD                     | Firmware upgrade | TXD - Debug STDOUT / Download mode  |
| IO21        | 42  | I/O  | GPIO21                    | Accel / Hum-Temp | SDA                                 |
| VDDA        | 43  | P    | Analog power supply       | -                | +3v3                                |
| XTAL_N_NC   | 44  | -    | NC                        | -                | -                                   |
| XTAL_P_NC   | 45  | -    | NC                        | -                | N/C                                 |
| VDDA        | 46  | P    | Analog power supply       | -                | +3v3                                |
| CAP2_NC     | 47  | -    | NC                        | -                | N/C                                 |
| CAP1_NC     | 48  | -    | NC                        | -                | N/C                                 |
