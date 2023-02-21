/*
* Copyright (c) 2014-2016 IBM Corporation.
* All rights reserved.
*
* Copyright (c) 2017 MCCI Corporation
* All rights reserved.
*
*  Redistribution and use in source and binary forms, with or without
*  modification, are permitted provided that the following conditions are met:
*  * Redistributions of source code must retain the above copyright
*    notice, this list of conditions and the following disclaimer.
*  * Redistributions in binary form must reproduce the above copyright
*    notice, this list of conditions and the following disclaimer in the
*    documentation and/or other materials provided with the distribution.
*  * Neither the name of the <organization> nor the
*    names of its contributors may be used to endorse or promote products
*    derived from this software without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
* ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
* WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
* DISCLAIMED. IN NO EVENT SHALL <COPYRIGHT HOLDER> BE LIABLE FOR ANY
* DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
* (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
* LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
* ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
* (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
* SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#ifndef _lorabase_ru864_h_
#define _lorabase_ru864_h_

#ifndef _LMIC_CONFIG_PRECONDITIONS_H_
# include "lmic_config_preconditions.h"
#endif

/****************************************************************************\
|
| Basic definitions for RU864 (always in scope)
|
\****************************************************************************/

enum _dr_ru864_t {
        RU864_DR_SF12 = 0,
        RU864_DR_SF11,
        RU864_DR_SF10,
        RU864_DR_SF9,
        RU864_DR_SF8,
        RU864_DR_SF7,
        RU864_DR_SF7B,
        RU864_DR_FSK,
        RU864_DR_NONE
};

// Bands:
//  g1 :   1%  16dBm
//                 freq                band     datarates
enum {

        RU864_F1    = 868900000,      // g1   SF7-12
        RU864_F2    = 869100000,      // g1   SF7-12
        RU864_FDOWN = 869100000,      //      (RX2 freq, DR2)
        RU864_FBCN  = 869100000,      //      default BCN, DR3
        RU864_FPING = 868900000,      //      default ping, DR3

};
enum {
        RU864_FREQ_MIN = 864000000,
        RU864_FREQ_MAX = 870000000
};
enum {
        RU864_TX_EIRP_MAX_DBM = 16      // 16 dBm EIRP. So subtract 3 dBm for a 3 dBi antenna.
};

enum { RU864_LMIC_REGION_EIRP = 1 };         // region uses EIRP

enum { DR_PAGE_RU864 = 0x10 * (LMIC_REGION_ru864 - 1) };

#endif /* _lorabase_ru864_h_ */
