/*
    OpenXG MIB/SIB1 asn1 decoder
*/

#include "sib1_asn1_decoder.h"


int decode_sib1(uint8_t* msg_arr, uint msg_len)
{
  NR_BCCH_DL_SCH_Message_t *sib1_msg = NULL;
  asn_dec_rval_t dec_rval = uper_decode_complete(NULL,
                                                  &asn_DEF_NR_BCCH_DL_SCH_Message,
                                                  (void **)&sib1_msg,
                                                  (const void *)msg_arr,
                                                  msg_len);

  if ((dec_rval.code != RC_OK) || (dec_rval.consumed == 0)) {
    printf("decode error\n");
  }
  else {
    printf("input deocde siez %d, decoded %ld ok\n",msg_len,dec_rval.consumed);
    //xer_fprint(stdout, &asn_DEF_NR_BCCH_DL_SCH_Message ,sib1_msg);
    struct NR_TDD_UL_DL_ConfigCommon *tdd = sib1_msg->message.choice.c1->choice.systemInformationBlockType1->servingCellConfigCommon->tdd_UL_DL_ConfigurationCommon;
    printf("SIB1 TDD Config NR_SubcarrierSpacing is %ld \n",tdd->referenceSubcarrierSpacing);
    printf("SIB1 TDD Config dl_UL_TransmissionPeriodicity is %ld \n",tdd->pattern1.dl_UL_TransmissionPeriodicity);
    printf("SIB1 TDD Config nrofDownlinkSlots is %ld \n",tdd->pattern1.nrofDownlinkSlots);
    printf("SIB1 TDD Config nrofDownlinkSymbols is %ld \n",tdd->pattern1.nrofDownlinkSymbols);
    printf("SIB1 TDD Config nrofUplinkSlots is %ld \n",tdd->pattern1.nrofUplinkSlots);
    printf("SIB1 TDD Config nrofUplinkSymbols is %ld \n",tdd->pattern1.nrofUplinkSymbols);
  }

  return dec_rval.consumed;
}
