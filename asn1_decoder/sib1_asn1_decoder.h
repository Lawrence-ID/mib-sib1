/*
    OpenXG MIB/SIB1 asn1 decoder
*/

#include <stdio.h>
#include <errno.h>
#include "NR_BCCH-BCH-Message.h"  
#include "NR_BCCH-DL-SCH-Message.h"
#include "asn_internal.h"
#include "xer_decoder.h"

#define ANS1_DECODE_NR_BCCH_BCH_Message       0
#define ANS1_DECODE_NR_BCCH_DL_SCH_Message    1
#define ANS1_DECODE_NR_UL_CCCH_Message        2
#define ANS1_DECODE_NR_DL_CCCH_Message        3
#define ANS1_DECODE_NR_UL_DCCH_Message        4
#define ANS1_DECODE_NR_DL_DCCH_Message        5

int decoder(uint8_t* msg_arr, uint msg_len);
