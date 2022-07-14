/*
 * Generated by asn1c-0.9.29 (http://lionet.info/asn1c)
 * From ASN.1 module "NR-RRC-Definitions"
 * 	found in "/home/witcomm/work/wf/1/openair2/RRC/NR/MESSAGES/asn1c/ASN1_files/nr-rrc-16.4.1.asn1"
 * 	`asn1c -pdu=all -fcompound-names -gen-PER -no-gen-OER -no-gen-example -findirect-choice -D /home/witcomm/work/wf/1/cmake_targets/ran_build/build/CMakeFiles/RRC_NR_Rel16`
 */

#include "NR_T312-r16.h"

/*
 * This type is implemented using NativeEnumerated,
 * so here we adjust the DEF accordingly.
 */
asn_per_constraints_t asn_PER_type_NR_T312_r16_constr_1 CC_NOTUSED = {
	{ APC_CONSTRAINED,	 3,  3,  0,  7 }	/* (0..7) */,
	{ APC_UNCONSTRAINED,	-1, -1,  0,  0 },
	0, 0	/* No PER value map */
};
static const asn_INTEGER_enum_map_t asn_MAP_NR_T312_r16_value2enum_1[] = {
	{ 0,	3,	"ms0" },
	{ 1,	4,	"ms50" },
	{ 2,	5,	"ms100" },
	{ 3,	5,	"ms200" },
	{ 4,	5,	"ms300" },
	{ 5,	5,	"ms400" },
	{ 6,	5,	"ms500" },
	{ 7,	6,	"ms1000" }
};
static const unsigned int asn_MAP_NR_T312_r16_enum2value_1[] = {
	0,	/* ms0(0) */
	2,	/* ms100(2) */
	7,	/* ms1000(7) */
	3,	/* ms200(3) */
	4,	/* ms300(4) */
	5,	/* ms400(5) */
	1,	/* ms50(1) */
	6	/* ms500(6) */
};
const asn_INTEGER_specifics_t asn_SPC_NR_T312_r16_specs_1 = {
	asn_MAP_NR_T312_r16_value2enum_1,	/* "tag" => N; sorted by tag */
	asn_MAP_NR_T312_r16_enum2value_1,	/* N => "tag"; sorted by N */
	8,	/* Number of elements in the maps */
	0,	/* Enumeration is not extensible */
	1,	/* Strict enumeration */
	0,	/* Native long size */
	0
};
static const ber_tlv_tag_t asn_DEF_NR_T312_r16_tags_1[] = {
	(ASN_TAG_CLASS_UNIVERSAL | (10 << 2))
};
asn_TYPE_descriptor_t asn_DEF_NR_T312_r16 = {
	"T312-r16",
	"T312-r16",
	&asn_OP_NativeEnumerated,
	asn_DEF_NR_T312_r16_tags_1,
	sizeof(asn_DEF_NR_T312_r16_tags_1)
		/sizeof(asn_DEF_NR_T312_r16_tags_1[0]), /* 1 */
	asn_DEF_NR_T312_r16_tags_1,	/* Same as above */
	sizeof(asn_DEF_NR_T312_r16_tags_1)
		/sizeof(asn_DEF_NR_T312_r16_tags_1[0]), /* 1 */
	{ 0, &asn_PER_type_NR_T312_r16_constr_1, NativeEnumerated_constraint },
	0, 0,	/* Defined elsewhere */
	&asn_SPC_NR_T312_r16_specs_1	/* Additional specs */
};

