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

#ifndef _lorabase_ma869_h_
#define _lorabase_ma869_h_

#ifndef _LMIC_CONFIG_PRECONDITIONS_H_
# include "lmic_config_preconditions.h"
#endif

/****************************************************************************\
|
| Basic definitions for IN866 (always in scope)
|
\****************************************************************************/

enum _dr_ma869_t {
        MA869_DR_SF12 = 0,      // DR0
        MA869_DR_SF11,          // DR1
        MA869_DR_SF10,          // DR2
        MA869_DR_SF9,           // DR3
        MA869_DR_SF8,           // DR4
        MA869_DR_SF7,           // DR5
        MA869_DR_SF7B,
        MA869_DR_FSK,           // DR7
        MA869_DR_NONE
};

// There is no dwell-time or duty-cycle limitation for IN
//
// max power: 30dBM
//
//                 freq                 datarates
enum {
        MA869_F1 = 869100000,      //   SF7-12 (DR0-5)
        MA869_F2 = 869300000,      //   SF7-12 (DR0-5)
        MA869_F3 = 869500000,      //   SF7-12 (DR0-5)
        MA869_FB = 869525000,      //   beacon/ping
};
enum {
        MA869_FREQ_MIN = 863000000,
        MA869_FREQ_MAX = 870000000
};
enum {
        MA869_TX_EIRP_MAX_DBM = 16      // 16 dBm
};
enum { DR_PAGE_MA869 = 0x10 * (LMIC_REGION_ma869 - 1) };

enum { MA869_LMIC_REGION_EIRP = 1 };         // region uses EIRP

#endif /* _lorabase_ma869_ */