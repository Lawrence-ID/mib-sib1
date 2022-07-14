/*
 * Generated by asn1c-0.9.29 (http://lionet.info/asn1c)
 * From ASN.1 module "NR-RRC-Definitions"
 * 	found in "/home/witcomm/work/wf/1/openair2/RRC/NR/MESSAGES/asn1c/ASN1_files/nr-rrc-16.4.1.asn1"
 * 	`asn1c -pdu=all -fcompound-names -gen-PER -no-gen-OER -no-gen-example -findirect-choice -D /home/witcomm/work/wf/1/cmake_targets/ran_build/build/CMakeFiles/RRC_NR_Rel16`
 */

#include "NR_PDSCH-ConfigCommon.h"

asn_TYPE_member_t asn_MBR_NR_PDSCH_ConfigCommon_1[] = {
	{ ATF_POINTER, 1, offsetof(struct NR_PDSCH_ConfigCommon, pdsch_TimeDomainAllocationList),
		(ASN_TAG_CLASS_CONTEXT | (0 << 2)),
		-1,	/* IMPLICIT tag at current level */
		&asn_DEF_NR_PDSCH_TimeDomainResourceAllocationList,
		0,
		{ 0, 0, 0 },
		0, 0, /* No default value */
		"pdsch-TimeDomainAllocationList"
		},
};
static const int asn_MAP_NR_PDSCH_ConfigCommon_oms_1[] = { 0 };
static const ber_tlv_tag_t asn_DEF_NR_PDSCH_ConfigCommon_tags_1[] = {
	(ASN_TAG_CLASS_UNIVERSAL | (16 << 2))
};
static const asn_TYPE_tag2member_t asn_MAP_NR_PDSCH_ConfigCommon_tag2el_1[] = {
    { (ASN_TAG_CLASS_CONTEXT | (0 << 2)), 0, 0, 0 } /* pdsch-TimeDomainAllocationList */
};
asn_SEQUENCE_specifics_t asn_SPC_NR_PDSCH_ConfigCommon_specs_1 = {
	sizeof(struct NR_PDSCH_ConfigCommon),
	offsetof(struct NR_PDSCH_ConfigCommon, _asn_ctx),
	asn_MAP_NR_PDSCH_ConfigCommon_tag2el_1,
	1,	/* Count of tags in the map */
	asn_MAP_NR_PDSCH_ConfigCommon_oms_1,	/* Optional members */
	1, 0,	/* Root/Additions */
	1,	/* First extension addition */
};
asn_TYPE_descriptor_t asn_DEF_NR_PDSCH_ConfigCommon = {
	"PDSCH-ConfigCommon",
	"PDSCH-ConfigCommon",
	&asn_OP_SEQUENCE,
	asn_DEF_NR_PDSCH_ConfigCommon_tags_1,
	sizeof(asn_DEF_NR_PDSCH_ConfigCommon_tags_1)
		/sizeof(asn_DEF_NR_PDSCH_ConfigCommon_tags_1[0]), /* 1 */
	asn_DEF_NR_PDSCH_ConfigCommon_tags_1,	/* Same as above */
	sizeof(asn_DEF_NR_PDSCH_ConfigCommon_tags_1)
		/sizeof(asn_DEF_NR_PDSCH_ConfigCommon_tags_1[0]), /* 1 */
	{ 0, 0, SEQUENCE_constraint },
	asn_MBR_NR_PDSCH_ConfigCommon_1,
	1,	/* Elements count */
	&asn_SPC_NR_PDSCH_ConfigCommon_specs_1	/* Additional specs */
};

