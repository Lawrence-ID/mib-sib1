/*
 * Generated by asn1c-0.9.29 (http://lionet.info/asn1c)
 * From ASN.1 module "NR-RRC-Definitions"
 * 	found in "/home/witcomm/work/wf/1/openair2/RRC/NR/MESSAGES/asn1c/ASN1_files/nr-rrc-16.4.1.asn1"
 * 	`asn1c -pdu=all -fcompound-names -gen-PER -no-gen-OER -no-gen-example -findirect-choice -D /home/witcomm/work/wf/1/cmake_targets/ran_build/build/CMakeFiles/RRC_NR_Rel16`
 */

#ifndef	_NR_MCC_H_
#define	_NR_MCC_H_


#include <asn_application.h>

/* Including external dependencies */
#include "NR_MCC-MNC-Digit.h"
#include <asn_SEQUENCE_OF.h>
#include <constr_SEQUENCE_OF.h>

#ifdef __cplusplus
extern "C" {
#endif

/* NR_MCC */
typedef struct NR_MCC {
	A_SEQUENCE_OF(NR_MCC_MNC_Digit_t) list;
	
	/* Context for parsing across buffer boundaries */
	asn_struct_ctx_t _asn_ctx;
} NR_MCC_t;

/* Implementation */
extern asn_TYPE_descriptor_t asn_DEF_NR_MCC;
extern asn_SET_OF_specifics_t asn_SPC_NR_MCC_specs_1;
extern asn_TYPE_member_t asn_MBR_NR_MCC_1[1];
extern asn_per_constraints_t asn_PER_type_NR_MCC_constr_1;

#ifdef __cplusplus
}
#endif

#endif	/* _NR_MCC_H_ */
#include <asn_internal.h>
