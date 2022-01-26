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


#  Software
* Examples  
  * LoRaWAN -- OTAA_FULL_FUNCTION : OTAA includes three axes, temperature and humidity, Bluetooth, WiFi, gps
  * None-LoRaWAN -- Point-to-Point : Point to Point Communication

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
* LoRa Transceiver - one of:
  * HopeRF RFM92W/93W (Cf [Semtech SX1272/SX1273](https://semtech.com/products/wireless-rf/lora-core/sx1272#download-resources) as it seems Semtech docs are more up to date)
  * [HopeRF RFM95W](https://www.hoperf.com/data/upload/portal/20190801/RFM95W-V2.0.pdf) (Cf [Semtech SX1276](https://semtech.com/products/wireless-rf/lora-core/sx1276#download-resources) as it seems Semtech docs are more up to date)
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

# Development

## Using PlatformIO (Command line)
Install the following tools:
* [PlatformIO Core](http://docs.platformio.org/page/core.html)

Run these commands:
```console
# Change directory to project
$ cd Example/LoRaWAN

# Build project (need to do this after each update of the files)
$ pio run
Processing dragino_lbt2 (platform: https://github.com/platformio/platform-espressif32.git#feature/arduino-upstream; board: pico32; framework: arduino)
------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
* Adding toolchain toolchain-riscv32-esp with version 8.4.0+2021r1
* Adding toolchain toolchain-xtensa-esp32 with version 8.4.0+2021r1
* Adding toolchain toolchain-xtensa-esp32s2 with version 8.4.0+2021r1
Verbose mode can be enabled via `-v, --verbose` option
* Adding toolchain toolchain-riscv32-esp with version 8.4.0+2021r1
* Adding toolchain toolchain-xtensa-esp32 with version 8.4.0+2021r1
* Adding toolchain toolchain-xtensa-esp32s2 with version 8.4.0+2021r1
CONFIGURATION: https://docs.platformio.org/page/boards/espressif32/pico32.html
PLATFORM: Espressif 32 (3.3.1+sha.3784198) > ESP32 Pico Kit
HARDWARE: ESP32 240MHz, 320KB RAM, 4MB Flash
DEBUG: Current (esp-prog) External (esp-prog, iot-bus-jtag, jlink, minimodule, olimex-arm-usb-ocd, olimex-arm-usb-ocd-h, olimex-arm-usb-tiny-h, olimex-jtag-tiny, tumpa)
PACKAGES:
 - framework-arduinoespressif32 0.0.0+sha.15bbd0a
 - tool-esptoolpy 1.30100.210531 (3.1.0)
 - toolchain-xtensa-esp32 8.4.0+2021r1
Converting OTAA_FULL_FUNCTION.ino
LDF: Library Dependency Finder -> https://bit.ly/configure-pio-ldf
LDF Modes: Finder ~ chain, Compatibility ~ soft
Found 33 compatible libraries
Scanning dependencies...
Dependency Graph
|-- <MCCI LoRaWAN LMIC library> 4.1.1
|   |-- <SPI> 2.0.0
|-- <TinyGPSPlus> 1.0.2
|-- <SPI> 2.0.0
|-- <EEPROM> 2.0.0
|-- <WiFi> 2.0.0
|-- <Wire> 2.0.0
Building in release mode
Compiling .pio/build/dragino_lbt2/src/GXHT30.cpp.o
Compiling .pio/build/dragino_lbt2/src/OTAA_FULL_FUNCTION.ino.cpp.o
Linking .pio/build/dragino_lbt2/firmware.elf
Retrieving maximum program size .pio/build/dragino_lbt2/firmware.elf
Checking size .pio/build/dragino_lbt2/firmware.elf
Advanced Memory Usage is available via "PlatformIO Home > Project Inspect"
RAM:   [=         ]  12.4% (used 40484 bytes from 327680 bytes)
Flash: [=====     ]  54.4% (used 712397 bytes from 1310720 bytes)
Building .pio/build/dragino_lbt2/firmware.bin
esptool.py v3.1
Merged 25 ELF sections
========================================================================================================= [SUCCESS] Took 6.53 seconds =========================================================================================================


# Upload firmware (easy way, full firmware bootloader + partitions + application)
$ pio run --target upload --upload-port /dev/ttyUSB0
Configuring upload protocol...
AVAILABLE: esp-prog, espota, esptool, iot-bus-jtag, jlink, minimodule, olimex-arm-usb-ocd, olimex-arm-usb-ocd-h, olimex-arm-usb-tiny-h, olimex-jtag-tiny, tumpa
CURRENT: upload_protocol = esptool
Looking for upload port...
Use manually specified: /dev/ttyUSB0
Uploading .pio/build/dragino_lbt2/firmware.bin
esptool.py v3.1
Serial port /dev/ttyUSB0
Connecting......
Chip is ESP32-PICO-D4 (revision 1)
Features: WiFi, BT, Dual Core, 240MHz, Embedded Flash, VRef calibration in efuse, Coding Scheme None
Crystal is 40MHz
MAC: 30:83:98:da:6c:3c
Uploading stub...
Running stub...
Stub running...
Changing baud rate to 921600
Changed.
Configuring flash size...
Auto-detected Flash size: 4MB
Flash will be erased from 0x00001000 to 0x00005fff...
...

...
Writing at 0x000b7c39... (96 %)
Writing at 0x000bd407... (100 %)
Wrote 719168 bytes (464768 compressed) at 0x00010000 in 7.4 seconds (effective 777.4 kbit/s)...
Hash of data verified.

Leaving...
Hard resetting via RTS pin...
========================================================================================================= [SUCCESS] Took 13.42 seconds =========================================================================================================

# or

# Upload firmware (fast way, only application is updated)
$ esptool.py --chip esp32 --port "/dev/ttyUSB0" --baud 921600 --before no_reset --after no_reset write_flash --compress --flash_mode dio --flash_size detect 0x10000 .pio/build/dragino_lbt2/firmware.bin
esptool.py v3.2
Serial port /dev/ttyUSB0
WARNING: Pre-connection option "no_reset" was selected. Connection may fail if the chip is not in bootloader or flasher stub mode.
Connecting....
Chip is ESP32-PICO-D4 (revision 1)
Features: WiFi, BT, Dual Core, 240MHz, Embedded Flash, VRef calibration in efuse, Coding Scheme None
Crystal is 40MHz
MAC: 30:83:98:da:6c:3c
Uploading stub...
Running stub...
Stub running...
Changing baud rate to 921600
Changed.
Configuring flash size...
Auto-detected Flash size: 4MB
Flash will be erased from 0x00010000 to 0x000bffff...
Compressed 719168 bytes to 464768...
Wrote 719168 bytes (464768 compressed) at 0x00010000 in 7.4 seconds (effective 778.7 kbit/s)...
Hash of data verified.

Leaving...
Staying in bootloader.



# Check device output
$ pio device monitor
--- Available filters and text transformations: colorize, debug, default, direct, esp32_exception_decoder, hexlify, log2file, nocontrol, printable, send_on_enter, time
--- More details at https://bit.ly/pio-monitor-filters
--- Miniterm on /dev/ttyUSB0  115200,8,N,1 ---
--- Quit: Ctrl+C | Menu: Ctrl+T | Help: Ctrl+T followed by Ctrl+H ---
ets Jun  8 2016 00:22:57

rst:0x1 (POWERON_RESET),boot:0x13 (SPI_FAST_FLASH_BOOT)
configsip: 188777542, SPIWP:0xee
clk_drv:0x00,q_drv:0x00,d_drv:0x00,cs0_drv:0x00,hd_drv:0x00,wp_drv:0x00
mode:DIO, clock div:1
load:0x3fff0030,len:1284
load:0x40078000,len:12836
load:0x40080400,len:3032
entry 0x400805e4
Wakeup was not caused by deep sleep: 0
RXMODE_RSSI
Connecting to
Minitor Dragino LoRa GPS Shield Status
Time:155
HUM:39.17
TEM:25.96
BAT:3.30 mV
...

...
--- exit ---


# Clean build files if needed
$ pio run --target clean
Processing dragino_lbt2 (platform: https://github.com/platformio/platform-espressif32.git#feature/arduino-upstream; board: pico32; framework: arduino)
------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
* Adding toolchain toolchain-riscv32-esp with version 8.4.0+2021r1
* Adding toolchain toolchain-xtensa-esp32 with version 8.4.0+2021r1
* Adding toolchain toolchain-xtensa-esp32s2 with version 8.4.0+2021r1
Removed .pio/build/dragino_lbt2/partitions.bin
Removed .pio/build/dragino_lbt2/libFrameworkArduinoVariant.a
Removed .pio/build/dragino_lbt2/libFrameworkArduino.a
...

...
Removed .pio/build/dragino_lbt2/FrameworkArduino/libb64/cencode.c.d
Removed .pio/build/dragino_lbt2/FrameworkArduino/libb64/cencode.c.o
Done cleaning
========================================================================================================= [SUCCESS] Took 0.31 seconds =========================================================================================================
```
