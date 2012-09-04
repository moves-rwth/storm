/* /////////////////////////////////////////////////////////////////////////
 * File:        pantheios/internal/generated/log_functions.h
 *
 * Purpose:     Declarations of the pantheios_log_<N>() functions
 *
 * Generated:   8th January 2011
 *
 * Status:      This file is auto-generated: DO NOT EDIT!
 *
 * Copyright:   The copyright restrictions of the Pantheios library,
 *              enumerated in the header file <pantheios/pantheios.h>,
 *              apply to this file
 *
 * ////////////////////////////////////////////////////////////////////// */


#ifndef PANTHEIOS_INCLUDING_C_API_FUNCTIONS
# error This file is included by the Pantheios API, and cannot be included directly
#endif /* !PANTHEIOS_INCLUDING_C_API_FUNCTIONS */

#define PANTHEIOS_APPL_PARAMS_LIMIT_MAX_GENERATED   (32)

/** Logs 1 parameter, subject to external (not in \ref group__core_library) severity-level filtering 
 * \ingroup group__application_layer_interface__generated
 */
PANTHEIOS_CALL(int) pantheios_log_1(pan_sev_t severity
 , pan_char_t const* p0, int l0 /* -1 => 'strlen(p0)' */
);

/** Logs 2 parameters, subject to external (not in \ref group__core_library) severity-level filtering 
 * \ingroup group__application_layer_interface__generated
 */
PANTHEIOS_CALL(int) pantheios_log_2(pan_sev_t severity
 , pan_char_t const* p0, int l0 /* -1 => 'strlen(p0)' */
 , pan_char_t const* p1, int l1 /* -1 => 'strlen(p1)' */
);

/** Logs 3 parameters, subject to external (not in \ref group__core_library) severity-level filtering 
 * \ingroup group__application_layer_interface__generated
 */
PANTHEIOS_CALL(int) pantheios_log_3(pan_sev_t severity
 , pan_char_t const* p0, int l0 /* -1 => 'strlen(p0)' */
 , pan_char_t const* p1, int l1 /* -1 => 'strlen(p1)' */
 , pan_char_t const* p2, int l2 /* -1 => 'strlen(p2)' */
);

/** Logs 4 parameters, subject to external (not in \ref group__core_library) severity-level filtering 
 * \ingroup group__application_layer_interface__generated
 */
PANTHEIOS_CALL(int) pantheios_log_4(pan_sev_t severity
 , pan_char_t const* p0, int l0 /* -1 => 'strlen(p0)' */
 , pan_char_t const* p1, int l1 /* -1 => 'strlen(p1)' */
 , pan_char_t const* p2, int l2 /* -1 => 'strlen(p2)' */
 , pan_char_t const* p3, int l3 /* -1 => 'strlen(p3)' */
);

/** Logs 5 parameters, subject to external (not in \ref group__core_library) severity-level filtering 
 * \ingroup group__application_layer_interface__generated
 */
PANTHEIOS_CALL(int) pantheios_log_5(pan_sev_t severity
 , pan_char_t const* p0, int l0 /* -1 => 'strlen(p0)' */
 , pan_char_t const* p1, int l1 /* -1 => 'strlen(p1)' */
 , pan_char_t const* p2, int l2 /* -1 => 'strlen(p2)' */
 , pan_char_t const* p3, int l3 /* -1 => 'strlen(p3)' */
 , pan_char_t const* p4, int l4 /* -1 => 'strlen(p4)' */
);

/** Logs 6 parameters, subject to external (not in \ref group__core_library) severity-level filtering 
 * \ingroup group__application_layer_interface__generated
 */
PANTHEIOS_CALL(int) pantheios_log_6(pan_sev_t severity
 , pan_char_t const* p0, int l0 /* -1 => 'strlen(p0)' */
 , pan_char_t const* p1, int l1 /* -1 => 'strlen(p1)' */
 , pan_char_t const* p2, int l2 /* -1 => 'strlen(p2)' */
 , pan_char_t const* p3, int l3 /* -1 => 'strlen(p3)' */
 , pan_char_t const* p4, int l4 /* -1 => 'strlen(p4)' */
 , pan_char_t const* p5, int l5 /* -1 => 'strlen(p5)' */
);

/** Logs 7 parameters, subject to external (not in \ref group__core_library) severity-level filtering 
 * \ingroup group__application_layer_interface__generated
 */
PANTHEIOS_CALL(int) pantheios_log_7(pan_sev_t severity
 , pan_char_t const* p0, int l0 /* -1 => 'strlen(p0)' */
 , pan_char_t const* p1, int l1 /* -1 => 'strlen(p1)' */
 , pan_char_t const* p2, int l2 /* -1 => 'strlen(p2)' */
 , pan_char_t const* p3, int l3 /* -1 => 'strlen(p3)' */
 , pan_char_t const* p4, int l4 /* -1 => 'strlen(p4)' */
 , pan_char_t const* p5, int l5 /* -1 => 'strlen(p5)' */
 , pan_char_t const* p6, int l6 /* -1 => 'strlen(p6)' */
);

/** Logs 8 parameters, subject to external (not in \ref group__core_library) severity-level filtering 
 * \ingroup group__application_layer_interface__generated
 */
PANTHEIOS_CALL(int) pantheios_log_8(pan_sev_t severity
 , pan_char_t const* p0, int l0 /* -1 => 'strlen(p0)' */
 , pan_char_t const* p1, int l1 /* -1 => 'strlen(p1)' */
 , pan_char_t const* p2, int l2 /* -1 => 'strlen(p2)' */
 , pan_char_t const* p3, int l3 /* -1 => 'strlen(p3)' */
 , pan_char_t const* p4, int l4 /* -1 => 'strlen(p4)' */
 , pan_char_t const* p5, int l5 /* -1 => 'strlen(p5)' */
 , pan_char_t const* p6, int l6 /* -1 => 'strlen(p6)' */
 , pan_char_t const* p7, int l7 /* -1 => 'strlen(p7)' */
);

/** Logs 9 parameters, subject to external (not in \ref group__core_library) severity-level filtering 
 * \ingroup group__application_layer_interface__generated
 */
PANTHEIOS_CALL(int) pantheios_log_9(pan_sev_t severity
 , pan_char_t const* p0, int l0 /* -1 => 'strlen(p0)' */
 , pan_char_t const* p1, int l1 /* -1 => 'strlen(p1)' */
 , pan_char_t const* p2, int l2 /* -1 => 'strlen(p2)' */
 , pan_char_t const* p3, int l3 /* -1 => 'strlen(p3)' */
 , pan_char_t const* p4, int l4 /* -1 => 'strlen(p4)' */
 , pan_char_t const* p5, int l5 /* -1 => 'strlen(p5)' */
 , pan_char_t const* p6, int l6 /* -1 => 'strlen(p6)' */
 , pan_char_t const* p7, int l7 /* -1 => 'strlen(p7)' */
 , pan_char_t const* p8, int l8 /* -1 => 'strlen(p8)' */
);

/** Logs 10 parameters, subject to external (not in \ref group__core_library) severity-level filtering 
 * \ingroup group__application_layer_interface__generated
 */
PANTHEIOS_CALL(int) pantheios_log_10(pan_sev_t severity
 , pan_char_t const* p0, int l0 /* -1 => 'strlen(p0)' */
 , pan_char_t const* p1, int l1 /* -1 => 'strlen(p1)' */
 , pan_char_t const* p2, int l2 /* -1 => 'strlen(p2)' */
 , pan_char_t const* p3, int l3 /* -1 => 'strlen(p3)' */
 , pan_char_t const* p4, int l4 /* -1 => 'strlen(p4)' */
 , pan_char_t const* p5, int l5 /* -1 => 'strlen(p5)' */
 , pan_char_t const* p6, int l6 /* -1 => 'strlen(p6)' */
 , pan_char_t const* p7, int l7 /* -1 => 'strlen(p7)' */
 , pan_char_t const* p8, int l8 /* -1 => 'strlen(p8)' */
 , pan_char_t const* p9, int l9 /* -1 => 'strlen(p9)' */
);

/** Logs 11 parameters, subject to external (not in \ref group__core_library) severity-level filtering 
 * \ingroup group__application_layer_interface__generated
 */
PANTHEIOS_CALL(int) pantheios_log_11(pan_sev_t severity
 , pan_char_t const* p0, int l0 /* -1 => 'strlen(p0)' */
 , pan_char_t const* p1, int l1 /* -1 => 'strlen(p1)' */
 , pan_char_t const* p2, int l2 /* -1 => 'strlen(p2)' */
 , pan_char_t const* p3, int l3 /* -1 => 'strlen(p3)' */
 , pan_char_t const* p4, int l4 /* -1 => 'strlen(p4)' */
 , pan_char_t const* p5, int l5 /* -1 => 'strlen(p5)' */
 , pan_char_t const* p6, int l6 /* -1 => 'strlen(p6)' */
 , pan_char_t const* p7, int l7 /* -1 => 'strlen(p7)' */
 , pan_char_t const* p8, int l8 /* -1 => 'strlen(p8)' */
 , pan_char_t const* p9, int l9 /* -1 => 'strlen(p9)' */
 , pan_char_t const* p10, int l10 /* -1 => 'strlen(p10)' */
);

/** Logs 12 parameters, subject to external (not in \ref group__core_library) severity-level filtering 
 * \ingroup group__application_layer_interface__generated
 */
PANTHEIOS_CALL(int) pantheios_log_12(pan_sev_t severity
 , pan_char_t const* p0, int l0 /* -1 => 'strlen(p0)' */
 , pan_char_t const* p1, int l1 /* -1 => 'strlen(p1)' */
 , pan_char_t const* p2, int l2 /* -1 => 'strlen(p2)' */
 , pan_char_t const* p3, int l3 /* -1 => 'strlen(p3)' */
 , pan_char_t const* p4, int l4 /* -1 => 'strlen(p4)' */
 , pan_char_t const* p5, int l5 /* -1 => 'strlen(p5)' */
 , pan_char_t const* p6, int l6 /* -1 => 'strlen(p6)' */
 , pan_char_t const* p7, int l7 /* -1 => 'strlen(p7)' */
 , pan_char_t const* p8, int l8 /* -1 => 'strlen(p8)' */
 , pan_char_t const* p9, int l9 /* -1 => 'strlen(p9)' */
 , pan_char_t const* p10, int l10 /* -1 => 'strlen(p10)' */
 , pan_char_t const* p11, int l11 /* -1 => 'strlen(p11)' */
);

/** Logs 13 parameters, subject to external (not in \ref group__core_library) severity-level filtering 
 * \ingroup group__application_layer_interface__generated
 */
PANTHEIOS_CALL(int) pantheios_log_13(pan_sev_t severity
 , pan_char_t const* p0, int l0 /* -1 => 'strlen(p0)' */
 , pan_char_t const* p1, int l1 /* -1 => 'strlen(p1)' */
 , pan_char_t const* p2, int l2 /* -1 => 'strlen(p2)' */
 , pan_char_t const* p3, int l3 /* -1 => 'strlen(p3)' */
 , pan_char_t const* p4, int l4 /* -1 => 'strlen(p4)' */
 , pan_char_t const* p5, int l5 /* -1 => 'strlen(p5)' */
 , pan_char_t const* p6, int l6 /* -1 => 'strlen(p6)' */
 , pan_char_t const* p7, int l7 /* -1 => 'strlen(p7)' */
 , pan_char_t const* p8, int l8 /* -1 => 'strlen(p8)' */
 , pan_char_t const* p9, int l9 /* -1 => 'strlen(p9)' */
 , pan_char_t const* p10, int l10 /* -1 => 'strlen(p10)' */
 , pan_char_t const* p11, int l11 /* -1 => 'strlen(p11)' */
 , pan_char_t const* p12, int l12 /* -1 => 'strlen(p12)' */
);

/** Logs 14 parameters, subject to external (not in \ref group__core_library) severity-level filtering 
 * \ingroup group__application_layer_interface__generated
 */
PANTHEIOS_CALL(int) pantheios_log_14(pan_sev_t severity
 , pan_char_t const* p0, int l0 /* -1 => 'strlen(p0)' */
 , pan_char_t const* p1, int l1 /* -1 => 'strlen(p1)' */
 , pan_char_t const* p2, int l2 /* -1 => 'strlen(p2)' */
 , pan_char_t const* p3, int l3 /* -1 => 'strlen(p3)' */
 , pan_char_t const* p4, int l4 /* -1 => 'strlen(p4)' */
 , pan_char_t const* p5, int l5 /* -1 => 'strlen(p5)' */
 , pan_char_t const* p6, int l6 /* -1 => 'strlen(p6)' */
 , pan_char_t const* p7, int l7 /* -1 => 'strlen(p7)' */
 , pan_char_t const* p8, int l8 /* -1 => 'strlen(p8)' */
 , pan_char_t const* p9, int l9 /* -1 => 'strlen(p9)' */
 , pan_char_t const* p10, int l10 /* -1 => 'strlen(p10)' */
 , pan_char_t const* p11, int l11 /* -1 => 'strlen(p11)' */
 , pan_char_t const* p12, int l12 /* -1 => 'strlen(p12)' */
 , pan_char_t const* p13, int l13 /* -1 => 'strlen(p13)' */
);

/** Logs 15 parameters, subject to external (not in \ref group__core_library) severity-level filtering 
 * \ingroup group__application_layer_interface__generated
 */
PANTHEIOS_CALL(int) pantheios_log_15(pan_sev_t severity
 , pan_char_t const* p0, int l0 /* -1 => 'strlen(p0)' */
 , pan_char_t const* p1, int l1 /* -1 => 'strlen(p1)' */
 , pan_char_t const* p2, int l2 /* -1 => 'strlen(p2)' */
 , pan_char_t const* p3, int l3 /* -1 => 'strlen(p3)' */
 , pan_char_t const* p4, int l4 /* -1 => 'strlen(p4)' */
 , pan_char_t const* p5, int l5 /* -1 => 'strlen(p5)' */
 , pan_char_t const* p6, int l6 /* -1 => 'strlen(p6)' */
 , pan_char_t const* p7, int l7 /* -1 => 'strlen(p7)' */
 , pan_char_t const* p8, int l8 /* -1 => 'strlen(p8)' */
 , pan_char_t const* p9, int l9 /* -1 => 'strlen(p9)' */
 , pan_char_t const* p10, int l10 /* -1 => 'strlen(p10)' */
 , pan_char_t const* p11, int l11 /* -1 => 'strlen(p11)' */
 , pan_char_t const* p12, int l12 /* -1 => 'strlen(p12)' */
 , pan_char_t const* p13, int l13 /* -1 => 'strlen(p13)' */
 , pan_char_t const* p14, int l14 /* -1 => 'strlen(p14)' */
);

/** Logs 16 parameters, subject to external (not in \ref group__core_library) severity-level filtering 
 * \ingroup group__application_layer_interface__generated
 */
PANTHEIOS_CALL(int) pantheios_log_16(pan_sev_t severity
 , pan_char_t const* p0, int l0 /* -1 => 'strlen(p0)' */
 , pan_char_t const* p1, int l1 /* -1 => 'strlen(p1)' */
 , pan_char_t const* p2, int l2 /* -1 => 'strlen(p2)' */
 , pan_char_t const* p3, int l3 /* -1 => 'strlen(p3)' */
 , pan_char_t const* p4, int l4 /* -1 => 'strlen(p4)' */
 , pan_char_t const* p5, int l5 /* -1 => 'strlen(p5)' */
 , pan_char_t const* p6, int l6 /* -1 => 'strlen(p6)' */
 , pan_char_t const* p7, int l7 /* -1 => 'strlen(p7)' */
 , pan_char_t const* p8, int l8 /* -1 => 'strlen(p8)' */
 , pan_char_t const* p9, int l9 /* -1 => 'strlen(p9)' */
 , pan_char_t const* p10, int l10 /* -1 => 'strlen(p10)' */
 , pan_char_t const* p11, int l11 /* -1 => 'strlen(p11)' */
 , pan_char_t const* p12, int l12 /* -1 => 'strlen(p12)' */
 , pan_char_t const* p13, int l13 /* -1 => 'strlen(p13)' */
 , pan_char_t const* p14, int l14 /* -1 => 'strlen(p14)' */
 , pan_char_t const* p15, int l15 /* -1 => 'strlen(p15)' */
);

/** Logs 17 parameters, subject to external (not in \ref group__core_library) severity-level filtering 
 * \ingroup group__application_layer_interface__generated
 */
PANTHEIOS_CALL(int) pantheios_log_17(pan_sev_t severity
 , pan_char_t const* p0, int l0 /* -1 => 'strlen(p0)' */
 , pan_char_t const* p1, int l1 /* -1 => 'strlen(p1)' */
 , pan_char_t const* p2, int l2 /* -1 => 'strlen(p2)' */
 , pan_char_t const* p3, int l3 /* -1 => 'strlen(p3)' */
 , pan_char_t const* p4, int l4 /* -1 => 'strlen(p4)' */
 , pan_char_t const* p5, int l5 /* -1 => 'strlen(p5)' */
 , pan_char_t const* p6, int l6 /* -1 => 'strlen(p6)' */
 , pan_char_t const* p7, int l7 /* -1 => 'strlen(p7)' */
 , pan_char_t const* p8, int l8 /* -1 => 'strlen(p8)' */
 , pan_char_t const* p9, int l9 /* -1 => 'strlen(p9)' */
 , pan_char_t const* p10, int l10 /* -1 => 'strlen(p10)' */
 , pan_char_t const* p11, int l11 /* -1 => 'strlen(p11)' */
 , pan_char_t const* p12, int l12 /* -1 => 'strlen(p12)' */
 , pan_char_t const* p13, int l13 /* -1 => 'strlen(p13)' */
 , pan_char_t const* p14, int l14 /* -1 => 'strlen(p14)' */
 , pan_char_t const* p15, int l15 /* -1 => 'strlen(p15)' */
 , pan_char_t const* p16, int l16 /* -1 => 'strlen(p16)' */
);

/** Logs 18 parameters, subject to external (not in \ref group__core_library) severity-level filtering 
 * \ingroup group__application_layer_interface__generated
 */
PANTHEIOS_CALL(int) pantheios_log_18(pan_sev_t severity
 , pan_char_t const* p0, int l0 /* -1 => 'strlen(p0)' */
 , pan_char_t const* p1, int l1 /* -1 => 'strlen(p1)' */
 , pan_char_t const* p2, int l2 /* -1 => 'strlen(p2)' */
 , pan_char_t const* p3, int l3 /* -1 => 'strlen(p3)' */
 , pan_char_t const* p4, int l4 /* -1 => 'strlen(p4)' */
 , pan_char_t const* p5, int l5 /* -1 => 'strlen(p5)' */
 , pan_char_t const* p6, int l6 /* -1 => 'strlen(p6)' */
 , pan_char_t const* p7, int l7 /* -1 => 'strlen(p7)' */
 , pan_char_t const* p8, int l8 /* -1 => 'strlen(p8)' */
 , pan_char_t const* p9, int l9 /* -1 => 'strlen(p9)' */
 , pan_char_t const* p10, int l10 /* -1 => 'strlen(p10)' */
 , pan_char_t const* p11, int l11 /* -1 => 'strlen(p11)' */
 , pan_char_t const* p12, int l12 /* -1 => 'strlen(p12)' */
 , pan_char_t const* p13, int l13 /* -1 => 'strlen(p13)' */
 , pan_char_t const* p14, int l14 /* -1 => 'strlen(p14)' */
 , pan_char_t const* p15, int l15 /* -1 => 'strlen(p15)' */
 , pan_char_t const* p16, int l16 /* -1 => 'strlen(p16)' */
 , pan_char_t const* p17, int l17 /* -1 => 'strlen(p17)' */
);

/** Logs 19 parameters, subject to external (not in \ref group__core_library) severity-level filtering 
 * \ingroup group__application_layer_interface__generated
 */
PANTHEIOS_CALL(int) pantheios_log_19(pan_sev_t severity
 , pan_char_t const* p0, int l0 /* -1 => 'strlen(p0)' */
 , pan_char_t const* p1, int l1 /* -1 => 'strlen(p1)' */
 , pan_char_t const* p2, int l2 /* -1 => 'strlen(p2)' */
 , pan_char_t const* p3, int l3 /* -1 => 'strlen(p3)' */
 , pan_char_t const* p4, int l4 /* -1 => 'strlen(p4)' */
 , pan_char_t const* p5, int l5 /* -1 => 'strlen(p5)' */
 , pan_char_t const* p6, int l6 /* -1 => 'strlen(p6)' */
 , pan_char_t const* p7, int l7 /* -1 => 'strlen(p7)' */
 , pan_char_t const* p8, int l8 /* -1 => 'strlen(p8)' */
 , pan_char_t const* p9, int l9 /* -1 => 'strlen(p9)' */
 , pan_char_t const* p10, int l10 /* -1 => 'strlen(p10)' */
 , pan_char_t const* p11, int l11 /* -1 => 'strlen(p11)' */
 , pan_char_t const* p12, int l12 /* -1 => 'strlen(p12)' */
 , pan_char_t const* p13, int l13 /* -1 => 'strlen(p13)' */
 , pan_char_t const* p14, int l14 /* -1 => 'strlen(p14)' */
 , pan_char_t const* p15, int l15 /* -1 => 'strlen(p15)' */
 , pan_char_t const* p16, int l16 /* -1 => 'strlen(p16)' */
 , pan_char_t const* p17, int l17 /* -1 => 'strlen(p17)' */
 , pan_char_t const* p18, int l18 /* -1 => 'strlen(p18)' */
);

/** Logs 20 parameters, subject to external (not in \ref group__core_library) severity-level filtering 
 * \ingroup group__application_layer_interface__generated
 */
PANTHEIOS_CALL(int) pantheios_log_20(pan_sev_t severity
 , pan_char_t const* p0, int l0 /* -1 => 'strlen(p0)' */
 , pan_char_t const* p1, int l1 /* -1 => 'strlen(p1)' */
 , pan_char_t const* p2, int l2 /* -1 => 'strlen(p2)' */
 , pan_char_t const* p3, int l3 /* -1 => 'strlen(p3)' */
 , pan_char_t const* p4, int l4 /* -1 => 'strlen(p4)' */
 , pan_char_t const* p5, int l5 /* -1 => 'strlen(p5)' */
 , pan_char_t const* p6, int l6 /* -1 => 'strlen(p6)' */
 , pan_char_t const* p7, int l7 /* -1 => 'strlen(p7)' */
 , pan_char_t const* p8, int l8 /* -1 => 'strlen(p8)' */
 , pan_char_t const* p9, int l9 /* -1 => 'strlen(p9)' */
 , pan_char_t const* p10, int l10 /* -1 => 'strlen(p10)' */
 , pan_char_t const* p11, int l11 /* -1 => 'strlen(p11)' */
 , pan_char_t const* p12, int l12 /* -1 => 'strlen(p12)' */
 , pan_char_t const* p13, int l13 /* -1 => 'strlen(p13)' */
 , pan_char_t const* p14, int l14 /* -1 => 'strlen(p14)' */
 , pan_char_t const* p15, int l15 /* -1 => 'strlen(p15)' */
 , pan_char_t const* p16, int l16 /* -1 => 'strlen(p16)' */
 , pan_char_t const* p17, int l17 /* -1 => 'strlen(p17)' */
 , pan_char_t const* p18, int l18 /* -1 => 'strlen(p18)' */
 , pan_char_t const* p19, int l19 /* -1 => 'strlen(p19)' */
);

/** Logs 21 parameters, subject to external (not in \ref group__core_library) severity-level filtering 
 * \ingroup group__application_layer_interface__generated
 */
PANTHEIOS_CALL(int) pantheios_log_21(pan_sev_t severity
 , pan_char_t const* p0, int l0 /* -1 => 'strlen(p0)' */
 , pan_char_t const* p1, int l1 /* -1 => 'strlen(p1)' */
 , pan_char_t const* p2, int l2 /* -1 => 'strlen(p2)' */
 , pan_char_t const* p3, int l3 /* -1 => 'strlen(p3)' */
 , pan_char_t const* p4, int l4 /* -1 => 'strlen(p4)' */
 , pan_char_t const* p5, int l5 /* -1 => 'strlen(p5)' */
 , pan_char_t const* p6, int l6 /* -1 => 'strlen(p6)' */
 , pan_char_t const* p7, int l7 /* -1 => 'strlen(p7)' */
 , pan_char_t const* p8, int l8 /* -1 => 'strlen(p8)' */
 , pan_char_t const* p9, int l9 /* -1 => 'strlen(p9)' */
 , pan_char_t const* p10, int l10 /* -1 => 'strlen(p10)' */
 , pan_char_t const* p11, int l11 /* -1 => 'strlen(p11)' */
 , pan_char_t const* p12, int l12 /* -1 => 'strlen(p12)' */
 , pan_char_t const* p13, int l13 /* -1 => 'strlen(p13)' */
 , pan_char_t const* p14, int l14 /* -1 => 'strlen(p14)' */
 , pan_char_t const* p15, int l15 /* -1 => 'strlen(p15)' */
 , pan_char_t const* p16, int l16 /* -1 => 'strlen(p16)' */
 , pan_char_t const* p17, int l17 /* -1 => 'strlen(p17)' */
 , pan_char_t const* p18, int l18 /* -1 => 'strlen(p18)' */
 , pan_char_t const* p19, int l19 /* -1 => 'strlen(p19)' */
 , pan_char_t const* p20, int l20 /* -1 => 'strlen(p20)' */
);

/** Logs 22 parameters, subject to external (not in \ref group__core_library) severity-level filtering 
 * \ingroup group__application_layer_interface__generated
 */
PANTHEIOS_CALL(int) pantheios_log_22(pan_sev_t severity
 , pan_char_t const* p0, int l0 /* -1 => 'strlen(p0)' */
 , pan_char_t const* p1, int l1 /* -1 => 'strlen(p1)' */
 , pan_char_t const* p2, int l2 /* -1 => 'strlen(p2)' */
 , pan_char_t const* p3, int l3 /* -1 => 'strlen(p3)' */
 , pan_char_t const* p4, int l4 /* -1 => 'strlen(p4)' */
 , pan_char_t const* p5, int l5 /* -1 => 'strlen(p5)' */
 , pan_char_t const* p6, int l6 /* -1 => 'strlen(p6)' */
 , pan_char_t const* p7, int l7 /* -1 => 'strlen(p7)' */
 , pan_char_t const* p8, int l8 /* -1 => 'strlen(p8)' */
 , pan_char_t const* p9, int l9 /* -1 => 'strlen(p9)' */
 , pan_char_t const* p10, int l10 /* -1 => 'strlen(p10)' */
 , pan_char_t const* p11, int l11 /* -1 => 'strlen(p11)' */
 , pan_char_t const* p12, int l12 /* -1 => 'strlen(p12)' */
 , pan_char_t const* p13, int l13 /* -1 => 'strlen(p13)' */
 , pan_char_t const* p14, int l14 /* -1 => 'strlen(p14)' */
 , pan_char_t const* p15, int l15 /* -1 => 'strlen(p15)' */
 , pan_char_t const* p16, int l16 /* -1 => 'strlen(p16)' */
 , pan_char_t const* p17, int l17 /* -1 => 'strlen(p17)' */
 , pan_char_t const* p18, int l18 /* -1 => 'strlen(p18)' */
 , pan_char_t const* p19, int l19 /* -1 => 'strlen(p19)' */
 , pan_char_t const* p20, int l20 /* -1 => 'strlen(p20)' */
 , pan_char_t const* p21, int l21 /* -1 => 'strlen(p21)' */
);

/** Logs 23 parameters, subject to external (not in \ref group__core_library) severity-level filtering 
 * \ingroup group__application_layer_interface__generated
 */
PANTHEIOS_CALL(int) pantheios_log_23(pan_sev_t severity
 , pan_char_t const* p0, int l0 /* -1 => 'strlen(p0)' */
 , pan_char_t const* p1, int l1 /* -1 => 'strlen(p1)' */
 , pan_char_t const* p2, int l2 /* -1 => 'strlen(p2)' */
 , pan_char_t const* p3, int l3 /* -1 => 'strlen(p3)' */
 , pan_char_t const* p4, int l4 /* -1 => 'strlen(p4)' */
 , pan_char_t const* p5, int l5 /* -1 => 'strlen(p5)' */
 , pan_char_t const* p6, int l6 /* -1 => 'strlen(p6)' */
 , pan_char_t const* p7, int l7 /* -1 => 'strlen(p7)' */
 , pan_char_t const* p8, int l8 /* -1 => 'strlen(p8)' */
 , pan_char_t const* p9, int l9 /* -1 => 'strlen(p9)' */
 , pan_char_t const* p10, int l10 /* -1 => 'strlen(p10)' */
 , pan_char_t const* p11, int l11 /* -1 => 'strlen(p11)' */
 , pan_char_t const* p12, int l12 /* -1 => 'strlen(p12)' */
 , pan_char_t const* p13, int l13 /* -1 => 'strlen(p13)' */
 , pan_char_t const* p14, int l14 /* -1 => 'strlen(p14)' */
 , pan_char_t const* p15, int l15 /* -1 => 'strlen(p15)' */
 , pan_char_t const* p16, int l16 /* -1 => 'strlen(p16)' */
 , pan_char_t const* p17, int l17 /* -1 => 'strlen(p17)' */
 , pan_char_t const* p18, int l18 /* -1 => 'strlen(p18)' */
 , pan_char_t const* p19, int l19 /* -1 => 'strlen(p19)' */
 , pan_char_t const* p20, int l20 /* -1 => 'strlen(p20)' */
 , pan_char_t const* p21, int l21 /* -1 => 'strlen(p21)' */
 , pan_char_t const* p22, int l22 /* -1 => 'strlen(p22)' */
);

/** Logs 24 parameters, subject to external (not in \ref group__core_library) severity-level filtering 
 * \ingroup group__application_layer_interface__generated
 */
PANTHEIOS_CALL(int) pantheios_log_24(pan_sev_t severity
 , pan_char_t const* p0, int l0 /* -1 => 'strlen(p0)' */
 , pan_char_t const* p1, int l1 /* -1 => 'strlen(p1)' */
 , pan_char_t const* p2, int l2 /* -1 => 'strlen(p2)' */
 , pan_char_t const* p3, int l3 /* -1 => 'strlen(p3)' */
 , pan_char_t const* p4, int l4 /* -1 => 'strlen(p4)' */
 , pan_char_t const* p5, int l5 /* -1 => 'strlen(p5)' */
 , pan_char_t const* p6, int l6 /* -1 => 'strlen(p6)' */
 , pan_char_t const* p7, int l7 /* -1 => 'strlen(p7)' */
 , pan_char_t const* p8, int l8 /* -1 => 'strlen(p8)' */
 , pan_char_t const* p9, int l9 /* -1 => 'strlen(p9)' */
 , pan_char_t const* p10, int l10 /* -1 => 'strlen(p10)' */
 , pan_char_t const* p11, int l11 /* -1 => 'strlen(p11)' */
 , pan_char_t const* p12, int l12 /* -1 => 'strlen(p12)' */
 , pan_char_t const* p13, int l13 /* -1 => 'strlen(p13)' */
 , pan_char_t const* p14, int l14 /* -1 => 'strlen(p14)' */
 , pan_char_t const* p15, int l15 /* -1 => 'strlen(p15)' */
 , pan_char_t const* p16, int l16 /* -1 => 'strlen(p16)' */
 , pan_char_t const* p17, int l17 /* -1 => 'strlen(p17)' */
 , pan_char_t const* p18, int l18 /* -1 => 'strlen(p18)' */
 , pan_char_t const* p19, int l19 /* -1 => 'strlen(p19)' */
 , pan_char_t const* p20, int l20 /* -1 => 'strlen(p20)' */
 , pan_char_t const* p21, int l21 /* -1 => 'strlen(p21)' */
 , pan_char_t const* p22, int l22 /* -1 => 'strlen(p22)' */
 , pan_char_t const* p23, int l23 /* -1 => 'strlen(p23)' */
);

/** Logs 25 parameters, subject to external (not in \ref group__core_library) severity-level filtering 
 * \ingroup group__application_layer_interface__generated
 */
PANTHEIOS_CALL(int) pantheios_log_25(pan_sev_t severity
 , pan_char_t const* p0, int l0 /* -1 => 'strlen(p0)' */
 , pan_char_t const* p1, int l1 /* -1 => 'strlen(p1)' */
 , pan_char_t const* p2, int l2 /* -1 => 'strlen(p2)' */
 , pan_char_t const* p3, int l3 /* -1 => 'strlen(p3)' */
 , pan_char_t const* p4, int l4 /* -1 => 'strlen(p4)' */
 , pan_char_t const* p5, int l5 /* -1 => 'strlen(p5)' */
 , pan_char_t const* p6, int l6 /* -1 => 'strlen(p6)' */
 , pan_char_t const* p7, int l7 /* -1 => 'strlen(p7)' */
 , pan_char_t const* p8, int l8 /* -1 => 'strlen(p8)' */
 , pan_char_t const* p9, int l9 /* -1 => 'strlen(p9)' */
 , pan_char_t const* p10, int l10 /* -1 => 'strlen(p10)' */
 , pan_char_t const* p11, int l11 /* -1 => 'strlen(p11)' */
 , pan_char_t const* p12, int l12 /* -1 => 'strlen(p12)' */
 , pan_char_t const* p13, int l13 /* -1 => 'strlen(p13)' */
 , pan_char_t const* p14, int l14 /* -1 => 'strlen(p14)' */
 , pan_char_t const* p15, int l15 /* -1 => 'strlen(p15)' */
 , pan_char_t const* p16, int l16 /* -1 => 'strlen(p16)' */
 , pan_char_t const* p17, int l17 /* -1 => 'strlen(p17)' */
 , pan_char_t const* p18, int l18 /* -1 => 'strlen(p18)' */
 , pan_char_t const* p19, int l19 /* -1 => 'strlen(p19)' */
 , pan_char_t const* p20, int l20 /* -1 => 'strlen(p20)' */
 , pan_char_t const* p21, int l21 /* -1 => 'strlen(p21)' */
 , pan_char_t const* p22, int l22 /* -1 => 'strlen(p22)' */
 , pan_char_t const* p23, int l23 /* -1 => 'strlen(p23)' */
 , pan_char_t const* p24, int l24 /* -1 => 'strlen(p24)' */
);

/** Logs 26 parameters, subject to external (not in \ref group__core_library) severity-level filtering 
 * \ingroup group__application_layer_interface__generated
 */
PANTHEIOS_CALL(int) pantheios_log_26(pan_sev_t severity
 , pan_char_t const* p0, int l0 /* -1 => 'strlen(p0)' */
 , pan_char_t const* p1, int l1 /* -1 => 'strlen(p1)' */
 , pan_char_t const* p2, int l2 /* -1 => 'strlen(p2)' */
 , pan_char_t const* p3, int l3 /* -1 => 'strlen(p3)' */
 , pan_char_t const* p4, int l4 /* -1 => 'strlen(p4)' */
 , pan_char_t const* p5, int l5 /* -1 => 'strlen(p5)' */
 , pan_char_t const* p6, int l6 /* -1 => 'strlen(p6)' */
 , pan_char_t const* p7, int l7 /* -1 => 'strlen(p7)' */
 , pan_char_t const* p8, int l8 /* -1 => 'strlen(p8)' */
 , pan_char_t const* p9, int l9 /* -1 => 'strlen(p9)' */
 , pan_char_t const* p10, int l10 /* -1 => 'strlen(p10)' */
 , pan_char_t const* p11, int l11 /* -1 => 'strlen(p11)' */
 , pan_char_t const* p12, int l12 /* -1 => 'strlen(p12)' */
 , pan_char_t const* p13, int l13 /* -1 => 'strlen(p13)' */
 , pan_char_t const* p14, int l14 /* -1 => 'strlen(p14)' */
 , pan_char_t const* p15, int l15 /* -1 => 'strlen(p15)' */
 , pan_char_t const* p16, int l16 /* -1 => 'strlen(p16)' */
 , pan_char_t const* p17, int l17 /* -1 => 'strlen(p17)' */
 , pan_char_t const* p18, int l18 /* -1 => 'strlen(p18)' */
 , pan_char_t const* p19, int l19 /* -1 => 'strlen(p19)' */
 , pan_char_t const* p20, int l20 /* -1 => 'strlen(p20)' */
 , pan_char_t const* p21, int l21 /* -1 => 'strlen(p21)' */
 , pan_char_t const* p22, int l22 /* -1 => 'strlen(p22)' */
 , pan_char_t const* p23, int l23 /* -1 => 'strlen(p23)' */
 , pan_char_t const* p24, int l24 /* -1 => 'strlen(p24)' */
 , pan_char_t const* p25, int l25 /* -1 => 'strlen(p25)' */
);

/** Logs 27 parameters, subject to external (not in \ref group__core_library) severity-level filtering 
 * \ingroup group__application_layer_interface__generated
 */
PANTHEIOS_CALL(int) pantheios_log_27(pan_sev_t severity
 , pan_char_t const* p0, int l0 /* -1 => 'strlen(p0)' */
 , pan_char_t const* p1, int l1 /* -1 => 'strlen(p1)' */
 , pan_char_t const* p2, int l2 /* -1 => 'strlen(p2)' */
 , pan_char_t const* p3, int l3 /* -1 => 'strlen(p3)' */
 , pan_char_t const* p4, int l4 /* -1 => 'strlen(p4)' */
 , pan_char_t const* p5, int l5 /* -1 => 'strlen(p5)' */
 , pan_char_t const* p6, int l6 /* -1 => 'strlen(p6)' */
 , pan_char_t const* p7, int l7 /* -1 => 'strlen(p7)' */
 , pan_char_t const* p8, int l8 /* -1 => 'strlen(p8)' */
 , pan_char_t const* p9, int l9 /* -1 => 'strlen(p9)' */
 , pan_char_t const* p10, int l10 /* -1 => 'strlen(p10)' */
 , pan_char_t const* p11, int l11 /* -1 => 'strlen(p11)' */
 , pan_char_t const* p12, int l12 /* -1 => 'strlen(p12)' */
 , pan_char_t const* p13, int l13 /* -1 => 'strlen(p13)' */
 , pan_char_t const* p14, int l14 /* -1 => 'strlen(p14)' */
 , pan_char_t const* p15, int l15 /* -1 => 'strlen(p15)' */
 , pan_char_t const* p16, int l16 /* -1 => 'strlen(p16)' */
 , pan_char_t const* p17, int l17 /* -1 => 'strlen(p17)' */
 , pan_char_t const* p18, int l18 /* -1 => 'strlen(p18)' */
 , pan_char_t const* p19, int l19 /* -1 => 'strlen(p19)' */
 , pan_char_t const* p20, int l20 /* -1 => 'strlen(p20)' */
 , pan_char_t const* p21, int l21 /* -1 => 'strlen(p21)' */
 , pan_char_t const* p22, int l22 /* -1 => 'strlen(p22)' */
 , pan_char_t const* p23, int l23 /* -1 => 'strlen(p23)' */
 , pan_char_t const* p24, int l24 /* -1 => 'strlen(p24)' */
 , pan_char_t const* p25, int l25 /* -1 => 'strlen(p25)' */
 , pan_char_t const* p26, int l26 /* -1 => 'strlen(p26)' */
);

/** Logs 28 parameters, subject to external (not in \ref group__core_library) severity-level filtering 
 * \ingroup group__application_layer_interface__generated
 */
PANTHEIOS_CALL(int) pantheios_log_28(pan_sev_t severity
 , pan_char_t const* p0, int l0 /* -1 => 'strlen(p0)' */
 , pan_char_t const* p1, int l1 /* -1 => 'strlen(p1)' */
 , pan_char_t const* p2, int l2 /* -1 => 'strlen(p2)' */
 , pan_char_t const* p3, int l3 /* -1 => 'strlen(p3)' */
 , pan_char_t const* p4, int l4 /* -1 => 'strlen(p4)' */
 , pan_char_t const* p5, int l5 /* -1 => 'strlen(p5)' */
 , pan_char_t const* p6, int l6 /* -1 => 'strlen(p6)' */
 , pan_char_t const* p7, int l7 /* -1 => 'strlen(p7)' */
 , pan_char_t const* p8, int l8 /* -1 => 'strlen(p8)' */
 , pan_char_t const* p9, int l9 /* -1 => 'strlen(p9)' */
 , pan_char_t const* p10, int l10 /* -1 => 'strlen(p10)' */
 , pan_char_t const* p11, int l11 /* -1 => 'strlen(p11)' */
 , pan_char_t const* p12, int l12 /* -1 => 'strlen(p12)' */
 , pan_char_t const* p13, int l13 /* -1 => 'strlen(p13)' */
 , pan_char_t const* p14, int l14 /* -1 => 'strlen(p14)' */
 , pan_char_t const* p15, int l15 /* -1 => 'strlen(p15)' */
 , pan_char_t const* p16, int l16 /* -1 => 'strlen(p16)' */
 , pan_char_t const* p17, int l17 /* -1 => 'strlen(p17)' */
 , pan_char_t const* p18, int l18 /* -1 => 'strlen(p18)' */
 , pan_char_t const* p19, int l19 /* -1 => 'strlen(p19)' */
 , pan_char_t const* p20, int l20 /* -1 => 'strlen(p20)' */
 , pan_char_t const* p21, int l21 /* -1 => 'strlen(p21)' */
 , pan_char_t const* p22, int l22 /* -1 => 'strlen(p22)' */
 , pan_char_t const* p23, int l23 /* -1 => 'strlen(p23)' */
 , pan_char_t const* p24, int l24 /* -1 => 'strlen(p24)' */
 , pan_char_t const* p25, int l25 /* -1 => 'strlen(p25)' */
 , pan_char_t const* p26, int l26 /* -1 => 'strlen(p26)' */
 , pan_char_t const* p27, int l27 /* -1 => 'strlen(p27)' */
);

/** Logs 29 parameters, subject to external (not in \ref group__core_library) severity-level filtering 
 * \ingroup group__application_layer_interface__generated
 */
PANTHEIOS_CALL(int) pantheios_log_29(pan_sev_t severity
 , pan_char_t const* p0, int l0 /* -1 => 'strlen(p0)' */
 , pan_char_t const* p1, int l1 /* -1 => 'strlen(p1)' */
 , pan_char_t const* p2, int l2 /* -1 => 'strlen(p2)' */
 , pan_char_t const* p3, int l3 /* -1 => 'strlen(p3)' */
 , pan_char_t const* p4, int l4 /* -1 => 'strlen(p4)' */
 , pan_char_t const* p5, int l5 /* -1 => 'strlen(p5)' */
 , pan_char_t const* p6, int l6 /* -1 => 'strlen(p6)' */
 , pan_char_t const* p7, int l7 /* -1 => 'strlen(p7)' */
 , pan_char_t const* p8, int l8 /* -1 => 'strlen(p8)' */
 , pan_char_t const* p9, int l9 /* -1 => 'strlen(p9)' */
 , pan_char_t const* p10, int l10 /* -1 => 'strlen(p10)' */
 , pan_char_t const* p11, int l11 /* -1 => 'strlen(p11)' */
 , pan_char_t const* p12, int l12 /* -1 => 'strlen(p12)' */
 , pan_char_t const* p13, int l13 /* -1 => 'strlen(p13)' */
 , pan_char_t const* p14, int l14 /* -1 => 'strlen(p14)' */
 , pan_char_t const* p15, int l15 /* -1 => 'strlen(p15)' */
 , pan_char_t const* p16, int l16 /* -1 => 'strlen(p16)' */
 , pan_char_t const* p17, int l17 /* -1 => 'strlen(p17)' */
 , pan_char_t const* p18, int l18 /* -1 => 'strlen(p18)' */
 , pan_char_t const* p19, int l19 /* -1 => 'strlen(p19)' */
 , pan_char_t const* p20, int l20 /* -1 => 'strlen(p20)' */
 , pan_char_t const* p21, int l21 /* -1 => 'strlen(p21)' */
 , pan_char_t const* p22, int l22 /* -1 => 'strlen(p22)' */
 , pan_char_t const* p23, int l23 /* -1 => 'strlen(p23)' */
 , pan_char_t const* p24, int l24 /* -1 => 'strlen(p24)' */
 , pan_char_t const* p25, int l25 /* -1 => 'strlen(p25)' */
 , pan_char_t const* p26, int l26 /* -1 => 'strlen(p26)' */
 , pan_char_t const* p27, int l27 /* -1 => 'strlen(p27)' */
 , pan_char_t const* p28, int l28 /* -1 => 'strlen(p28)' */
);

/** Logs 30 parameters, subject to external (not in \ref group__core_library) severity-level filtering 
 * \ingroup group__application_layer_interface__generated
 */
PANTHEIOS_CALL(int) pantheios_log_30(pan_sev_t severity
 , pan_char_t const* p0, int l0 /* -1 => 'strlen(p0)' */
 , pan_char_t const* p1, int l1 /* -1 => 'strlen(p1)' */
 , pan_char_t const* p2, int l2 /* -1 => 'strlen(p2)' */
 , pan_char_t const* p3, int l3 /* -1 => 'strlen(p3)' */
 , pan_char_t const* p4, int l4 /* -1 => 'strlen(p4)' */
 , pan_char_t const* p5, int l5 /* -1 => 'strlen(p5)' */
 , pan_char_t const* p6, int l6 /* -1 => 'strlen(p6)' */
 , pan_char_t const* p7, int l7 /* -1 => 'strlen(p7)' */
 , pan_char_t const* p8, int l8 /* -1 => 'strlen(p8)' */
 , pan_char_t const* p9, int l9 /* -1 => 'strlen(p9)' */
 , pan_char_t const* p10, int l10 /* -1 => 'strlen(p10)' */
 , pan_char_t const* p11, int l11 /* -1 => 'strlen(p11)' */
 , pan_char_t const* p12, int l12 /* -1 => 'strlen(p12)' */
 , pan_char_t const* p13, int l13 /* -1 => 'strlen(p13)' */
 , pan_char_t const* p14, int l14 /* -1 => 'strlen(p14)' */
 , pan_char_t const* p15, int l15 /* -1 => 'strlen(p15)' */
 , pan_char_t const* p16, int l16 /* -1 => 'strlen(p16)' */
 , pan_char_t const* p17, int l17 /* -1 => 'strlen(p17)' */
 , pan_char_t const* p18, int l18 /* -1 => 'strlen(p18)' */
 , pan_char_t const* p19, int l19 /* -1 => 'strlen(p19)' */
 , pan_char_t const* p20, int l20 /* -1 => 'strlen(p20)' */
 , pan_char_t const* p21, int l21 /* -1 => 'strlen(p21)' */
 , pan_char_t const* p22, int l22 /* -1 => 'strlen(p22)' */
 , pan_char_t const* p23, int l23 /* -1 => 'strlen(p23)' */
 , pan_char_t const* p24, int l24 /* -1 => 'strlen(p24)' */
 , pan_char_t const* p25, int l25 /* -1 => 'strlen(p25)' */
 , pan_char_t const* p26, int l26 /* -1 => 'strlen(p26)' */
 , pan_char_t const* p27, int l27 /* -1 => 'strlen(p27)' */
 , pan_char_t const* p28, int l28 /* -1 => 'strlen(p28)' */
 , pan_char_t const* p29, int l29 /* -1 => 'strlen(p29)' */
);

/** Logs 31 parameters, subject to external (not in \ref group__core_library) severity-level filtering 
 * \ingroup group__application_layer_interface__generated
 */
PANTHEIOS_CALL(int) pantheios_log_31(pan_sev_t severity
 , pan_char_t const* p0, int l0 /* -1 => 'strlen(p0)' */
 , pan_char_t const* p1, int l1 /* -1 => 'strlen(p1)' */
 , pan_char_t const* p2, int l2 /* -1 => 'strlen(p2)' */
 , pan_char_t const* p3, int l3 /* -1 => 'strlen(p3)' */
 , pan_char_t const* p4, int l4 /* -1 => 'strlen(p4)' */
 , pan_char_t const* p5, int l5 /* -1 => 'strlen(p5)' */
 , pan_char_t const* p6, int l6 /* -1 => 'strlen(p6)' */
 , pan_char_t const* p7, int l7 /* -1 => 'strlen(p7)' */
 , pan_char_t const* p8, int l8 /* -1 => 'strlen(p8)' */
 , pan_char_t const* p9, int l9 /* -1 => 'strlen(p9)' */
 , pan_char_t const* p10, int l10 /* -1 => 'strlen(p10)' */
 , pan_char_t const* p11, int l11 /* -1 => 'strlen(p11)' */
 , pan_char_t const* p12, int l12 /* -1 => 'strlen(p12)' */
 , pan_char_t const* p13, int l13 /* -1 => 'strlen(p13)' */
 , pan_char_t const* p14, int l14 /* -1 => 'strlen(p14)' */
 , pan_char_t const* p15, int l15 /* -1 => 'strlen(p15)' */
 , pan_char_t const* p16, int l16 /* -1 => 'strlen(p16)' */
 , pan_char_t const* p17, int l17 /* -1 => 'strlen(p17)' */
 , pan_char_t const* p18, int l18 /* -1 => 'strlen(p18)' */
 , pan_char_t const* p19, int l19 /* -1 => 'strlen(p19)' */
 , pan_char_t const* p20, int l20 /* -1 => 'strlen(p20)' */
 , pan_char_t const* p21, int l21 /* -1 => 'strlen(p21)' */
 , pan_char_t const* p22, int l22 /* -1 => 'strlen(p22)' */
 , pan_char_t const* p23, int l23 /* -1 => 'strlen(p23)' */
 , pan_char_t const* p24, int l24 /* -1 => 'strlen(p24)' */
 , pan_char_t const* p25, int l25 /* -1 => 'strlen(p25)' */
 , pan_char_t const* p26, int l26 /* -1 => 'strlen(p26)' */
 , pan_char_t const* p27, int l27 /* -1 => 'strlen(p27)' */
 , pan_char_t const* p28, int l28 /* -1 => 'strlen(p28)' */
 , pan_char_t const* p29, int l29 /* -1 => 'strlen(p29)' */
 , pan_char_t const* p30, int l30 /* -1 => 'strlen(p30)' */
);

/** Logs 32 parameters, subject to external (not in \ref group__core_library) severity-level filtering 
 * \ingroup group__application_layer_interface__generated
 */
PANTHEIOS_CALL(int) pantheios_log_32(pan_sev_t severity
 , pan_char_t const* p0, int l0 /* -1 => 'strlen(p0)' */
 , pan_char_t const* p1, int l1 /* -1 => 'strlen(p1)' */
 , pan_char_t const* p2, int l2 /* -1 => 'strlen(p2)' */
 , pan_char_t const* p3, int l3 /* -1 => 'strlen(p3)' */
 , pan_char_t const* p4, int l4 /* -1 => 'strlen(p4)' */
 , pan_char_t const* p5, int l5 /* -1 => 'strlen(p5)' */
 , pan_char_t const* p6, int l6 /* -1 => 'strlen(p6)' */
 , pan_char_t const* p7, int l7 /* -1 => 'strlen(p7)' */
 , pan_char_t const* p8, int l8 /* -1 => 'strlen(p8)' */
 , pan_char_t const* p9, int l9 /* -1 => 'strlen(p9)' */
 , pan_char_t const* p10, int l10 /* -1 => 'strlen(p10)' */
 , pan_char_t const* p11, int l11 /* -1 => 'strlen(p11)' */
 , pan_char_t const* p12, int l12 /* -1 => 'strlen(p12)' */
 , pan_char_t const* p13, int l13 /* -1 => 'strlen(p13)' */
 , pan_char_t const* p14, int l14 /* -1 => 'strlen(p14)' */
 , pan_char_t const* p15, int l15 /* -1 => 'strlen(p15)' */
 , pan_char_t const* p16, int l16 /* -1 => 'strlen(p16)' */
 , pan_char_t const* p17, int l17 /* -1 => 'strlen(p17)' */
 , pan_char_t const* p18, int l18 /* -1 => 'strlen(p18)' */
 , pan_char_t const* p19, int l19 /* -1 => 'strlen(p19)' */
 , pan_char_t const* p20, int l20 /* -1 => 'strlen(p20)' */
 , pan_char_t const* p21, int l21 /* -1 => 'strlen(p21)' */
 , pan_char_t const* p22, int l22 /* -1 => 'strlen(p22)' */
 , pan_char_t const* p23, int l23 /* -1 => 'strlen(p23)' */
 , pan_char_t const* p24, int l24 /* -1 => 'strlen(p24)' */
 , pan_char_t const* p25, int l25 /* -1 => 'strlen(p25)' */
 , pan_char_t const* p26, int l26 /* -1 => 'strlen(p26)' */
 , pan_char_t const* p27, int l27 /* -1 => 'strlen(p27)' */
 , pan_char_t const* p28, int l28 /* -1 => 'strlen(p28)' */
 , pan_char_t const* p29, int l29 /* -1 => 'strlen(p29)' */
 , pan_char_t const* p30, int l30 /* -1 => 'strlen(p30)' */
 , pan_char_t const* p31, int l31 /* -1 => 'strlen(p31)' */
);

