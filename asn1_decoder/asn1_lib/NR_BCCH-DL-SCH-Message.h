/*
 * Generated by asn1c-0.9.29 (http://lionet.info/asn1c)
 * From ASN.1 module "NR-RRC-Definitions"
 * 	found in "/home/witcomm/work/wf/1/openair2/RRC/NR/MESSAGES/asn1c/ASN1_files/nr-rrc-16.4.1.asn1"
 * 	`asn1c -pdu=all -fcompound-names -gen-PER -no-gen-OER -no-gen-example -findirect-choice -D /home/witcomm/work/wf/1/cmake_targets/ran_build/build/CMakeFiles/RRC_NR_Rel16`
 */

#ifndef	_NR_BCCH_DL_SCH_Message_H_
#define	_NR_BCCH_DL_SCH_Message_H_


#include <asn_application.h>

/* Including external dependencies */
#include "NR_BCCH-DL-SCH-MessageType.h"
#include <constr_SEQUENCE.h>

#ifdef __cplusplus
extern "C" {
#endif

/* NR_BCCH-DL-SCH-Message */
typedef struct NR_BCCH_DL_SCH_Message {
	NR_BCCH_DL_SCH_MessageType_t	 message;
	
	/* Context for parsing across buffer boundaries */
	asn_struct_ctx_t _asn_ctx;
} NR_BCCH_DL_SCH_Message_t;

/* Implementation */
extern asn_TYPE_descriptor_t asn_DEF_NR_BCCH_DL_SCH_Message;

#ifdef __cplusplus
}
#endif

#endif	/* _NR_BCCH_DL_SCH_Message_H_ */
#include <asn_internal.h>
