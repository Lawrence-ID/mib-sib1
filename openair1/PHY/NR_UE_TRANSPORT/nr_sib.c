/*
 * Licensed to the OpenAirInterface (OAI) Software Alliance under one or more
 * contributor license agreements.  See the NOTICE file distributed with
 * this work for additional information regarding copyright ownership.
 * The OpenAirInterface Software Alliance licenses this file to You under
 * the OAI Public License, Version 1.0  (the "License"); you may not use this file
 * except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.openairinterface.org/?page_id=698
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *-------------------------------------------------------------------------------
 * For more information about the OpenAirInterface (OAI) Software Alliance:
 *      contact@openairinterface.org
 */

/*! \file PHY/LTE_TRANSPORT/pbch.c
* \brief Top-level routines for generating and decoding  the PBCH/BCH physical/transport channel V8.6 2009-03
* \author R. Knopp, F. Kaltenberger
* \date 2011
* \version 0.1
* \company Eurecom
* \email: knopp@eurecom.fr,florian.kaltenberger.fr
* \note
* \warning
*/
#include "PHY/defs_nr_common.h"
#include "PHY/defs_nr_UE.h"
//#include "PHY/CODING/coding_extern.h"
#include "PHY/phy_extern_nr_ue.h"
#include "PHY/sse_intrin.h"
//#include "PHY/LTE_REFSIG/lte_refsig.h"
#include "PHY/INIT/phy_init.h"
#include "openair1/SCHED_NR_UE/defs.h"

//#define DEBUG_PBCH 
//#define DEBUG_PBCH_ENCODING

#ifdef OPENAIR2
  //#include "PHY_INTERFACE/defs.h"
#endif

#define reserved 0xffff
const uint8_t nrnew_slots_per_frame[5] = {10, 20, 40, 80, 160};

const int32_t tablenew_38213_13_1_c1[16] = {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, reserved}; // index 15 reserved
const int32_t tablenew_38213_13_1_c2[16] = {24, 24, 24, 24, 24, 24, 48, 48, 48, 48, 48, 48, 96, 96, 96, reserved}; // index 15 reserved
const int32_t tablenew_38213_13_1_c3[16] = { 2,  2,  2,  3,  3,  3,  1,  1,  2,  2,  3,  3,  1,  2,  3, reserved}; // index 15 reserved
const int32_t tablenew_38213_13_1_c4[16] = { 0,  2,  4,  0,  2,  4, 12, 16, 12, 16, 12, 16, 38, 38, 38, reserved}; // index 15 reserved

const int32_t tablenew_38213_13_2_c1[16] = {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, reserved, reserved}; // index 14-15 reserved
const int32_t tablenew_38213_13_2_c2[16] = {24, 24, 24, 24, 24, 24, 24, 24, 48, 48, 48, 48, 48, 48, reserved, reserved}; // index 14-15 reserved
const int32_t tablenew_38213_13_2_c3[16] = { 2,  2,  2,  2,  3,  3,  3,  3,  1,  1,  2,  2,  3,  3, reserved, reserved}; // index 14-15 reserved
const int32_t tablenew_38213_13_2_c4[16] = { 5,  6,  7,  8,  5,  6,  7,  8, 18, 20, 18, 20, 18, 20, reserved, reserved}; // index 14-15 reserved

const int32_t tablenew_38213_13_3_c1[16] = {1, 1, 1, 1, 1, 1, 1, 1, 1, reserved, reserved, reserved, reserved, reserved, reserved, reserved}; // index 09-15 reserved
const int32_t tablenew_38213_13_3_c2[16] = {48, 48, 48, 48, 48, 48, 96, 96, 96, reserved, reserved, reserved, reserved, reserved, reserved, reserved}; // index 09-15 reserved
const int32_t tablenew_38213_13_3_c3[16] = { 1,  1,  2,  2,  3,  3,  1,  2,  3, reserved, reserved, reserved, reserved, reserved, reserved, reserved}; // index 09-15 reserved
const int32_t tablenew_38213_13_3_c4[16] = { 2,  6,  2,  6,  2,  6, 28, 28, 28, reserved, reserved, reserved, reserved, reserved, reserved, reserved}; // index 09-15 reserved

const int32_t tablenew_38213_13_4_c1[16] = {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1};
const int32_t tablenew_38213_13_4_c2[16] = {24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 48, 48, 48, 48, 48, 48};
const int32_t tablenew_38213_13_4_c3[16] = { 2,  2,  2,  2,  2,  3,  3,  3,  3,  3,  1,  1,  1,  2,  2,  2};
const int32_t tablenew_38213_13_4_c4[16] = { 0,  1,  2,  3,  4,  0,  1,  2,  3,  4, 12, 14, 16, 12, 14, 16};

const int32_t tablenew_38213_13_5_c1[16] = {1, 1, 1, 1, 1, 1, 1, 1, 1, reserved, reserved, reserved, reserved, reserved, reserved, reserved}; // index 09-15 reserved
const int32_t tablenew_38213_13_5_c2[16] = {48, 48, 48, 96, 96, 96, 96, 96, 96, reserved, reserved, reserved, reserved, reserved, reserved, reserved}; // index 09-15 reserved
const int32_t tablenew_38213_13_5_c3[16] = { 1,  2,  3,  1,  1,  2,  2,  3,  3, reserved, reserved, reserved, reserved, reserved, reserved, reserved}; // index 09-15 reserved
const int32_t tablenew_38213_13_5_c4[16] = { 4,  4,  4,  0, 56,  0, 56,  0, 56, reserved, reserved, reserved, reserved, reserved, reserved, reserved}; // index 09-15 reserved

const int32_t tablenew_38213_13_6_c1[16] = {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, reserved, reserved, reserved, reserved, reserved, reserved}; // index 10-15 reserved
const int32_t tablenew_38213_13_6_c2[16] = {24, 24, 24, 24, 48, 48, 48, 48, 48, 48, reserved, reserved, reserved, reserved, reserved, reserved}; // index 10-15 reserved
const int32_t tablenew_38213_13_6_c3[16] = { 2,  2,  3,  3,  1,  1,  2,  2,  3,  3, reserved, reserved, reserved, reserved, reserved, reserved}; // index 10-15 reserved
const int32_t tablenew_38213_13_6_c4[16] = { 0,  4,  0,  4,  0, 28,  0, 28,  0, 28, reserved, reserved, reserved, reserved, reserved, reserved}; // index 10-15 reserved

const int32_t tablenew_38213_13_7_c1[16] = {1, 1, 1, 1, 1, 1, 1, 1, 2, 2, 2, 2, reserved, reserved, reserved, reserved}; // index 12-15 reserved
const int32_t tablenew_38213_13_7_c2[16] = {48, 48, 48, 48, 48, 48, 96, 96, 48, 48, 96, 96, reserved, reserved, reserved, reserved}; // index 12-15 reserved
const int32_t tablenew_38213_13_7_c3[16] = { 1,  1,  2,  2,  3,  3,  1,  2,  1,  1,  1,  1, reserved, reserved, reserved, reserved}; // index 12-15 reserved
const int32_t tablenew_38213_13_7_c4[16] = { 0,  8,  0,  8,  0,  8, 28, 28,-41, 49,-41, 97, reserved, reserved, reserved, reserved}; // index 12-15 reserved, condition A as default

const int32_t tablenew_38213_13_8_c1[16] = { 1,  1,  1,  1,  3,  3,  3,  3, reserved, reserved, reserved, reserved, reserved, reserved, reserved, reserved}; // index 15 reserved
const int32_t tablenew_38213_13_8_c2[16] = {24, 24, 48, 48, 24, 24, 48, 48, reserved, reserved, reserved, reserved, reserved, reserved, reserved, reserved}; // index 15 reserved
const int32_t tablenew_38213_13_8_c3[16] = { 2,  2,  1,  2,  2,  2,  2,  2, reserved, reserved, reserved, reserved, reserved, reserved, reserved, reserved}; // index 15 reserved
const int32_t tablenew_38213_13_8_c4[16] = { 0,  4, 14, 14,-20, 24,-20, 48, reserved, reserved, reserved, reserved, reserved, reserved, reserved, reserved}; // index 15 reserved, condition A as default

const int32_t tablenew_38213_13_9_c1[16] = {1, 1, 1, 1, reserved, reserved, reserved, reserved, reserved, reserved, reserved, reserved, reserved, reserved, reserved, reserved}; // index 04-15 reserved
const int32_t tablenew_38213_13_9_c2[16] = {96, 96, 96, 96, reserved, reserved, reserved, reserved, reserved, reserved, reserved, reserved, reserved, reserved, reserved, reserved}; // index 04-15 reserved
const int32_t tablenew_38213_13_9_c3[16] = { 1,  1,  2,  2, reserved, reserved, reserved, reserved, reserved, reserved, reserved, reserved, reserved, reserved, reserved, reserved}; // index 04-15 reserved
const int32_t tablenew_38213_13_9_c4[16] = { 0, 16,  0, 16, reserved, reserved, reserved, reserved, reserved, reserved, reserved, reserved, reserved, reserved, reserved, reserved}; // index 04-15 reserved

const int32_t tablenew_38213_13_10_c1[16] = {1, 1, 1, 1, 2, 2, 2, 2, reserved, reserved, reserved, reserved, reserved, reserved, reserved, reserved}; // index 08-15 reserved
const int32_t tablenew_38213_13_10_c2[16] = {48, 48, 48, 48, 24, 24, 48, 48, reserved, reserved, reserved, reserved, reserved, reserved, reserved, reserved}; // index 08-15 reserved
const int32_t tablenew_38213_13_10_c3[16] = { 1,  1,  2,  2,  1,  1,  1,  1, reserved, reserved, reserved, reserved, reserved, reserved, reserved, reserved}; // index 08-15 reserved
const int32_t tablenew_38213_13_10_c4[16] = { 0,  8,  0,  8,-41, 25,-41, 49, reserved, reserved, reserved, reserved, reserved, reserved, reserved, reserved}; // index 08-15 reserved, condition A as default

const float   tablenew_38213_13_11_c1[16] = { 0,  0,  2,  2,  5,  5,  7,  7,  0,  5,  0,  0,  2,  2,  5,  5};	//	O
const int32_t tablenew_38213_13_11_c2[16] = { 1,  2,  1,  2,  1,  2,  1,  2,  1,  1,  1,  1,  1,  1,  1,  1};
const float   tablenew_38213_13_11_c3[16] = { 1, 0.5f, 1, 0.5f, 1, 0.5f, 1, 0.5f,  1,  1,  1,  1,  1,  1,  1,  1};	//	M
const int32_t tablenew_38213_13_11_c4[16] = { 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  1,  2,  1,  2,  1,  2};	// i is even as default

const float   tablenew_38213_13_12_c1[16] = { 0, 0, 2.5f, 2.5f, 5, 5, 0, 2.5f, 5, 7.5f, 7.5f, 7.5f, 0, 5, reserved, reserved}; // O, index 14-15 reserved
const int32_t tablenew_38213_13_12_c2[16] = { 1,  2,  1,  2,  1,  2,  2,  2,  2,  1,  2,  2,  1,  1,  reserved,  reserved}; // index 14-15 reserved
const float   tablenew_38213_13_12_c3[16] = { 1, 0.5f, 1, 0.5f, 1, 0.5f, 0.5f, 0.5f, 0.5f, 1, 0.5f, 0.5f, 1, 1,  reserved,  reserved}; // M, index 14-15 reserved

const int32_t tablenew_38213_10_1_1_c2[5] = { 0, 0, 4, 2, 1 };

// for PDSCH from TS 38.214 subclause 5.1.2.1.1
const uint8_t tablenew_5_1_2_1_1_2_time_dom_res_alloc_A_dmrs_typeA_pos2[16][4]={
    {1,0,2,12},   // row index 1
    {1,0,2,10},   // row index 2
    {1,0,2,9},    // row index 3
    {1,0,2,7},    // row index 4
    {1,0,2,5},    // row index 5
    {0,0,9,4},    // row index 6
    {0,0,4,4},    // row index 7
    {0,0,5,7},    // row index 8
    {0,0,5,2},    // row index 9
    {0,0,9,2},    // row index 10
    {0,0,12,2},   // row index 11
    {1,0,1,13},   // row index 12
    {1,0,1,6},    // row index 13
    {1,0,2,4},    // row index 14
    {0,0,4,7},    // row index 15
    {0,0,8,4}     // row index 16
};
const uint8_t tablenew_5_1_2_1_1_2_time_dom_res_alloc_A_dmrs_typeA_pos3[16][4]={
    {1,0,3,11},   // row index 1
    {1,0,3,9},    // row index 2
    {1,0,3,8},    // row index 3
    {1,0,3,6},    // row index 4
    {1,0,3,4},    // row index 5
    {0,0,10,4},   // row index 6
    {0,0,6,4},    // row index 7
    {0,0,5,7},    // row index 8
    {0,0,5,2},    // row index 9
    {0,0,9,2},    // row index 10
    {0,0,12,2},   // row index 11
    {1,0,1,13},   // row index 12
    {1,0,1,6},    // row index 13
    {1,0,2,4},    // row index 14
    {0,0,4,7},    // row index 15
    {0,0,8,4}     // row index 16
};
const uint8_t tablenew_5_1_2_1_1_3_time_dom_res_alloc_A_extCP_dmrs_typeA_pos2[16][4]={
    {1,0,2,6},    // row index 1
    {1,0,2,10},   // row index 2
    {1,0,2,9},    // row index 3
    {1,0,2,7},    // row index 4
    {1,0,2,5},    // row index 5
    {0,0,6,4},    // row index 6
    {0,0,4,4},    // row index 7
    {0,0,5,6},    // row index 8
    {0,0,5,2},    // row index 9
    {0,0,9,2},    // row index 10
    {0,0,10,2},   // row index 11
    {1,0,1,11},   // row index 12
    {1,0,1,6},    // row index 13
    {1,0,2,4},    // row index 14
    {0,0,4,6},    // row index 15
    {0,0,8,4}     // row index 16
};
const uint8_t tablenew_5_1_2_1_1_3_time_dom_res_alloc_A_extCP_dmrs_typeA_pos3[16][4]={
    {1,0,3,5},    // row index 1
    {1,0,3,9},    // row index 2
    {1,0,3,8},    // row index 3
    {1,0,3,6},    // row index 4
    {1,0,3,4},    // row index 5
    {0,0,8,2},    // row index 6
    {0,0,6,4},    // row index 7
    {0,0,5,6},    // row index 8
    {0,0,5,2},    // row index 9
    {0,0,9,2},    // row index 10
    {0,0,10,2},   // row index 11
    {1,0,1,11},   // row index 12
    {1,0,1,6},    // row index 13
    {1,0,2,4},    // row index 14
    {0,0,4,6},    // row index 15
    {0,0,8,4}     // row index 16
};
const uint8_t tablenew_5_1_2_1_1_4_time_dom_res_alloc_B_dmrs_typeA_pos2[16][4]={
    {0,0,2,2},    // row index 1
    {0,0,4,2},    // row index 2
    {0,0,6,2},    // row index 3
    {0,0,8,2},    // row index 4
    {0,0,10,2},   // row index 5
    {0,1,2,2},    // row index 6
    {0,1,4,2},    // row index 7
    {0,0,2,4},    // row index 8
    {0,0,4,4},    // row index 9
    {0,0,6,4},    // row index 10
    {0,0,8,4},    // row index 11
    {0,0,10,4},   // row index 12
    {0,0,2,7},    // row index 13
    {1,0,2,12},   // row index 14
    {0,1,2,4},    // row index 15
    {0,0,0,0}     // row index 16
};
const uint8_t tablenew_5_1_2_1_1_4_time_dom_res_alloc_B_dmrs_typeA_pos3[16][4]={
    {0,0,2,2},    // row index 1
    {0,0,4,2},    // row index 2
    {0,0,6,2},    // row index 3
    {0,0,8,2},    // row index 4
    {0,0,10,2},   // row index 5
    {0,1,2,2},    // row index 6
    {0,1,4,2},    // row index 7
    {0,0,2,4},    // row index 8
    {0,0,4,4},    // row index 9
    {0,0,6,4},    // row index 10
    {0,0,8,4},    // row index 11
    {0,0,10,4},   // row index 12
    {0,0,2,7},    // row index 13
    {1,0,3,11},   // row index 14
    {0,1,2,4},    // row index 15
    {0,0,0,0}     // row index 16
};
const uint8_t tablenew_5_1_2_1_1_5_time_dom_res_alloc_C_dmrs_typeA_pos2[16][4]={
    {0,0,2,2},  // row index 1
    {0,0,4,2},  // row index 2
    {0,0,6,2},  // row index 3
    {0,0,8,2},  // row index 4
    {0,0,10,2}, // row index 5
    {0,0,0,0},  // row index 6
    {0,0,0,0},  // row index 7
    {0,0,2,4},  // row index 8
    {0,0,4,4},  // row index 9
    {0,0,6,4},  // row index 10
    {0,0,8,4},  // row index 11
    {0,0,10,4}, // row index 12
    {0,0,2,7},  // row index 13
    {1,0,2,12},  // row index 14
    {1,0,0,6},  // row index 15
    {1,0,2,6}   // row index 16
};
const uint8_t tablenew_5_1_2_1_1_5_time_dom_res_alloc_C_dmrs_typeA_pos3[16][4]={
    {0,0,2,2},  // row index 1
    {0,0,4,2},  // row index 2
    {0,0,6,2},  // row index 3
    {0,0,8,2},  // row index 4
    {0,0,10,2}, // row index 5
    {0,0,0,0},  // row index 6
    {0,0,0,0},  // row index 7
    {0,0,2,4},  // row index 8
    {0,0,4,4},  // row index 9
    {0,0,6,4},  // row index 10
    {0,0,8,4},  // row index 11
    {0,0,10,4}, // row index 12
    {0,0,2,7},  // row index 13
    {1,0,3,11},  // row index 14
    {1,0,0,6},  // row index 15
    {1,0,2,6}   // row index 16
};

int32_t tablenew_7_4_1_1_2_3_pdsch_dmrs_positions_l [13][8] = {                             // Duration in symbols
{-1,          -1,          -1,         -1,          1,          1,         1,         1},       //2              // (DMRS l' position)
{0,            0,           0,          0,          1,          1,         1,         1},       //3              // (DMRS l' position)
{0,            0,           0,          0,          1,          1,         1,         1},       //4               // (DMRS l' position)
{0,            0,           0,          0,          1,          17,        17,       17},       //5               // (DMRS l' position)
{0,            0,           0,          0,          1,          17,        17,       17},       //6               // (DMRS l' position)
{0,            0,           0,          0,          1,          17,        17,       17},       //7               // (DMRS l' position)
{0,          128,         128,        128,          1,          65,        73,       73},       //8               // (DMRS l' position)
{0,          128,         128,        128,          1,         129,       145,      145},       //9               // (DMRS l' position)
{0,          512,         576,        576,          1,         129,       145,      145},       //10              // (DMRS l' position)
{0,          512,         576,        576,          1,         257,       273,      585},       //11              // (DMRS l' position)
{0,          512,         576,       2336,          1,         513,       545,      585},       //12              // (DMRS l' position)
{0,         2048,        2176,       2336,          1,         513,       545,      585},       //13              // (DMRS l' position)
{0,         2048,        2176,       2336,         -1,          -1,       -1,        -1},       //14              // (DMRS l' position)
};

int get_info_from_tda_tables_new(int default_abc,
                              int tda,
                              int dmrs_TypeA_Position,
                              int normal_CP,
                              int *startSymbolIndex,
                              int *nrOfSymbols) {
  int k0 = 0;
  int is_mapping_typeA = 1;
  switch(default_abc){
    case 1:
      if (normal_CP){
        if (dmrs_TypeA_Position){
          is_mapping_typeA = tablenew_5_1_2_1_1_2_time_dom_res_alloc_A_dmrs_typeA_pos3[tda][0];
          k0 = tablenew_5_1_2_1_1_2_time_dom_res_alloc_A_dmrs_typeA_pos3[tda][1];
          *startSymbolIndex = tablenew_5_1_2_1_1_2_time_dom_res_alloc_A_dmrs_typeA_pos3[tda][2];
          *nrOfSymbols = tablenew_5_1_2_1_1_2_time_dom_res_alloc_A_dmrs_typeA_pos3[tda][3];
        }
        else{
          is_mapping_typeA = tablenew_5_1_2_1_1_2_time_dom_res_alloc_A_dmrs_typeA_pos2[tda][0];
          k0 = tablenew_5_1_2_1_1_2_time_dom_res_alloc_A_dmrs_typeA_pos2[tda][1];
          *startSymbolIndex = tablenew_5_1_2_1_1_2_time_dom_res_alloc_A_dmrs_typeA_pos2[tda][2];
          *nrOfSymbols = tablenew_5_1_2_1_1_2_time_dom_res_alloc_A_dmrs_typeA_pos2[tda][3];
        }
      }
      else{
        if (dmrs_TypeA_Position){
          is_mapping_typeA = tablenew_5_1_2_1_1_3_time_dom_res_alloc_A_extCP_dmrs_typeA_pos3[tda][0];
          k0 = tablenew_5_1_2_1_1_3_time_dom_res_alloc_A_extCP_dmrs_typeA_pos3[tda][1];
          *startSymbolIndex = tablenew_5_1_2_1_1_3_time_dom_res_alloc_A_extCP_dmrs_typeA_pos3[tda][2];
          *nrOfSymbols = tablenew_5_1_2_1_1_3_time_dom_res_alloc_A_extCP_dmrs_typeA_pos3[tda][3];
        }
        else{
          is_mapping_typeA = tablenew_5_1_2_1_1_3_time_dom_res_alloc_A_extCP_dmrs_typeA_pos2[tda][0];
          k0 = tablenew_5_1_2_1_1_3_time_dom_res_alloc_A_extCP_dmrs_typeA_pos2[tda][1];
          *startSymbolIndex = tablenew_5_1_2_1_1_3_time_dom_res_alloc_A_extCP_dmrs_typeA_pos2[tda][2];
          *nrOfSymbols = tablenew_5_1_2_1_1_3_time_dom_res_alloc_A_extCP_dmrs_typeA_pos2[tda][3];
        }
      }
      break;
    case 2:
      if (dmrs_TypeA_Position){
        is_mapping_typeA = tablenew_5_1_2_1_1_4_time_dom_res_alloc_B_dmrs_typeA_pos3[tda][0];
        k0 = tablenew_5_1_2_1_1_4_time_dom_res_alloc_B_dmrs_typeA_pos3[tda][1];
        *startSymbolIndex = tablenew_5_1_2_1_1_4_time_dom_res_alloc_B_dmrs_typeA_pos3[tda][2];
        *nrOfSymbols = tablenew_5_1_2_1_1_4_time_dom_res_alloc_B_dmrs_typeA_pos3[tda][3];
      }
      else{
        is_mapping_typeA = tablenew_5_1_2_1_1_4_time_dom_res_alloc_B_dmrs_typeA_pos2[tda][0];
        k0 = tablenew_5_1_2_1_1_4_time_dom_res_alloc_B_dmrs_typeA_pos2[tda][1];
        *startSymbolIndex = tablenew_5_1_2_1_1_4_time_dom_res_alloc_B_dmrs_typeA_pos2[tda][2];
        *nrOfSymbols = tablenew_5_1_2_1_1_4_time_dom_res_alloc_B_dmrs_typeA_pos2[tda][3];
      }
      break;
    case 3:
      if (dmrs_TypeA_Position){
        is_mapping_typeA = tablenew_5_1_2_1_1_5_time_dom_res_alloc_C_dmrs_typeA_pos3[tda][0];
        k0 = tablenew_5_1_2_1_1_5_time_dom_res_alloc_C_dmrs_typeA_pos3[tda][1];
        *startSymbolIndex = tablenew_5_1_2_1_1_5_time_dom_res_alloc_C_dmrs_typeA_pos3[tda][2];
        *nrOfSymbols = tablenew_5_1_2_1_1_5_time_dom_res_alloc_C_dmrs_typeA_pos3[tda][3];
      }
      else{
        is_mapping_typeA = tablenew_5_1_2_1_1_5_time_dom_res_alloc_C_dmrs_typeA_pos2[tda][0];
        k0 = tablenew_5_1_2_1_1_5_time_dom_res_alloc_C_dmrs_typeA_pos2[tda][1];
        *startSymbolIndex = tablenew_5_1_2_1_1_5_time_dom_res_alloc_C_dmrs_typeA_pos2[tda][2];
        *nrOfSymbols = tablenew_5_1_2_1_1_5_time_dom_res_alloc_C_dmrs_typeA_pos2[tda][3];
      }
      break;
    default:
     AssertFatal(1==0,"Invalid default time domaing allocation type\n");
  }
  AssertFatal(k0==0,"Only k0 = 0 is supported\n");
  AssertFatal(is_mapping_typeA==1,"Only mapping type A is currently supported\n");
  return is_mapping_typeA ? typeA : typeB;
}

void get_type0_PDCCH_CSS_config(NR_Type0_PDCCH_CSS_config_new_t *type0_PDCCH_CSS_config,
                                           int scs_common,
                                           uint32_t ssb_index,
                                           uint8_t  config_sib1,
                                           uint32_t scs_ssb,
                                           frequency_range_nr_t frequency_range,
                                           uint8_t num_slot_per_frame,
                                           uint16_t ssb_start_symbol,
                                           uint8_t ssb_subcarrier_offset,
                                           uint32_t ssb_offset_point_a) {

  nr_scs_e scs_pdcch;

  channel_bandwidth_new_t min_channel_bw = bwnew_10MHz; // TODO remove hardcoding and implement Table 5.3.5-1 in 38.104

  if (frequency_range == NR_FR2) {
    if(scs_common == SCS15_OR_60)
      scs_pdcch = kHz60;
    else
      scs_pdcch = kHz120;
  }
  else {
    frequency_range = NR_FR1;
    if(scs_common == SCS15_OR_60)
      scs_pdcch = kHz15;
    else
      scs_pdcch = kHz30;
  }
  type0_PDCCH_CSS_config->scs_pdcch = scs_pdcch;
  type0_PDCCH_CSS_config->ssb_index = ssb_index;
  //type0_PDCCH_CSS_config->frame = frameP;

  uint8_t ssb_slot = ssb_start_symbol/14;

  uint32_t is_condition_A = (ssb_subcarrier_offset == 0);   //  38.213 ch.13
  uint32_t index_4msb = config_sib1 >> 4;
  uint32_t index_4lsb = config_sib1 & 0x0F;

  type0_PDCCH_CSS_config->num_rbs = -1;
  type0_PDCCH_CSS_config->num_symbols = -1;
  type0_PDCCH_CSS_config->rb_offset = -1;
  //LOG_D(NR_MAC,"NR_SubcarrierSpacing_kHz30 %d, scs_ssb %d, scs_pdcch %d, min_chan_bw %d\n",(int)NR_SubcarrierSpacing_kHz30,(int)scs_ssb,(int)scs_pdcch,min_channel_bw);

  //  type0-pdcch coreset
  switch( ((int)scs_ssb << 3) | (int)scs_pdcch ){
    case (kHz15 << 3) | kHz15:
      AssertFatal(index_4msb < 15, "38.213 Table 13-1 4 MSB out of range\n");
      type0_PDCCH_CSS_config->type0_pdcch_ss_mux_pattern = 1;
      type0_PDCCH_CSS_config->num_rbs     = tablenew_38213_13_1_c2[index_4msb];
      type0_PDCCH_CSS_config->num_symbols = tablenew_38213_13_1_c3[index_4msb];
      type0_PDCCH_CSS_config->rb_offset   = tablenew_38213_13_1_c4[index_4msb];
      break;

    case (kHz15 << 3) | kHz30:
      AssertFatal(index_4msb < 14, "38.213 Table 13-2 4 MSB out of range\n");
      type0_PDCCH_CSS_config->type0_pdcch_ss_mux_pattern = 1;
      type0_PDCCH_CSS_config->num_rbs     = tablenew_38213_13_2_c2[index_4msb];
      type0_PDCCH_CSS_config->num_symbols = tablenew_38213_13_2_c3[index_4msb];
      type0_PDCCH_CSS_config->rb_offset   = tablenew_38213_13_2_c4[index_4msb];
      break;

    case (kHz30 << 3) | kHz15:
      if((min_channel_bw & bwnew_5MHz) | (min_channel_bw & bwnew_10MHz)){
        AssertFatal(index_4msb < 9, "38.213 Table 13-3 4 MSB out of range\n");
        type0_PDCCH_CSS_config->type0_pdcch_ss_mux_pattern = 1;
        type0_PDCCH_CSS_config->num_rbs     = tablenew_38213_13_3_c2[index_4msb];
        type0_PDCCH_CSS_config->num_symbols = tablenew_38213_13_3_c3[index_4msb];
        type0_PDCCH_CSS_config->rb_offset   = tablenew_38213_13_3_c4[index_4msb];
      }else if(min_channel_bw & bwnew_40MHz){
        AssertFatal(index_4msb < 9, "38.213 Table 13-5 4 MSB out of range\n");
        type0_PDCCH_CSS_config->type0_pdcch_ss_mux_pattern = 1;
        type0_PDCCH_CSS_config->num_rbs     = tablenew_38213_13_5_c2[index_4msb];
        type0_PDCCH_CSS_config->num_symbols = tablenew_38213_13_5_c3[index_4msb];
        type0_PDCCH_CSS_config->rb_offset   = tablenew_38213_13_5_c4[index_4msb];
      }else{ ; }

      break;
 
    case (kHz30 << 3) | kHz30:
      if((min_channel_bw & bwnew_5MHz) | (min_channel_bw & bwnew_10MHz)){
        type0_PDCCH_CSS_config->type0_pdcch_ss_mux_pattern = 1;
        type0_PDCCH_CSS_config->num_rbs     = tablenew_38213_13_4_c2[index_4msb];
        type0_PDCCH_CSS_config->num_symbols = tablenew_38213_13_4_c3[index_4msb];
        type0_PDCCH_CSS_config->rb_offset   = tablenew_38213_13_4_c4[index_4msb];

      }else if(min_channel_bw & bwnew_40MHz){
        AssertFatal(index_4msb < 10, "38.213 Table 13-6 4 MSB out of range\n");
        type0_PDCCH_CSS_config->type0_pdcch_ss_mux_pattern = 1;
        type0_PDCCH_CSS_config->num_rbs     = tablenew_38213_13_6_c2[index_4msb];
        type0_PDCCH_CSS_config->num_symbols = tablenew_38213_13_6_c3[index_4msb];
        type0_PDCCH_CSS_config->rb_offset   = tablenew_38213_13_6_c4[index_4msb];
      }else{ ; }
      break;

    case (kHz120 << 3) | kHz60:
      AssertFatal(index_4msb < 12, "38.213 Table 13-7 4 MSB out of range\n");
      if(index_4msb & 0x7){
        type0_PDCCH_CSS_config->type0_pdcch_ss_mux_pattern = 1;
      }else if(index_4msb & 0x18){
        type0_PDCCH_CSS_config->type0_pdcch_ss_mux_pattern = 2;
      }else{ ; }

      type0_PDCCH_CSS_config->num_rbs     = tablenew_38213_13_7_c2[index_4msb];
      type0_PDCCH_CSS_config->num_symbols = tablenew_38213_13_7_c3[index_4msb];
      if(!is_condition_A && (index_4msb == 8 || index_4msb == 10)){
        type0_PDCCH_CSS_config->rb_offset   = tablenew_38213_13_7_c4[index_4msb] - 1;
      }else{
        type0_PDCCH_CSS_config->rb_offset   = tablenew_38213_13_7_c4[index_4msb];
      }
      break;

    case (kHz120 << 3) | kHz120:
      AssertFatal(index_4msb < 8, "38.213 Table 13-8 4 MSB out of range\n");
      if(index_4msb & 0x3){
        type0_PDCCH_CSS_config->type0_pdcch_ss_mux_pattern = 1;
      }else if(index_4msb & 0x0c){
        type0_PDCCH_CSS_config->type0_pdcch_ss_mux_pattern = 3;
      }

      type0_PDCCH_CSS_config->num_rbs     = tablenew_38213_13_8_c2[index_4msb];
      type0_PDCCH_CSS_config->num_symbols = tablenew_38213_13_8_c3[index_4msb];
      if(!is_condition_A && (index_4msb == 4 || index_4msb == 6)){
        type0_PDCCH_CSS_config->rb_offset   = tablenew_38213_13_8_c4[index_4msb] - 1;
      }else{
        type0_PDCCH_CSS_config->rb_offset   = tablenew_38213_13_8_c4[index_4msb];
      }
      break;

    case (kHz240 << 3) | kHz60:
      AssertFatal(index_4msb < 4, "38.213 Table 13-9 4 MSB out of range\n");
      type0_PDCCH_CSS_config->type0_pdcch_ss_mux_pattern = 1;
      type0_PDCCH_CSS_config->num_rbs     = tablenew_38213_13_9_c2[index_4msb];
      type0_PDCCH_CSS_config->num_symbols = tablenew_38213_13_9_c3[index_4msb];
      type0_PDCCH_CSS_config->rb_offset   = tablenew_38213_13_9_c4[index_4msb];
      break;

    case (kHz240 << 3) | kHz120:
      AssertFatal(index_4msb < 8, "38.213 Table 13-10 4 MSB out of range\n");
      if(index_4msb & 0x3){
        type0_PDCCH_CSS_config->type0_pdcch_ss_mux_pattern = 1;
      }else if(index_4msb & 0x0c){
        type0_PDCCH_CSS_config->type0_pdcch_ss_mux_pattern = 2;
      }
      type0_PDCCH_CSS_config->num_rbs     = tablenew_38213_13_10_c2[index_4msb];
      type0_PDCCH_CSS_config->num_symbols = tablenew_38213_13_10_c3[index_4msb];
      if(!is_condition_A && (index_4msb == 4 || index_4msb == 6)){
        type0_PDCCH_CSS_config->rb_offset   = tablenew_38213_13_10_c4[index_4msb]-1;
      }else{
        type0_PDCCH_CSS_config->rb_offset   = tablenew_38213_13_10_c4[index_4msb];
      }

      break;

    default:
      LOG_E(NR_MAC,"scs_ssb %d, scs_pdcch %d, min_chan_bw %d\n", (int)scs_ssb,(int)scs_pdcch,min_channel_bw);
      break;
  }

  LOG_I(NR_MAC,"Coreset0: index_4msb=%d, num_rbs=%d, num_symb=%d, rb_offset=%d\n",
        index_4msb,type0_PDCCH_CSS_config->num_rbs,type0_PDCCH_CSS_config->num_symbols,type0_PDCCH_CSS_config->rb_offset );

  AssertFatal(type0_PDCCH_CSS_config->num_rbs != -1, "Type0 PDCCH coreset num_rbs undefined, index_4msb=%d, min_channel_bw %d, scs_ssb %d, scs_pdcch %d\n",index_4msb,min_channel_bw,(int)scs_ssb,(int)scs_pdcch);
  AssertFatal(type0_PDCCH_CSS_config->num_symbols != -1, "Type0 PDCCH coreset num_symbols undefined");
  AssertFatal(type0_PDCCH_CSS_config->rb_offset != -1, "Type0 PDCCH coreset rb_offset undefined");


  // type0-pdcch search space
  float big_o = 0.0f;
  float big_m = 0.0f;
  type0_PDCCH_CSS_config->sfn_c = SFN_C_IMPOSSIBLE_NEW;   //  only valid for mux=1
  type0_PDCCH_CSS_config->n_c = UINT_MAX;
  type0_PDCCH_CSS_config->number_of_search_space_per_slot = UINT_MAX;
  type0_PDCCH_CSS_config->first_symbol_index = UINT_MAX;
  type0_PDCCH_CSS_config->search_space_duration = 0;  //  element of search space
  //  38.213 table 10.1-1

  /// MUX PATTERN 1
  if(type0_PDCCH_CSS_config->type0_pdcch_ss_mux_pattern == 1 && frequency_range == NR_FR1){
    big_o = tablenew_38213_13_11_c1[index_4lsb];
    type0_PDCCH_CSS_config->number_of_search_space_per_slot = tablenew_38213_13_11_c2[index_4lsb];
    big_m = tablenew_38213_13_11_c3[index_4lsb];

    uint32_t temp = (uint32_t)(big_o*(1<<scs_pdcch)) + (uint32_t)(type0_PDCCH_CSS_config->ssb_index*big_m);
    type0_PDCCH_CSS_config->n_c = temp / num_slot_per_frame;
    if((temp/num_slot_per_frame) & 0x1){
      type0_PDCCH_CSS_config->sfn_c = SFN_C_MOD_2_EQ_1_NEW;
    }else{
      type0_PDCCH_CSS_config->sfn_c = SFN_C_MOD_2_EQ_0_NEW;
    }

    if((index_4lsb == 1 || index_4lsb == 3 || index_4lsb == 5 || index_4lsb == 7) && (type0_PDCCH_CSS_config->ssb_index&1)){
      type0_PDCCH_CSS_config->first_symbol_index = type0_PDCCH_CSS_config->num_symbols;
    }else{
      type0_PDCCH_CSS_config->first_symbol_index = tablenew_38213_13_11_c4[index_4lsb];
    }
    //  38.213 chapter 13: over two consecutive slots
    type0_PDCCH_CSS_config->search_space_duration = 2;
    // two frames
    type0_PDCCH_CSS_config->search_space_frame_period = nrnew_slots_per_frame[scs_ssb]<<1;
  }

  if(type0_PDCCH_CSS_config->type0_pdcch_ss_mux_pattern == 1 && frequency_range == NR_FR2){
    big_o = tablenew_38213_13_12_c1[index_4lsb];
    type0_PDCCH_CSS_config->number_of_search_space_per_slot = tablenew_38213_13_11_c2[index_4lsb];
    big_m = tablenew_38213_13_12_c3[index_4lsb];

    if((index_4lsb == 1 || index_4lsb == 3 || index_4lsb == 5 || index_4lsb == 10) && (type0_PDCCH_CSS_config->ssb_index&1)){
      type0_PDCCH_CSS_config->first_symbol_index = 7;
    }else if((index_4lsb == 6 || index_4lsb == 7 || index_4lsb == 8 || index_4lsb == 11) && (type0_PDCCH_CSS_config->ssb_index&1)){
      type0_PDCCH_CSS_config->first_symbol_index = type0_PDCCH_CSS_config->num_symbols;
    }else{
      type0_PDCCH_CSS_config->first_symbol_index = 0;
    }
    //  38.213 chapter 13: over two consecutive slots
    type0_PDCCH_CSS_config->search_space_duration = 2;
    // two frames
    type0_PDCCH_CSS_config->search_space_frame_period = nrnew_slots_per_frame[scs_ssb]<<1;
  }

  /// MUX PATTERN 2
  if(type0_PDCCH_CSS_config->type0_pdcch_ss_mux_pattern == 2){

    if((scs_ssb == kHz120) && (scs_pdcch == kHz60)){
      //  38.213 Table 13-13
      AssertFatal(index_4lsb == 0, "38.213 Table 13-13 4 LSB out of range\n");
      //  PDCCH monitoring occasions (SFN and slot number) same as SSB frame-slot
      //                sfn_c = SFN_C_EQ_SFN_SSB;
      type0_PDCCH_CSS_config->n_c = ssb_slot;
      switch(type0_PDCCH_CSS_config->ssb_index & 0x3){    //  ssb_index(i) mod 4
        case 0:
          type0_PDCCH_CSS_config->first_symbol_index = 0;
          break;
        case 1:
          type0_PDCCH_CSS_config->first_symbol_index = 1;
          break;
        case 2:
          type0_PDCCH_CSS_config->first_symbol_index = 6;
          break;
        case 3:
          type0_PDCCH_CSS_config->first_symbol_index = 7;
          break;
        default: break;
      }

    }else if((scs_ssb == kHz240) && (scs_pdcch == kHz120)){
      //  38.213 Table 13-14
      AssertFatal(index_4lsb == 0, "38.213 Table 13-14 4 LSB out of range\n");
      //  PDCCH monitoring occasions (SFN and slot number) same as SSB frame-slot
      //                sfn_c = SFN_C_EQ_SFN_SSB;
      type0_PDCCH_CSS_config->n_c = ssb_slot;
      switch(type0_PDCCH_CSS_config->ssb_index & 0x7){    //  ssb_index(i) mod 8
        case 0:
          type0_PDCCH_CSS_config->first_symbol_index = 0;
          break;
        case 1:
          type0_PDCCH_CSS_config->first_symbol_index = 1;
          break;
        case 2:
          type0_PDCCH_CSS_config->first_symbol_index = 2;
          break;
        case 3:
          type0_PDCCH_CSS_config->first_symbol_index = 3;
          break;
        case 4:
          type0_PDCCH_CSS_config->first_symbol_index = 12;
          type0_PDCCH_CSS_config->n_c = ssb_slot - 1;
          break;
        case 5:
          type0_PDCCH_CSS_config->first_symbol_index = 13;
          type0_PDCCH_CSS_config->n_c = ssb_slot - 1;
          break;
        case 6:
          type0_PDCCH_CSS_config->first_symbol_index = 0;
          break;
        case 7:
          type0_PDCCH_CSS_config->first_symbol_index = 1;
          break;
        default: break;
      }
    }else{ ; }
    //  38.213 chapter 13: over one slot
    type0_PDCCH_CSS_config->search_space_duration = 1;
    // SSB periodicity in slots
    //type0_PDCCH_CSS_config->search_space_frame_period = ssb_period*nr_slots_per_frame[scs_ssb];
  }

  /// MUX PATTERN 3
  if(type0_PDCCH_CSS_config->type0_pdcch_ss_mux_pattern == 3){
    if((scs_ssb == kHz120) && (scs_pdcch == kHz120)){
      //  38.213 Table 13-15
      AssertFatal(index_4lsb == 0, "38.213 Table 13-15 4 LSB out of range\n");
      //  PDCCH monitoring occasions (SFN and slot number) same as SSB frame-slot
      //                sfn_c = SFN_C_EQ_SFN_SSB;
      type0_PDCCH_CSS_config->n_c = ssb_slot;
      switch(type0_PDCCH_CSS_config->ssb_index & 0x3){    //  ssb_index(i) mod 4
        case 0:
          type0_PDCCH_CSS_config->first_symbol_index = 4;
          break;
        case 1:
          type0_PDCCH_CSS_config->first_symbol_index = 8;
          break;
        case 2:
          type0_PDCCH_CSS_config->first_symbol_index = 2;
          break;
        case 3:
          type0_PDCCH_CSS_config->first_symbol_index = 6;
          break;
        default: break;
      }
    }else{ ; }
    //  38.213 chapter 13: over one slot
    type0_PDCCH_CSS_config->search_space_duration = 1;
    // SSB periodicity in slots
    //type0_PDCCH_CSS_config->search_space_frame_period = ssb_period*nr_slots_per_frame[scs_ssb];
  }

  AssertFatal(type0_PDCCH_CSS_config->number_of_search_space_per_slot!=UINT_MAX,"");

//  uint32_t coreset_duration = num_symbols * number_of_search_space_per_slot;
//    mac->type0_pdcch_dci_config.number_of_candidates[0] = table_38213_10_1_1_c2[0];
//    mac->type0_pdcch_dci_config.number_of_candidates[1] = table_38213_10_1_1_c2[1];
//    mac->type0_pdcch_dci_config.number_of_candidates[2] = table_38213_10_1_1_c2[2];   //  CCE aggregation level = 4
//    mac->type0_pdcch_dci_config.number_of_candidates[3] = table_38213_10_1_1_c2[3];   //  CCE aggregation level = 8
//    mac->type0_pdcch_dci_config.number_of_candidates[4] = table_38213_10_1_1_c2[4];   //  CCE aggregation level = 16
//    mac->type0_pdcch_dci_config.duration = search_space_duration;
//    mac->type0_pdcch_dci_config.coreset.duration = coreset_duration;   //  coreset
//    AssertFatal(first_symbol_index!=UINT_MAX,"");
//    mac->type0_pdcch_dci_config.monitoring_symbols_within_slot = (0x3fff << first_symbol_index) & (0x3fff >> (14-coreset_duration-first_symbol_index)) & 0x3fff;

  AssertFatal(type0_PDCCH_CSS_config->sfn_c!=SFN_C_IMPOSSIBLE_NEW,"");
  AssertFatal(type0_PDCCH_CSS_config->n_c!=UINT_MAX,"");

  type0_PDCCH_CSS_config->n_0 = ((uint32_t)(big_o*(1<<scs_pdcch)) + (uint32_t)(type0_PDCCH_CSS_config->ssb_index*big_m))%num_slot_per_frame;
  type0_PDCCH_CSS_config->cset_start_rb = ssb_offset_point_a - type0_PDCCH_CSS_config->rb_offset;

  int max_agg = (type0_PDCCH_CSS_config->num_symbols*type0_PDCCH_CSS_config->num_rbs)/6;
  int agl4,agl8, agl16;
  agl4 = (((max_agg>>2) > 4)? 4 : max_agg>>2);
  agl8 = (((max_agg>>3) > 2)? 2 : max_agg>>3);
  agl16 = (((max_agg>>4) > 1)? 1 : max_agg>>4);
  int i = 0;
  int j = 0;


  for (int j=0; j < agl16; j++)
  {
    type0_PDCCH_CSS_config->CCE[i]  = j * 16;
    type0_PDCCH_CSS_config->L[i] = 16;
    i++;
  }
    for (int j=0; j < agl8; j++)
  {
    type0_PDCCH_CSS_config->CCE[i]  = j * 8;
    type0_PDCCH_CSS_config->L[i] = 8;
    i++;
  }
    for (int j=0; j < agl4; j++)
  {
    type0_PDCCH_CSS_config->CCE[i]  = j * 4;
    type0_PDCCH_CSS_config->L[i] = 4;
    i++;
  }
 

  type0_PDCCH_CSS_config->number_of_candidates = i;
  type0_PDCCH_CSS_config->dci_size = 28 + ceil( log2( (type0_PDCCH_CSS_config->num_rbs*(type0_PDCCH_CSS_config->num_rbs+1))>>1 ) );
  LOG_I(PHY, "sfn_c %d, n_c %d, n_0 %d, number_of_search_space_per_slot %d, first_symbol_index %d, search_space_duration %d, cset_start_rb %d, caditates %d, dci size %d\n",
 type0_PDCCH_CSS_config->sfn_c,
 type0_PDCCH_CSS_config->n_c,
 type0_PDCCH_CSS_config->n_0,
 type0_PDCCH_CSS_config->number_of_search_space_per_slot,
 type0_PDCCH_CSS_config->first_symbol_index,
 type0_PDCCH_CSS_config->search_space_duration,
 type0_PDCCH_CSS_config->cset_start_rb,
 type0_PDCCH_CSS_config->number_of_candidates,
 type0_PDCCH_CSS_config->dci_size);
}

int fill_dmrs_mask_sib(int typeA_pos, int numSymb, int startSymb)
{
   int32_t l_prime = -1;
   int ld = numSymb + startSymb;
   int row = ld - 2;
   int l0 = 1 << typeA_pos;
   l_prime = tablenew_7_4_1_1_2_3_pdsch_dmrs_positions_l[row][2];
   l_prime = (l_prime | l0);
   return l_prime;
}
