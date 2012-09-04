/* /////////////////////////////////////////////////////////////////////////
 * File:        pantheios/internal/generated/log_dispatch_functions.hpp
 *
 * Purpose:     Inline definitions of the log_dispatch_<N>() functions
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


#ifndef PANTHEIOS_INCL_PANTHEIOS_HPP_PANTHEIOS
# error This file is included by the Pantheios API, and cannot be included directly
#endif /* !PANTHEIOS_INCL_PANTHEIOS_HPP_PANTHEIOS */


#if PANTHEIOS_APPL_PARAMS_LIMIT >= 1

inline int log_dispatch_1(pan_sev_t severity
 , size_t l0, pan_char_t const* p0
)
{
  return pantheios_log_1_no_test(severity
 , pan_slice_t(p0, l0)
 );
}

#if PANTHEIOS_APPL_PARAMS_LIMIT >= 2

inline int log_dispatch_2(pan_sev_t severity
 , size_t l0, pan_char_t const* p0
 , size_t l1, pan_char_t const* p1
)
{
  return pantheios_log_2_no_test(severity
 , pan_slice_t(p0, l0)
 , pan_slice_t(p1, l1)
 );
}

#if PANTHEIOS_APPL_PARAMS_LIMIT >= 3

inline int log_dispatch_3(pan_sev_t severity
 , size_t l0, pan_char_t const* p0
 , size_t l1, pan_char_t const* p1
 , size_t l2, pan_char_t const* p2
)
{
  return pantheios_log_3_no_test(severity
 , pan_slice_t(p0, l0)
 , pan_slice_t(p1, l1)
 , pan_slice_t(p2, l2)
 );
}

#if PANTHEIOS_APPL_PARAMS_LIMIT >= 4

inline int log_dispatch_4(pan_sev_t severity
 , size_t l0, pan_char_t const* p0
 , size_t l1, pan_char_t const* p1
 , size_t l2, pan_char_t const* p2
 , size_t l3, pan_char_t const* p3
)
{
  return pantheios_log_4_no_test(severity
 , pan_slice_t(p0, l0)
 , pan_slice_t(p1, l1)
 , pan_slice_t(p2, l2)
 , pan_slice_t(p3, l3)
 );
}

#if PANTHEIOS_APPL_PARAMS_LIMIT >= 5

inline int log_dispatch_5(pan_sev_t severity
 , size_t l0, pan_char_t const* p0
 , size_t l1, pan_char_t const* p1
 , size_t l2, pan_char_t const* p2
 , size_t l3, pan_char_t const* p3
 , size_t l4, pan_char_t const* p4
)
{
  return pantheios_log_5_no_test(severity
 , pan_slice_t(p0, l0)
 , pan_slice_t(p1, l1)
 , pan_slice_t(p2, l2)
 , pan_slice_t(p3, l3)
 , pan_slice_t(p4, l4)
 );
}

#if PANTHEIOS_APPL_PARAMS_LIMIT >= 6

inline int log_dispatch_6(pan_sev_t severity
 , size_t l0, pan_char_t const* p0
 , size_t l1, pan_char_t const* p1
 , size_t l2, pan_char_t const* p2
 , size_t l3, pan_char_t const* p3
 , size_t l4, pan_char_t const* p4
 , size_t l5, pan_char_t const* p5
)
{
  return pantheios_log_6_no_test(severity
 , pan_slice_t(p0, l0)
 , pan_slice_t(p1, l1)
 , pan_slice_t(p2, l2)
 , pan_slice_t(p3, l3)
 , pan_slice_t(p4, l4)
 , pan_slice_t(p5, l5)
 );
}

#if PANTHEIOS_APPL_PARAMS_LIMIT >= 7

inline int log_dispatch_7(pan_sev_t severity
 , size_t l0, pan_char_t const* p0
 , size_t l1, pan_char_t const* p1
 , size_t l2, pan_char_t const* p2
 , size_t l3, pan_char_t const* p3
 , size_t l4, pan_char_t const* p4
 , size_t l5, pan_char_t const* p5
 , size_t l6, pan_char_t const* p6
)
{
  return pantheios_log_7_no_test(severity
 , pan_slice_t(p0, l0)
 , pan_slice_t(p1, l1)
 , pan_slice_t(p2, l2)
 , pan_slice_t(p3, l3)
 , pan_slice_t(p4, l4)
 , pan_slice_t(p5, l5)
 , pan_slice_t(p6, l6)
 );
}

#if PANTHEIOS_APPL_PARAMS_LIMIT >= 8

inline int log_dispatch_8(pan_sev_t severity
 , size_t l0, pan_char_t const* p0
 , size_t l1, pan_char_t const* p1
 , size_t l2, pan_char_t const* p2
 , size_t l3, pan_char_t const* p3
 , size_t l4, pan_char_t const* p4
 , size_t l5, pan_char_t const* p5
 , size_t l6, pan_char_t const* p6
 , size_t l7, pan_char_t const* p7
)
{
  return pantheios_log_8_no_test(severity
 , pan_slice_t(p0, l0)
 , pan_slice_t(p1, l1)
 , pan_slice_t(p2, l2)
 , pan_slice_t(p3, l3)
 , pan_slice_t(p4, l4)
 , pan_slice_t(p5, l5)
 , pan_slice_t(p6, l6)
 , pan_slice_t(p7, l7)
 );
}

#if PANTHEIOS_APPL_PARAMS_LIMIT >= 9

inline int log_dispatch_9(pan_sev_t severity
 , size_t l0, pan_char_t const* p0
 , size_t l1, pan_char_t const* p1
 , size_t l2, pan_char_t const* p2
 , size_t l3, pan_char_t const* p3
 , size_t l4, pan_char_t const* p4
 , size_t l5, pan_char_t const* p5
 , size_t l6, pan_char_t const* p6
 , size_t l7, pan_char_t const* p7
 , size_t l8, pan_char_t const* p8
)
{
  return pantheios_log_9_no_test(severity
 , pan_slice_t(p0, l0)
 , pan_slice_t(p1, l1)
 , pan_slice_t(p2, l2)
 , pan_slice_t(p3, l3)
 , pan_slice_t(p4, l4)
 , pan_slice_t(p5, l5)
 , pan_slice_t(p6, l6)
 , pan_slice_t(p7, l7)
 , pan_slice_t(p8, l8)
 );
}

#if PANTHEIOS_APPL_PARAMS_LIMIT >= 10

inline int log_dispatch_10(pan_sev_t severity
 , size_t l0, pan_char_t const* p0
 , size_t l1, pan_char_t const* p1
 , size_t l2, pan_char_t const* p2
 , size_t l3, pan_char_t const* p3
 , size_t l4, pan_char_t const* p4
 , size_t l5, pan_char_t const* p5
 , size_t l6, pan_char_t const* p6
 , size_t l7, pan_char_t const* p7
 , size_t l8, pan_char_t const* p8
 , size_t l9, pan_char_t const* p9
)
{
  return pantheios_log_10_no_test(severity
 , pan_slice_t(p0, l0)
 , pan_slice_t(p1, l1)
 , pan_slice_t(p2, l2)
 , pan_slice_t(p3, l3)
 , pan_slice_t(p4, l4)
 , pan_slice_t(p5, l5)
 , pan_slice_t(p6, l6)
 , pan_slice_t(p7, l7)
 , pan_slice_t(p8, l8)
 , pan_slice_t(p9, l9)
 );
}

#if PANTHEIOS_APPL_PARAMS_LIMIT >= 11

inline int log_dispatch_11(pan_sev_t severity
 , size_t l0, pan_char_t const* p0
 , size_t l1, pan_char_t const* p1
 , size_t l2, pan_char_t const* p2
 , size_t l3, pan_char_t const* p3
 , size_t l4, pan_char_t const* p4
 , size_t l5, pan_char_t const* p5
 , size_t l6, pan_char_t const* p6
 , size_t l7, pan_char_t const* p7
 , size_t l8, pan_char_t const* p8
 , size_t l9, pan_char_t const* p9
 , size_t l10, pan_char_t const* p10
)
{
  return pantheios_log_11_no_test(severity
 , pan_slice_t(p0, l0)
 , pan_slice_t(p1, l1)
 , pan_slice_t(p2, l2)
 , pan_slice_t(p3, l3)
 , pan_slice_t(p4, l4)
 , pan_slice_t(p5, l5)
 , pan_slice_t(p6, l6)
 , pan_slice_t(p7, l7)
 , pan_slice_t(p8, l8)
 , pan_slice_t(p9, l9)
 , pan_slice_t(p10, l10)
 );
}

#if PANTHEIOS_APPL_PARAMS_LIMIT >= 12

inline int log_dispatch_12(pan_sev_t severity
 , size_t l0, pan_char_t const* p0
 , size_t l1, pan_char_t const* p1
 , size_t l2, pan_char_t const* p2
 , size_t l3, pan_char_t const* p3
 , size_t l4, pan_char_t const* p4
 , size_t l5, pan_char_t const* p5
 , size_t l6, pan_char_t const* p6
 , size_t l7, pan_char_t const* p7
 , size_t l8, pan_char_t const* p8
 , size_t l9, pan_char_t const* p9
 , size_t l10, pan_char_t const* p10
 , size_t l11, pan_char_t const* p11
)
{
  return pantheios_log_12_no_test(severity
 , pan_slice_t(p0, l0)
 , pan_slice_t(p1, l1)
 , pan_slice_t(p2, l2)
 , pan_slice_t(p3, l3)
 , pan_slice_t(p4, l4)
 , pan_slice_t(p5, l5)
 , pan_slice_t(p6, l6)
 , pan_slice_t(p7, l7)
 , pan_slice_t(p8, l8)
 , pan_slice_t(p9, l9)
 , pan_slice_t(p10, l10)
 , pan_slice_t(p11, l11)
 );
}

#if PANTHEIOS_APPL_PARAMS_LIMIT >= 13

inline int log_dispatch_13(pan_sev_t severity
 , size_t l0, pan_char_t const* p0
 , size_t l1, pan_char_t const* p1
 , size_t l2, pan_char_t const* p2
 , size_t l3, pan_char_t const* p3
 , size_t l4, pan_char_t const* p4
 , size_t l5, pan_char_t const* p5
 , size_t l6, pan_char_t const* p6
 , size_t l7, pan_char_t const* p7
 , size_t l8, pan_char_t const* p8
 , size_t l9, pan_char_t const* p9
 , size_t l10, pan_char_t const* p10
 , size_t l11, pan_char_t const* p11
 , size_t l12, pan_char_t const* p12
)
{
  return pantheios_log_13_no_test(severity
 , pan_slice_t(p0, l0)
 , pan_slice_t(p1, l1)
 , pan_slice_t(p2, l2)
 , pan_slice_t(p3, l3)
 , pan_slice_t(p4, l4)
 , pan_slice_t(p5, l5)
 , pan_slice_t(p6, l6)
 , pan_slice_t(p7, l7)
 , pan_slice_t(p8, l8)
 , pan_slice_t(p9, l9)
 , pan_slice_t(p10, l10)
 , pan_slice_t(p11, l11)
 , pan_slice_t(p12, l12)
 );
}

#if PANTHEIOS_APPL_PARAMS_LIMIT >= 14

inline int log_dispatch_14(pan_sev_t severity
 , size_t l0, pan_char_t const* p0
 , size_t l1, pan_char_t const* p1
 , size_t l2, pan_char_t const* p2
 , size_t l3, pan_char_t const* p3
 , size_t l4, pan_char_t const* p4
 , size_t l5, pan_char_t const* p5
 , size_t l6, pan_char_t const* p6
 , size_t l7, pan_char_t const* p7
 , size_t l8, pan_char_t const* p8
 , size_t l9, pan_char_t const* p9
 , size_t l10, pan_char_t const* p10
 , size_t l11, pan_char_t const* p11
 , size_t l12, pan_char_t const* p12
 , size_t l13, pan_char_t const* p13
)
{
  return pantheios_log_14_no_test(severity
 , pan_slice_t(p0, l0)
 , pan_slice_t(p1, l1)
 , pan_slice_t(p2, l2)
 , pan_slice_t(p3, l3)
 , pan_slice_t(p4, l4)
 , pan_slice_t(p5, l5)
 , pan_slice_t(p6, l6)
 , pan_slice_t(p7, l7)
 , pan_slice_t(p8, l8)
 , pan_slice_t(p9, l9)
 , pan_slice_t(p10, l10)
 , pan_slice_t(p11, l11)
 , pan_slice_t(p12, l12)
 , pan_slice_t(p13, l13)
 );
}

#if PANTHEIOS_APPL_PARAMS_LIMIT >= 15

inline int log_dispatch_15(pan_sev_t severity
 , size_t l0, pan_char_t const* p0
 , size_t l1, pan_char_t const* p1
 , size_t l2, pan_char_t const* p2
 , size_t l3, pan_char_t const* p3
 , size_t l4, pan_char_t const* p4
 , size_t l5, pan_char_t const* p5
 , size_t l6, pan_char_t const* p6
 , size_t l7, pan_char_t const* p7
 , size_t l8, pan_char_t const* p8
 , size_t l9, pan_char_t const* p9
 , size_t l10, pan_char_t const* p10
 , size_t l11, pan_char_t const* p11
 , size_t l12, pan_char_t const* p12
 , size_t l13, pan_char_t const* p13
 , size_t l14, pan_char_t const* p14
)
{
  return pantheios_log_15_no_test(severity
 , pan_slice_t(p0, l0)
 , pan_slice_t(p1, l1)
 , pan_slice_t(p2, l2)
 , pan_slice_t(p3, l3)
 , pan_slice_t(p4, l4)
 , pan_slice_t(p5, l5)
 , pan_slice_t(p6, l6)
 , pan_slice_t(p7, l7)
 , pan_slice_t(p8, l8)
 , pan_slice_t(p9, l9)
 , pan_slice_t(p10, l10)
 , pan_slice_t(p11, l11)
 , pan_slice_t(p12, l12)
 , pan_slice_t(p13, l13)
 , pan_slice_t(p14, l14)
 );
}

#if PANTHEIOS_APPL_PARAMS_LIMIT >= 16

inline int log_dispatch_16(pan_sev_t severity
 , size_t l0, pan_char_t const* p0
 , size_t l1, pan_char_t const* p1
 , size_t l2, pan_char_t const* p2
 , size_t l3, pan_char_t const* p3
 , size_t l4, pan_char_t const* p4
 , size_t l5, pan_char_t const* p5
 , size_t l6, pan_char_t const* p6
 , size_t l7, pan_char_t const* p7
 , size_t l8, pan_char_t const* p8
 , size_t l9, pan_char_t const* p9
 , size_t l10, pan_char_t const* p10
 , size_t l11, pan_char_t const* p11
 , size_t l12, pan_char_t const* p12
 , size_t l13, pan_char_t const* p13
 , size_t l14, pan_char_t const* p14
 , size_t l15, pan_char_t const* p15
)
{
  return pantheios_log_16_no_test(severity
 , pan_slice_t(p0, l0)
 , pan_slice_t(p1, l1)
 , pan_slice_t(p2, l2)
 , pan_slice_t(p3, l3)
 , pan_slice_t(p4, l4)
 , pan_slice_t(p5, l5)
 , pan_slice_t(p6, l6)
 , pan_slice_t(p7, l7)
 , pan_slice_t(p8, l8)
 , pan_slice_t(p9, l9)
 , pan_slice_t(p10, l10)
 , pan_slice_t(p11, l11)
 , pan_slice_t(p12, l12)
 , pan_slice_t(p13, l13)
 , pan_slice_t(p14, l14)
 , pan_slice_t(p15, l15)
 );
}

#if PANTHEIOS_APPL_PARAMS_LIMIT >= 17

inline int log_dispatch_17(pan_sev_t severity
 , size_t l0, pan_char_t const* p0
 , size_t l1, pan_char_t const* p1
 , size_t l2, pan_char_t const* p2
 , size_t l3, pan_char_t const* p3
 , size_t l4, pan_char_t const* p4
 , size_t l5, pan_char_t const* p5
 , size_t l6, pan_char_t const* p6
 , size_t l7, pan_char_t const* p7
 , size_t l8, pan_char_t const* p8
 , size_t l9, pan_char_t const* p9
 , size_t l10, pan_char_t const* p10
 , size_t l11, pan_char_t const* p11
 , size_t l12, pan_char_t const* p12
 , size_t l13, pan_char_t const* p13
 , size_t l14, pan_char_t const* p14
 , size_t l15, pan_char_t const* p15
 , size_t l16, pan_char_t const* p16
)
{
  return pantheios_log_17_no_test(severity
 , pan_slice_t(p0, l0)
 , pan_slice_t(p1, l1)
 , pan_slice_t(p2, l2)
 , pan_slice_t(p3, l3)
 , pan_slice_t(p4, l4)
 , pan_slice_t(p5, l5)
 , pan_slice_t(p6, l6)
 , pan_slice_t(p7, l7)
 , pan_slice_t(p8, l8)
 , pan_slice_t(p9, l9)
 , pan_slice_t(p10, l10)
 , pan_slice_t(p11, l11)
 , pan_slice_t(p12, l12)
 , pan_slice_t(p13, l13)
 , pan_slice_t(p14, l14)
 , pan_slice_t(p15, l15)
 , pan_slice_t(p16, l16)
 );
}

#if PANTHEIOS_APPL_PARAMS_LIMIT >= 18

inline int log_dispatch_18(pan_sev_t severity
 , size_t l0, pan_char_t const* p0
 , size_t l1, pan_char_t const* p1
 , size_t l2, pan_char_t const* p2
 , size_t l3, pan_char_t const* p3
 , size_t l4, pan_char_t const* p4
 , size_t l5, pan_char_t const* p5
 , size_t l6, pan_char_t const* p6
 , size_t l7, pan_char_t const* p7
 , size_t l8, pan_char_t const* p8
 , size_t l9, pan_char_t const* p9
 , size_t l10, pan_char_t const* p10
 , size_t l11, pan_char_t const* p11
 , size_t l12, pan_char_t const* p12
 , size_t l13, pan_char_t const* p13
 , size_t l14, pan_char_t const* p14
 , size_t l15, pan_char_t const* p15
 , size_t l16, pan_char_t const* p16
 , size_t l17, pan_char_t const* p17
)
{
  return pantheios_log_18_no_test(severity
 , pan_slice_t(p0, l0)
 , pan_slice_t(p1, l1)
 , pan_slice_t(p2, l2)
 , pan_slice_t(p3, l3)
 , pan_slice_t(p4, l4)
 , pan_slice_t(p5, l5)
 , pan_slice_t(p6, l6)
 , pan_slice_t(p7, l7)
 , pan_slice_t(p8, l8)
 , pan_slice_t(p9, l9)
 , pan_slice_t(p10, l10)
 , pan_slice_t(p11, l11)
 , pan_slice_t(p12, l12)
 , pan_slice_t(p13, l13)
 , pan_slice_t(p14, l14)
 , pan_slice_t(p15, l15)
 , pan_slice_t(p16, l16)
 , pan_slice_t(p17, l17)
 );
}

#if PANTHEIOS_APPL_PARAMS_LIMIT >= 19

inline int log_dispatch_19(pan_sev_t severity
 , size_t l0, pan_char_t const* p0
 , size_t l1, pan_char_t const* p1
 , size_t l2, pan_char_t const* p2
 , size_t l3, pan_char_t const* p3
 , size_t l4, pan_char_t const* p4
 , size_t l5, pan_char_t const* p5
 , size_t l6, pan_char_t const* p6
 , size_t l7, pan_char_t const* p7
 , size_t l8, pan_char_t const* p8
 , size_t l9, pan_char_t const* p9
 , size_t l10, pan_char_t const* p10
 , size_t l11, pan_char_t const* p11
 , size_t l12, pan_char_t const* p12
 , size_t l13, pan_char_t const* p13
 , size_t l14, pan_char_t const* p14
 , size_t l15, pan_char_t const* p15
 , size_t l16, pan_char_t const* p16
 , size_t l17, pan_char_t const* p17
 , size_t l18, pan_char_t const* p18
)
{
  return pantheios_log_19_no_test(severity
 , pan_slice_t(p0, l0)
 , pan_slice_t(p1, l1)
 , pan_slice_t(p2, l2)
 , pan_slice_t(p3, l3)
 , pan_slice_t(p4, l4)
 , pan_slice_t(p5, l5)
 , pan_slice_t(p6, l6)
 , pan_slice_t(p7, l7)
 , pan_slice_t(p8, l8)
 , pan_slice_t(p9, l9)
 , pan_slice_t(p10, l10)
 , pan_slice_t(p11, l11)
 , pan_slice_t(p12, l12)
 , pan_slice_t(p13, l13)
 , pan_slice_t(p14, l14)
 , pan_slice_t(p15, l15)
 , pan_slice_t(p16, l16)
 , pan_slice_t(p17, l17)
 , pan_slice_t(p18, l18)
 );
}

#if PANTHEIOS_APPL_PARAMS_LIMIT >= 20

inline int log_dispatch_20(pan_sev_t severity
 , size_t l0, pan_char_t const* p0
 , size_t l1, pan_char_t const* p1
 , size_t l2, pan_char_t const* p2
 , size_t l3, pan_char_t const* p3
 , size_t l4, pan_char_t const* p4
 , size_t l5, pan_char_t const* p5
 , size_t l6, pan_char_t const* p6
 , size_t l7, pan_char_t const* p7
 , size_t l8, pan_char_t const* p8
 , size_t l9, pan_char_t const* p9
 , size_t l10, pan_char_t const* p10
 , size_t l11, pan_char_t const* p11
 , size_t l12, pan_char_t const* p12
 , size_t l13, pan_char_t const* p13
 , size_t l14, pan_char_t const* p14
 , size_t l15, pan_char_t const* p15
 , size_t l16, pan_char_t const* p16
 , size_t l17, pan_char_t const* p17
 , size_t l18, pan_char_t const* p18
 , size_t l19, pan_char_t const* p19
)
{
  return pantheios_log_20_no_test(severity
 , pan_slice_t(p0, l0)
 , pan_slice_t(p1, l1)
 , pan_slice_t(p2, l2)
 , pan_slice_t(p3, l3)
 , pan_slice_t(p4, l4)
 , pan_slice_t(p5, l5)
 , pan_slice_t(p6, l6)
 , pan_slice_t(p7, l7)
 , pan_slice_t(p8, l8)
 , pan_slice_t(p9, l9)
 , pan_slice_t(p10, l10)
 , pan_slice_t(p11, l11)
 , pan_slice_t(p12, l12)
 , pan_slice_t(p13, l13)
 , pan_slice_t(p14, l14)
 , pan_slice_t(p15, l15)
 , pan_slice_t(p16, l16)
 , pan_slice_t(p17, l17)
 , pan_slice_t(p18, l18)
 , pan_slice_t(p19, l19)
 );
}

#if PANTHEIOS_APPL_PARAMS_LIMIT >= 21

inline int log_dispatch_21(pan_sev_t severity
 , size_t l0, pan_char_t const* p0
 , size_t l1, pan_char_t const* p1
 , size_t l2, pan_char_t const* p2
 , size_t l3, pan_char_t const* p3
 , size_t l4, pan_char_t const* p4
 , size_t l5, pan_char_t const* p5
 , size_t l6, pan_char_t const* p6
 , size_t l7, pan_char_t const* p7
 , size_t l8, pan_char_t const* p8
 , size_t l9, pan_char_t const* p9
 , size_t l10, pan_char_t const* p10
 , size_t l11, pan_char_t const* p11
 , size_t l12, pan_char_t const* p12
 , size_t l13, pan_char_t const* p13
 , size_t l14, pan_char_t const* p14
 , size_t l15, pan_char_t const* p15
 , size_t l16, pan_char_t const* p16
 , size_t l17, pan_char_t const* p17
 , size_t l18, pan_char_t const* p18
 , size_t l19, pan_char_t const* p19
 , size_t l20, pan_char_t const* p20
)
{
  return pantheios_log_21_no_test(severity
 , pan_slice_t(p0, l0)
 , pan_slice_t(p1, l1)
 , pan_slice_t(p2, l2)
 , pan_slice_t(p3, l3)
 , pan_slice_t(p4, l4)
 , pan_slice_t(p5, l5)
 , pan_slice_t(p6, l6)
 , pan_slice_t(p7, l7)
 , pan_slice_t(p8, l8)
 , pan_slice_t(p9, l9)
 , pan_slice_t(p10, l10)
 , pan_slice_t(p11, l11)
 , pan_slice_t(p12, l12)
 , pan_slice_t(p13, l13)
 , pan_slice_t(p14, l14)
 , pan_slice_t(p15, l15)
 , pan_slice_t(p16, l16)
 , pan_slice_t(p17, l17)
 , pan_slice_t(p18, l18)
 , pan_slice_t(p19, l19)
 , pan_slice_t(p20, l20)
 );
}

#if PANTHEIOS_APPL_PARAMS_LIMIT >= 22

inline int log_dispatch_22(pan_sev_t severity
 , size_t l0, pan_char_t const* p0
 , size_t l1, pan_char_t const* p1
 , size_t l2, pan_char_t const* p2
 , size_t l3, pan_char_t const* p3
 , size_t l4, pan_char_t const* p4
 , size_t l5, pan_char_t const* p5
 , size_t l6, pan_char_t const* p6
 , size_t l7, pan_char_t const* p7
 , size_t l8, pan_char_t const* p8
 , size_t l9, pan_char_t const* p9
 , size_t l10, pan_char_t const* p10
 , size_t l11, pan_char_t const* p11
 , size_t l12, pan_char_t const* p12
 , size_t l13, pan_char_t const* p13
 , size_t l14, pan_char_t const* p14
 , size_t l15, pan_char_t const* p15
 , size_t l16, pan_char_t const* p16
 , size_t l17, pan_char_t const* p17
 , size_t l18, pan_char_t const* p18
 , size_t l19, pan_char_t const* p19
 , size_t l20, pan_char_t const* p20
 , size_t l21, pan_char_t const* p21
)
{
  return pantheios_log_22_no_test(severity
 , pan_slice_t(p0, l0)
 , pan_slice_t(p1, l1)
 , pan_slice_t(p2, l2)
 , pan_slice_t(p3, l3)
 , pan_slice_t(p4, l4)
 , pan_slice_t(p5, l5)
 , pan_slice_t(p6, l6)
 , pan_slice_t(p7, l7)
 , pan_slice_t(p8, l8)
 , pan_slice_t(p9, l9)
 , pan_slice_t(p10, l10)
 , pan_slice_t(p11, l11)
 , pan_slice_t(p12, l12)
 , pan_slice_t(p13, l13)
 , pan_slice_t(p14, l14)
 , pan_slice_t(p15, l15)
 , pan_slice_t(p16, l16)
 , pan_slice_t(p17, l17)
 , pan_slice_t(p18, l18)
 , pan_slice_t(p19, l19)
 , pan_slice_t(p20, l20)
 , pan_slice_t(p21, l21)
 );
}

#if PANTHEIOS_APPL_PARAMS_LIMIT >= 23

inline int log_dispatch_23(pan_sev_t severity
 , size_t l0, pan_char_t const* p0
 , size_t l1, pan_char_t const* p1
 , size_t l2, pan_char_t const* p2
 , size_t l3, pan_char_t const* p3
 , size_t l4, pan_char_t const* p4
 , size_t l5, pan_char_t const* p5
 , size_t l6, pan_char_t const* p6
 , size_t l7, pan_char_t const* p7
 , size_t l8, pan_char_t const* p8
 , size_t l9, pan_char_t const* p9
 , size_t l10, pan_char_t const* p10
 , size_t l11, pan_char_t const* p11
 , size_t l12, pan_char_t const* p12
 , size_t l13, pan_char_t const* p13
 , size_t l14, pan_char_t const* p14
 , size_t l15, pan_char_t const* p15
 , size_t l16, pan_char_t const* p16
 , size_t l17, pan_char_t const* p17
 , size_t l18, pan_char_t const* p18
 , size_t l19, pan_char_t const* p19
 , size_t l20, pan_char_t const* p20
 , size_t l21, pan_char_t const* p21
 , size_t l22, pan_char_t const* p22
)
{
  return pantheios_log_23_no_test(severity
 , pan_slice_t(p0, l0)
 , pan_slice_t(p1, l1)
 , pan_slice_t(p2, l2)
 , pan_slice_t(p3, l3)
 , pan_slice_t(p4, l4)
 , pan_slice_t(p5, l5)
 , pan_slice_t(p6, l6)
 , pan_slice_t(p7, l7)
 , pan_slice_t(p8, l8)
 , pan_slice_t(p9, l9)
 , pan_slice_t(p10, l10)
 , pan_slice_t(p11, l11)
 , pan_slice_t(p12, l12)
 , pan_slice_t(p13, l13)
 , pan_slice_t(p14, l14)
 , pan_slice_t(p15, l15)
 , pan_slice_t(p16, l16)
 , pan_slice_t(p17, l17)
 , pan_slice_t(p18, l18)
 , pan_slice_t(p19, l19)
 , pan_slice_t(p20, l20)
 , pan_slice_t(p21, l21)
 , pan_slice_t(p22, l22)
 );
}

#if PANTHEIOS_APPL_PARAMS_LIMIT >= 24

inline int log_dispatch_24(pan_sev_t severity
 , size_t l0, pan_char_t const* p0
 , size_t l1, pan_char_t const* p1
 , size_t l2, pan_char_t const* p2
 , size_t l3, pan_char_t const* p3
 , size_t l4, pan_char_t const* p4
 , size_t l5, pan_char_t const* p5
 , size_t l6, pan_char_t const* p6
 , size_t l7, pan_char_t const* p7
 , size_t l8, pan_char_t const* p8
 , size_t l9, pan_char_t const* p9
 , size_t l10, pan_char_t const* p10
 , size_t l11, pan_char_t const* p11
 , size_t l12, pan_char_t const* p12
 , size_t l13, pan_char_t const* p13
 , size_t l14, pan_char_t const* p14
 , size_t l15, pan_char_t const* p15
 , size_t l16, pan_char_t const* p16
 , size_t l17, pan_char_t const* p17
 , size_t l18, pan_char_t const* p18
 , size_t l19, pan_char_t const* p19
 , size_t l20, pan_char_t const* p20
 , size_t l21, pan_char_t const* p21
 , size_t l22, pan_char_t const* p22
 , size_t l23, pan_char_t const* p23
)
{
  return pantheios_log_24_no_test(severity
 , pan_slice_t(p0, l0)
 , pan_slice_t(p1, l1)
 , pan_slice_t(p2, l2)
 , pan_slice_t(p3, l3)
 , pan_slice_t(p4, l4)
 , pan_slice_t(p5, l5)
 , pan_slice_t(p6, l6)
 , pan_slice_t(p7, l7)
 , pan_slice_t(p8, l8)
 , pan_slice_t(p9, l9)
 , pan_slice_t(p10, l10)
 , pan_slice_t(p11, l11)
 , pan_slice_t(p12, l12)
 , pan_slice_t(p13, l13)
 , pan_slice_t(p14, l14)
 , pan_slice_t(p15, l15)
 , pan_slice_t(p16, l16)
 , pan_slice_t(p17, l17)
 , pan_slice_t(p18, l18)
 , pan_slice_t(p19, l19)
 , pan_slice_t(p20, l20)
 , pan_slice_t(p21, l21)
 , pan_slice_t(p22, l22)
 , pan_slice_t(p23, l23)
 );
}

#if PANTHEIOS_APPL_PARAMS_LIMIT >= 25

inline int log_dispatch_25(pan_sev_t severity
 , size_t l0, pan_char_t const* p0
 , size_t l1, pan_char_t const* p1
 , size_t l2, pan_char_t const* p2
 , size_t l3, pan_char_t const* p3
 , size_t l4, pan_char_t const* p4
 , size_t l5, pan_char_t const* p5
 , size_t l6, pan_char_t const* p6
 , size_t l7, pan_char_t const* p7
 , size_t l8, pan_char_t const* p8
 , size_t l9, pan_char_t const* p9
 , size_t l10, pan_char_t const* p10
 , size_t l11, pan_char_t const* p11
 , size_t l12, pan_char_t const* p12
 , size_t l13, pan_char_t const* p13
 , size_t l14, pan_char_t const* p14
 , size_t l15, pan_char_t const* p15
 , size_t l16, pan_char_t const* p16
 , size_t l17, pan_char_t const* p17
 , size_t l18, pan_char_t const* p18
 , size_t l19, pan_char_t const* p19
 , size_t l20, pan_char_t const* p20
 , size_t l21, pan_char_t const* p21
 , size_t l22, pan_char_t const* p22
 , size_t l23, pan_char_t const* p23
 , size_t l24, pan_char_t const* p24
)
{
  return pantheios_log_25_no_test(severity
 , pan_slice_t(p0, l0)
 , pan_slice_t(p1, l1)
 , pan_slice_t(p2, l2)
 , pan_slice_t(p3, l3)
 , pan_slice_t(p4, l4)
 , pan_slice_t(p5, l5)
 , pan_slice_t(p6, l6)
 , pan_slice_t(p7, l7)
 , pan_slice_t(p8, l8)
 , pan_slice_t(p9, l9)
 , pan_slice_t(p10, l10)
 , pan_slice_t(p11, l11)
 , pan_slice_t(p12, l12)
 , pan_slice_t(p13, l13)
 , pan_slice_t(p14, l14)
 , pan_slice_t(p15, l15)
 , pan_slice_t(p16, l16)
 , pan_slice_t(p17, l17)
 , pan_slice_t(p18, l18)
 , pan_slice_t(p19, l19)
 , pan_slice_t(p20, l20)
 , pan_slice_t(p21, l21)
 , pan_slice_t(p22, l22)
 , pan_slice_t(p23, l23)
 , pan_slice_t(p24, l24)
 );
}

#if PANTHEIOS_APPL_PARAMS_LIMIT >= 26

inline int log_dispatch_26(pan_sev_t severity
 , size_t l0, pan_char_t const* p0
 , size_t l1, pan_char_t const* p1
 , size_t l2, pan_char_t const* p2
 , size_t l3, pan_char_t const* p3
 , size_t l4, pan_char_t const* p4
 , size_t l5, pan_char_t const* p5
 , size_t l6, pan_char_t const* p6
 , size_t l7, pan_char_t const* p7
 , size_t l8, pan_char_t const* p8
 , size_t l9, pan_char_t const* p9
 , size_t l10, pan_char_t const* p10
 , size_t l11, pan_char_t const* p11
 , size_t l12, pan_char_t const* p12
 , size_t l13, pan_char_t const* p13
 , size_t l14, pan_char_t const* p14
 , size_t l15, pan_char_t const* p15
 , size_t l16, pan_char_t const* p16
 , size_t l17, pan_char_t const* p17
 , size_t l18, pan_char_t const* p18
 , size_t l19, pan_char_t const* p19
 , size_t l20, pan_char_t const* p20
 , size_t l21, pan_char_t const* p21
 , size_t l22, pan_char_t const* p22
 , size_t l23, pan_char_t const* p23
 , size_t l24, pan_char_t const* p24
 , size_t l25, pan_char_t const* p25
)
{
  return pantheios_log_26_no_test(severity
 , pan_slice_t(p0, l0)
 , pan_slice_t(p1, l1)
 , pan_slice_t(p2, l2)
 , pan_slice_t(p3, l3)
 , pan_slice_t(p4, l4)
 , pan_slice_t(p5, l5)
 , pan_slice_t(p6, l6)
 , pan_slice_t(p7, l7)
 , pan_slice_t(p8, l8)
 , pan_slice_t(p9, l9)
 , pan_slice_t(p10, l10)
 , pan_slice_t(p11, l11)
 , pan_slice_t(p12, l12)
 , pan_slice_t(p13, l13)
 , pan_slice_t(p14, l14)
 , pan_slice_t(p15, l15)
 , pan_slice_t(p16, l16)
 , pan_slice_t(p17, l17)
 , pan_slice_t(p18, l18)
 , pan_slice_t(p19, l19)
 , pan_slice_t(p20, l20)
 , pan_slice_t(p21, l21)
 , pan_slice_t(p22, l22)
 , pan_slice_t(p23, l23)
 , pan_slice_t(p24, l24)
 , pan_slice_t(p25, l25)
 );
}

#if PANTHEIOS_APPL_PARAMS_LIMIT >= 27

inline int log_dispatch_27(pan_sev_t severity
 , size_t l0, pan_char_t const* p0
 , size_t l1, pan_char_t const* p1
 , size_t l2, pan_char_t const* p2
 , size_t l3, pan_char_t const* p3
 , size_t l4, pan_char_t const* p4
 , size_t l5, pan_char_t const* p5
 , size_t l6, pan_char_t const* p6
 , size_t l7, pan_char_t const* p7
 , size_t l8, pan_char_t const* p8
 , size_t l9, pan_char_t const* p9
 , size_t l10, pan_char_t const* p10
 , size_t l11, pan_char_t const* p11
 , size_t l12, pan_char_t const* p12
 , size_t l13, pan_char_t const* p13
 , size_t l14, pan_char_t const* p14
 , size_t l15, pan_char_t const* p15
 , size_t l16, pan_char_t const* p16
 , size_t l17, pan_char_t const* p17
 , size_t l18, pan_char_t const* p18
 , size_t l19, pan_char_t const* p19
 , size_t l20, pan_char_t const* p20
 , size_t l21, pan_char_t const* p21
 , size_t l22, pan_char_t const* p22
 , size_t l23, pan_char_t const* p23
 , size_t l24, pan_char_t const* p24
 , size_t l25, pan_char_t const* p25
 , size_t l26, pan_char_t const* p26
)
{
  return pantheios_log_27_no_test(severity
 , pan_slice_t(p0, l0)
 , pan_slice_t(p1, l1)
 , pan_slice_t(p2, l2)
 , pan_slice_t(p3, l3)
 , pan_slice_t(p4, l4)
 , pan_slice_t(p5, l5)
 , pan_slice_t(p6, l6)
 , pan_slice_t(p7, l7)
 , pan_slice_t(p8, l8)
 , pan_slice_t(p9, l9)
 , pan_slice_t(p10, l10)
 , pan_slice_t(p11, l11)
 , pan_slice_t(p12, l12)
 , pan_slice_t(p13, l13)
 , pan_slice_t(p14, l14)
 , pan_slice_t(p15, l15)
 , pan_slice_t(p16, l16)
 , pan_slice_t(p17, l17)
 , pan_slice_t(p18, l18)
 , pan_slice_t(p19, l19)
 , pan_slice_t(p20, l20)
 , pan_slice_t(p21, l21)
 , pan_slice_t(p22, l22)
 , pan_slice_t(p23, l23)
 , pan_slice_t(p24, l24)
 , pan_slice_t(p25, l25)
 , pan_slice_t(p26, l26)
 );
}

#if PANTHEIOS_APPL_PARAMS_LIMIT >= 28

inline int log_dispatch_28(pan_sev_t severity
 , size_t l0, pan_char_t const* p0
 , size_t l1, pan_char_t const* p1
 , size_t l2, pan_char_t const* p2
 , size_t l3, pan_char_t const* p3
 , size_t l4, pan_char_t const* p4
 , size_t l5, pan_char_t const* p5
 , size_t l6, pan_char_t const* p6
 , size_t l7, pan_char_t const* p7
 , size_t l8, pan_char_t const* p8
 , size_t l9, pan_char_t const* p9
 , size_t l10, pan_char_t const* p10
 , size_t l11, pan_char_t const* p11
 , size_t l12, pan_char_t const* p12
 , size_t l13, pan_char_t const* p13
 , size_t l14, pan_char_t const* p14
 , size_t l15, pan_char_t const* p15
 , size_t l16, pan_char_t const* p16
 , size_t l17, pan_char_t const* p17
 , size_t l18, pan_char_t const* p18
 , size_t l19, pan_char_t const* p19
 , size_t l20, pan_char_t const* p20
 , size_t l21, pan_char_t const* p21
 , size_t l22, pan_char_t const* p22
 , size_t l23, pan_char_t const* p23
 , size_t l24, pan_char_t const* p24
 , size_t l25, pan_char_t const* p25
 , size_t l26, pan_char_t const* p26
 , size_t l27, pan_char_t const* p27
)
{
  return pantheios_log_28_no_test(severity
 , pan_slice_t(p0, l0)
 , pan_slice_t(p1, l1)
 , pan_slice_t(p2, l2)
 , pan_slice_t(p3, l3)
 , pan_slice_t(p4, l4)
 , pan_slice_t(p5, l5)
 , pan_slice_t(p6, l6)
 , pan_slice_t(p7, l7)
 , pan_slice_t(p8, l8)
 , pan_slice_t(p9, l9)
 , pan_slice_t(p10, l10)
 , pan_slice_t(p11, l11)
 , pan_slice_t(p12, l12)
 , pan_slice_t(p13, l13)
 , pan_slice_t(p14, l14)
 , pan_slice_t(p15, l15)
 , pan_slice_t(p16, l16)
 , pan_slice_t(p17, l17)
 , pan_slice_t(p18, l18)
 , pan_slice_t(p19, l19)
 , pan_slice_t(p20, l20)
 , pan_slice_t(p21, l21)
 , pan_slice_t(p22, l22)
 , pan_slice_t(p23, l23)
 , pan_slice_t(p24, l24)
 , pan_slice_t(p25, l25)
 , pan_slice_t(p26, l26)
 , pan_slice_t(p27, l27)
 );
}

#if PANTHEIOS_APPL_PARAMS_LIMIT >= 29

inline int log_dispatch_29(pan_sev_t severity
 , size_t l0, pan_char_t const* p0
 , size_t l1, pan_char_t const* p1
 , size_t l2, pan_char_t const* p2
 , size_t l3, pan_char_t const* p3
 , size_t l4, pan_char_t const* p4
 , size_t l5, pan_char_t const* p5
 , size_t l6, pan_char_t const* p6
 , size_t l7, pan_char_t const* p7
 , size_t l8, pan_char_t const* p8
 , size_t l9, pan_char_t const* p9
 , size_t l10, pan_char_t const* p10
 , size_t l11, pan_char_t const* p11
 , size_t l12, pan_char_t const* p12
 , size_t l13, pan_char_t const* p13
 , size_t l14, pan_char_t const* p14
 , size_t l15, pan_char_t const* p15
 , size_t l16, pan_char_t const* p16
 , size_t l17, pan_char_t const* p17
 , size_t l18, pan_char_t const* p18
 , size_t l19, pan_char_t const* p19
 , size_t l20, pan_char_t const* p20
 , size_t l21, pan_char_t const* p21
 , size_t l22, pan_char_t const* p22
 , size_t l23, pan_char_t const* p23
 , size_t l24, pan_char_t const* p24
 , size_t l25, pan_char_t const* p25
 , size_t l26, pan_char_t const* p26
 , size_t l27, pan_char_t const* p27
 , size_t l28, pan_char_t const* p28
)
{
  return pantheios_log_29_no_test(severity
 , pan_slice_t(p0, l0)
 , pan_slice_t(p1, l1)
 , pan_slice_t(p2, l2)
 , pan_slice_t(p3, l3)
 , pan_slice_t(p4, l4)
 , pan_slice_t(p5, l5)
 , pan_slice_t(p6, l6)
 , pan_slice_t(p7, l7)
 , pan_slice_t(p8, l8)
 , pan_slice_t(p9, l9)
 , pan_slice_t(p10, l10)
 , pan_slice_t(p11, l11)
 , pan_slice_t(p12, l12)
 , pan_slice_t(p13, l13)
 , pan_slice_t(p14, l14)
 , pan_slice_t(p15, l15)
 , pan_slice_t(p16, l16)
 , pan_slice_t(p17, l17)
 , pan_slice_t(p18, l18)
 , pan_slice_t(p19, l19)
 , pan_slice_t(p20, l20)
 , pan_slice_t(p21, l21)
 , pan_slice_t(p22, l22)
 , pan_slice_t(p23, l23)
 , pan_slice_t(p24, l24)
 , pan_slice_t(p25, l25)
 , pan_slice_t(p26, l26)
 , pan_slice_t(p27, l27)
 , pan_slice_t(p28, l28)
 );
}

#if PANTHEIOS_APPL_PARAMS_LIMIT >= 30

inline int log_dispatch_30(pan_sev_t severity
 , size_t l0, pan_char_t const* p0
 , size_t l1, pan_char_t const* p1
 , size_t l2, pan_char_t const* p2
 , size_t l3, pan_char_t const* p3
 , size_t l4, pan_char_t const* p4
 , size_t l5, pan_char_t const* p5
 , size_t l6, pan_char_t const* p6
 , size_t l7, pan_char_t const* p7
 , size_t l8, pan_char_t const* p8
 , size_t l9, pan_char_t const* p9
 , size_t l10, pan_char_t const* p10
 , size_t l11, pan_char_t const* p11
 , size_t l12, pan_char_t const* p12
 , size_t l13, pan_char_t const* p13
 , size_t l14, pan_char_t const* p14
 , size_t l15, pan_char_t const* p15
 , size_t l16, pan_char_t const* p16
 , size_t l17, pan_char_t const* p17
 , size_t l18, pan_char_t const* p18
 , size_t l19, pan_char_t const* p19
 , size_t l20, pan_char_t const* p20
 , size_t l21, pan_char_t const* p21
 , size_t l22, pan_char_t const* p22
 , size_t l23, pan_char_t const* p23
 , size_t l24, pan_char_t const* p24
 , size_t l25, pan_char_t const* p25
 , size_t l26, pan_char_t const* p26
 , size_t l27, pan_char_t const* p27
 , size_t l28, pan_char_t const* p28
 , size_t l29, pan_char_t const* p29
)
{
  return pantheios_log_30_no_test(severity
 , pan_slice_t(p0, l0)
 , pan_slice_t(p1, l1)
 , pan_slice_t(p2, l2)
 , pan_slice_t(p3, l3)
 , pan_slice_t(p4, l4)
 , pan_slice_t(p5, l5)
 , pan_slice_t(p6, l6)
 , pan_slice_t(p7, l7)
 , pan_slice_t(p8, l8)
 , pan_slice_t(p9, l9)
 , pan_slice_t(p10, l10)
 , pan_slice_t(p11, l11)
 , pan_slice_t(p12, l12)
 , pan_slice_t(p13, l13)
 , pan_slice_t(p14, l14)
 , pan_slice_t(p15, l15)
 , pan_slice_t(p16, l16)
 , pan_slice_t(p17, l17)
 , pan_slice_t(p18, l18)
 , pan_slice_t(p19, l19)
 , pan_slice_t(p20, l20)
 , pan_slice_t(p21, l21)
 , pan_slice_t(p22, l22)
 , pan_slice_t(p23, l23)
 , pan_slice_t(p24, l24)
 , pan_slice_t(p25, l25)
 , pan_slice_t(p26, l26)
 , pan_slice_t(p27, l27)
 , pan_slice_t(p28, l28)
 , pan_slice_t(p29, l29)
 );
}

#if PANTHEIOS_APPL_PARAMS_LIMIT >= 31

inline int log_dispatch_31(pan_sev_t severity
 , size_t l0, pan_char_t const* p0
 , size_t l1, pan_char_t const* p1
 , size_t l2, pan_char_t const* p2
 , size_t l3, pan_char_t const* p3
 , size_t l4, pan_char_t const* p4
 , size_t l5, pan_char_t const* p5
 , size_t l6, pan_char_t const* p6
 , size_t l7, pan_char_t const* p7
 , size_t l8, pan_char_t const* p8
 , size_t l9, pan_char_t const* p9
 , size_t l10, pan_char_t const* p10
 , size_t l11, pan_char_t const* p11
 , size_t l12, pan_char_t const* p12
 , size_t l13, pan_char_t const* p13
 , size_t l14, pan_char_t const* p14
 , size_t l15, pan_char_t const* p15
 , size_t l16, pan_char_t const* p16
 , size_t l17, pan_char_t const* p17
 , size_t l18, pan_char_t const* p18
 , size_t l19, pan_char_t const* p19
 , size_t l20, pan_char_t const* p20
 , size_t l21, pan_char_t const* p21
 , size_t l22, pan_char_t const* p22
 , size_t l23, pan_char_t const* p23
 , size_t l24, pan_char_t const* p24
 , size_t l25, pan_char_t const* p25
 , size_t l26, pan_char_t const* p26
 , size_t l27, pan_char_t const* p27
 , size_t l28, pan_char_t const* p28
 , size_t l29, pan_char_t const* p29
 , size_t l30, pan_char_t const* p30
)
{
  return pantheios_log_31_no_test(severity
 , pan_slice_t(p0, l0)
 , pan_slice_t(p1, l1)
 , pan_slice_t(p2, l2)
 , pan_slice_t(p3, l3)
 , pan_slice_t(p4, l4)
 , pan_slice_t(p5, l5)
 , pan_slice_t(p6, l6)
 , pan_slice_t(p7, l7)
 , pan_slice_t(p8, l8)
 , pan_slice_t(p9, l9)
 , pan_slice_t(p10, l10)
 , pan_slice_t(p11, l11)
 , pan_slice_t(p12, l12)
 , pan_slice_t(p13, l13)
 , pan_slice_t(p14, l14)
 , pan_slice_t(p15, l15)
 , pan_slice_t(p16, l16)
 , pan_slice_t(p17, l17)
 , pan_slice_t(p18, l18)
 , pan_slice_t(p19, l19)
 , pan_slice_t(p20, l20)
 , pan_slice_t(p21, l21)
 , pan_slice_t(p22, l22)
 , pan_slice_t(p23, l23)
 , pan_slice_t(p24, l24)
 , pan_slice_t(p25, l25)
 , pan_slice_t(p26, l26)
 , pan_slice_t(p27, l27)
 , pan_slice_t(p28, l28)
 , pan_slice_t(p29, l29)
 , pan_slice_t(p30, l30)
 );
}

#if PANTHEIOS_APPL_PARAMS_LIMIT >= 32

inline int log_dispatch_32(pan_sev_t severity
 , size_t l0, pan_char_t const* p0
 , size_t l1, pan_char_t const* p1
 , size_t l2, pan_char_t const* p2
 , size_t l3, pan_char_t const* p3
 , size_t l4, pan_char_t const* p4
 , size_t l5, pan_char_t const* p5
 , size_t l6, pan_char_t const* p6
 , size_t l7, pan_char_t const* p7
 , size_t l8, pan_char_t const* p8
 , size_t l9, pan_char_t const* p9
 , size_t l10, pan_char_t const* p10
 , size_t l11, pan_char_t const* p11
 , size_t l12, pan_char_t const* p12
 , size_t l13, pan_char_t const* p13
 , size_t l14, pan_char_t const* p14
 , size_t l15, pan_char_t const* p15
 , size_t l16, pan_char_t const* p16
 , size_t l17, pan_char_t const* p17
 , size_t l18, pan_char_t const* p18
 , size_t l19, pan_char_t const* p19
 , size_t l20, pan_char_t const* p20
 , size_t l21, pan_char_t const* p21
 , size_t l22, pan_char_t const* p22
 , size_t l23, pan_char_t const* p23
 , size_t l24, pan_char_t const* p24
 , size_t l25, pan_char_t const* p25
 , size_t l26, pan_char_t const* p26
 , size_t l27, pan_char_t const* p27
 , size_t l28, pan_char_t const* p28
 , size_t l29, pan_char_t const* p29
 , size_t l30, pan_char_t const* p30
 , size_t l31, pan_char_t const* p31
)
{
  return pantheios_log_32_no_test(severity
 , pan_slice_t(p0, l0)
 , pan_slice_t(p1, l1)
 , pan_slice_t(p2, l2)
 , pan_slice_t(p3, l3)
 , pan_slice_t(p4, l4)
 , pan_slice_t(p5, l5)
 , pan_slice_t(p6, l6)
 , pan_slice_t(p7, l7)
 , pan_slice_t(p8, l8)
 , pan_slice_t(p9, l9)
 , pan_slice_t(p10, l10)
 , pan_slice_t(p11, l11)
 , pan_slice_t(p12, l12)
 , pan_slice_t(p13, l13)
 , pan_slice_t(p14, l14)
 , pan_slice_t(p15, l15)
 , pan_slice_t(p16, l16)
 , pan_slice_t(p17, l17)
 , pan_slice_t(p18, l18)
 , pan_slice_t(p19, l19)
 , pan_slice_t(p20, l20)
 , pan_slice_t(p21, l21)
 , pan_slice_t(p22, l22)
 , pan_slice_t(p23, l23)
 , pan_slice_t(p24, l24)
 , pan_slice_t(p25, l25)
 , pan_slice_t(p26, l26)
 , pan_slice_t(p27, l27)
 , pan_slice_t(p28, l28)
 , pan_slice_t(p29, l29)
 , pan_slice_t(p30, l30)
 , pan_slice_t(p31, l31)
 );
}

#endif /* PANTHEIOS_APPL_PARAMS_LIMIT >= 32 */
#endif /* PANTHEIOS_APPL_PARAMS_LIMIT >= 31 */
#endif /* PANTHEIOS_APPL_PARAMS_LIMIT >= 30 */
#endif /* PANTHEIOS_APPL_PARAMS_LIMIT >= 29 */
#endif /* PANTHEIOS_APPL_PARAMS_LIMIT >= 28 */
#endif /* PANTHEIOS_APPL_PARAMS_LIMIT >= 27 */
#endif /* PANTHEIOS_APPL_PARAMS_LIMIT >= 26 */
#endif /* PANTHEIOS_APPL_PARAMS_LIMIT >= 25 */
#endif /* PANTHEIOS_APPL_PARAMS_LIMIT >= 24 */
#endif /* PANTHEIOS_APPL_PARAMS_LIMIT >= 23 */
#endif /* PANTHEIOS_APPL_PARAMS_LIMIT >= 22 */
#endif /* PANTHEIOS_APPL_PARAMS_LIMIT >= 21 */
#endif /* PANTHEIOS_APPL_PARAMS_LIMIT >= 20 */
#endif /* PANTHEIOS_APPL_PARAMS_LIMIT >= 19 */
#endif /* PANTHEIOS_APPL_PARAMS_LIMIT >= 18 */
#endif /* PANTHEIOS_APPL_PARAMS_LIMIT >= 17 */
#endif /* PANTHEIOS_APPL_PARAMS_LIMIT >= 16 */
#endif /* PANTHEIOS_APPL_PARAMS_LIMIT >= 15 */
#endif /* PANTHEIOS_APPL_PARAMS_LIMIT >= 14 */
#endif /* PANTHEIOS_APPL_PARAMS_LIMIT >= 13 */
#endif /* PANTHEIOS_APPL_PARAMS_LIMIT >= 12 */
#endif /* PANTHEIOS_APPL_PARAMS_LIMIT >= 11 */
#endif /* PANTHEIOS_APPL_PARAMS_LIMIT >= 10 */
#endif /* PANTHEIOS_APPL_PARAMS_LIMIT >= 9 */
#endif /* PANTHEIOS_APPL_PARAMS_LIMIT >= 8 */
#endif /* PANTHEIOS_APPL_PARAMS_LIMIT >= 7 */
#endif /* PANTHEIOS_APPL_PARAMS_LIMIT >= 6 */
#endif /* PANTHEIOS_APPL_PARAMS_LIMIT >= 5 */
#endif /* PANTHEIOS_APPL_PARAMS_LIMIT >= 4 */
#endif /* PANTHEIOS_APPL_PARAMS_LIMIT >= 3 */
#endif /* PANTHEIOS_APPL_PARAMS_LIMIT >= 2 */
#endif /* PANTHEIOS_APPL_PARAMS_LIMIT >= 1 */
