/*
 * Generated by asn1c-0.9.29 (http://lionet.info/asn1c)
 * From ASN.1 module "NR-RRC-Definitions"
 * 	found in "/home/witcomm/work/wf/1/openair2/RRC/NR/MESSAGES/asn1c/ASN1_files/nr-rrc-16.4.1.asn1"
 * 	`asn1c -pdu=all -fcompound-names -gen-PER -no-gen-OER -no-gen-example -findirect-choice -D /home/witcomm/work/wf/1/cmake_targets/ran_build/build/CMakeFiles/RRC_NR_Rel16`
 */

#include "NR_TimeAlignmentTimer.h"

/*
 * This type is implemented using NativeEnumerated,
 * so here we adjust the DEF accordingly.
 */
asn_per_constraints_t asn_PER_type_NR_TimeAlignmentTimer_constr_1 CC_NOTUSED = {
	{ APC_CONSTRAINED,	 3,  3,  0,  7 }	/* (0..7) */,
	{ APC_UNCONSTRAINED,	-1, -1,  0,  0 },
	0, 0	/* No PER value map */
};
static const asn_INTEGER_enum_map_t asn_MAP_NR_TimeAlignmentTimer_value2enum_1[] = {
	{ 0,	5,	"ms500" },
	{ 1,	5,	"ms750" },
	{ 2,	6,	"ms1280" },
	{ 3,	6,	"ms1920" },
	{ 4,	6,	"ms2560" },
	{ 5,	6,	"ms5120" },
	{ 6,	7,	"ms10240" },
	{ 7,	8,	"infinity" }
};
static const unsigned int asn_MAP_NR_TimeAlignmentTimer_enum2value_1[] = {
	7,	/* infinity(7) */
	6,	/* ms10240(6) */
	2,	/* ms1280(2) */
	3,	/* ms1920(3) */
	4,	/* ms2560(4) */
	0,	/* ms500(0) */
	5,	/* ms5120(5) */
	1	/* ms750(1) */
};
const asn_INTEGER_specifics_t asn_SPC_NR_TimeAlignmentTimer_specs_1 = {
	asn_MAP_NR_TimeAlignmentTimer_value2enum_1,	/* "tag" => N; sorted by tag */
	asn_MAP_NR_TimeAlignmentTimer_enum2value_1,	/* N => "tag"; sorted by N */
	8,	/* Number of elements in the maps */
	0,	/* Enumeration is not extensible */
	1,	/* Strict enumeration */
	0,	/* Native long size */
	0
};
static const ber_tlv_tag_t asn_DEF_NR_TimeAlignmentTimer_tags_1[] = {
	(ASN_TAG_CLASS_UNIVERSAL | (10 << 2))
};
asn_TYPE_descriptor_t asn_DEF_NR_TimeAlignmentTimer = {
	"TimeAlignmentTimer",
	"TimeAlignmentTimer",
	&asn_OP_NativeEnumerated,
	asn_DEF_NR_TimeAlignmentTimer_tags_1,
	sizeof(asn_DEF_NR_TimeAlignmentTimer_tags_1)
		/sizeof(asn_DEF_NR_TimeAlignmentTimer_tags_1[0]), /* 1 */
	asn_DEF_NR_TimeAlignmentTimer_tags_1,	/* Same as above */
	sizeof(asn_DEF_NR_TimeAlignmentTimer_tags_1)
		/sizeof(asn_DEF_NR_TimeAlignmentTimer_tags_1[0]), /* 1 */
	{ 0, &asn_PER_type_NR_TimeAlignmentTimer_constr_1, NativeEnumerated_constraint },
	0, 0,	/* Defined elsewhere */
	&asn_SPC_NR_TimeAlignmentTimer_specs_1	/* Additional specs */
};
