/*
 * Generated by asn1c-0.9.29 (http://lionet.info/asn1c)
 * From ASN.1 module "NR-RRC-Definitions"
 * 	found in "/home/witcomm/work/wf/1/openair2/RRC/NR/MESSAGES/asn1c/ASN1_files/nr-rrc-16.4.1.asn1"
 * 	`asn1c -pdu=all -fcompound-names -gen-PER -no-gen-OER -no-gen-example -findirect-choice -D /home/witcomm/work/wf/1/cmake_targets/ran_build/build/CMakeFiles/RRC_NR_Rel16`
 */

#ifndef	_NR_TimeAlignmentTimer_H_
#define	_NR_TimeAlignmentTimer_H_


#include <asn_application.h>

/* Including external dependencies */
#include <NativeEnumerated.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Dependencies */
typedef enum NR_TimeAlignmentTimer {
	NR_TimeAlignmentTimer_ms500	= 0,
	NR_TimeAlignmentTimer_ms750	= 1,
	NR_TimeAlignmentTimer_ms1280	= 2,
	NR_TimeAlignmentTimer_ms1920	= 3,
	NR_TimeAlignmentTimer_ms2560	= 4,
	NR_TimeAlignmentTimer_ms5120	= 5,
	NR_TimeAlignmentTimer_ms10240	= 6,
	NR_TimeAlignmentTimer_infinity	= 7
} e_NR_TimeAlignmentTimer;

/* NR_TimeAlignmentTimer */
typedef long	 NR_TimeAlignmentTimer_t;

/* Implementation */
extern asn_per_constraints_t asn_PER_type_NR_TimeAlignmentTimer_constr_1;
extern asn_TYPE_descriptor_t asn_DEF_NR_TimeAlignmentTimer;
extern const asn_INTEGER_specifics_t asn_SPC_NR_TimeAlignmentTimer_specs_1;
asn_struct_free_f NR_TimeAlignmentTimer_free;
asn_struct_print_f NR_TimeAlignmentTimer_print;
asn_constr_check_f NR_TimeAlignmentTimer_constraint;
ber_type_decoder_f NR_TimeAlignmentTimer_decode_ber;
der_type_encoder_f NR_TimeAlignmentTimer_encode_der;
xer_type_decoder_f NR_TimeAlignmentTimer_decode_xer;
xer_type_encoder_f NR_TimeAlignmentTimer_encode_xer;
per_type_decoder_f NR_TimeAlignmentTimer_decode_uper;
per_type_encoder_f NR_TimeAlignmentTimer_encode_uper;
per_type_decoder_f NR_TimeAlignmentTimer_decode_aper;
per_type_encoder_f NR_TimeAlignmentTimer_encode_aper;

#ifdef __cplusplus
}
#endif

#endif	/* _NR_TimeAlignmentTimer_H_ */
#include <asn_internal.h>
