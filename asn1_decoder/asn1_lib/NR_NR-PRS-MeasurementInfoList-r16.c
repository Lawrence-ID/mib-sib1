/*
 * Generated by asn1c-0.9.29 (http://lionet.info/asn1c)
 * From ASN.1 module "NR-RRC-Definitions"
 * 	found in "/home/witcomm/work/wf/1/openair2/RRC/NR/MESSAGES/asn1c/ASN1_files/nr-rrc-16.4.1.asn1"
 * 	`asn1c -pdu=all -fcompound-names -gen-PER -no-gen-OER -no-gen-example -findirect-choice -D /home/witcomm/work/wf/1/cmake_targets/ran_build/build/CMakeFiles/RRC_NR_Rel16`
 */

#include "NR_NR-PRS-MeasurementInfoList-r16.h"

asn_per_constraints_t asn_PER_type_NR_NR_PRS_MeasurementInfoList_r16_constr_1 CC_NOTUSED = {
	{ APC_UNCONSTRAINED,	-1, -1,  0,  0 },
	{ APC_CONSTRAINED,	 2,  2,  1,  4 }	/* (SIZE(1..4)) */,
	0, 0	/* No PER value map */
};
asn_TYPE_member_t asn_MBR_NR_NR_PRS_MeasurementInfoList_r16_1[] = {
	{ ATF_POINTER, 0, 0,
		(ASN_TAG_CLASS_UNIVERSAL | (16 << 2)),
		0,
		&asn_DEF_NR_NR_PRS_MeasurementInfo_r16,
		0,
		{ 0, 0, 0 },
		0, 0, /* No default value */
		""
		},
};
static const ber_tlv_tag_t asn_DEF_NR_NR_PRS_MeasurementInfoList_r16_tags_1[] = {
	(ASN_TAG_CLASS_UNIVERSAL | (16 << 2))
};
asn_SET_OF_specifics_t asn_SPC_NR_NR_PRS_MeasurementInfoList_r16_specs_1 = {
	sizeof(struct NR_NR_PRS_MeasurementInfoList_r16),
	offsetof(struct NR_NR_PRS_MeasurementInfoList_r16, _asn_ctx),
	0,	/* XER encoding is XMLDelimitedItemList */
};
asn_TYPE_descriptor_t asn_DEF_NR_NR_PRS_MeasurementInfoList_r16 = {
	"NR-PRS-MeasurementInfoList-r16",
	"NR-PRS-MeasurementInfoList-r16",
	&asn_OP_SEQUENCE_OF,
	asn_DEF_NR_NR_PRS_MeasurementInfoList_r16_tags_1,
	sizeof(asn_DEF_NR_NR_PRS_MeasurementInfoList_r16_tags_1)
		/sizeof(asn_DEF_NR_NR_PRS_MeasurementInfoList_r16_tags_1[0]), /* 1 */
	asn_DEF_NR_NR_PRS_MeasurementInfoList_r16_tags_1,	/* Same as above */
	sizeof(asn_DEF_NR_NR_PRS_MeasurementInfoList_r16_tags_1)
		/sizeof(asn_DEF_NR_NR_PRS_MeasurementInfoList_r16_tags_1[0]), /* 1 */
	{ 0, &asn_PER_type_NR_NR_PRS_MeasurementInfoList_r16_constr_1, SEQUENCE_OF_constraint },
	asn_MBR_NR_NR_PRS_MeasurementInfoList_r16_1,
	1,	/* Single element */
	&asn_SPC_NR_NR_PRS_MeasurementInfoList_r16_specs_1	/* Additional specs */
};

