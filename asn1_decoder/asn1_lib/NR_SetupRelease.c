#include "NR_SetupRelease.h"

asn_per_constraints_t asn_PER_type_NR_SetupRelease_PDCCH_ConfigCommon_constr_40 CC_NOTUSED = {
	{ APC_CONSTRAINED,	 1,  1,  0,  1 }	/* (0..1) */,
	{ APC_UNCONSTRAINED,	-1, -1,  0,  0 },
	0, 0	/* No PER value map */
};
asn_per_constraints_t asn_PER_type_NR_SetupRelease_PDSCH_ConfigCommon_constr_43 CC_NOTUSED = {
	{ APC_CONSTRAINED,	 1,  1,  0,  1 }	/* (0..1) */,
	{ APC_UNCONSTRAINED,	-1, -1,  0,  0 },
	0, 0	/* No PER value map */
};
asn_per_constraints_t asn_PER_type_NR_SetupRelease_RACH_ConfigCommon_constr_61 CC_NOTUSED = {
	{ APC_CONSTRAINED,	 1,  1,  0,  1 }	/* (0..1) */,
	{ APC_UNCONSTRAINED,	-1, -1,  0,  0 },
	0, 0	/* No PER value map */
};
asn_per_constraints_t asn_PER_type_NR_SetupRelease_PUSCH_ConfigCommon_constr_64 CC_NOTUSED = {
	{ APC_CONSTRAINED,	 1,  1,  0,  1 }	/* (0..1) */,
	{ APC_UNCONSTRAINED,	-1, -1,  0,  0 },
	0, 0	/* No PER value map */
};
asn_per_constraints_t asn_PER_type_NR_SetupRelease_PUCCH_ConfigCommon_constr_67 CC_NOTUSED = {
	{ APC_CONSTRAINED,	 1,  1,  0,  1 }	/* (0..1) */,
	{ APC_UNCONSTRAINED,	-1, -1,  0,  0 },
	0, 0	/* No PER value map */
};
asn_per_constraints_t asn_PER_type_NR_SetupRelease_MsgA_ConfigCommon_r16_constr_70 CC_NOTUSED = {
	{ APC_CONSTRAINED,	 1,  1,  0,  1 }	/* (0..1) */,
	{ APC_UNCONSTRAINED,	-1, -1,  0,  0 },
	0, 0	/* No PER value map */
};




asn_TYPE_member_t asn_MBR_NR_SetupRelease_PDCCH_ConfigCommon_40[] = {
	{ ATF_NOFLAGS, 0, offsetof(struct NR_SetupRelease_PDCCH_ConfigCommon, choice.release),
		(ASN_TAG_CLASS_CONTEXT | (0 << 2)),
		-1,	/* IMPLICIT tag at current level */
		&asn_DEF_NULL,
		0,
		{ 0, 0, 0 },
		0, 0, /* No default value */
		"release"
		},
	{ ATF_POINTER, 0, offsetof(struct NR_SetupRelease_PDCCH_ConfigCommon, choice.setup),
		(ASN_TAG_CLASS_CONTEXT | (1 << 2)),
		-1,	/* IMPLICIT tag at current level */
		&asn_DEF_NR_PDCCH_ConfigCommon,
		0,
		{ 0, 0, 0 },
		0, 0, /* No default value */
		"setup"
		},
};
static const asn_TYPE_tag2member_t asn_MAP_NR_SetupRelease_PDCCH_ConfigCommon_tag2el_40[] = {
    { (ASN_TAG_CLASS_CONTEXT | (0 << 2)), 0, 0, 0 }, /* release */
    { (ASN_TAG_CLASS_CONTEXT | (1 << 2)), 1, 0, 0 } /* setup */
};
asn_CHOICE_specifics_t asn_SPC_NR_SetupRelease_PDCCH_ConfigCommon_specs_40 = {
	sizeof(struct NR_SetupRelease_PDCCH_ConfigCommon),
	offsetof(struct NR_SetupRelease_PDCCH_ConfigCommon, _asn_ctx),
	offsetof(struct NR_SetupRelease_PDCCH_ConfigCommon, present),
	sizeof(((struct NR_SetupRelease_PDCCH_ConfigCommon *)0)->present),
	asn_MAP_NR_SetupRelease_PDCCH_ConfigCommon_tag2el_40,
	2,	/* Count of tags in the map */
	0, 0,
	-1	/* Extensions start */
};
asn_TYPE_descriptor_t asn_DEF_NR_SetupRelease_PDCCH_ConfigCommon = {
	"SetupRelease_PDCCH-ConfigCommon",
	"SetupRelease_PDCCH-ConfigCommon",
	&asn_OP_CHOICE,
	0,	/* No effective tags (pointer) */
	0,	/* No effective tags (count) */
	0,	/* No tags (pointer) */
	0,	/* No tags (count) */
	{ 0, &asn_PER_type_NR_SetupRelease_PDCCH_ConfigCommon_constr_40, CHOICE_constraint },
	asn_MBR_NR_SetupRelease_PDCCH_ConfigCommon_40,
	2,	/* Elements count */
	&asn_SPC_NR_SetupRelease_PDCCH_ConfigCommon_specs_40	/* Additional specs */
};

asn_TYPE_member_t asn_MBR_NR_SetupRelease_PDSCH_ConfigCommon_43[] = {
	{ ATF_NOFLAGS, 0, offsetof(struct NR_SetupRelease_PDSCH_ConfigCommon, choice.release),
		(ASN_TAG_CLASS_CONTEXT | (0 << 2)),
		-1,	/* IMPLICIT tag at current level */
		&asn_DEF_NULL,
		0,
		{ 0, 0, 0 },
		0, 0, /* No default value */
		"release"
		},
	{ ATF_POINTER, 0, offsetof(struct NR_SetupRelease_PDSCH_ConfigCommon, choice.setup),
		(ASN_TAG_CLASS_CONTEXT | (1 << 2)),
		-1,	/* IMPLICIT tag at current level */
		&asn_DEF_NR_PDSCH_ConfigCommon,
		0,
		{ 0, 0, 0 },
		0, 0, /* No default value */
		"setup"
		},
};
static const asn_TYPE_tag2member_t asn_MAP_NR_SetupRelease_PDSCH_ConfigCommon_tag2el_43[] = {
    { (ASN_TAG_CLASS_CONTEXT | (0 << 2)), 0, 0, 0 }, /* release */
    { (ASN_TAG_CLASS_CONTEXT | (1 << 2)), 1, 0, 0 } /* setup */
};
asn_CHOICE_specifics_t asn_SPC_NR_SetupRelease_PDSCH_ConfigCommon_specs_43 = {
	sizeof(struct NR_SetupRelease_PDSCH_ConfigCommon),
	offsetof(struct NR_SetupRelease_PDSCH_ConfigCommon, _asn_ctx),
	offsetof(struct NR_SetupRelease_PDSCH_ConfigCommon, present),
	sizeof(((struct NR_SetupRelease_PDSCH_ConfigCommon *)0)->present),
	asn_MAP_NR_SetupRelease_PDSCH_ConfigCommon_tag2el_43,
	2,	/* Count of tags in the map */
	0, 0,
	-1	/* Extensions start */
};
asn_TYPE_descriptor_t asn_DEF_NR_SetupRelease_PDSCH_ConfigCommon = {
	"SetupRelease_PDSCH-ConfigCommon",
	"SetupRelease_PDSCH-ConfigCommon",
	&asn_OP_CHOICE,
	0,	/* No effective tags (pointer) */
	0,	/* No effective tags (count) */
	0,	/* No tags (pointer) */
	0,	/* No tags (count) */
	{ 0, &asn_PER_type_NR_SetupRelease_PDSCH_ConfigCommon_constr_43, CHOICE_constraint },
	asn_MBR_NR_SetupRelease_PDSCH_ConfigCommon_43,
	2,	/* Elements count */
	&asn_SPC_NR_SetupRelease_PDSCH_ConfigCommon_specs_43	/* Additional specs */
};


asn_TYPE_member_t asn_MBR_NR_SetupRelease_RACH_ConfigCommon_61[] = {
	{ ATF_NOFLAGS, 0, offsetof(struct NR_SetupRelease_RACH_ConfigCommon, choice.release),
		(ASN_TAG_CLASS_CONTEXT | (0 << 2)),
		-1,	/* IMPLICIT tag at current level */
		&asn_DEF_NULL,
		0,
		{ 0, 0, 0 },
		0, 0, /* No default value */
		"release"
		},
	{ ATF_POINTER, 0, offsetof(struct NR_SetupRelease_RACH_ConfigCommon, choice.setup),
		(ASN_TAG_CLASS_CONTEXT | (1 << 2)),
		-1,	/* IMPLICIT tag at current level */
		&asn_DEF_NR_RACH_ConfigCommon,
		0,
		{ 0, 0, 0 },
		0, 0, /* No default value */
		"setup"
		},
};
static const asn_TYPE_tag2member_t asn_MAP_NR_SetupRelease_RACH_ConfigCommon_tag2el_61[] = {
    { (ASN_TAG_CLASS_CONTEXT | (0 << 2)), 0, 0, 0 }, /* release */
    { (ASN_TAG_CLASS_CONTEXT | (1 << 2)), 1, 0, 0 } /* setup */
};
asn_CHOICE_specifics_t asn_SPC_NR_SetupRelease_RACH_ConfigCommon_specs_61 = {
	sizeof(struct NR_SetupRelease_RACH_ConfigCommon),
	offsetof(struct NR_SetupRelease_RACH_ConfigCommon, _asn_ctx),
	offsetof(struct NR_SetupRelease_RACH_ConfigCommon, present),
	sizeof(((struct NR_SetupRelease_RACH_ConfigCommon *)0)->present),
	asn_MAP_NR_SetupRelease_RACH_ConfigCommon_tag2el_61,
	2,	/* Count of tags in the map */
	0, 0,
	-1	/* Extensions start */
};
asn_TYPE_descriptor_t asn_DEF_NR_SetupRelease_RACH_ConfigCommon = {
	"SetupRelease_RACH-ConfigCommon",
	"SetupRelease_RACH-ConfigCommon",
	&asn_OP_CHOICE,
	0,	/* No effective tags (pointer) */
	0,	/* No effective tags (count) */
	0,	/* No tags (pointer) */
	0,	/* No tags (count) */
	{ 0, &asn_PER_type_NR_SetupRelease_RACH_ConfigCommon_constr_61, CHOICE_constraint },
	asn_MBR_NR_SetupRelease_RACH_ConfigCommon_61,
	2,	/* Elements count */
	&asn_SPC_NR_SetupRelease_RACH_ConfigCommon_specs_61	/* Additional specs */
};

asn_TYPE_member_t asn_MBR_NR_SetupRelease_PUSCH_ConfigCommon_64[] = {
	{ ATF_NOFLAGS, 0, offsetof(struct NR_SetupRelease_PUSCH_ConfigCommon, choice.release),
		(ASN_TAG_CLASS_CONTEXT | (0 << 2)),
		-1,	/* IMPLICIT tag at current level */
		&asn_DEF_NULL,
		0,
		{ 0, 0, 0 },
		0, 0, /* No default value */
		"release"
		},
	{ ATF_POINTER, 0, offsetof(struct NR_SetupRelease_PUSCH_ConfigCommon, choice.setup),
		(ASN_TAG_CLASS_CONTEXT | (1 << 2)),
		-1,	/* IMPLICIT tag at current level */
		&asn_DEF_NR_PUSCH_ConfigCommon,
		0,
		{ 0, 0, 0 },
		0, 0, /* No default value */
		"setup"
		},
};
static const asn_TYPE_tag2member_t asn_MAP_NR_SetupRelease_PUSCH_ConfigCommon_tag2el_64[] = {
    { (ASN_TAG_CLASS_CONTEXT | (0 << 2)), 0, 0, 0 }, /* release */
    { (ASN_TAG_CLASS_CONTEXT | (1 << 2)), 1, 0, 0 } /* setup */
};
asn_CHOICE_specifics_t asn_SPC_NR_SetupRelease_PUSCH_ConfigCommon_specs_64 = {
	sizeof(struct NR_SetupRelease_PUSCH_ConfigCommon),
	offsetof(struct NR_SetupRelease_PUSCH_ConfigCommon, _asn_ctx),
	offsetof(struct NR_SetupRelease_PUSCH_ConfigCommon, present),
	sizeof(((struct NR_SetupRelease_PUSCH_ConfigCommon *)0)->present),
	asn_MAP_NR_SetupRelease_PUSCH_ConfigCommon_tag2el_64,
	2,	/* Count of tags in the map */
	0, 0,
	-1	/* Extensions start */
};
asn_TYPE_descriptor_t asn_DEF_NR_SetupRelease_PUSCH_ConfigCommon = {
	"SetupRelease_PUSCH-ConfigCommon",
	"SetupRelease_PUSCH-ConfigCommon",
	&asn_OP_CHOICE,
	0,	/* No effective tags (pointer) */
	0,	/* No effective tags (count) */
	0,	/* No tags (pointer) */
	0,	/* No tags (count) */
	{ 0, &asn_PER_type_NR_SetupRelease_PUSCH_ConfigCommon_constr_64, CHOICE_constraint },
	asn_MBR_NR_SetupRelease_PUSCH_ConfigCommon_64,
	2,	/* Elements count */
	&asn_SPC_NR_SetupRelease_PUSCH_ConfigCommon_specs_64	/* Additional specs */
};

asn_TYPE_member_t asn_MBR_NR_SetupRelease_PUCCH_ConfigCommon_67[] = {
	{ ATF_NOFLAGS, 0, offsetof(struct NR_SetupRelease_PUCCH_ConfigCommon, choice.release),
		(ASN_TAG_CLASS_CONTEXT | (0 << 2)),
		-1,	/* IMPLICIT tag at current level */
		&asn_DEF_NULL,
		0,
		{ 0, 0, 0 },
		0, 0, /* No default value */
		"release"
		},
	{ ATF_POINTER, 0, offsetof(struct NR_SetupRelease_PUCCH_ConfigCommon, choice.setup),
		(ASN_TAG_CLASS_CONTEXT | (1 << 2)),
		-1,	/* IMPLICIT tag at current level */
		&asn_DEF_NR_PUCCH_ConfigCommon,
		0,
		{ 0, 0, 0 },
		0, 0, /* No default value */
		"setup"
		},
};
static const asn_TYPE_tag2member_t asn_MAP_NR_SetupRelease_PUCCH_ConfigCommon_tag2el_67[] = {
    { (ASN_TAG_CLASS_CONTEXT | (0 << 2)), 0, 0, 0 }, /* release */
    { (ASN_TAG_CLASS_CONTEXT | (1 << 2)), 1, 0, 0 } /* setup */
};
asn_CHOICE_specifics_t asn_SPC_NR_SetupRelease_PUCCH_ConfigCommon_specs_67 = {
	sizeof(struct NR_SetupRelease_PUCCH_ConfigCommon),
	offsetof(struct NR_SetupRelease_PUCCH_ConfigCommon, _asn_ctx),
	offsetof(struct NR_SetupRelease_PUCCH_ConfigCommon, present),
	sizeof(((struct NR_SetupRelease_PUCCH_ConfigCommon *)0)->present),
	asn_MAP_NR_SetupRelease_PUCCH_ConfigCommon_tag2el_67,
	2,	/* Count of tags in the map */
	0, 0,
	-1	/* Extensions start */
};
asn_TYPE_descriptor_t asn_DEF_NR_SetupRelease_PUCCH_ConfigCommon = {
	"SetupRelease_PUCCH-ConfigCommon",
	"SetupRelease_PUCCH-ConfigCommon",
	&asn_OP_CHOICE,
	0,	/* No effective tags (pointer) */
	0,	/* No effective tags (count) */
	0,	/* No tags (pointer) */
	0,	/* No tags (count) */
	{ 0, &asn_PER_type_NR_SetupRelease_PUCCH_ConfigCommon_constr_67, CHOICE_constraint },
	asn_MBR_NR_SetupRelease_PUCCH_ConfigCommon_67,
	2,	/* Elements count */
	&asn_SPC_NR_SetupRelease_PUCCH_ConfigCommon_specs_67	/* Additional specs */
};

asn_TYPE_member_t asn_MBR_NR_SetupRelease_MsgA_ConfigCommon_r16_70[] = {
	{ ATF_NOFLAGS, 0, offsetof(struct NR_SetupRelease_MsgA_ConfigCommon_r16, choice.release),
		(ASN_TAG_CLASS_CONTEXT | (0 << 2)),
		-1,	/* IMPLICIT tag at current level */
		&asn_DEF_NULL,
		0,
		{ 0, 0, 0 },
		0, 0, /* No default value */
		"release"
		},
	{ ATF_POINTER, 0, offsetof(struct NR_SetupRelease_MsgA_ConfigCommon_r16, choice.setup),
		(ASN_TAG_CLASS_CONTEXT | (1 << 2)),
		-1,	/* IMPLICIT tag at current level */
		&asn_DEF_NR_MsgA_ConfigCommon_r16,
		0,
		{ 0, 0, 0 },
		0, 0, /* No default value */
		"setup"
		},
};
static const asn_TYPE_tag2member_t asn_MAP_NR_SetupRelease_MsgA_ConfigCommon_r16_tag2el_70[] = {
    { (ASN_TAG_CLASS_CONTEXT | (0 << 2)), 0, 0, 0 }, /* release */
    { (ASN_TAG_CLASS_CONTEXT | (1 << 2)), 1, 0, 0 } /* setup */
};
asn_CHOICE_specifics_t asn_SPC_NR_SetupRelease_MsgA_ConfigCommon_r16_specs_70 = {
	sizeof(struct NR_SetupRelease_MsgA_ConfigCommon_r16),
	offsetof(struct NR_SetupRelease_MsgA_ConfigCommon_r16, _asn_ctx),
	offsetof(struct NR_SetupRelease_MsgA_ConfigCommon_r16, present),
	sizeof(((struct NR_SetupRelease_MsgA_ConfigCommon_r16 *)0)->present),
	asn_MAP_NR_SetupRelease_MsgA_ConfigCommon_r16_tag2el_70,
	2,	/* Count of tags in the map */
	0, 0,
	-1	/* Extensions start */
};
asn_TYPE_descriptor_t asn_DEF_NR_SetupRelease_MsgA_ConfigCommon_r16 = {
	"SetupRelease_MsgA-ConfigCommon-r16",
	"SetupRelease_MsgA-ConfigCommon-r16",
	&asn_OP_CHOICE,
	0,	/* No effective tags (pointer) */
	0,	/* No effective tags (count) */
	0,	/* No tags (pointer) */
	0,	/* No tags (count) */
	{ 0, &asn_PER_type_NR_SetupRelease_MsgA_ConfigCommon_r16_constr_70, CHOICE_constraint },
	asn_MBR_NR_SetupRelease_MsgA_ConfigCommon_r16_70,
	2,	/* Elements count */
	&asn_SPC_NR_SetupRelease_MsgA_ConfigCommon_r16_specs_70	/* Additional specs */
};

