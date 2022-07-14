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


#define _GNU_SOURCE             /* See feature_test_macros(7) */
#include <sched.h>

//#include "T.h"
#include "assertions.h"
#include "PHY/types.h"
#include "PHY/defs_nr_UE.h"
#include "SCHED_NR_UE/defs.h"
//#include "common/ran_context.h"
//#include "common/config/config_userapi.h"
//#include "common/utils/threadPool/thread-pool.h"
//#include "common/utils/load_module_shlib.h"
//#undef FRAME_LENGTH_COMPLEX_SAMPLES //there are two conflicting definitions, so we better make sure we don't use it at all
//#include "common/utils/nr/nr_common.h"

//#include "../../ARCH/COMMON/common_lib.h"
//#include "../../ARCH/ETHERNET/USERSPACE/LIB/if_defs.h"

//#undef FRAME_LENGTH_COMPLEX_SAMPLES //there are two conflicting definitions, so we better make sure we don't use it at all
#include "openair1/PHY/MODULATION/nr_modulation.h"
#include "PHY/phy_vars_nr_ue.h"
#include "PHY/NR_UE_TRANSPORT/nr_transport_proto_ue.h"
//#include "SCHED/sched_common_vars.h"
//#include "PHY/MODULATION/modulation_vars.h"
//#include "PHY/NR_TRANSPORT/nr_dlsch.h"
//#include "../../SIMU/USER/init_lte.h"
//#include "PHY/thread_NR_UE.h"
// #include "LAYER2/MAC/mac_vars.h"
// #include "RRC/LTE/rrc_vars.h"
// #include "PHY_INTERFACE/phy_interface_vars.h"
// #include "openair1/SIMULATION/TOOLS/sim.h"

#ifdef SMBV
#include "PHY/TOOLS/smbv.h"
unsigned short config_frames[4] = {2,9,11,13};
#endif
//#include "common/utils/LOG/log.h"
//#include "common/utils/LOG/vcd_signal_dumper.h"

//#include "UTIL/OPT/opt.h"
//#include "enb_config.h"
//#include "PHY/TOOLS/time_meas.h"

// #ifndef OPENAIR2
//   #include "UTIL/OTG/otg_vars.h"
// #endif

//#include "intertask_interface.h"

#include "PHY/INIT/phy_init.h"

// #include <openair2/RRC/NR_UE/rrc_proto.h>
// #include <openair2/LAYER2/NR_MAC_UE/mac_defs.h>
// #include <openair2/LAYER2/NR_MAC_UE/mac_proto.h>
// #include <openair2/NR_UE_PHY_INTERFACE/NR_IF_Module.h>
// #include <openair1/SCHED_NR_UE/fapi_nr_ue_l1.h>

/* Callbacks, globals and object handlers */

//#include "stats.h"
// current status is that every UE has a DL scope for a SINGLE eNB (eNB_id=0)
//#include "PHY/TOOLS/phy_scope_interface.h"
//#include "PHY/TOOLS/nr_phy_scope.h"
//#include <executables/nr-uesoftmodem.h>
//#include "executables/softmodem-common.h"
//#include "executables/thread-common.h"

//#include "nr_nas_msg_sim.h"

//extern const char *duplex_mode[];
//THREAD_STRUCT thread_struct;
//nrUE_params_t nrUE_params;

// Thread variables
//pthread_cond_t nfapi_sync_cond;
//pthread_mutex_t nfapi_sync_mutex;
int nfapi_sync_var=-1; //!< protected by mutex \ref nfapi_sync_mutex
uint16_t sf_ahead=6; //??? value ???
//pthread_cond_t sync_cond;
//pthread_mutex_t sync_mutex;
int sync_var=-1; //!< protected by mutex \ref sync_mutex.
int config_sync_var=-1;



//RAN_CONTEXT_t RC;
volatile int             oai_exit = 0;


extern int16_t  nr_dlsch_demod_shift;
//static int      tx_max_power[MAX_NUM_CCs] = {0};

//int      single_thread_flag = 1;
int                 tddflag = 0;
//int                 vcdflag = 0;

double          rx_gain_off = 0.0;
//char             *usrp_args = NULL;
//char             *yunsdr_args = NULL;
char       *rrc_config_path = NULL;
int               dumpframe = 0;

uint64_t        downlink_frequency[MAX_NUM_CCs][4];
int32_t         uplink_frequency_offset[MAX_NUM_CCs][4];
int             rx_input_level_dBm;

// #if MAX_NUM_CCs == 1
// rx_gain_t                rx_gain_mode[MAX_NUM_CCs][4] = {{max_gain,max_gain,max_gain,max_gain}};
// double tx_gain[MAX_NUM_CCs][4] = {{20,0,0,0}};
// double rx_gain[MAX_NUM_CCs][4] = {{110,0,0,0}};
// #else
// rx_gain_t                rx_gain_mode[MAX_NUM_CCs][4] = {{max_gain,max_gain,max_gain,max_gain},{max_gain,max_gain,max_gain,max_gain}};
// double tx_gain[MAX_NUM_CCs][4] = {{20,0,0,0},{20,0,0,0}};
// double rx_gain[MAX_NUM_CCs][4] = {{110,0,0,0},{20,0,0,0}};
// #endif

// UE and OAI config variables

//openair0_config_t openair0_cfg[MAX_CARDS];
int16_t           node_synch_ref[MAX_NUM_CCs];
int               otg_enabled;
double            cpuf;


int          chain_offset = 0;
int           card_offset = 0;
uint64_t num_missed_slots = 0; // counter for the number of missed slots
int     transmission_mode = 1;
int            numerology = 0;
int           oaisim_flag = 0;
int            emulate_rf = 0;
uint32_t       N_RB_DL    = 106;
char         uecap_xer_in = 0;
char         uecap_xer[1024];

/* see file openair2/LAYER2/MAC/main.c for why abstraction_flag is needed
 * this is very hackish - find a proper solution
 */
uint8_t abstraction_flag=0;
#if 0
/*---------------------BMC: timespec helpers -----------------------------*/

struct timespec min_diff_time = { .tv_sec = 0, .tv_nsec = 0 };
struct timespec max_diff_time = { .tv_sec = 0, .tv_nsec = 0 };

struct timespec clock_difftime(struct timespec start, struct timespec end) {
  struct timespec temp;

  if ((end.tv_nsec-start.tv_nsec)<0) {
    temp.tv_sec = end.tv_sec-start.tv_sec-1;
    temp.tv_nsec = 1000000000+end.tv_nsec-start.tv_nsec;
  } else {
    temp.tv_sec = end.tv_sec-start.tv_sec;
    temp.tv_nsec = end.tv_nsec-start.tv_nsec;
  }

  return temp;
}

void print_difftimes(void) {
  LOG_I(HW,"difftimes min = %lu ns ; max = %lu ns\n", min_diff_time.tv_nsec, max_diff_time.tv_nsec);
}


void exit_function(const char *file, const char *function, const int line, const char *s) {
  int CC_id;

  if (s != NULL) {
    printf("%s:%d %s() Exiting OAI softmodem: %s\n",file,line, function, s);
  }

  //close_log_mem();
  oai_exit = 1;

  // if (PHY_vars_UE_g && PHY_vars_UE_g[0]) {
  //   for(CC_id=0; CC_id<MAX_NUM_CCs; CC_id++) {
  //     if (PHY_vars_UE_g[0][CC_id] && PHY_vars_UE_g[0][CC_id]->rfdevice.trx_end_func)
  //       PHY_vars_UE_g[0][CC_id]->rfdevice.trx_end_func(&PHY_vars_UE_g[0][CC_id]->rfdevice);
  //   }
  // }

  sleep(1); //allow lte-softmodem threads to exit first
  exit(1);
}
#endif
// uint64_t get_nrUE_optmask(void) {
//   return nrUE_params.optmask;
// }

// uint64_t set_nrUE_optmask(uint64_t bitmask) {
//   nrUE_params.optmask = nrUE_params.optmask | bitmask;
//   return nrUE_params.optmask;
// }

// nrUE_params_t *get_nrUE_params(void) {
//   return &nrUE_params;
// }
#if 0
/* initialie thread pools used for NRUE processing paralleliation */ 
void init_tpools(uint8_t nun_dlsch_threads) {
  char *params = NULL;
  if (IS_SOFTMODEM_RFSIM) {
    params = calloc(1,2);
    memcpy(params,"N",1);
  }
  else {
    params = calloc(1,(NR_RX_NB_TH*NR_NB_TH_SLOT*3)+1);
    for (int i=0; i<NR_RX_NB_TH*NR_NB_TH_SLOT; i++) {
      memcpy(params+(i*3),"-1,",3);
    }
  }
  initTpool(params, &(nrUE_params.Tpool), false);
  free(params);
  init_dlsch_tpool( nun_dlsch_threads);
}
#endif
// static void get_options(void) {

//   //nrUE_params.ofdm_offset_divisor = 8;
//   paramdef_t cmdline_params[] =CMDLINE_NRUEPARAMS_DESC ;
//   int numparams = sizeof(cmdline_params)/sizeof(paramdef_t);
//   config_process_cmdline( cmdline_params,numparams,NULL);



//   // if (vcdflag > 0)
//   //   ouput_vcd = 1;

//   if ( !(CONFIG_ISFLAGSET(CONFIG_ABORT))  && (!(CONFIG_ISFLAGSET(CONFIG_NOOOPT))) ) {
//     // Here the configuration file is the XER encoded UE capabilities
//     // Read it in and store in asn1c data structures
//     sprintf(uecap_xer,"%stargets/PROJECTS/GENERIC-LTE-EPC/CONF/UE_config.xml",getenv("OPENAIR_HOME"));
//     printf("%s\n",uecap_xer);
//     uecap_xer_in=1;
//   } /* UE with config file  */
// }

// set PHY vars from command line
void set_options(int CC_id, PHY_VARS_NR_UE *UE){
  NR_DL_FRAME_PARMS *fp       = &UE->frame_parms;
  // paramdef_t cmdline_params[] = CMDLINE_NRUE_PHYPARAMS_DESC ;
  // int numparams               = sizeof(cmdline_params)/sizeof(paramdef_t);

  // UE->mode = normal_txrx;

  // config_get(cmdline_params,numparams,NULL);

  // int pindex = config_paramidx_fromname(cmdline_params,numparams, CALIBRX_OPT);
  // if ( (cmdline_params[pindex].paramflags &  PARAMFLAG_PARAMSET) != 0) UE->mode = rx_calib_ue;
  
  // pindex = config_paramidx_fromname(cmdline_params,numparams, CALIBRXMED_OPT);
  // if ( (cmdline_params[pindex].paramflags &  PARAMFLAG_PARAMSET) != 0) UE->mode = rx_calib_ue_med;

  // pindex = config_paramidx_fromname(cmdline_params,numparams, CALIBRXBYP_OPT);              
  // if ( (cmdline_params[pindex].paramflags &  PARAMFLAG_PARAMSET) != 0) UE->mode = rx_calib_ue_byp;

  // pindex = config_paramidx_fromname(cmdline_params,numparams, DBGPRACH_OPT); 
  // if (cmdline_params[pindex].uptr)
  //   if ( *(cmdline_params[pindex].uptr) > 0) UE->mode = debug_prach;

  // pindex = config_paramidx_fromname(cmdline_params,numparams,NOL2CONNECT_OPT ); 
  // if (cmdline_params[pindex].uptr)
  //   if ( *(cmdline_params[pindex].uptr) > 0)  UE->mode = no_L2_connect;

  // pindex = config_paramidx_fromname(cmdline_params,numparams,CALIBPRACH_OPT );
  // if (cmdline_params[pindex].uptr)
  //   if ( *(cmdline_params[pindex].uptr) > 0) UE->mode = calib_prach_tx;

  // pindex = config_paramidx_fromname(cmdline_params,numparams,DUMPFRAME_OPT );
  // if ((cmdline_params[pindex].paramflags & PARAMFLAG_PARAMSET) != 0)
  //   UE->mode = rx_dump_frame;

  // // Init power variables
  // tx_max_power[CC_id] = tx_max_power[0];
  // rx_gain[0][CC_id]   = rx_gain[0][0];
  // tx_gain[0][CC_id]   = tx_gain[0][0];

  // // Set UE variables
  // UE->rx_total_gain_dB     = (int)rx_gain[CC_id][0] + rx_gain_off;
  // UE->tx_total_gain_dB     = (int)tx_gain[CC_id][0];
  // UE->tx_power_max_dBm     = tx_max_power[CC_id];
  // UE->rf_map.card          = card_offset;
  // UE->rf_map.chain         = CC_id + chain_offset;

  //LOG_I(PHY,"Set UE mode %d, UE_fo_compensation %d, UE_scan_carrier %d, UE_no_timing_correction %d \n, do_prb_interpolation %d\n",
  //	   UE->mode, UE->UE_fo_compensation, UE->UE_scan_carrier, UE->no_timing_correction, UE->prb_interpolation);

  // Set FP variables

  downlink_frequency[CC_id][0] = 3619200000;
  uplink_frequency_offset[CC_id][0] = 0;
  fp->numerology_index = 1;
  fp->nb_antennas_rx = 1;
  fp->nb_antennas_tx = 1;
  fp->N_RB_DL = 106;
  fp->ssb_start_subcarrier = 516;
  tddflag = 1;
  if (tddflag){
    fp->frame_type = TDD;
    LOG_I(PHY, "Set UE frame_type %d\n", fp->frame_type);
  }

  LOG_I(PHY, "Set UE nb_rx_antenna %d, nb_tx_antenna %d, threequarter_fs %d, ssb_start_subcarrier %d\n", fp->nb_antennas_rx, fp->nb_antennas_tx, fp->threequarter_fs, fp->ssb_start_subcarrier);

  fp->ofdm_offset_divisor = 8;

}

void init_nr_ue_vars(PHY_VARS_NR_UE *ue,
                     uint8_t UE_id,
                     uint8_t abstraction_flag)
{

  int nb_connected_gNB = 1, gNB_id;

  ue->Mod_id      = UE_id;
  ue->mac_enabled = 1;
  //ue->if_inst     = nr_ue_if_module_init(0);
  ue->dci_thres   = 0;

  // Setting UE mode to NOT_SYNCHED by default
  for (gNB_id = 0; gNB_id < nb_connected_gNB; gNB_id++){
    ue->UE_mode[gNB_id] = NOT_SYNCHED;
   }

  // initialize all signal buffers
  init_nr_ue_signal(ue, nb_connected_gNB, abstraction_flag);

  // intialize transport
  init_nr_ue_transport(ue, abstraction_flag);

  // init N_TA offset
  init_N_TA_offset(ue);
}

int main( int argc, char **argv ) {
  //uint8_t beta_ACK=0,beta_RI=0,beta_CQI=2;
  PHY_VARS_NR_UE *UE[MAX_NUM_CCs];

  LOG_I(HW, "Version: %s\n", PACKAGE_VERSION);

  PHY_vars_UE_g = malloc(sizeof(PHY_VARS_NR_UE **));
  PHY_vars_UE_g[0] = malloc(sizeof(PHY_VARS_NR_UE *)*MAX_NUM_CCs);

  for (int CC_id=0; CC_id<MAX_NUM_CCs; CC_id++) {

    PHY_vars_UE_g[0][CC_id] = (PHY_VARS_NR_UE *)malloc(sizeof(PHY_VARS_NR_UE));
    UE[CC_id] = PHY_vars_UE_g[0][CC_id];
    memset(UE[CC_id],0,sizeof(PHY_VARS_NR_UE));

    set_options(CC_id, UE[CC_id]);

    uint16_t nr_band = get_band(downlink_frequency[CC_id][0],uplink_frequency_offset[CC_id][0]);

    nr_init_frame_parms_ue_sa(&UE[CC_id]->frame_parms,
                                downlink_frequency[CC_id][0],
                                uplink_frequency_offset[CC_id][0],
                                UE[CC_id]->frame_parms.numerology_index,
                                nr_band);


    init_symbol_rotation(&UE[CC_id]->frame_parms);
    init_timeshift_rotation(&UE[CC_id]->frame_parms);
    init_nr_ue_vars(UE[CC_id], 0, abstraction_flag);

  }

  UE_nr_rxtx_proc_t proc;
  proc.thread_id = 0;

   FILE *fd = fopen("rxsig_2frame.dat","r");                       
              
  if (fd != NULL) {
          printf("opening rxsig_2frame file \n");
    int readlen = fread(&UE[0]->common_vars.rxdata[0][0],  
                       sizeof(int32_t),  
                       UE[0]->frame_parms.samples_per_frame * 2, fd);
    if (readlen != UE[0]->frame_parms.samples_per_frame * 2) 
            printf("error reading from file, len %d, %d\n", readlen, UE[0]->frame_parms.samples_per_frame * 2);
    fclose(fd );
    }

  nr_initial_sync((UE_nr_rxtx_proc_t *)&proc,
                    UE[0],
                    2, 1,1);
                    
  return 0;
}
