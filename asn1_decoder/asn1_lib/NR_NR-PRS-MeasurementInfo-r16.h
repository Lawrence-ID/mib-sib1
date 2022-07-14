/*
 * Generated by asn1c-0.9.29 (http://lionet.info/asn1c)
 * From ASN.1 module "NR-RRC-Definitions"
 * 	found in "/home/witcomm/work/wf/1/openair2/RRC/NR/MESSAGES/asn1c/ASN1_files/nr-rrc-16.4.1.asn1"
 * 	`asn1c -pdu=all -fcompound-names -gen-PER -no-gen-OER -no-gen-example -findirect-choice -D /home/witcomm/work/wf/1/cmake_targets/ran_build/build/CMakeFiles/RRC_NR_Rel16`
 */

#ifndef	_NR_NR_PRS_MeasurementInfo_r16_H_
#define	_NR_NR_PRS_MeasurementInfo_r16_H_


#include <asn_application.h>

/* Including external dependencies */
#include "NR_ARFCN-ValueNR.h"
#include <NativeEnumerated.h>
#include <NativeInteger.h>
#include <constr_CHOICE.h>
#include <constr_SEQUENCE.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Dependencies */
typedef enum NR_NR_PRS_MeasurementInfo_r16__nr_MeasPRS_RepetitionAndOffset_r16_PR {
	NR_NR_PRS_MeasurementInfo_r16__nr_MeasPRS_RepetitionAndOffset_r16_PR_NOTHING,	/* No components present */
	NR_NR_PRS_MeasurementInfo_r16__nr_MeasPRS_RepetitionAndOffset_r16_PR_ms20_r16,
	NR_NR_PRS_MeasurementInfo_r16__nr_MeasPRS_RepetitionAndOffset_r16_PR_ms40_r16,
	NR_NR_PRS_MeasurementInfo_r16__nr_MeasPRS_RepetitionAndOffset_r16_PR_ms80_r16,
	NR_NR_PRS_MeasurementInfo_r16__nr_MeasPRS_RepetitionAndOffset_r16_PR_ms160_r16
	/* Extensions may appear below */
	
} NR_NR_PRS_MeasurementInfo_r16__nr_MeasPRS_RepetitionAndOffset_r16_PR;
typedef enum NR_NR_PRS_MeasurementInfo_r16__nr_MeasPRS_length_r16 {
	NR_NR_PRS_MeasurementInfo_r16__nr_MeasPRS_length_r16_ms1dot5	= 0,
	NR_NR_PRS_MeasurementInfo_r16__nr_MeasPRS_length_r16_ms3	= 1,
	NR_NR_PRS_MeasurementInfo_r16__nr_MeasPRS_length_r16_ms3dot5	= 2,
	NR_NR_PRS_MeasurementInfo_r16__nr_MeasPRS_length_r16_ms4	= 3,
	NR_NR_PRS_MeasurementInfo_r16__nr_MeasPRS_length_r16_ms5dot5	= 4,
	NR_NR_PRS_MeasurementInfo_r16__nr_MeasPRS_length_r16_ms6	= 5,
	NR_NR_PRS_MeasurementInfo_r16__nr_MeasPRS_length_r16_ms10	= 6,
	NR_NR_PRS_MeasurementInfo_r16__nr_MeasPRS_length_r16_ms20	= 7
} e_NR_NR_PRS_MeasurementInfo_r16__nr_MeasPRS_length_r16;

/* NR_NR-PRS-MeasurementInfo-r16 */
typedef struct NR_NR_PRS_MeasurementInfo_r16 {
	NR_ARFCN_ValueNR_t	 dl_PRS_PointA_r16;
	struct NR_NR_PRS_MeasurementInfo_r16__nr_MeasPRS_RepetitionAndOffset_r16 {
		NR_NR_PRS_MeasurementInfo_r16__nr_MeasPRS_RepetitionAndOffset_r16_PR present;
		union NR_NR_PRS_MeasurementInfo_r16__NR_nr_MeasPRS_RepetitionAndOffset_r16_u {
			long	 ms20_r16;
			long	 ms40_r16;
			long	 ms80_r16;
			long	 ms160_r16;
			/*
			 * This type is extensible,
			 * possible extensions are below.
			 */
		} choice;
		
		/* Context for parsing across buffer boundaries */
		asn_struct_ctx_t _asn_ctx;
	} nr_MeasPRS_RepetitionAndOffset_r16;
	long	 nr_MeasPRS_length_r16;
	/*
	 * This type is extensible,
	 * possible extensions are below.
	 */
	
	/* Context for parsing across buffer boundaries */
	asn_struct_ctx_t _asn_ctx;
} NR_NR_PRS_MeasurementInfo_r16_t;

/* Implementation */
/* extern asn_TYPE_descriptor_t asn_DEF_NR_nr_MeasPRS_length_r16_9;	// (Use -fall-defs-global to expose) */
extern asn_TYPE_descriptor_t asn_DEF_NR_NR_PRS_MeasurementInfo_r16;
extern asn_SEQUENCE_specifics_t asn_SPC_NR_NR_PRS_MeasurementInfo_r16_specs_1;
extern asn_TYPE_member_t asn_MBR_NR_NR_PRS_MeasurementInfo_r16_1[3];

#ifdef __cplusplus
}
#endif

#endif	/* _NR_NR_PRS_MeasurementInfo_r16_H_ */
#include <asn_internal.h>
