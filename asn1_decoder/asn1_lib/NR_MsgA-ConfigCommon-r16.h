/*
 * Generated by asn1c-0.9.29 (http://lionet.info/asn1c)
 * From ASN.1 module "NR-RRC-Definitions"
 * 	found in "/home/witcomm/work/wf/1/openair2/RRC/NR/MESSAGES/asn1c/ASN1_files/nr-rrc-16.4.1.asn1"
 * 	`asn1c -pdu=all -fcompound-names -gen-PER -no-gen-OER -no-gen-example -findirect-choice -D /home/witcomm/work/wf/1/cmake_targets/ran_build/build/CMakeFiles/RRC_NR_Rel16`
 */

#ifndef	_NR_MsgA_ConfigCommon_r16_H_
#define	_NR_MsgA_ConfigCommon_r16_H_


#include <asn_application.h>

/* Including external dependencies */
#include "NR_RACH-ConfigCommonTwoStepRA-r16.h"
#include <constr_SEQUENCE.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Forward declarations */
struct NR_MsgA_PUSCH_Config_r16;

/* NR_MsgA-ConfigCommon-r16 */
typedef struct NR_MsgA_ConfigCommon_r16 {
	NR_RACH_ConfigCommonTwoStepRA_r16_t	 rach_ConfigCommonTwoStepRA_r16;
	struct NR_MsgA_PUSCH_Config_r16	*msgA_PUSCH_Config_r16;	/* OPTIONAL */
	
	/* Context for parsing across buffer boundaries */
	asn_struct_ctx_t _asn_ctx;
} NR_MsgA_ConfigCommon_r16_t;

/* Implementation */
extern asn_TYPE_descriptor_t asn_DEF_NR_MsgA_ConfigCommon_r16;
extern asn_SEQUENCE_specifics_t asn_SPC_NR_MsgA_ConfigCommon_r16_specs_1;
extern asn_TYPE_member_t asn_MBR_NR_MsgA_ConfigCommon_r16_1[2];

#ifdef __cplusplus
}
#endif

/* Referred external types */
#include "NR_MsgA-PUSCH-Config-r16.h"

#endif	/* _NR_MsgA_ConfigCommon_r16_H_ */
#include <asn_internal.h>
