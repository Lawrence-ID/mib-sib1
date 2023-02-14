/*
 * Licensed to the OpenAirInterface (OAI) Software Alliance under one or more
 * contributor license agreements.  See the NOTICE file distributed with
 * this work for additional information regarding copyright ownership.
 * The OpenAirInterface Software Alliance licenses this file to You under
 * the OAI Public License, Version 1.1  (the "License"); you may not use this file
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

/*! \file phy_procedures_nr_ue.c
 * \brief Implementation of UE procedures from 36.213 LTE specifications
 * \author R. Knopp, F. Kaltenberger, N. Nikaein, A. Mico Pereperez, G. Casati
 * \date 2018
 * \version 0.1
 * \company Eurecom
 * \email: knopp@eurecom.fr,florian.kaltenberger@eurecom.fr, navid.nikaein@eurecom.fr, guido.casati@iis.fraunhofer.de
 * \note
 * \warning
 */

#define _GNU_SOURCE

//#include "nr/nr_common.h"
#include "assertions.h"
#include "defs.h"
#include "PHY/defs_nr_UE.h"
#include "PHY/phy_extern_nr_ue.h"
#include "PHY/MODULATION/modulation_UE.h"
#include "PHY/NR_REFSIG/refsig_defs_ue.h"
#include "PHY/NR_UE_TRANSPORT/nr_transport_ue.h"
#include "PHY/NR_UE_TRANSPORT/nr_transport_proto_ue.h"
#include "SCHED_NR_UE/defs.h"
//#include "SCHED_NR_UE/pucch_uci_ue_nr.h"
//#include "SCHED_NR/extern.h"
//#include "SCHED_NR_UE/phy_sch_processing_time.h"
#include "PHY/NR_UE_ESTIMATION/nr_estimation.h"
//#include "PHY/NR_TRANSPORT/nr_dci.h"
#ifdef EMOS
#include "SCHED/phy_procedures_emos.h"
#endif
//#include "executables/softmodem-common.h"
//#include "executables/nr-uesoftmodem.h"
//#include "openair2/LAYER2/NR_MAC_UE/mac_proto.h"

//#define DEBUG_PHY_PROC
#define NR_PDCCH_SCHED
//#define NR_PDCCH_SCHED_DEBUG
//#define NR_PUCCH_SCHED
//#define NR_PUCCH_SCHED_DEBUG

#ifndef PUCCH
#define PUCCH
#endif

//#include "LAYER2/NR_MAC_UE/mac_defs.h"
//#include "LAYER2/NR_MAC_UE/mac_proto.h"
//#include "common/utils/LOG/log.h"
#include "PHY/nw_log.h"
#ifdef EMOS
fifo_dump_emos_UE emos_dump_UE;
#endif

// #include "common/utils/LOG/vcd_signal_dumper.h"
// #include "UTIL/OPT/opt.h"
// #include "intertask_interface.h"
//#include "T.h"

char nr_mode_string[NUM_UE_MODE][20] = {"NOT SYNCHED","PRACH","RAR","RA_WAIT_CR", "PUSCH", "RESYNCH"};

const uint8_t nr_rv_round_map_ue[4] = {0, 2, 1, 3};

#if defined(EXMIMO) || defined(OAI_USRP) || defined(OAI_BLADERF) || defined(OAI_LMSSDR) || defined(OAI_ADRV9371_ZC706) || defined(OAI_YUNSDR)
extern uint64_t downlink_frequency[MAX_NUM_CCs][4];
#endif
#if defined(__arm__) || defined(__aarch64__)
extern volatile int8_t symbol_end_flag[14];
extern pthread_cond_t sym_cond;
extern pthread_mutex_t sym_mutex;
#endif

unsigned int gain_table[31] = {100,112,126,141,158,178,200,224,251,282,316,359,398,447,501,562,631,708,794,891,1000,1122,1258,1412,1585,1778,1995,2239,2512,2818,3162};

UE_MODE_t get_nrUE_mode(uint8_t Mod_id,uint8_t CC_id,uint8_t gNB_id){
  return(PHY_vars_UE_g[Mod_id][CC_id]->UE_mode[gNB_id]);
}

#if 0
uint8_t get_ra_PreambleIndex(uint8_t Mod_id, uint8_t CC_id, uint8_t gNB_id){

  return PHY_vars_UE_g[Mod_id][CC_id]->prach_resources[gNB_id]->ra_PreambleIndex;

}
#endif

// convert time factor "16 * 64 * T_c / (2^mu)" in N_TA calculation in TS38.213 section 4.2 to samples by multiplying with samples per second
//   16 * 64 * T_c            / (2^mu) * samples_per_second
// = 16 * T_s                 / (2^mu) * samples_per_second
// = 16 * 1 / (15 kHz * 2048) / (2^mu) * (15 kHz * 2^mu * ofdm_symbol_size)
// = 16 * 1 /           2048           *                  ofdm_symbol_size
// = 16 * ofdm_symbol_size / 2048
static inline
uint16_t get_bw_scaling(uint16_t ofdm_symbol_size){
  return 16 * ofdm_symbol_size / 2048;
}

#if 0
void nr_ue_measurement_procedures(uint16_t l,
                                  PHY_VARS_NR_UE *ue,
                                  UE_nr_rxtx_proc_t *proc,
                                  uint8_t gNB_id,
                                  uint16_t slot){

  NR_DL_FRAME_PARMS *frame_parms=&ue->frame_parms;
  int frame_rx   = proc->frame_rx;
  int nr_slot_rx = proc->nr_slot_rx;
  //VCD_SIGNAL_DUMPER_DUMP_FUNCTION_BY_NAME(VCD_SIGNAL_DUMPER_FUNCTIONS_UE_MEASUREMENT_PROCEDURES, VCD_FUNCTION_IN);

  if (l==2) {

    LOG_D(PHY,"Doing UE measurement procedures in symbol l %u Ncp %d nr_slot_rx %d, rxdata %p\n",
      l,
      ue->frame_parms.Ncp,
      nr_slot_rx,
      ue->common_vars.rxdata);

    nr_ue_measurements(ue, proc, nr_slot_rx);

#if T_TRACER
    if(slot == 0)
      T(T_UE_PHY_MEAS, T_INT(gNB_id),  T_INT(ue->Mod_id), T_INT(frame_rx%1024), T_INT(nr_slot_rx),
	T_INT((int)(10*log10(ue->measurements.rsrp[0])-ue->rx_total_gain_dB)),
	T_INT((int)ue->measurements.rx_rssi_dBm[0]),
	T_INT((int)(ue->measurements.rx_power_avg_dB[0] - ue->measurements.n0_power_avg_dB)),
	T_INT((int)ue->measurements.rx_power_avg_dB[0]),
	T_INT((int)ue->measurements.n0_power_avg_dB),
	T_INT((int)ue->measurements.wideband_cqi_avg[0]),
	T_INT((int)ue->common_vars.freq_offset));
#endif
  }

  // accumulate and filter timing offset estimation every subframe (instead of every frame)
  if (( slot == 2) && (l==(2-frame_parms->Ncp))) {

    // AGC

    //VCD_SIGNAL_DUMPER_DUMP_FUNCTION_BY_NAME(VCD_SIGNAL_DUMPER_FUNCTIONS_UE_GAIN_CONTROL, VCD_FUNCTION_IN);


    //printf("start adjust gain power avg db %d\n", ue->measurements.rx_power_avg_dB[gNB_id]);
    phy_adjust_gain_nr (ue,ue->measurements.rx_power_avg_dB[gNB_id],gNB_id);
    
    //VCD_SIGNAL_DUMPER_DUMP_FUNCTION_BY_NAME(VCD_SIGNAL_DUMPER_FUNCTIONS_UE_GAIN_CONTROL, VCD_FUNCTION_OUT);

}

  //VCD_SIGNAL_DUMPER_DUMP_FUNCTION_BY_NAME(VCD_SIGNAL_DUMPER_FUNCTIONS_UE_MEASUREMENT_PROCEDURES, VCD_FUNCTION_OUT);
}
#endif
#if 0
int nr_ue_pbch_procedures(uint8_t gNB_id,
			   PHY_VARS_NR_UE *ue,
			   UE_nr_rxtx_proc_t *proc,
			   uint8_t abstraction_flag)
{
  int ret = 0;

  DevAssert(ue);

  int frame_rx = proc->frame_rx;
  int nr_slot_rx = proc->nr_slot_rx;

  //VCD_SIGNAL_DUMPER_DUMP_FUNCTION_BY_NAME(VCD_SIGNAL_DUMPER_FUNCTIONS_UE_PBCH_PROCEDURES, VCD_FUNCTION_IN);

  LOG_D(PHY,"[UE  %d] Frame %d Slot %d, Trying PBCH (NidCell %d, gNB_id %d)\n",ue->Mod_id,frame_rx,nr_slot_rx,ue->frame_parms.Nid_cell,gNB_id);

  ret = nr_rx_pbch(ue, proc,
                   ue->pbch_vars[gNB_id],
                   &ue->frame_parms,
                   gNB_id,
                   (ue->frame_parms.ssb_index)&7);

  if (ret==0) {

    ue->pbch_vars[gNB_id]->pdu_errors_conseq = 0;

    // Switch to PRACH state if it is first PBCH after initial synch and no timing correction is performed
    if (ue->UE_mode[gNB_id] == NOT_SYNCHED && ue->no_timing_correction == 1){
      if (get_softmodem_params()->do_ra) {
        ue->UE_mode[gNB_id] = PRACH;
      } else {
        ue->UE_mode[gNB_id] = PUSCH;
      }
    }

#ifdef DEBUG_PHY_PROC
    uint16_t frame_tx;
    LOG_D(PHY,"[UE %d] frame %d, nr_slot_rx %d, Received PBCH (MIB): frame_tx %d. N_RB_DL %d\n",
    ue->Mod_id,
    frame_rx,
    nr_slot_rx,
    frame_tx,
    ue->frame_parms.N_RB_DL);
#endif

  } else {
    LOG_E(PHY,"[UE %d] frame %d, nr_slot_rx %d, Error decoding PBCH!\n",
	  ue->Mod_id,frame_rx, nr_slot_rx);
    /*FILE *fd;
    if ((fd = fopen("rxsig_frame0.dat","w")) != NULL) {
                  fwrite((void *)&ue->common_vars.rxdata[0][0],
                         sizeof(int32_t),
                         ue->frame_parms.samples_per_frame,
                         fd);
                  LOG_I(PHY,"Dummping Frame ... bye bye \n");
                  fclose(fd);
                  exit(0);
                }*/

    /*
    write_output("rxsig0.m","rxs0", ue->common_vars.rxdata[0],ue->frame_parms.samples_per_subframe,1,1);


      write_output("H00.m","h00",&(ue->common_vars.dl_ch_estimates[0][0][0]),((ue->frame_parms.Ncp==0)?7:6)*(ue->frame_parms.ofdm_symbol_size),1,1);
      write_output("H10.m","h10",&(ue->common_vars.dl_ch_estimates[0][2][0]),((ue->frame_parms.Ncp==0)?7:6)*(ue->frame_parms.ofdm_symbol_size),1,1);

      write_output("rxsigF0.m","rxsF0", ue->common_vars.rxdataF[0],8*ue->frame_parms.ofdm_symbol_size,1,1);
      write_output("PBCH_rxF0_ext.m","pbch0_ext",ue->pbch_vars[0]->rxdataF_ext[0],12*4*6,1,1);
      write_output("PBCH_rxF0_comp.m","pbch0_comp",ue->pbch_vars[0]->rxdataF_comp[0],12*4*6,1,1);
      write_output("PBCH_rxF_llr.m","pbch_llr",ue->pbch_vars[0]->llr,(ue->frame_parms.Ncp==0) ? 1920 : 1728,1,4);
      exit(-1);
    */

    ue->pbch_vars[gNB_id]->pdu_errors_conseq++;
    ue->pbch_vars[gNB_id]->pdu_errors++;

    if (ue->pbch_vars[gNB_id]->pdu_errors_conseq>=100) {
      if (get_softmodem_params()->non_stop) {
        LOG_E(PHY,"More that 100 consecutive PBCH errors! Going back to Sync mode!\n");
        ue->lost_sync = 1;
      } else {
        LOG_E(PHY,"More that 100 consecutive PBCH errors! Exiting!\n");
        //exit_fun("More that 100 consecutive PBCH errors! Exiting!\n");
      }
    }
  }

  if (frame_rx % 100 == 0) {
    ue->pbch_vars[gNB_id]->pdu_fer = ue->pbch_vars[gNB_id]->pdu_errors - ue->pbch_vars[gNB_id]->pdu_errors_last;
    ue->pbch_vars[gNB_id]->pdu_errors_last = ue->pbch_vars[gNB_id]->pdu_errors;
  }

#ifdef DEBUG_PHY_PROC
  LOG_D(PHY,"[UE %d] frame %d, slot %d, PBCH errors = %d, consecutive errors = %d!\n",
	ue->Mod_id,frame_rx, nr_slot_rx,
	ue->pbch_vars[gNB_id]->pdu_errors,
	ue->pbch_vars[gNB_id]->pdu_errors_conseq);
#endif
  //VCD_SIGNAL_DUMPER_DUMP_FUNCTION_BY_NAME(VCD_SIGNAL_DUMPER_FUNCTIONS_UE_PBCH_PROCEDURES, VCD_FUNCTION_OUT);
  return ret;
}
#endif

#ifdef NR_PDCCH_SCHED

int nr_ue_pdcch_procedures(uint8_t gNB_id,
                           PHY_VARS_NR_UE *ue,
                           UE_nr_rxtx_proc_t *proc,
                           int n_ss)
{
  int frame_rx = proc->frame_rx;
  int nr_slot_rx = proc->nr_slot_rx;
  unsigned int dci_cnt=0;

  NR_UE_PDCCH *pdcch_vars = ue->pdcch_vars[proc->thread_id][gNB_id];
   
  //VCD_SIGNAL_DUMPER_DUMP_FUNCTION_BY_NAME(VCD_SIGNAL_DUMPER_FUNCTIONS_RX_PDCCH, VCD_FUNCTION_IN);
  nr_rx_pdcch_sib(ue, proc);
  //VCD_SIGNAL_DUMPER_DUMP_FUNCTION_BY_NAME(VCD_SIGNAL_DUMPER_FUNCTIONS_RX_PDCCH, VCD_FUNCTION_OUT);
  

  //VCD_SIGNAL_DUMPER_DUMP_FUNCTION_BY_NAME(VCD_SIGNAL_DUMPER_FUNCTIONS_DCI_DECODING, VCD_FUNCTION_IN);

#ifdef NR_PDCCH_SCHED_DEBUG
  printf("<-NR_PDCCH_PHY_PROCEDURES_LTE_UE (nr_ue_pdcch_procedures)-> Entering function nr_dci_decoding_procedure for search space %d)\n",
	 n_ss);
#endif

  dci_cnt = nr_dci_decoding_procedure(ue, proc);

#ifdef NR_PDCCH_SCHED_DEBUG
  LOG_I(PHY,"<-NR_PDCCH_PHY_PROCEDURES_LTE_UE (nr_ue_pdcch_procedures)-> Ending function nr_dci_decoding_procedure() -> dci_cnt=%u\n",dci_cnt);
#endif
  
  //VCD_SIGNAL_DUMPER_DUMP_FUNCTION_BY_NAME(VCD_SIGNAL_DUMPER_FUNCTIONS_DCI_DECODING, VCD_FUNCTION_OUT);
  //LOG_D(PHY,"[UE  %d][PUSCH] Frame %d nr_slot_rx %d PHICH RX\n",ue->Mod_id,frame_rx,nr_slot_rx);

  pdcch_vars->dci_received += dci_cnt;

  if (dci_cnt == 1)
  {
    int N_RB = 0;
    int pos = 0;
    int fsize = 0;
    int frequency_domain_assignment;
    int time_domain_assignment;
    int vrb_to_prb_mapping;
    int mcs;
    int rv;
    int system_info_indicator;
    int dci_size = ue->type0_PDCCH_CSS_config.dci_size;
    uint64_t * dci_pdu = &pdcch_vars->dci_payload[0];

    N_RB = ue->type0_PDCCH_CSS_config.num_rbs;
        // Freq domain assignment 0-16 bit
    fsize = (int)ceil( log2( (N_RB*(N_RB+1))>>1 ) );
    pos+=fsize;
    frequency_domain_assignment = (*dci_pdu>>(dci_size-pos))&((1<<fsize)-1);
    // Time domain assignment 4 bit
    pos+=4;
    time_domain_assignment = (*dci_pdu>>(dci_size-pos))&0xf;
    // VRB to PRB mapping 1 bit
    pos++;
    vrb_to_prb_mapping = (*dci_pdu>>(dci_size-pos))&0x1;
    // MCS 5bit  //bit over 32, so dci_pdu ++
    pos+=5;
    mcs = (*dci_pdu>>(dci_size-pos))&0x1f;
    // Redundancy version  2 bit
    pos+=2;
    rv = (*dci_pdu>>(dci_size-pos))&3;
    // System information indicator 1 bit
    pos++;
    system_info_indicator = (*dci_pdu>>(dci_size-pos))&0x1;

    LOG_I(MAC,"N_RB = %i\n", N_RB);
    LOG_I(MAC,"dci_size = %i\n", dci_size);
    LOG_I(MAC,"fsize = %i\n", fsize);
    LOG_I(MAC,"dci_pdu_rel15->frequency_domain_assignment.val = %i\n", frequency_domain_assignment);
    LOG_I(MAC,"dci_pdu_rel15->time_domain_assignment.val = %i\n", time_domain_assignment);
    LOG_I(MAC,"dci_pdu_rel15->vrb_to_prb_mapping.val = %i\n", vrb_to_prb_mapping);
    LOG_I(MAC,"dci_pdu_rel15->mcs = %i\n", mcs);
    LOG_I(MAC,"dci_pdu_rel15->rv = %i\n", rv);
    LOG_I(MAC,"dci_pdu_rel15->system_info_indicator = %i\n", system_info_indicator);
    
      int current_harq_pid = 0;
      NR_UE_DLSCH_t *dlsch0 = ue->dlsch_SI[0];
      dlsch0->rnti_type = _SI_RNTI_;
      dlsch0->current_harq_pid = current_harq_pid;
      dlsch0->harq_processes[current_harq_pid]->status = ACTIVE;
      dlsch0->active = 1;
      dlsch0->rnti = 65535;
      NR_DL_UE_HARQ_t *dlsch0_harq = dlsch0->harq_processes[current_harq_pid];
      if (dlsch0_harq){
        int default_abc = 1;
        int mapping_type ;
        dlsch0_harq->BWPStart = ue->type0_PDCCH_CSS_config.cset_start_rb;
        dlsch0_harq->BWPSize = ue->type0_PDCCH_CSS_config.num_rbs;
        dlsch0_harq->nb_rb = NRRIV2BW(frequency_domain_assignment, ue->type0_PDCCH_CSS_config.num_rbs);
        dlsch0_harq->start_rb = NRRIV2PRBOFFSET(frequency_domain_assignment,ue->type0_PDCCH_CSS_config.num_rbs);
        mapping_type = get_info_from_tda_tables_new(default_abc,
                                time_domain_assignment,
                                0, // 0: ue->type0_PDCCH_CSS_config.dmrs_TypeA_Position2, 1: ue->type0_PDCCH_CSS_config.dmrs_TypeA_Position2,
                                1, // normal CP
                                &dlsch0_harq->start_symbol,
                                &dlsch0_harq->nb_symbols);


        dlsch0_harq->dlDmrsSymbPos = fill_dmrs_mask_sib(ue->type0_PDCCH_CSS_config.dmrs_TypeA_Position,
                                                    dlsch0_harq->nb_symbols,
                                                    dlsch0_harq->start_symbol);
        dlsch0_harq->dmrsConfigType = NFAPI_NR_DMRS_TYPE1;
        dlsch0_harq->n_dmrs_cdm_groups = 2;
        dlsch0_harq->mcs = mcs;
        dlsch0_harq->rvidx = rv;
        dlsch0_harq->Nl = 1;
        dlsch0_harq->mcs_table = 0;
        LOG_I(PHY,"BWP %d %d, rb %d %d, mapping_type %d, dmrspos %d, symbol %d %d, mcs %d\n",
        dlsch0_harq->BWPStart,
        dlsch0_harq->BWPSize,
        dlsch0_harq->start_rb,
        dlsch0_harq->nb_rb,
        mapping_type,
        dlsch0_harq->dlDmrsSymbPos,
        dlsch0_harq->start_symbol,
        dlsch0_harq->nb_symbols,
        dlsch0_harq->mcs);
      }
  }
#if UE_TIMING_TRACE
  stop_meas(&ue->dlsch_rx_pdcch_stats);
#endif
    
  //VCD_SIGNAL_DUMPER_DUMP_FUNCTION_BY_NAME(VCD_SIGNAL_DUMPER_FUNCTIONS_UE_PDCCH_PROCEDURES, VCD_FUNCTION_OUT);
  return(dci_cnt);
}
#endif // NR_PDCCH_SCHED

int nr_ue_pdsch_procedures(PHY_VARS_NR_UE *ue, UE_nr_rxtx_proc_t *proc, int gNB_id, PDSCH_t pdsch, NR_UE_DLSCH_t *dlsch0, NR_UE_DLSCH_t *dlsch1) {

  int frame_rx = proc->frame_rx;
  int nr_slot_rx = proc->nr_slot_rx;
  int m;
  int i_mod,gNB_id_i,dual_stream_UE;
  int first_symbol_flag=0;

  if (!dlsch0)
    return 0;
  if (dlsch0->active == 0)
    return 0;

  if (!dlsch1)  {
    int harq_pid = dlsch0->current_harq_pid;
    NR_DL_UE_HARQ_t *dlsch0_harq = dlsch0->harq_processes[harq_pid];
    uint16_t BWPStart       = dlsch0_harq->BWPStart;
    uint16_t pdsch_start_rb = dlsch0_harq->start_rb;
    uint16_t pdsch_nb_rb    = dlsch0_harq->nb_rb;
    uint16_t s0             = dlsch0_harq->start_symbol;
    uint16_t s1             = dlsch0_harq->nb_symbols;
    bool is_SI              = dlsch0->rnti_type == _SI_RNTI_;

    LOG_I(PHY,"[UE %d] PDSCH type %d active in nr_slot_rx %d, harq_pid %d (%d), rb_start %d, nb_rb %d, symbol_start %d, nb_symbols %d, DMRS mask %x\n",ue->Mod_id,pdsch,nr_slot_rx,harq_pid,dlsch0_harq->status,pdsch_start_rb,pdsch_nb_rb,s0,s1,dlsch0_harq->dlDmrsSymbPos);

    for (m = s0; m < (s0 +s1); m++) {
      if (dlsch0_harq->dlDmrsSymbPos & (1 << m)) {
        for (uint8_t aatx=0; aatx<dlsch0_harq->Nl; aatx++) {//for MIMO Config: it shall loop over no_layers
          nr_pdsch_channel_estimation(ue,
                                      proc,
                                      gNB_id,
                                      is_SI,
                                      nr_slot_rx,
                                      aatx /*p*/,
                                      m,
                                      BWPStart,
                                      ue->frame_parms.first_carrier_offset+(BWPStart + pdsch_start_rb)*12,
                                      pdsch_nb_rb);
          LOG_I(PHY,"PDSCH Channel estimation gNB id %d, PDSCH antenna port %d, slot %d, symbol %d\n",0,aatx,nr_slot_rx,m);
#if 0
          ///LOG_M: the channel estimation
          int nr_frame_rx = proc->frame_rx;
          char filename[100];
          for (uint8_t aarx=0; aarx<ue->frame_parms.nb_antennas_rx; aarx++) {
            sprintf(filename,"PDSCH_CHANNEL_frame%d_slot%d_sym%d_port%d_rx%d.m", nr_frame_rx, nr_slot_rx, m, aatx,aarx);
            int **dl_ch_estimates = ue->pdsch_vars[proc->thread_id][gNB_id]->dl_ch_estimates;
            LOG_M(filename,"channel_F",&dl_ch_estimates[aatx*ue->frame_parms.nb_antennas_rx+aarx][ue->frame_parms.ofdm_symbol_size*m],ue->frame_parms.ofdm_symbol_size, 1, 1);
          }
#endif
        }
      }
    }

    uint16_t first_symbol_with_data = s0;
    uint32_t dmrs_data_re;

    if (dlsch0_harq->dmrsConfigType == NFAPI_NR_DMRS_TYPE1)
      dmrs_data_re = 12 - 6 * dlsch0_harq->n_dmrs_cdm_groups;
    else
      dmrs_data_re = 12 - 4 * dlsch0_harq->n_dmrs_cdm_groups;

    while ((dmrs_data_re == 0) && (dlsch0_harq->dlDmrsSymbPos & (1 << first_symbol_with_data))) {
      first_symbol_with_data++;
    }

    for (m = s0; m < (s1 + s0); m++) {
 
      dual_stream_UE = 0;
      gNB_id_i = gNB_id+1;
      i_mod = 0;
      if (( m==first_symbol_with_data ) && (m<4))
        first_symbol_flag = 1;
      else
        first_symbol_flag = 0;

#if UE_TIMING_TRACE
      uint8_t slot = 0;
      if(m >= ue->frame_parms.symbols_per_slot>>1)
        slot = 1;
      start_meas(&ue->dlsch_llr_stats_parallelization[proc->thread_id][slot]);
#endif
      // process DLSCH received symbols in the slot
      // symbol by symbol processing (if data/DMRS are multiplexed is checked inside the function)
      if (pdsch == PDSCH || pdsch == SI_PDSCH || pdsch == RA_PDSCH) {
        if (nr_rx_pdsch(ue,
                        proc,
                        pdsch,
                        gNB_id,
                        gNB_id_i,
                        frame_rx,
                        nr_slot_rx,
                        m,
                        first_symbol_flag,
                        dual_stream_UE,
                        i_mod,
                        harq_pid) < 0)
          {
             printf("nr_rx_pdsch < 0, m %d first_symbol_flag %d\n", m, first_symbol_flag );
             return -1;
          }
      } else AssertFatal(1==0,"Not RA_PDSCH, SI_PDSCH or PDSCH\n");

#if UE_TIMING_TRACE
      stop_meas(&ue->dlsch_llr_stats_parallelization[proc->thread_id][slot]);
#if DISABLE_LOG_X
      printf("[AbsSFN %d.%d] LLR Computation Symbol %d %5.2f \n",frame_rx,nr_slot_rx,m,ue->dlsch_llr_stats_parallelization[proc->thread_id][slot].p_time/(cpuf*1000.0));
#else
      LOG_D(PHY, "[AbsSFN %d.%d] LLR Computation Symbol %d %5.2f \n",frame_rx,nr_slot_rx,m,ue->dlsch_llr_stats_parallelization[proc->thread_id][slot].p_time/(cpuf*1000.0));
#endif
#endif

      if(first_symbol_flag) {
        proc->first_symbol_available = 1;
      }
    } // CRNTI active
  }
  return 0;
}

bool nr_ue_dlsch_procedures(PHY_VARS_NR_UE *ue,
                            UE_nr_rxtx_proc_t *proc,
                            int gNB_id,
                            PDSCH_t pdsch,
                            NR_UE_DLSCH_t *dlsch0,
                            NR_UE_DLSCH_t *dlsch1,
                            int *dlsch_errors,
                            uint8_t dlsch_parallel) {
  if (dlsch0==NULL)
    AssertFatal(0,"dlsch0 should be defined at this level \n");
  bool dec = false;
  int harq_pid = dlsch0->current_harq_pid;
  int frame_rx = proc->frame_rx;
  int nr_slot_rx = proc->nr_slot_rx;
  uint32_t ret = UINT32_MAX, ret1 = UINT32_MAX;
  NR_UE_PDSCH *pdsch_vars;
  uint16_t dmrs_len = get_num_dmrs(dlsch0->harq_processes[dlsch0->current_harq_pid]->dlDmrsSymbPos);
  // nr_downlink_indication_t dl_indication;
  // fapi_nr_rx_indication_t rx_ind;
  uint16_t number_pdus = 1;
  // params for UL time alignment procedure
  //NR_UL_TIME_ALIGNMENT_t *ul_time_alignment = &ue->ul_time_alignment[gNB_id];

  uint8_t is_cw0_active = dlsch0->harq_processes[harq_pid]->status;
  uint16_t nb_symb_sch = dlsch0->harq_processes[harq_pid]->nb_symbols;
  uint16_t start_symbol = dlsch0->harq_processes[harq_pid]->start_symbol;
  uint8_t dmrs_type = dlsch0->harq_processes[harq_pid]->dmrsConfigType;

  uint8_t nb_re_dmrs;
  if (dmrs_type==NFAPI_NR_DMRS_TYPE1) {
    nb_re_dmrs = 6*dlsch0->harq_processes[harq_pid]->n_dmrs_cdm_groups;
  }
  else {
    nb_re_dmrs = 4*dlsch0->harq_processes[harq_pid]->n_dmrs_cdm_groups;
  }

  uint8_t is_cw1_active = 0;
  if(dlsch1)
    is_cw1_active = dlsch1->harq_processes[harq_pid]->status;

  LOG_D(PHY,"AbsSubframe %d.%d Start LDPC Decoder for CW0 [harq_pid %d] ? %d \n", frame_rx%1024, nr_slot_rx, harq_pid, is_cw0_active);
  LOG_D(PHY,"AbsSubframe %d.%d Start LDPC Decoder for CW1 [harq_pid %d] ? %d \n", frame_rx%1024, nr_slot_rx, harq_pid, is_cw1_active);

  if(is_cw0_active && is_cw1_active)
    {
      dlsch0->Kmimo = 2;
      dlsch1->Kmimo = 2;
    }
  else
    {
      dlsch0->Kmimo = 1;
    }
//  if (1) {
    switch (pdsch) {
    case SI_PDSCH:
    case RA_PDSCH:
    case P_PDSCH:
    case PDSCH:
      pdsch_vars = ue->pdsch_vars[proc->thread_id][gNB_id];
      break;
    case PMCH:
    case PDSCH1:
      LOG_E(PHY,"Illegal PDSCH %d for ue_pdsch_procedures\n",pdsch);
      pdsch_vars = NULL;
      return false;
      break;
    default:
      pdsch_vars = NULL;
      return false;
      break;

    }
    if (frame_rx < *dlsch_errors)
      *dlsch_errors=0;

    // if (pdsch == RA_PDSCH) {
    //   if (ue->prach_resources[gNB_id]!=NULL)
    //     dlsch0->rnti = ue->prach_resources[gNB_id]->ra_RNTI;
    //   else {
    //     LOG_E(PHY,"[UE %d] Frame %d, nr_slot_rx %d: FATAL, prach_resources is NULL\n", ue->Mod_id, frame_rx, nr_slot_rx);
    //     //mac_xface->macphy_exit("prach_resources is NULL");
    //     return false;
    //   }
    // }

    // exit dlsch procedures as there are no active dlsch
    if (is_cw0_active != ACTIVE && is_cw1_active != ACTIVE)
      return false;

    // start ldpc decode for CW 0
    dlsch0->harq_processes[harq_pid]->G = nr_get_G(dlsch0->harq_processes[harq_pid]->nb_rb,
                                                   nb_symb_sch,
                                                   nb_re_dmrs,
                                                   dmrs_len,
                                                   dlsch0->harq_processes[harq_pid]->Qm,
                                                   dlsch0->harq_processes[harq_pid]->Nl);
#if UE_TIMING_TRACE
    start_meas(&ue->dlsch_unscrambling_stats);
#endif
    nr_dlsch_unscrambling(pdsch_vars->llr[0],
                          dlsch0->harq_processes[harq_pid]->G,
                          0,
                          ue->frame_parms.Nid_cell,
                          dlsch0->rnti);
      

#if UE_TIMING_TRACE
    stop_meas(&ue->dlsch_unscrambling_stats);
#endif

#if 0
      LOG_I(PHY," ------ start ldpc decoder for AbsSubframe %d.%d / %d  ------  \n", frame_rx, nr_slot_rx, harq_pid);
      LOG_I(PHY,"start ldpc decode for CW 0 for AbsSubframe %d.%d / %d --> nb_rb %d \n", frame_rx, nr_slot_rx, harq_pid, dlsch0->harq_processes[harq_pid]->nb_rb);
      LOG_I(PHY,"start ldpc decode for CW 0 for AbsSubframe %d.%d / %d  --> rb_alloc_even %x \n", frame_rx, nr_slot_rx, harq_pid, dlsch0->harq_processes[harq_pid]->rb_alloc_even);
      LOG_I(PHY,"start ldpc decode for CW 0 for AbsSubframe %d.%d / %d  --> Qm %d \n", frame_rx, nr_slot_rx, harq_pid, dlsch0->harq_processes[harq_pid]->Qm);
      LOG_I(PHY,"start ldpc decode for CW 0 for AbsSubframe %d.%d / %d  --> Nl %d \n", frame_rx, nr_slot_rx, harq_pid, dlsch0->harq_processes[harq_pid]->Nl);
      LOG_I(PHY,"start ldpc decode for CW 0 for AbsSubframe %d.%d / %d  --> G  %d \n", frame_rx, nr_slot_rx, harq_pid, dlsch0->harq_processes[harq_pid]->G);
      LOG_I(PHY,"start ldpc decode for CW 0 for AbsSubframe %d.%d / %d  --> Kmimo  %d \n", frame_rx, nr_slot_rx, harq_pid, dlsch0->Kmimo);
      LOG_I(PHY,"start ldpc decode for CW 0 for AbsSubframe %d.%d / %d  --> Pdcch Sym  %d \n", frame_rx, nr_slot_rx, harq_pid, ue->pdcch_vars[proc->thread_id][gNB_id]->num_pdcch_symbols);
#endif

#if UE_TIMING_TRACE
    start_meas(&ue->dlsch_decoding_stats[proc->thread_id]);
#endif

    // if( dlsch_parallel) {
    //   ret = nr_dlsch_decoding_mthread(ue,
    //                                   proc,
    //                                   gNB_id,
    //                                   pdsch_vars->llr[0],
    //                                   &ue->frame_parms,
    //                                   dlsch0,
    //                                   dlsch0->harq_processes[harq_pid],
    //                                   frame_rx,
    //                                   nb_symb_sch,
    //                                   nr_slot_rx,
    //                                   harq_pid,
    //                                   pdsch==PDSCH?1:0,
    //                                   dlsch0->harq_processes[harq_pid]->TBS>256?1:0);

    //   LOG_T(PHY,"dlsch decoding is parallelized, ret = %d\n", ret);
    // }
    // else {
      ret = nr_dlsch_decoding(ue,
                              proc,
                              gNB_id,
                              pdsch_vars->llr[0],
                              &ue->frame_parms,
                              dlsch0,
                              dlsch0->harq_processes[harq_pid],
                              frame_rx,
                              nb_symb_sch,
                              nr_slot_rx,
                              harq_pid,
                              pdsch==PDSCH?1:0,
                              dlsch0->harq_processes[harq_pid]->TBS>256?1:0);
      LOG_T(PHY,"Sequential dlsch decoding , ret = %d\n", ret);
    //}

    if(ret<dlsch0->max_ldpc_iterations+1)
      dec = true;
#if 0
    switch (pdsch) {
      
      case RA_PDSCH:
        nr_fill_dl_indication(&dl_indication, NULL, &rx_ind, proc, ue, gNB_id);
        nr_fill_rx_indication(&rx_ind, FAPI_NR_RX_PDU_TYPE_RAR, gNB_id, ue, dlsch0, NULL, number_pdus);
        ue->UE_mode[gNB_id] = RA_RESPONSE;
        break;
      case PDSCH:
        nr_fill_dl_indication(&dl_indication, NULL, &rx_ind, proc, ue, gNB_id);
        nr_fill_rx_indication(&rx_ind, FAPI_NR_RX_PDU_TYPE_DLSCH, gNB_id, ue, dlsch0, NULL, number_pdus);
        break;      
      case SI_PDSCH:
        nr_fill_dl_indication(&dl_indication, NULL, &rx_ind, proc, ue, gNB_id);
        nr_fill_rx_indication(&rx_ind, FAPI_NR_RX_PDU_TYPE_SIB, gNB_id, ue, dlsch0, NULL, number_pdus);
        break;
      default:
        break;
    }
#endif
    LOG_D(PHY, "In %s DL PDU length in bits: %d, in bytes: %d \n", __FUNCTION__, dlsch0->harq_processes[harq_pid]->TBS, dlsch0->harq_processes[harq_pid]->TBS / 8);



#if UE_TIMING_TRACE
    stop_meas(&ue->dlsch_decoding_stats[proc->thread_id]);
#if DISABLE_LOG_X
    printf(" --> Unscrambling for CW0 %5.3f\n",
           (ue->dlsch_unscrambling_stats.p_time)/(cpuf*1000.0));
    printf("AbsSubframe %d.%d --> LDPC Decoding for CW0 %5.3f\n",
           frame_rx%1024, nr_slot_rx,(ue->dlsch_decoding_stats[proc->thread_id].p_time)/(cpuf*1000.0));
#else
    LOG_I(PHY, " --> Unscrambling for CW0 %5.3f\n",
          (ue->dlsch_unscrambling_stats.p_time)/(cpuf*1000.0));
    LOG_I(PHY, "AbsSubframe %d.%d --> LDPC Decoding for CW0 %5.3f\n",
          frame_rx%1024, nr_slot_rx,(ue->dlsch_decoding_stats[proc->thread_id].p_time)/(cpuf*1000.0));
#endif

#endif
    if(is_cw1_active) {
      // start ldpc decode for CW 1
      dlsch1->harq_processes[harq_pid]->G = nr_get_G(dlsch1->harq_processes[harq_pid]->nb_rb,
                                                     nb_symb_sch,
                                                     nb_re_dmrs,
                                                     dmrs_len,
                                                     dlsch1->harq_processes[harq_pid]->Qm,
                                                     dlsch1->harq_processes[harq_pid]->Nl);
#if UE_TIMING_TRACE
      start_meas(&ue->dlsch_unscrambling_stats);
#endif
      nr_dlsch_unscrambling(pdsch_vars->llr[1],
                            dlsch1->harq_processes[harq_pid]->G,
                            0,
                            ue->frame_parms.Nid_cell,
                            dlsch1->rnti);
#if UE_TIMING_TRACE
      stop_meas(&ue->dlsch_unscrambling_stats);
#endif

#if 0
          LOG_I(PHY,"start ldpc decode for CW 1 for AbsSubframe %d.%d / %d --> nb_rb %d \n", frame_rx, nr_slot_rx, harq_pid, dlsch1->harq_processes[harq_pid]->nb_rb);
          LOG_I(PHY,"start ldpc decode for CW 1 for AbsSubframe %d.%d / %d  --> rb_alloc_even %x \n", frame_rx, nr_slot_rx, harq_pid, dlsch1->harq_processes[harq_pid]->rb_alloc_even);
          LOG_I(PHY,"start ldpc decode for CW 1 for AbsSubframe %d.%d / %d  --> Qm %d \n", frame_rx, nr_slot_rx, harq_pid, dlsch1->harq_processes[harq_pid]->Qm);
          LOG_I(PHY,"start ldpc decode for CW 1 for AbsSubframe %d.%d / %d  --> Nl %d \n", frame_rx, nr_slot_rx, harq_pid, dlsch1->harq_processes[harq_pid]->Nl);
          LOG_I(PHY,"start ldpc decode for CW 1 for AbsSubframe %d.%d / %d  --> G  %d \n", frame_rx, nr_slot_rx, harq_pid, dlsch1->harq_processes[harq_pid]->G);
          LOG_I(PHY,"start ldpc decode for CW 1 for AbsSubframe %d.%d / %d  --> Kmimo  %d \n", frame_rx, nr_slot_rx, harq_pid, dlsch1->Kmimo);
          LOG_I(PHY,"start ldpc decode for CW 1 for AbsSubframe %d.%d / %d  --> Pdcch Sym  %d \n", frame_rx, nr_slot_rx, harq_pid, ue->pdcch_vars[proc->thread_id][gNB_id]->num_pdcch_symbols);
#endif

#if UE_TIMING_TRACE
      start_meas(&ue->dlsch_decoding_stats[proc->thread_id]);
#endif

    LOG_D(PHY," ------ end ldpc decoder for AbsSubframe %d.%d ------ decoded in %d \n", frame_rx, nr_slot_rx, ret);
  }
  return dec;
}


/*!
 * \brief This is the UE synchronize thread.
 * It performs band scanning and synchonization.
 * \param arg is a pointer to a \ref PHY_VARS_NR_UE structure.
 * \returns a pointer to an int. The storage is not on the heap and must not be freed.
 */
#ifdef UE_SLOT_PARALLELISATION
#define FIFO_PRIORITY   40
void *UE_thread_slot1_dl_processing(void *arg) {

  static __thread int UE_dl_slot1_processing_retval;
  struct rx_tx_thread_data *rtd = arg;
  UE_nr_rxtx_proc_t *proc = rtd->proc;
  PHY_VARS_NR_UE    *ue   = rtd->UE;

  uint8_t pilot1;

  proc->instance_cnt_slot1_dl_processing=-1;
  proc->nr_slot_rx = proc->sub_frame_start * ue->frame_parms.slots_per_subframe;

  char threadname[256];
  sprintf(threadname,"UE_thread_slot1_dl_processing_%d", proc->sub_frame_start);

  cpu_set_t cpuset;
  CPU_ZERO(&cpuset);
  if ( (proc->sub_frame_start+1)%RX_NB_TH == 0 && threads.slot1_proc_one != -1 )
    CPU_SET(threads.slot1_proc_one, &cpuset);
  if ( RX_NB_TH > 1 && (proc->sub_frame_start+1)%RX_NB_TH == 1 && threads.slot1_proc_two != -1 )
    CPU_SET(threads.slot1_proc_two, &cpuset);
  if ( RX_NB_TH > 2 && (proc->sub_frame_start+1)%RX_NB_TH == 2 && threads.slot1_proc_three != -1 )
    CPU_SET(threads.slot1_proc_three, &cpuset);

  init_thread(900000,1000000 , FIFO_PRIORITY-1, &cpuset,
	      threadname);

  while (!oai_exit) {
    if (pthread_mutex_lock(&proc->mutex_slot1_dl_processing) != 0) {
      LOG_E( PHY, "[SCHED][UE] error locking mutex for UE slot1 dl processing\n" );
      exit_fun("nothing to add");
    }
    while (proc->instance_cnt_slot1_dl_processing < 0) {
      // most of the time, the thread is waiting here
      pthread_cond_wait( &proc->cond_slot1_dl_processing, &proc->mutex_slot1_dl_processing );
    }
    if (pthread_mutex_unlock(&proc->mutex_slot1_dl_processing) != 0) {
      LOG_E( PHY, "[SCHED][UE] error unlocking mutex for UE slot1 dl processing \n" );
      exit_fun("nothing to add");
    }

    int frame_rx            = proc->frame_rx;
    uint8_t subframe_rx         = proc->nr_slot_rx / ue->frame_parms.slots_per_subframe;
    uint8_t next_subframe_rx    = (1 + subframe_rx) % NR_NUMBER_OF_SUBFRAMES_PER_FRAME;
    uint8_t next_subframe_slot0 = next_subframe_rx * ue->frame_parms.slots_per_subframe;

    uint8_t slot1  = proc->nr_slot_rx + 1;
    uint8_t pilot0 = 0;

    //printf("AbsSubframe %d.%d execute dl slot1 processing \n", frame_rx, nr_slot_rx);

    if (ue->frame_parms.Ncp == 0) {  // normal prefix
      pilot1 = 4;
    } else { // extended prefix
      pilot1 = 3;
    }

    /**** Slot1 FE Processing ****/
#if UE_TIMING_TRACE
    start_meas(&ue->ue_front_end_per_slot_stat[proc->thread_id][1]);
#endif
    // I- start dl slot1 processing
    // do first symbol of next downlink nr_slot_rx for channel estimation
    /*
    // 1- perform FFT for pilot ofdm symbols first (ofdmSym0 next nr_slot_rx ofdmSym11)
    if (nr_subframe_select(&ue->frame_parms,next_nr_slot_rx) != SF_UL)
    {
    front_end_fft(ue,
    pilot0,
    next_subframe_slot0,
    0,
    0);
    }

    front_end_fft(ue,
    pilot1,
    slot1,
    0,
    0);
    */
    // 1- perform FFT
    for (int l=1; l<ue->frame_parms.symbols_per_slot>>1; l++)
      {
	//if( (l != pilot0) && (l != pilot1))
	{
#if UE_TIMING_TRACE
	  start_meas(&ue->ofdm_demod_stats);
#endif
	  //VCD_SIGNAL_DUMPER_DUMP_FUNCTION_BY_NAME(VCD_SIGNAL_DUMPER_FUNCTIONS_UE_SLOT_FEP, VCD_FUNCTION_IN);
	  //printf("AbsSubframe %d.%d FFT slot %d, symbol %d\n", frame_rx,nr_slot_rx,slot1,l);
	  front_end_fft(ue,
                        l,
                        slot1,
                        0,
                        0);
	  //VCD_SIGNAL_DUMPER_DUMP_FUNCTION_BY_NAME(VCD_SIGNAL_DUMPER_FUNCTIONS_UE_SLOT_FEP, VCD_FUNCTION_OUT);
#if UE_TIMING_TRACE
	  stop_meas(&ue->ofdm_demod_stats);
#endif
	}
      } // for l=1..l2

    if (nr_subframe_select(&ue->frame_parms,next_nr_slot_rx) != SF_UL)
      {
	//printf("AbsSubframe %d.%d FFT slot %d, symbol %d\n", frame_rx,nr_slot_rx,next_subframe_slot0,pilot0);
	front_end_fft(ue,
		      pilot0,
		      next_subframe_slot0,
		      0,
		      0);
      }

    // 2- perform Channel Estimation for slot1
    for (int l=1; l<ue->frame_parms.symbols_per_slot>>1; l++)
      {
	if(l == pilot1)
	  {
	    //wait until channel estimation for pilot0/slot1 is available
	    uint32_t wait = 0;
	    while(proc->chan_est_pilot0_slot1_available == 0)
	      {
		usleep(1);
		wait++;
	      }
	    //printf("[slot1 dl processing] ChanEst symbol %d slot %d wait%d\n",l,slot1,wait);
	  }
	//printf("AbsSubframe %d.%d ChanEst slot %d, symbol %d\n", frame_rx,nr_slot_rx,slot1,l);
	front_end_chanEst(ue,
			  l,
			  slot1,
			  0);
	ue_measurement_procedures(l-1,ue,proc,0,slot1,0,ue->mode);
      }
    //printf("AbsSubframe %d.%d ChanEst slot %d, symbol %d\n", frame_rx,nr_slot_rx,next_subframe_slot0,pilot0);
    front_end_chanEst(ue,
		      pilot0,
		      next_subframe_slot0,
		      0);

    if ( (nr_slot_rx == 0) && (ue->decode_MIB == 1))
      {
	ue_pbch_procedures(0,ue,proc,0);
      }

    proc->chan_est_slot1_available = 1;
    //printf("Set available slot 1channelEst to 1 AbsSubframe %d.%d \n",frame_rx,nr_slot_rx);
    //printf(" [slot1 dl processing] ==> FFT/CHanEst Done for AbsSubframe %d.%d \n", proc->frame_rx, proc->nr_slot_rx);

    //printf(" [slot1 dl processing] ==> Start LLR Comuptation slot1 for AbsSubframe %d.%d \n", proc->frame_rx, proc->nr_slot_rx);


#if UE_TIMING_TRACE
    stop_meas(&ue->ue_front_end_per_slot_stat[proc->thread_id][1]);
#if DISABLE_LOG_X
    printf("[AbsSFN %d.%d] Slot1: FFT + Channel Estimate + Pdsch Proc Slot0 %5.2f \n",frame_rx,nr_slot_rx,ue->ue_front_end_per_slot_stat[proc->thread_id][1].p_time/(cpuf*1000.0));
#else
    LOG_D(PHY, "[AbsSFN %d.%d] Slot1: FFT + Channel Estimate + Pdsch Proc Slot0 %5.2f \n",frame_rx,nr_slot_rx,ue->ue_front_end_per_slot_stat[proc->thread_id][1].p_time/(cpuf*1000.0));
#endif
#endif


    //wait until pdcch is decoded
    uint32_t wait = 0;
    while(proc->dci_slot0_available == 0)
      {
        usleep(1);
        wait++;
      }
    //printf("[slot1 dl processing] AbsSubframe %d.%d LLR Computation Start wait DCI %d\n",frame_rx,nr_slot_rx,wait);

    /**** Pdsch Procedure Slot1 ****/
    // start slot1 thread for Pdsch Procedure (slot1)
    // do procedures for C-RNTI
    //printf("AbsSubframe %d.%d Pdsch Procedure (slot1)\n",frame_rx,nr_slot_rx);


#if UE_TIMING_TRACE
    start_meas(&ue->pdsch_procedures_per_slot_stat[proc->thread_id][1]);
#endif
    // start slave thread for Pdsch Procedure (slot1)
    // do procedures for C-RNTI
    uint8_t gNB_id = 0;

    if (ue->dlsch[proc->thread_id][gNB_id][0]->active == 1) {
      //wait until first ofdm symbol is processed
      //wait = 0;
      //while(proc->first_symbol_available == 0)
      //{
      //    usleep(1);
      //    wait++;
      //}
      //printf("[slot1 dl processing] AbsSubframe %d.%d LLR Computation Start wait First Ofdm Sym %d\n",frame_rx,nr_slot_rx,wait);

      //VCD_SIGNAL_DUMPER_DUMP_FUNCTION_BY_NAME(VCD_SIGNAL_DUMPER_FUNCTIONS_PDSCH_PROC, VCD_FUNCTION_IN);
      ue_pdsch_procedures(ue,
			  proc,
			  gNB_id,
			  PDSCH,
			  ue->dlsch[proc->thread_id][gNB_id][0],
			  NULL,
			  (ue->frame_parms.symbols_per_slot>>1),
			  ue->frame_parms.symbols_per_slot-1,
			  abstraction_flag);
      LOG_D(PHY," ------ end PDSCH ChannelComp/LLR slot 0: AbsSubframe %d.%d ------  \n", frame_rx%1024, nr_slot_rx);
      LOG_D(PHY," ------ --> PDSCH Turbo Decoder slot 0/1: AbsSubframe %d.%d ------  \n", frame_rx%1024, nr_slot_rx);
    }

    // do procedures for SI-RNTI
    if ((ue->dlsch_SI[gNB_id]) && (ue->dlsch_SI[gNB_id]->active == 1)) {
      ue_pdsch_procedures(ue,
			  proc,
			  gNB_id,
			  SI_PDSCH,
			  ue->dlsch_SI[gNB_id],
			  NULL,
			  (ue->frame_parms.symbols_per_slot>>1),
			  ue->frame_parms.symbols_per_slot-1,
			  abstraction_flag);
    }

    // do procedures for P-RNTI
    if ((ue->dlsch_p[gNB_id]) && (ue->dlsch_p[gNB_id]->active == 1)) {
      ue_pdsch_procedures(ue,
			  proc,
			  gNB_id,
			  P_PDSCH,
			  ue->dlsch_p[gNB_id],
			  NULL,
			  (ue->frame_parms.symbols_per_slot>>1),
			  ue->frame_parms.symbols_per_slot-1,
			  abstraction_flag);
    }
    // do procedures for RA-RNTI
    if ((ue->dlsch_ra[gNB_id]) && (ue->dlsch_ra[gNB_id]->active == 1) && (UE_mode != PUSCH)) {
      ue_pdsch_procedures(ue,
			  proc,
			  gNB_id,
			  RA_PDSCH,
			  ue->dlsch_ra[gNB_id],
			  NULL,
			  (ue->frame_parms.symbols_per_slot>>1),
			  ue->frame_parms.symbols_per_slot-1,
			  abstraction_flag);
    }

    proc->llr_slot1_available=1;
    //printf("Set available LLR slot1 to 1 AbsSubframe %d.%d \n",frame_rx,nr_slot_rx);

#if UE_TIMING_TRACE
    stop_meas(&ue->pdsch_procedures_per_slot_stat[proc->thread_id][1]);
#if DISABLE_LOG_X
    printf("[AbsSFN %d.%d] Slot1: LLR Computation %5.2f \n",frame_rx,nr_slot_rx,ue->pdsch_procedures_per_slot_stat[proc->thread_id][1].p_time/(cpuf*1000.0));
#else
    LOG_D(PHY, "[AbsSFN %d.%d] Slot1: LLR Computation %5.2f \n",frame_rx,nr_slot_rx,ue->pdsch_procedures_per_slot_stat[proc->thread_id][1].p_time/(cpuf*1000.0));
#endif
#endif

    if (pthread_mutex_lock(&proc->mutex_slot1_dl_processing) != 0) {
      LOG_E( PHY, "[SCHED][UE] error locking mutex for UE RXTX\n" );
      exit_fun("noting to add");
    }
    proc->instance_cnt_slot1_dl_processing--;
    if (pthread_mutex_unlock(&proc->mutex_slot1_dl_processing) != 0) {
      LOG_E( PHY, "[SCHED][UE] error unlocking mutex for UE FEP Slo1\n" );
      exit_fun("noting to add");
    }
  }
  // thread finished
  free(arg);
  return &UE_dl_slot1_processing_retval;
}
#endif

#if 0
int is_ssb_in_slot(fapi_nr_config_request_t *config, int frame, int slot, NR_DL_FRAME_PARMS *fp)
{
  int mu = fp->numerology_index;
  //uint8_t half_frame_index = fp->half_frame_bit;
  //uint8_t i_ssb = fp->ssb_index;
  uint8_t Lmax = fp->Lmax;

  if (!(frame%(1<<(config->ssb_table.ssb_period-1)))){

    if(Lmax <= 8) {
      if(slot <=3 && (((config->ssb_table.ssb_mask_list[0].ssb_mask << 2*slot)&0x80000000) == 0x80000000 || ((config->ssb_table.ssb_mask_list[0].ssb_mask << (2*slot +1))&0x80000000) == 0x80000000))
      return 1;
      else return 0;
    
    }
    else if(Lmax == 64) {
      if (mu == NR_MU_3){

        if (slot>=0 && slot <= 7){
          if(((config->ssb_table.ssb_mask_list[0].ssb_mask << 2*slot)&0x80000000) == 0x80000000 || ((config->ssb_table.ssb_mask_list[0].ssb_mask << (2*slot +1))&0x80000000) == 0x80000000)
          return 1;
          else return 0;
        }
      else if (slot>=10 && slot <=17){
         if(((config->ssb_table.ssb_mask_list[0].ssb_mask << 2*(slot-2))&0x80000000) == 0x80000000 || ((config->ssb_table.ssb_mask_list[0].ssb_mask << (2*(slot-2) +1))&0x80000000) == 0x80000000)
         return 1;
         else return 0;
      }
      else if (slot>=20 && slot <=27){
         if(((config->ssb_table.ssb_mask_list[1].ssb_mask << 2*(slot-20))&0x80000000) == 0x80000000 || ((config->ssb_table.ssb_mask_list[1].ssb_mask << (2*(slot-20) +1))&0x80000000) == 0x80000000)
         return 1;
         else return 0;
      }
      else if (slot>=30 && slot <=37){
         if(((config->ssb_table.ssb_mask_list[1].ssb_mask << 2*(slot-22))&0x80000000) == 0x80000000 || ((config->ssb_table.ssb_mask_list[1].ssb_mask << (2*(slot-22) +1))&0x80000000) == 0x80000000)
         return 1;
         else return 0;
       }
      else return 0;

    }


    else if (mu == NR_MU_4) {
         AssertFatal(0==1, "not implemented for mu =  %d yet\n", mu);
    }
    else AssertFatal(0==1, "Invalid numerology index %d for the synchronization block\n", mu);
   }
   else AssertFatal(0==1, "Invalid Lmax %u for the synchronization block\n", Lmax);
  }
  else return 0;

}
#endif
#if 0
int is_pbch_in_slot(fapi_nr_config_request_t *config, int frame, int slot, NR_DL_FRAME_PARMS *fp)  {

  int ssb_slot_decoded = (fp->ssb_index>>1) + ((fp->ssb_index>>4)<<1); //slot in which the decoded SSB can be found

  if (config->ssb_table.ssb_period == 0) {  
    // check for pbch in corresponding slot each half frame
    if (fp->half_frame_bit)
      return(slot == ssb_slot_decoded || slot == ssb_slot_decoded - fp->slots_per_frame/2);
    else
      return(slot == ssb_slot_decoded || slot == ssb_slot_decoded + fp->slots_per_frame/2);
  }
  else {
    // if the current frame is supposed to contain ssb
    if (!(frame%(1<<(config->ssb_table.ssb_period-1))))
      return(slot == ssb_slot_decoded);
    else
      return 0;
  }
}
#endif
#if 0
int is_pbch_in_slot(int frame, int slot, NR_DL_FRAME_PARMS *fp)  {

  int ssb_slot_decoded = (fp->ssb_index>>1) + ((fp->ssb_index>>4)<<1); //slot in which the decoded SSB can be found

  return(slot == ssb_slot_decoded || slot == ssb_slot_decoded + fp->slots_per_frame/2);

}


int phy_procedures_nrUE_RX(PHY_VARS_NR_UE *ue,
                           UE_nr_rxtx_proc_t *proc,
                           uint8_t gNB_id,
                           uint8_t dlsch_parallel,
                           notifiedFIFO_t *txFifo
                           )
{                                         
  int frame_rx = proc->frame_rx;
  int nr_slot_rx = proc->nr_slot_rx;
  int slot_pbch = 0;
  int slot_ssb = 0;
  static int frame_mib_periond = -1;
  static int frame_mib_offset = -1;
  static int frame_mib_last = -1;
  static int slot_mib = -1;
  NR_UE_PDCCH *pdcch_vars  = ue->pdcch_vars[proc->thread_id][0];
  //fapi_nr_config_request_t *cfg = &ue->nrUE_config;

  //uint8_t nb_symb_pdcch = pdcch_vars->nb_search_space > 0 ? pdcch_vars->pdcch_config[0].coreset.duration : 0;
  uint8_t dci_cnt = 0;
  NR_DL_FRAME_PARMS *fp = &ue->frame_parms;
  
  //VCD_SIGNAL_DUMPER_DUMP_FUNCTION_BY_NAME(VCD_SIGNAL_DUMPER_FUNCTIONS_PHY_PROCEDURES_UE_RX, VCD_FUNCTION_IN);

  LOG_D(PHY," ****** start RX-Chain for Frame.Slot %d.%d (energy %d dB)******  \n",
        frame_rx%1024, nr_slot_rx,
        dB_fixed(signal_energy(ue->common_vars.common_vars_rx_data_per_thread[proc->thread_id].rxdataF[0],2048*14)));

  /*
  uint8_t next1_thread_id = proc->thread_id== (RX_NB_TH-1) ? 0:(proc->thread_id+1);
  uint8_t next2_thread_id = next1_thread_id== (RX_NB_TH-1) ? 0:(next1_thread_id+1);
  */

  //int coreset_nb_rb=0;
  //int coreset_start_rb=0;

  //if (pdcch_vars->nb_search_space > 0)
  //  get_coreset_rballoc(pdcch_vars->pdcch_config[0].coreset.frequency_domain_resource,&coreset_nb_rb,&coreset_start_rb);

  if ((slot_mib == -1) || (nr_slot_rx == slot_mib))
     slot_pbch = is_pbch_in_slot(frame_rx, nr_slot_rx, fp);
  //slot_ssb  = is_ssb_in_slot(cfg, frame_rx, nr_slot_rx, fp);

  // looking for pbch only in slot where it is supposed to be
  if (slot_pbch == 1)
  {
    if (frame_mib_periond == -1)
    {
      slot_ssb = 1;
    }
    else
    {
      if (((frame_rx % frame_mib_periond) == frame_mib_offset) && (nr_slot_rx == slot_mib))
        slot_ssb = 1;
    }
  }
  if (slot_ssb) {
    //VCD_SIGNAL_DUMPER_DUMP_FUNCTION_BY_NAME(VCD_SIGNAL_DUMPER_FUNCTIONS_UE_SLOT_FEP_PBCH, VCD_FUNCTION_IN);
    LOG_D(PHY," ------  PBCH ChannelComp/LLR: frame.slot %d.%d ------ %d.%d \n", frame_rx%1024, nr_slot_rx, ue->symbol_offset, fp->symbols_per_slot);
#if defined(__x86_64__) || defined(__i386__)
    for (int i=1; i<4; i++) {

      nr_slot_fep(ue,
                  proc,
                  (ue->symbol_offset+i)%(fp->symbols_per_slot),
                  nr_slot_rx);
#elif defined(__arm__) || defined(__aarch64__)
      pthread_mutex_lock(&sym_mutex);
      for (int i=2; i<4; i++) {
        symbol_end_flag[(ue->symbol_offset+i)%(fp->symbols_per_slot)] = 0;
      }
      pthread_cond_broadcast(&sym_cond);
      pthread_mutex_unlock(&sym_mutex);

      nr_slot_fep(ue,
                  proc,
                  (ue->symbol_offset+1)%(fp->symbols_per_slot),
                  nr_slot_rx);

      uint8_t end_flag;
      do{
        end_flag = 0;
        for (int i=2; i<4; i++) {
          if(symbol_end_flag[(ue->symbol_offset+i)%(fp->symbols_per_slot)] == 0){
            end_flag= 1; break;
          }
        }
      }while(end_flag == 1);

      for (int i=1; i<4; i++) {
#endif
#if UE_TIMING_TRACE
      start_meas(&ue->dlsch_channel_estimation_stats);
#endif
      nr_pbch_channel_estimation(ue,proc,gNB_id,nr_slot_rx,(ue->symbol_offset+i)%(fp->symbols_per_slot),i-1,(fp->ssb_index)&7,fp->half_frame_bit);
#if UE_TIMING_TRACE
      stop_meas(&ue->dlsch_channel_estimation_stats);
#endif
    }

    nr_ue_rsrp_measurements(ue, gNB_id, proc, nr_slot_rx, 0);

    if ((ue->decode_MIB == 1) && slot_pbch) {

      //LOG_I(PHY," ------  Decode MIB: frame.slot %d.%d ------  \n", frame_rx%1024, nr_slot_rx);
      int ret = nr_ue_pbch_procedures(gNB_id, ue, proc, 0);
      
      LOG_I(PHY," ------  Decode MIB: frame.slot %d.%d ret %d ------  \n", frame_rx%1024, nr_slot_rx, ret);

      if (ue->no_timing_correction==0) {
        LOG_D(PHY,"start adjust sync slot = %d no timing %d\n", nr_slot_rx, ue->no_timing_correction);
        nr_adjust_synch_ue(fp,
                           ue,
                           gNB_id,
                           frame_rx,
                           nr_slot_rx,
                           0,
                           16384);
      }
      if (ret == 0)
      {
         if (frame_mib_last == -1)
         {
           frame_mib_last = frame_rx;
           slot_mib = nr_slot_rx;
         }
         else if (frame_mib_periond == -1)
         {  
            frame_mib_periond = (frame_rx - frame_mib_last + 1024) % 1024;
            frame_mib_offset = frame_rx % frame_mib_periond;
         }
      }

      LOG_D(PHY, "Doing N0 measurements in %s\n", __FUNCTION__);
      nr_ue_rrc_measurements(ue, proc, nr_slot_rx);
      //VCD_SIGNAL_DUMPER_DUMP_FUNCTION_BY_NAME(VCD_SIGNAL_DUMPER_FUNCTIONS_UE_SLOT_FEP_PBCH, VCD_FUNCTION_OUT);
    }
  }

  if ((frame_rx%64 == 0) && (nr_slot_rx==0)) {
    LOG_I(NR_PHY,"============================================\n");
    LOG_I(NR_PHY,"Harq round stats for Downlink: %d/%d/%d/%d DLSCH errors: %d\n",ue->dl_stats[0],ue->dl_stats[1],ue->dl_stats[2],ue->dl_stats[3],ue->dl_stats[4]);
    LOG_I(NR_PHY,"============================================\n");
  }

if( (frame_rx%2 == ue->type0_PDCCH_CSS_config.sfn_c) && (nr_slot_rx == ue->type0_PDCCH_CSS_config.n_0) )
{
  double temp_power=0.0;
  double temp_power_db=0.0;
  int pdcch_sc_id= (fp->first_carrier_offset + ue->type0_PDCCH_CSS_config.cset_start_rb *12) % fp->ofdm_symbol_size;
  LOG_I(PHY, "frame_rx %d %d, search pdcch, firt sc offset %d, cset_start_rb %d, rb_offset %d, pdcch_sc_id %d, symbol %d %d\n", 
                  frame_rx, 
                  nr_slot_rx,
                  fp->first_carrier_offset,
                  ue->type0_PDCCH_CSS_config.cset_start_rb,
                  0,
                  pdcch_sc_id,
                  ue->type0_PDCCH_CSS_config.first_symbol_index,
                   ue->type0_PDCCH_CSS_config.num_symbols);
  for (int symbolId = ue->type0_PDCCH_CSS_config.first_symbol_index; symbolId < ue->type0_PDCCH_CSS_config.first_symbol_index + ue->type0_PDCCH_CSS_config.num_symbols; symbolId++)
  {
      nr_slot_fep(ue,
                proc,
                symbolId,
                nr_slot_rx);
      short *temp_IQ=&ue->common_vars.common_vars_rx_data_per_thread[proc->thread_id].rxdataF[0][fp->ofdm_symbol_size*symbolId];
      for(int pp=0; pp < ue->type0_PDCCH_CSS_config.num_rbs * 12 ; pp++)
      {
          temp_power+=temp_IQ[pdcch_sc_id*2]*temp_IQ[pdcch_sc_id*2]+temp_IQ[pdcch_sc_id*2+1]*temp_IQ[pdcch_sc_id*2+1];
          pdcch_sc_id++;
          if (pdcch_sc_id >= fp->ofdm_symbol_size)
             pdcch_sc_id = 0;
      }
      temp_power_db=log10(temp_power)*10;
          // note: this only works if RBs for PDCCH are contigous!
      LOG_I(PHY, "pdcch_channel_estimation: sf %d slot %d sym %d first_carrier_offset %d, BWPStart %d, coreset_start_rb %d %d power %lf\n",
            frame_rx%1024, nr_slot_rx,symbolId,fp->first_carrier_offset, ue->type0_PDCCH_CSS_config.cset_start_rb, ue->type0_PDCCH_CSS_config.rb_offset, ue->type0_PDCCH_CSS_config.num_rbs,temp_power_db);
      pdcch_sc_id= (fp->first_carrier_offset + ue->type0_PDCCH_CSS_config.cset_start_rb *12) % fp->ofdm_symbol_size;
      if( (ue->type0_PDCCH_CSS_config.num_rbs > 0)&&(temp_power_db>50.0) )
          nr_pdcch_channel_estimation(ue,
                                        proc,
                                        gNB_id,
                                        nr_slot_rx,
                                        symbolId,
                                        pdcch_sc_id,
                                        ue->type0_PDCCH_CSS_config.num_rbs);
  }
  dci_cnt = dci_cnt + nr_ue_pdcch_procedures(gNB_id, ue, proc, 0);
}
else
{
  return 0;
}

  //VCD_SIGNAL_DUMPER_DUMP_FUNCTION_BY_NAME(VCD_SIGNAL_DUMPER_FUNCTIONS_UE_SLOT_FEP_PDCCH, VCD_FUNCTION_OUT);

  if (dci_cnt > 0) {

    LOG_I(PHY,"[UE %d] Frame %d, nr_slot_rx %d: found %d DCIs\n", ue->Mod_id, frame_rx, nr_slot_rx, dci_cnt);

    NR_UE_DLSCH_t *dlsch = NULL;
    dlsch = ue->dlsch_SI[0];

    if (dlsch) {
      //VCD_SIGNAL_DUMPER_DUMP_FUNCTION_BY_NAME(VCD_SIGNAL_DUMPER_FUNCTIONS_UE_SLOT_FEP_PDSCH, VCD_FUNCTION_IN);
      uint8_t harq_pid = dlsch->current_harq_pid;
      NR_DL_UE_HARQ_t *dlsch0_harq = dlsch->harq_processes[harq_pid];
      uint16_t nb_symb_sch = dlsch0_harq->nb_symbols;
      uint16_t start_symb_sch = dlsch0_harq->start_symbol;

      LOG_D(PHY," ------ --> PDSCH ChannelComp/LLR Frame.slot %d.%d ------  \n", frame_rx%1024, nr_slot_rx);
      //to update from pdsch config
#if defined(__x86_64__) || defined(__i386__)
      for (uint16_t m=start_symb_sch;m<(nb_symb_sch+start_symb_sch) ; m++){
        nr_slot_fep(ue,
                    proc,
                    m,  //to be updated from higher layer
                    nr_slot_rx);
      }
#elif defined(__arm__) || defined(__aarch64__)
      if(nb_symb_sch > 0){
      pthread_mutex_lock(&sym_mutex);
      for (uint16_t m=start_symb_sch+1;m<(nb_symb_sch+start_symb_sch) ; m++){
        symbol_end_flag[m] = 0;
      }
      pthread_cond_broadcast(&sym_cond);
      pthread_mutex_unlock(&sym_mutex);

      nr_slot_fep(ue,
                  proc,
                  start_symb_sch,  //to be updated from higher layer
                  nr_slot_rx);
      do{
        end_flag = 0;
        for (uint16_t m=start_symb_sch+1;m<(nb_symb_sch+start_symb_sch) ; m++){
          if(symbol_end_flag[m] == 0){
            end_flag= 1;break;
          }
        }
      }while(end_flag == 1);
      }
#endif
      //VCD_SIGNAL_DUMPER_DUMP_FUNCTION_BY_NAME(VCD_SIGNAL_DUMPER_FUNCTIONS_UE_SLOT_FEP_PDSCH, VCD_FUNCTION_OUT);
    }
  } else {
    LOG_I(PHY,"[UE %d] Frame %d, nr_slot_rx %d: No DCIs found\n", ue->Mod_id, frame_rx, nr_slot_rx);
  }


#if 0
  // Start PUSCH processing here. It runs in parallel with PDSCH processing
  notifiedFIFO_elt_t *newElt = newNotifiedFIFO_elt(sizeof(nr_rxtx_thread_data_t), proc->nr_slot_tx,txFifo,processSlotTX);
  nr_rxtx_thread_data_t *curMsg=(nr_rxtx_thread_data_t *)NotifiedFifoData(newElt);
  curMsg->proc = *proc;
  curMsg->UE = ue;
  curMsg->ue_sched_mode = ONLY_PUSCH;
  pushTpool(&(get_nrUE_params()->Tpool), newElt);
#endif
#if UE_TIMING_TRACE
  start_meas(&ue->generic_stat);
#endif
  // do procedures for C-RNTI
#if 0  
  int ret_pdsch = 0;
  if (ue->dlsch[proc->thread_id][gNB_id][0]->active == 1) {

    //VCD_SIGNAL_DUMPER_DUMP_FUNCTION_BY_NAME(VCD_SIGNAL_DUMPER_FUNCTIONS_PDSCH_PROC_C, VCD_FUNCTION_IN);
    ret_pdsch = nr_ue_pdsch_procedures(ue,
                                       proc,
                                       gNB_id,
                                       PDSCH,
                                       ue->dlsch[proc->thread_id][gNB_id][0],
                                       NULL);

    nr_ue_measurement_procedures(2, ue, proc, gNB_id, nr_slot_rx);
    //VCD_SIGNAL_DUMPER_DUMP_FUNCTION_BY_NAME(VCD_SIGNAL_DUMPER_FUNCTIONS_PDSCH_PROC_C, VCD_FUNCTION_OUT);
  }
#endif
  // do procedures for SI-RNTI
  if ((ue->dlsch_SI[gNB_id]) && (ue->dlsch_SI[gNB_id]->active == 1)) {
    //VCD_SIGNAL_DUMPER_DUMP_FUNCTION_BY_NAME(VCD_SIGNAL_DUMPER_FUNCTIONS_PDSCH_PROC_SI, VCD_FUNCTION_IN);
    nr_ue_pdsch_procedures(ue,
                           proc,
                           gNB_id,
                           SI_PDSCH,
                           ue->dlsch_SI[gNB_id],
                           NULL);
    
    nr_ue_dlsch_procedures(ue,
                           proc,
                           gNB_id,
                           SI_PDSCH,
                           ue->dlsch_SI[gNB_id],
                           NULL,
                           &ue->dlsch_SI_errors[gNB_id],
                           dlsch_parallel);

    // deactivate dlsch once dlsch proc is done
    ue->dlsch_SI[gNB_id]->active = 0;

    //VCD_SIGNAL_DUMPER_DUMP_FUNCTION_BY_NAME(VCD_SIGNAL_DUMPER_FUNCTIONS_PDSCH_PROC_SI, VCD_FUNCTION_OUT);
  }

#if UE_TIMING_TRACE
start_meas(&ue->generic_stat);
#endif

  // duplicate harq structure
/*
  uint8_t          current_harq_pid        = ue->dlsch[proc->thread_id][gNB_id][0]->current_harq_pid;
  NR_DL_UE_HARQ_t *current_harq_processes = ue->dlsch[proc->thread_id][gNB_id][0]->harq_processes[current_harq_pid];
  NR_DL_UE_HARQ_t *harq_processes_dest    = ue->dlsch[next1_thread_id][gNB_id][0]->harq_processes[current_harq_pid];
  NR_DL_UE_HARQ_t *harq_processes_dest1    = ue->dlsch[next2_thread_id][gNB_id][0]->harq_processes[current_harq_pid];
  */
  /*nr_harq_status_t *current_harq_ack = &ue->dlsch[proc->thread_id][gNB_id][0]->harq_ack[nr_slot_rx];
  nr_harq_status_t *harq_ack_dest    = &ue->dlsch[next1_thread_id][gNB_id][0]->harq_ack[nr_slot_rx];
  nr_harq_status_t *harq_ack_dest1    = &ue->dlsch[next2_thread_id][gNB_id][0]->harq_ack[nr_slot_rx];
*/

  //copy_harq_proc_struct(harq_processes_dest, current_harq_processes);
//copy_ack_struct(harq_ack_dest, current_harq_ack);

//copy_harq_proc_struct(harq_processes_dest1, current_harq_processes);
//copy_ack_struct(harq_ack_dest1, current_harq_ack);

if (nr_slot_rx==9) {
  if (frame_rx % 10 == 0) {
    if ((ue->dlsch_received[gNB_id] - ue->dlsch_received_last[gNB_id]) != 0)
      ue->dlsch_fer[gNB_id] = (100*(ue->dlsch_errors[gNB_id] - ue->dlsch_errors_last[gNB_id]))/(ue->dlsch_received[gNB_id] - ue->dlsch_received_last[gNB_id]);

    ue->dlsch_errors_last[gNB_id] = ue->dlsch_errors[gNB_id];
    ue->dlsch_received_last[gNB_id] = ue->dlsch_received[gNB_id];
  }


  ue->bitrate[gNB_id] = (ue->total_TBS[gNB_id] - ue->total_TBS_last[gNB_id])*100;
  ue->total_TBS_last[gNB_id] = ue->total_TBS[gNB_id];
  LOG_D(PHY,"[UE %d] Calculating bitrate Frame %d: total_TBS = %d, total_TBS_last = %d, bitrate %f kbits\n",
	ue->Mod_id,frame_rx,ue->total_TBS[gNB_id],
	ue->total_TBS_last[gNB_id],(float) ue->bitrate[gNB_id]/1000.0);

#if UE_AUTOTEST_TRACE
  if ((frame_rx % 100 == 0)) {
    LOG_I(PHY,"[UE  %d] AUTOTEST Metric : UE_DLSCH_BITRATE = %5.2f kbps (frame = %d) \n", ue->Mod_id, (float) ue->bitrate[gNB_id]/1000.0, frame_rx);
  }
#endif

 }

#if UE_TIMING_TRACE
stop_meas(&ue->generic_stat);
printf("after tubo until end of Rx %5.2f \n",ue->generic_stat.p_time/(cpuf*1000.0));
#endif

#ifdef EMOS
phy_procedures_emos_UE_RX(ue,slot,gNB_id);
#endif


//VCD_SIGNAL_DUMPER_DUMP_FUNCTION_BY_NAME(VCD_SIGNAL_DUMPER_FUNCTIONS_PHY_PROCEDURES_UE_RX, VCD_FUNCTION_OUT);

#if UE_TIMING_TRACE
stop_meas(&ue->phy_proc_rx[proc->thread_id]);
#if DISABLE_LOG_X
printf("------FULL RX PROC [SFN %d]: %5.2f ------\n",nr_slot_rx,ue->phy_proc_rx[proc->thread_id].p_time/(cpuf*1000.0));
#else
LOG_D(PHY, "------FULL RX PROC [SFN %d]: %5.2f ------\n",nr_slot_rx,ue->phy_proc_rx[proc->thread_id].p_time/(cpuf*1000.0));
#endif
#endif

//#endif //pdsch

LOG_D(PHY," ****** end RX-Chain  for AbsSubframe %d.%d ******  \n", frame_rx%1024, nr_slot_rx);
return (0);
}
#endif