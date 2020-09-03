
#pragma once
#include "FigaroModelTemplate.h"
#include <array>
#include <map>
#include <vector>
#include <sstream> 
#include<math.h>
#include <set>

namespace storm{
    namespace figaro{
        class FigaroProgram2: public storm::figaro::FigaroProgram{
        public:
            FigaroProgram2(): FigaroProgram(

            
            
//            std::map<std::string, size_t> mFigaroboolelementindex =
            {
            	{"S_OF_auto_exclusions" , 0},
            	{"required_OF_AND_3" , 1},
            	{"already_S_OF_AND_3" , 2},
            	{"S_OF_AND_3" , 3},
            	{"relevant_evt_OF_AND_3" , 4},
            	{"required_OF_BATTERY_A_lost" , 5},
            	{"already_S_OF_BATTERY_A_lost" , 6},
            	{"S_OF_BATTERY_A_lost" , 7},
            	{"relevant_evt_OF_BATTERY_A_lost" , 8},
            	{"required_OF_BATTERY_B_lost" , 9},
            	{"already_S_OF_BATTERY_B_lost" , 10},
            	{"S_OF_BATTERY_B_lost" , 11},
            	{"relevant_evt_OF_BATTERY_B_lost" , 12},
            	{"required_OF_BATT_A1" , 13},
            	{"already_S_OF_BATT_A1" , 14},
            	{"S_OF_BATT_A1" , 15},
            	{"relevant_evt_OF_BATT_A1" , 16},
            	{"waiting_for_rep_OF_BATT_A1" , 17},
            	{"failF_OF_BATT_A1" , 18},
            	{"init_OF_BATT_A1" , 19},
            	{"required_OF_BATT_A2" , 20},
            	{"already_S_OF_BATT_A2" , 21},
            	{"S_OF_BATT_A2" , 22},
            	{"relevant_evt_OF_BATT_A2" , 23},
            	{"waiting_for_rep_OF_BATT_A2" , 24},
            	{"failF_OF_BATT_A2" , 25},
            	{"init_OF_BATT_A2" , 26},
            	{"required_OF_BATT_B1" , 27},
            	{"already_S_OF_BATT_B1" , 28},
            	{"S_OF_BATT_B1" , 29},
            	{"relevant_evt_OF_BATT_B1" , 30},
            	{"waiting_for_rep_OF_BATT_B1" , 31},
            	{"failF_OF_BATT_B1" , 32},
            	{"init_OF_BATT_B1" , 33},
            	{"required_OF_BATT_B2" , 34},
            	{"already_S_OF_BATT_B2" , 35},
            	{"S_OF_BATT_B2" , 36},
            	{"relevant_evt_OF_BATT_B2" , 37},
            	{"waiting_for_rep_OF_BATT_B2" , 38},
            	{"failF_OF_BATT_B2" , 39},
            	{"init_OF_BATT_B2" , 40},
            	{"required_OF_CB_LGD2_unable" , 41},
            	{"already_S_OF_CB_LGD2_unable" , 42},
            	{"S_OF_CB_LGD2_unable" , 43},
            	{"relevant_evt_OF_CB_LGD2_unable" , 44},
            	{"required_OF_CB_LGF2_unable" , 45},
            	{"already_S_OF_CB_LGF2_unable" , 46},
            	{"S_OF_CB_LGF2_unable" , 47},
            	{"relevant_evt_OF_CB_LGF2_unable" , 48},
            	{"required_OF_CB_LHA12_unable" , 49},
            	{"already_S_OF_CB_LHA12_unable" , 50},
            	{"S_OF_CB_LHA12_unable" , 51},
            	{"relevant_evt_OF_CB_LHA12_unable" , 52},
            	{"required_OF_CB_LHA3_unable" , 53},
            	{"already_S_OF_CB_LHA3_unable" , 54},
            	{"S_OF_CB_LHA3_unable" , 55},
            	{"relevant_evt_OF_CB_LHA3_unable" , 56},
            	{"required_OF_CB_LHB12_unable" , 57},
            	{"already_S_OF_CB_LHB12_unable" , 58},
            	{"S_OF_CB_LHB12_unable" , 59},
            	{"relevant_evt_OF_CB_LHB12_unable" , 60},
            	{"required_OF_CCF_DG" , 61},
            	{"already_S_OF_CCF_DG" , 62},
            	{"S_OF_CCF_DG" , 63},
            	{"relevant_evt_OF_CCF_DG" , 64},
            	{"waiting_for_rep_OF_CCF_DG" , 65},
            	{"failF_OF_CCF_DG" , 66},
            	{"init_OF_CCF_DG" , 67},
            	{"required_OF_CCF_GEV_LGR" , 68},
            	{"already_S_OF_CCF_GEV_LGR" , 69},
            	{"S_OF_CCF_GEV_LGR" , 70},
            	{"relevant_evt_OF_CCF_GEV_LGR" , 71},
            	{"waiting_for_rep_OF_CCF_GEV_LGR" , 72},
            	{"failF_OF_CCF_GEV_LGR" , 73},
            	{"init_OF_CCF_GEV_LGR" , 74},
            	{"required_OF_DGA_long" , 75},
            	{"already_S_OF_DGA_long" , 76},
            	{"S_OF_DGA_long" , 77},
            	{"relevant_evt_OF_DGA_long" , 78},
            	{"waiting_for_rep_OF_DGA_long" , 79},
            	{"failF_OF_DGA_long" , 80},
            	{"init_OF_DGA_long" , 81},
            	{"required_OF_DGA_lost" , 82},
            	{"already_S_OF_DGA_lost" , 83},
            	{"S_OF_DGA_lost" , 84},
            	{"relevant_evt_OF_DGA_lost" , 85},
            	{"required_OF_DGA_short" , 86},
            	{"already_S_OF_DGA_short" , 87},
            	{"S_OF_DGA_short" , 88},
            	{"relevant_evt_OF_DGA_short" , 89},
            	{"waiting_for_rep_OF_DGA_short" , 90},
            	{"failF_OF_DGA_short" , 91},
            	{"init_OF_DGA_short" , 92},
            	{"required_OF_DGB_long" , 93},
            	{"already_S_OF_DGB_long" , 94},
            	{"S_OF_DGB_long" , 95},
            	{"relevant_evt_OF_DGB_long" , 96},
            	{"waiting_for_rep_OF_DGB_long" , 97},
            	{"failF_OF_DGB_long" , 98},
            	{"init_OF_DGB_long" , 99},
            	{"required_OF_DGB_lost" , 100},
            	{"already_S_OF_DGB_lost" , 101},
            	{"S_OF_DGB_lost" , 102},
            	{"relevant_evt_OF_DGB_lost" , 103},
            	{"required_OF_DGB_short" , 104},
            	{"already_S_OF_DGB_short" , 105},
            	{"S_OF_DGB_short" , 106},
            	{"relevant_evt_OF_DGB_short" , 107},
            	{"waiting_for_rep_OF_DGB_short" , 108},
            	{"failF_OF_DGB_short" , 109},
            	{"init_OF_DGB_short" , 110},
            	{"required_OF_GEV" , 111},
            	{"already_S_OF_GEV" , 112},
            	{"S_OF_GEV" , 113},
            	{"relevant_evt_OF_GEV" , 114},
            	{"waiting_for_rep_OF_GEV" , 115},
            	{"failF_OF_GEV" , 116},
            	{"init_OF_GEV" , 117},
            	{"required_OF_GRID" , 118},
            	{"already_S_OF_GRID" , 119},
            	{"S_OF_GRID" , 120},
            	{"relevant_evt_OF_GRID" , 121},
            	{"waiting_for_rep_OF_GRID" , 122},
            	{"failF_OF_GRID" , 123},
            	{"init_OF_GRID" , 124},
            	{"required_OF_LBA" , 125},
            	{"already_S_OF_LBA" , 126},
            	{"S_OF_LBA" , 127},
            	{"relevant_evt_OF_LBA" , 128},
            	{"waiting_for_rep_OF_LBA" , 129},
            	{"failF_OF_LBA" , 130},
            	{"init_OF_LBA" , 131},
            	{"required_OF_LBA_by_line1_lost" , 132},
            	{"already_S_OF_LBA_by_line1_lost" , 133},
            	{"S_OF_LBA_by_line1_lost" , 134},
            	{"relevant_evt_OF_LBA_by_line1_lost" , 135},
            	{"waiting_for_rep_OF_LBA_by_line1_lost" , 136},
            	{"failAG_OF_LBA_by_line1_lost" , 137},
            	{"init_OF_LBA_by_line1_lost" , 138},
            	{"required_OF_LBA_by_line2_lost" , 139},
            	{"already_S_OF_LBA_by_line2_lost" , 140},
            	{"S_OF_LBA_by_line2_lost" , 141},
            	{"relevant_evt_OF_LBA_by_line2_lost" , 142},
            	{"waiting_for_rep_OF_LBA_by_line2_lost" , 143},
            	{"failAG_OF_LBA_by_line2_lost" , 144},
            	{"init_OF_LBA_by_line2_lost" , 145},
            	{"required_OF_LBA_by_others_lost" , 146},
            	{"already_S_OF_LBA_by_others_lost" , 147},
            	{"S_OF_LBA_by_others_lost" , 148},
            	{"relevant_evt_OF_LBA_by_others_lost" , 149},
            	{"required_OF_LBA_lost" , 150},
            	{"already_S_OF_LBA_lost" , 151},
            	{"S_OF_LBA_lost" , 152},
            	{"relevant_evt_OF_LBA_lost" , 153},
            	{"required_OF_LBA_not_fed" , 154},
            	{"already_S_OF_LBA_not_fed" , 155},
            	{"S_OF_LBA_not_fed" , 156},
            	{"relevant_evt_OF_LBA_not_fed" , 157},
            	{"required_OF_LBB" , 158},
            	{"already_S_OF_LBB" , 159},
            	{"S_OF_LBB" , 160},
            	{"relevant_evt_OF_LBB" , 161},
            	{"waiting_for_rep_OF_LBB" , 162},
            	{"failF_OF_LBB" , 163},
            	{"init_OF_LBB" , 164},
            	{"required_OF_LBB_by_line1_lost" , 165},
            	{"already_S_OF_LBB_by_line1_lost" , 166},
            	{"S_OF_LBB_by_line1_lost" , 167},
            	{"relevant_evt_OF_LBB_by_line1_lost" , 168},
            	{"waiting_for_rep_OF_LBB_by_line1_lost" , 169},
            	{"failAG_OF_LBB_by_line1_lost" , 170},
            	{"init_OF_LBB_by_line1_lost" , 171},
            	{"required_OF_LBB_by_line2_lost" , 172},
            	{"already_S_OF_LBB_by_line2_lost" , 173},
            	{"S_OF_LBB_by_line2_lost" , 174},
            	{"relevant_evt_OF_LBB_by_line2_lost" , 175},
            	{"waiting_for_rep_OF_LBB_by_line2_lost" , 176},
            	{"failAG_OF_LBB_by_line2_lost" , 177},
            	{"init_OF_LBB_by_line2_lost" , 178},
            	{"required_OF_LBB_lost" , 179},
            	{"already_S_OF_LBB_lost" , 180},
            	{"S_OF_LBB_lost" , 181},
            	{"relevant_evt_OF_LBB_lost" , 182},
            	{"required_OF_LBB_not_fed" , 183},
            	{"already_S_OF_LBB_not_fed" , 184},
            	{"S_OF_LBB_not_fed" , 185},
            	{"relevant_evt_OF_LBB_not_fed" , 186},
            	{"required_OF_LGA" , 187},
            	{"already_S_OF_LGA" , 188},
            	{"S_OF_LGA" , 189},
            	{"relevant_evt_OF_LGA" , 190},
            	{"waiting_for_rep_OF_LGA" , 191},
            	{"failF_OF_LGA" , 192},
            	{"init_OF_LGA" , 193},
            	{"required_OF_LGB" , 194},
            	{"already_S_OF_LGB" , 195},
            	{"S_OF_LGB" , 196},
            	{"relevant_evt_OF_LGB" , 197},
            	{"waiting_for_rep_OF_LGB" , 198},
            	{"failF_OF_LGB" , 199},
            	{"init_OF_LGB" , 200},
            	{"required_OF_LGD" , 201},
            	{"already_S_OF_LGD" , 202},
            	{"S_OF_LGD" , 203},
            	{"relevant_evt_OF_LGD" , 204},
            	{"waiting_for_rep_OF_LGD" , 205},
            	{"failF_OF_LGD" , 206},
            	{"init_OF_LGD" , 207},
            	{"required_OF_LGD_not_fed" , 208},
            	{"already_S_OF_LGD_not_fed" , 209},
            	{"S_OF_LGD_not_fed" , 210},
            	{"relevant_evt_OF_LGD_not_fed" , 211},
            	{"required_OF_LGE" , 212},
            	{"already_S_OF_LGE" , 213},
            	{"S_OF_LGE" , 214},
            	{"relevant_evt_OF_LGE" , 215},
            	{"waiting_for_rep_OF_LGE" , 216},
            	{"failF_OF_LGE" , 217},
            	{"init_OF_LGE" , 218},
            	{"required_OF_LGF" , 219},
            	{"already_S_OF_LGF" , 220},
            	{"S_OF_LGF" , 221},
            	{"relevant_evt_OF_LGF" , 222},
            	{"waiting_for_rep_OF_LGF" , 223},
            	{"failF_OF_LGF" , 224},
            	{"init_OF_LGF" , 225},
            	{"required_OF_LGF_not_fed" , 226},
            	{"already_S_OF_LGF_not_fed" , 227},
            	{"S_OF_LGF_not_fed" , 228},
            	{"relevant_evt_OF_LGF_not_fed" , 229},
            	{"required_OF_LGR" , 230},
            	{"already_S_OF_LGR" , 231},
            	{"S_OF_LGR" , 232},
            	{"relevant_evt_OF_LGR" , 233},
            	{"waiting_for_rep_OF_LGR" , 234},
            	{"failF_OF_LGR" , 235},
            	{"init_OF_LGR" , 236},
            	{"required_OF_LHA" , 237},
            	{"already_S_OF_LHA" , 238},
            	{"S_OF_LHA" , 239},
            	{"relevant_evt_OF_LHA" , 240},
            	{"waiting_for_rep_OF_LHA" , 241},
            	{"failF_OF_LHA" , 242},
            	{"init_OF_LHA" , 243},
            	{"required_OF_LHA_and_LHB_lost" , 244},
            	{"already_S_OF_LHA_and_LHB_lost" , 245},
            	{"S_OF_LHA_and_LHB_lost" , 246},
            	{"relevant_evt_OF_LHA_and_LHB_lost" , 247},
            	{"required_OF_LHA_lost" , 248},
            	{"already_S_OF_LHA_lost" , 249},
            	{"S_OF_LHA_lost" , 250},
            	{"relevant_evt_OF_LHA_lost" , 251},
            	{"required_OF_LHA_not_fed" , 252},
            	{"already_S_OF_LHA_not_fed" , 253},
            	{"S_OF_LHA_not_fed" , 254},
            	{"relevant_evt_OF_LHA_not_fed" , 255},
            	{"required_OF_LHB" , 256},
            	{"already_S_OF_LHB" , 257},
            	{"S_OF_LHB" , 258},
            	{"relevant_evt_OF_LHB" , 259},
            	{"waiting_for_rep_OF_LHB" , 260},
            	{"failF_OF_LHB" , 261},
            	{"init_OF_LHB" , 262},
            	{"required_OF_LHB_lost" , 263},
            	{"already_S_OF_LHB_lost" , 264},
            	{"S_OF_LHB_lost" , 265},
            	{"relevant_evt_OF_LHB_lost" , 266},
            	{"required_OF_LHB_not_fed" , 267},
            	{"already_S_OF_LHB_not_fed" , 268},
            	{"S_OF_LHB_not_fed" , 269},
            	{"relevant_evt_OF_LHB_not_fed" , 270},
            	{"required_OF_LKE" , 271},
            	{"already_S_OF_LKE" , 272},
            	{"S_OF_LKE" , 273},
            	{"relevant_evt_OF_LKE" , 274},
            	{"waiting_for_rep_OF_LKE" , 275},
            	{"failF_OF_LKE" , 276},
            	{"init_OF_LKE" , 277},
            	{"required_OF_LKI" , 278},
            	{"already_S_OF_LKI" , 279},
            	{"S_OF_LKI" , 280},
            	{"relevant_evt_OF_LKI" , 281},
            	{"waiting_for_rep_OF_LKI" , 282},
            	{"failF_OF_LKI" , 283},
            	{"init_OF_LKI" , 284},
            	{"required_OF_LLA" , 285},
            	{"already_S_OF_LLA" , 286},
            	{"S_OF_LLA" , 287},
            	{"relevant_evt_OF_LLA" , 288},
            	{"waiting_for_rep_OF_LLA" , 289},
            	{"failF_OF_LLA" , 290},
            	{"init_OF_LLA" , 291},
            	{"required_OF_LLD" , 292},
            	{"already_S_OF_LLD" , 293},
            	{"S_OF_LLD" , 294},
            	{"relevant_evt_OF_LLD" , 295},
            	{"waiting_for_rep_OF_LLD" , 296},
            	{"failF_OF_LLD" , 297},
            	{"init_OF_LLD" , 298},
            	{"required_OF_OR_14" , 299},
            	{"already_S_OF_OR_14" , 300},
            	{"S_OF_OR_14" , 301},
            	{"relevant_evt_OF_OR_14" , 302},
            	{"required_OF_RC_CB_LGD2" , 303},
            	{"already_S_OF_RC_CB_LGD2" , 304},
            	{"S_OF_RC_CB_LGD2" , 305},
            	{"relevant_evt_OF_RC_CB_LGD2" , 306},
            	{"waiting_for_rep_OF_RC_CB_LGD2" , 307},
            	{"failI_OF_RC_CB_LGD2" , 308},
            	{"to_be_fired_OF_RC_CB_LGD2" , 309},
            	{"already_standby_OF_RC_CB_LGD2" , 310},
            	{"already_required_OF_RC_CB_LGD2" , 311},
            	{"required_OF_RC_CB_LGD2_" , 312},
            	{"already_S_OF_RC_CB_LGD2_" , 313},
            	{"S_OF_RC_CB_LGD2_" , 314},
            	{"relevant_evt_OF_RC_CB_LGD2_" , 315},
            	{"required_OF_RC_CB_LGF2" , 316},
            	{"already_S_OF_RC_CB_LGF2" , 317},
            	{"S_OF_RC_CB_LGF2" , 318},
            	{"relevant_evt_OF_RC_CB_LGF2" , 319},
            	{"waiting_for_rep_OF_RC_CB_LGF2" , 320},
            	{"failI_OF_RC_CB_LGF2" , 321},
            	{"to_be_fired_OF_RC_CB_LGF2" , 322},
            	{"already_standby_OF_RC_CB_LGF2" , 323},
            	{"already_required_OF_RC_CB_LGF2" , 324},
            	{"required_OF_RC_CB_LGF2_" , 325},
            	{"already_S_OF_RC_CB_LGF2_" , 326},
            	{"S_OF_RC_CB_LGF2_" , 327},
            	{"relevant_evt_OF_RC_CB_LGF2_" , 328},
            	{"required_OF_RC_CB_LHA2" , 329},
            	{"already_S_OF_RC_CB_LHA2" , 330},
            	{"S_OF_RC_CB_LHA2" , 331},
            	{"relevant_evt_OF_RC_CB_LHA2" , 332},
            	{"waiting_for_rep_OF_RC_CB_LHA2" , 333},
            	{"failI_OF_RC_CB_LHA2" , 334},
            	{"to_be_fired_OF_RC_CB_LHA2" , 335},
            	{"already_standby_OF_RC_CB_LHA2" , 336},
            	{"already_required_OF_RC_CB_LHA2" , 337},
            	{"required_OF_RC_CB_LHA2_" , 338},
            	{"already_S_OF_RC_CB_LHA2_" , 339},
            	{"S_OF_RC_CB_LHA2_" , 340},
            	{"relevant_evt_OF_RC_CB_LHA2_" , 341},
            	{"required_OF_RC_CB_LHA3" , 342},
            	{"already_S_OF_RC_CB_LHA3" , 343},
            	{"S_OF_RC_CB_LHA3" , 344},
            	{"relevant_evt_OF_RC_CB_LHA3" , 345},
            	{"waiting_for_rep_OF_RC_CB_LHA3" , 346},
            	{"failI_OF_RC_CB_LHA3" , 347},
            	{"to_be_fired_OF_RC_CB_LHA3" , 348},
            	{"already_standby_OF_RC_CB_LHA3" , 349},
            	{"already_required_OF_RC_CB_LHA3" , 350},
            	{"required_OF_RC_CB_LHA3_" , 351},
            	{"already_S_OF_RC_CB_LHA3_" , 352},
            	{"S_OF_RC_CB_LHA3_" , 353},
            	{"relevant_evt_OF_RC_CB_LHA3_" , 354},
            	{"required_OF_RC_CB_LHB2" , 355},
            	{"already_S_OF_RC_CB_LHB2" , 356},
            	{"S_OF_RC_CB_LHB2" , 357},
            	{"relevant_evt_OF_RC_CB_LHB2" , 358},
            	{"waiting_for_rep_OF_RC_CB_LHB2" , 359},
            	{"failI_OF_RC_CB_LHB2" , 360},
            	{"to_be_fired_OF_RC_CB_LHB2" , 361},
            	{"already_standby_OF_RC_CB_LHB2" , 362},
            	{"already_required_OF_RC_CB_LHB2" , 363},
            	{"required_OF_RC_CB_LHB2_" , 364},
            	{"already_S_OF_RC_CB_LHB2_" , 365},
            	{"S_OF_RC_CB_LHB2_" , 366},
            	{"relevant_evt_OF_RC_CB_LHB2_" , 367},
            	{"required_OF_RDA1" , 368},
            	{"already_S_OF_RDA1" , 369},
            	{"S_OF_RDA1" , 370},
            	{"relevant_evt_OF_RDA1" , 371},
            	{"waiting_for_rep_OF_RDA1" , 372},
            	{"failF_OF_RDA1" , 373},
            	{"init_OF_RDA1" , 374},
            	{"required_OF_RDA2" , 375},
            	{"already_S_OF_RDA2" , 376},
            	{"S_OF_RDA2" , 377},
            	{"relevant_evt_OF_RDA2" , 378},
            	{"waiting_for_rep_OF_RDA2" , 379},
            	{"failF_OF_RDA2" , 380},
            	{"init_OF_RDA2" , 381},
            	{"required_OF_RDB1" , 382},
            	{"already_S_OF_RDB1" , 383},
            	{"S_OF_RDB1" , 384},
            	{"relevant_evt_OF_RDB1" , 385},
            	{"waiting_for_rep_OF_RDB1" , 386},
            	{"failF_OF_RDB1" , 387},
            	{"init_OF_RDB1" , 388},
            	{"required_OF_RDB2" , 389},
            	{"already_S_OF_RDB2" , 390},
            	{"S_OF_RDB2" , 391},
            	{"relevant_evt_OF_RDB2" , 392},
            	{"waiting_for_rep_OF_RDB2" , 393},
            	{"failF_OF_RDB2" , 394},
            	{"init_OF_RDB2" , 395},
            	{"required_OF_RO_CB_LHA1" , 396},
            	{"already_S_OF_RO_CB_LHA1" , 397},
            	{"S_OF_RO_CB_LHA1" , 398},
            	{"relevant_evt_OF_RO_CB_LHA1" , 399},
            	{"waiting_for_rep_OF_RO_CB_LHA1" , 400},
            	{"failI_OF_RO_CB_LHA1" , 401},
            	{"to_be_fired_OF_RO_CB_LHA1" , 402},
            	{"already_standby_OF_RO_CB_LHA1" , 403},
            	{"already_required_OF_RO_CB_LHA1" , 404},
            	{"required_OF_RO_CB_LHA1_" , 405},
            	{"already_S_OF_RO_CB_LHA1_" , 406},
            	{"S_OF_RO_CB_LHA1_" , 407},
            	{"relevant_evt_OF_RO_CB_LHA1_" , 408},
            	{"required_OF_RO_CB_LHA2" , 409},
            	{"already_S_OF_RO_CB_LHA2" , 410},
            	{"S_OF_RO_CB_LHA2" , 411},
            	{"relevant_evt_OF_RO_CB_LHA2" , 412},
            	{"waiting_for_rep_OF_RO_CB_LHA2" , 413},
            	{"failI_OF_RO_CB_LHA2" , 414},
            	{"to_be_fired_OF_RO_CB_LHA2" , 415},
            	{"already_standby_OF_RO_CB_LHA2" , 416},
            	{"already_required_OF_RO_CB_LHA2" , 417},
            	{"required_OF_RO_CB_LHA2_" , 418},
            	{"already_S_OF_RO_CB_LHA2_" , 419},
            	{"S_OF_RO_CB_LHA2_" , 420},
            	{"relevant_evt_OF_RO_CB_LHA2_" , 421},
            	{"required_OF_RO_CB_LHB1" , 422},
            	{"already_S_OF_RO_CB_LHB1" , 423},
            	{"S_OF_RO_CB_LHB1" , 424},
            	{"relevant_evt_OF_RO_CB_LHB1" , 425},
            	{"waiting_for_rep_OF_RO_CB_LHB1" , 426},
            	{"failI_OF_RO_CB_LHB1" , 427},
            	{"to_be_fired_OF_RO_CB_LHB1" , 428},
            	{"already_standby_OF_RO_CB_LHB1" , 429},
            	{"already_required_OF_RO_CB_LHB1" , 430},
            	{"required_OF_RO_CB_LHB1_" , 431},
            	{"already_S_OF_RO_CB_LHB1_" , 432},
            	{"S_OF_RO_CB_LHB1_" , 433},
            	{"relevant_evt_OF_RO_CB_LHB1_" , 434},
            	{"required_OF_SH_CB_GEV" , 435},
            	{"already_S_OF_SH_CB_GEV" , 436},
            	{"S_OF_SH_CB_GEV" , 437},
            	{"relevant_evt_OF_SH_CB_GEV" , 438},
            	{"waiting_for_rep_OF_SH_CB_GEV" , 439},
            	{"failF_OF_SH_CB_GEV" , 440},
            	{"init_OF_SH_CB_GEV" , 441},
            	{"required_OF_SH_CB_LBA1" , 442},
            	{"already_S_OF_SH_CB_LBA1" , 443},
            	{"S_OF_SH_CB_LBA1" , 444},
            	{"relevant_evt_OF_SH_CB_LBA1" , 445},
            	{"waiting_for_rep_OF_SH_CB_LBA1" , 446},
            	{"failF_OF_SH_CB_LBA1" , 447},
            	{"init_OF_SH_CB_LBA1" , 448},
            	{"required_OF_SH_CB_LBA2" , 449},
            	{"already_S_OF_SH_CB_LBA2" , 450},
            	{"S_OF_SH_CB_LBA2" , 451},
            	{"relevant_evt_OF_SH_CB_LBA2" , 452},
            	{"waiting_for_rep_OF_SH_CB_LBA2" , 453},
            	{"failF_OF_SH_CB_LBA2" , 454},
            	{"init_OF_SH_CB_LBA2" , 455},
            	{"required_OF_SH_CB_LBB1" , 456},
            	{"already_S_OF_SH_CB_LBB1" , 457},
            	{"S_OF_SH_CB_LBB1" , 458},
            	{"relevant_evt_OF_SH_CB_LBB1" , 459},
            	{"waiting_for_rep_OF_SH_CB_LBB1" , 460},
            	{"failF_OF_SH_CB_LBB1" , 461},
            	{"init_OF_SH_CB_LBB1" , 462},
            	{"required_OF_SH_CB_LBB2" , 463},
            	{"already_S_OF_SH_CB_LBB2" , 464},
            	{"S_OF_SH_CB_LBB2" , 465},
            	{"relevant_evt_OF_SH_CB_LBB2" , 466},
            	{"waiting_for_rep_OF_SH_CB_LBB2" , 467},
            	{"failF_OF_SH_CB_LBB2" , 468},
            	{"init_OF_SH_CB_LBB2" , 469},
            	{"required_OF_SH_CB_LGA" , 470},
            	{"already_S_OF_SH_CB_LGA" , 471},
            	{"S_OF_SH_CB_LGA" , 472},
            	{"relevant_evt_OF_SH_CB_LGA" , 473},
            	{"waiting_for_rep_OF_SH_CB_LGA" , 474},
            	{"failF_OF_SH_CB_LGA" , 475},
            	{"init_OF_SH_CB_LGA" , 476},
            	{"required_OF_SH_CB_LGB" , 477},
            	{"already_S_OF_SH_CB_LGB" , 478},
            	{"S_OF_SH_CB_LGB" , 479},
            	{"relevant_evt_OF_SH_CB_LGB" , 480},
            	{"waiting_for_rep_OF_SH_CB_LGB" , 481},
            	{"failF_OF_SH_CB_LGB" , 482},
            	{"init_OF_SH_CB_LGB" , 483},
            	{"required_OF_SH_CB_LGD1" , 484},
            	{"already_S_OF_SH_CB_LGD1" , 485},
            	{"S_OF_SH_CB_LGD1" , 486},
            	{"relevant_evt_OF_SH_CB_LGD1" , 487},
            	{"waiting_for_rep_OF_SH_CB_LGD1" , 488},
            	{"failF_OF_SH_CB_LGD1" , 489},
            	{"init_OF_SH_CB_LGD1" , 490},
            	{"required_OF_SH_CB_LGD2" , 491},
            	{"already_S_OF_SH_CB_LGD2" , 492},
            	{"S_OF_SH_CB_LGD2" , 493},
            	{"relevant_evt_OF_SH_CB_LGD2" , 494},
            	{"waiting_for_rep_OF_SH_CB_LGD2" , 495},
            	{"failF_OF_SH_CB_LGD2" , 496},
            	{"init_OF_SH_CB_LGD2" , 497},
            	{"required_OF_SH_CB_LGE1" , 498},
            	{"already_S_OF_SH_CB_LGE1" , 499},
            	{"S_OF_SH_CB_LGE1" , 500},
            	{"relevant_evt_OF_SH_CB_LGE1" , 501},
            	{"waiting_for_rep_OF_SH_CB_LGE1" , 502},
            	{"failF_OF_SH_CB_LGE1" , 503},
            	{"init_OF_SH_CB_LGE1" , 504},
            	{"required_OF_SH_CB_LGF1" , 505},
            	{"already_S_OF_SH_CB_LGF1" , 506},
            	{"S_OF_SH_CB_LGF1" , 507},
            	{"relevant_evt_OF_SH_CB_LGF1" , 508},
            	{"waiting_for_rep_OF_SH_CB_LGF1" , 509},
            	{"failF_OF_SH_CB_LGF1" , 510},
            	{"init_OF_SH_CB_LGF1" , 511},
            	{"required_OF_SH_CB_LGF2" , 512},
            	{"already_S_OF_SH_CB_LGF2" , 513},
            	{"S_OF_SH_CB_LGF2" , 514},
            	{"relevant_evt_OF_SH_CB_LGF2" , 515},
            	{"waiting_for_rep_OF_SH_CB_LGF2" , 516},
            	{"failF_OF_SH_CB_LGF2" , 517},
            	{"init_OF_SH_CB_LGF2" , 518},
            	{"required_OF_SH_CB_LHA1" , 519},
            	{"already_S_OF_SH_CB_LHA1" , 520},
            	{"S_OF_SH_CB_LHA1" , 521},
            	{"relevant_evt_OF_SH_CB_LHA1" , 522},
            	{"waiting_for_rep_OF_SH_CB_LHA1" , 523},
            	{"failF_OF_SH_CB_LHA1" , 524},
            	{"init_OF_SH_CB_LHA1" , 525},
            	{"required_OF_SH_CB_LHA2" , 526},
            	{"already_S_OF_SH_CB_LHA2" , 527},
            	{"S_OF_SH_CB_LHA2" , 528},
            	{"relevant_evt_OF_SH_CB_LHA2" , 529},
            	{"waiting_for_rep_OF_SH_CB_LHA2" , 530},
            	{"failF_OF_SH_CB_LHA2" , 531},
            	{"init_OF_SH_CB_LHA2" , 532},
            	{"required_OF_SH_CB_LHA3" , 533},
            	{"already_S_OF_SH_CB_LHA3" , 534},
            	{"S_OF_SH_CB_LHA3" , 535},
            	{"relevant_evt_OF_SH_CB_LHA3" , 536},
            	{"waiting_for_rep_OF_SH_CB_LHA3" , 537},
            	{"failF_OF_SH_CB_LHA3" , 538},
            	{"init_OF_SH_CB_LHA3" , 539},
            	{"required_OF_SH_CB_LHB1" , 540},
            	{"already_S_OF_SH_CB_LHB1" , 541},
            	{"S_OF_SH_CB_LHB1" , 542},
            	{"relevant_evt_OF_SH_CB_LHB1" , 543},
            	{"waiting_for_rep_OF_SH_CB_LHB1" , 544},
            	{"failF_OF_SH_CB_LHB1" , 545},
            	{"init_OF_SH_CB_LHB1" , 546},
            	{"required_OF_SH_CB_LHB2" , 547},
            	{"already_S_OF_SH_CB_LHB2" , 548},
            	{"S_OF_SH_CB_LHB2" , 549},
            	{"relevant_evt_OF_SH_CB_LHB2" , 550},
            	{"waiting_for_rep_OF_SH_CB_LHB2" , 551},
            	{"failF_OF_SH_CB_LHB2" , 552},
            	{"init_OF_SH_CB_LHB2" , 553},
            	{"required_OF_SH_CB_RDA1" , 554},
            	{"already_S_OF_SH_CB_RDA1" , 555},
            	{"S_OF_SH_CB_RDA1" , 556},
            	{"relevant_evt_OF_SH_CB_RDA1" , 557},
            	{"waiting_for_rep_OF_SH_CB_RDA1" , 558},
            	{"failF_OF_SH_CB_RDA1" , 559},
            	{"init_OF_SH_CB_RDA1" , 560},
            	{"required_OF_SH_CB_RDA2" , 561},
            	{"already_S_OF_SH_CB_RDA2" , 562},
            	{"S_OF_SH_CB_RDA2" , 563},
            	{"relevant_evt_OF_SH_CB_RDA2" , 564},
            	{"waiting_for_rep_OF_SH_CB_RDA2" , 565},
            	{"failF_OF_SH_CB_RDA2" , 566},
            	{"init_OF_SH_CB_RDA2" , 567},
            	{"required_OF_SH_CB_RDB1" , 568},
            	{"already_S_OF_SH_CB_RDB1" , 569},
            	{"S_OF_SH_CB_RDB1" , 570},
            	{"relevant_evt_OF_SH_CB_RDB1" , 571},
            	{"waiting_for_rep_OF_SH_CB_RDB1" , 572},
            	{"failF_OF_SH_CB_RDB1" , 573},
            	{"init_OF_SH_CB_RDB1" , 574},
            	{"required_OF_SH_CB_RDB2" , 575},
            	{"already_S_OF_SH_CB_RDB2" , 576},
            	{"S_OF_SH_CB_RDB2" , 577},
            	{"relevant_evt_OF_SH_CB_RDB2" , 578},
            	{"waiting_for_rep_OF_SH_CB_RDB2" , 579},
            	{"failF_OF_SH_CB_RDB2" , 580},
            	{"init_OF_SH_CB_RDB2" , 581},
            	{"required_OF_SH_CB_TUA1" , 582},
            	{"already_S_OF_SH_CB_TUA1" , 583},
            	{"S_OF_SH_CB_TUA1" , 584},
            	{"relevant_evt_OF_SH_CB_TUA1" , 585},
            	{"waiting_for_rep_OF_SH_CB_TUA1" , 586},
            	{"failF_OF_SH_CB_TUA1" , 587},
            	{"init_OF_SH_CB_TUA1" , 588},
            	{"required_OF_SH_CB_TUA2" , 589},
            	{"already_S_OF_SH_CB_TUA2" , 590},
            	{"S_OF_SH_CB_TUA2" , 591},
            	{"relevant_evt_OF_SH_CB_TUA2" , 592},
            	{"waiting_for_rep_OF_SH_CB_TUA2" , 593},
            	{"failF_OF_SH_CB_TUA2" , 594},
            	{"init_OF_SH_CB_TUA2" , 595},
            	{"required_OF_SH_CB_TUB1" , 596},
            	{"already_S_OF_SH_CB_TUB1" , 597},
            	{"S_OF_SH_CB_TUB1" , 598},
            	{"relevant_evt_OF_SH_CB_TUB1" , 599},
            	{"waiting_for_rep_OF_SH_CB_TUB1" , 600},
            	{"failF_OF_SH_CB_TUB1" , 601},
            	{"init_OF_SH_CB_TUB1" , 602},
            	{"required_OF_SH_CB_TUB2" , 603},
            	{"already_S_OF_SH_CB_TUB2" , 604},
            	{"S_OF_SH_CB_TUB2" , 605},
            	{"relevant_evt_OF_SH_CB_TUB2" , 606},
            	{"waiting_for_rep_OF_SH_CB_TUB2" , 607},
            	{"failF_OF_SH_CB_TUB2" , 608},
            	{"init_OF_SH_CB_TUB2" , 609},
            	{"required_OF_SH_CB_line_GEV" , 610},
            	{"already_S_OF_SH_CB_line_GEV" , 611},
            	{"S_OF_SH_CB_line_GEV" , 612},
            	{"relevant_evt_OF_SH_CB_line_GEV" , 613},
            	{"waiting_for_rep_OF_SH_CB_line_GEV" , 614},
            	{"failF_OF_SH_CB_line_GEV" , 615},
            	{"init_OF_SH_CB_line_GEV" , 616},
            	{"required_OF_SH_CB_line_LGR" , 617},
            	{"already_S_OF_SH_CB_line_LGR" , 618},
            	{"S_OF_SH_CB_line_LGR" , 619},
            	{"relevant_evt_OF_SH_CB_line_LGR" , 620},
            	{"waiting_for_rep_OF_SH_CB_line_LGR" , 621},
            	{"failF_OF_SH_CB_line_LGR" , 622},
            	{"init_OF_SH_CB_line_LGR" , 623},
            	{"required_OF_SH_GEV_or_LGR" , 624},
            	{"already_S_OF_SH_GEV_or_LGR" , 625},
            	{"S_OF_SH_GEV_or_LGR" , 626},
            	{"relevant_evt_OF_SH_GEV_or_LGR" , 627},
            	{"required_OF_SUBSTATION" , 628},
            	{"already_S_OF_SUBSTATION" , 629},
            	{"S_OF_SUBSTATION" , 630},
            	{"relevant_evt_OF_SUBSTATION" , 631},
            	{"waiting_for_rep_OF_SUBSTATION" , 632},
            	{"failF_OF_SUBSTATION" , 633},
            	{"init_OF_SUBSTATION" , 634},
            	{"required_OF_TA" , 635},
            	{"already_S_OF_TA" , 636},
            	{"S_OF_TA" , 637},
            	{"relevant_evt_OF_TA" , 638},
            	{"waiting_for_rep_OF_TA" , 639},
            	{"failF_OF_TA" , 640},
            	{"init_OF_TA" , 641},
            	{"required_OF_TAC" , 642},
            	{"already_S_OF_TAC" , 643},
            	{"S_OF_TAC" , 644},
            	{"relevant_evt_OF_TAC" , 645},
            	{"waiting_for_rep_OF_TAC" , 646},
            	{"failF_OF_TAC" , 647},
            	{"init_OF_TAC" , 648},
            	{"required_OF_TA_lost" , 649},
            	{"already_S_OF_TA_lost" , 650},
            	{"S_OF_TA_lost" , 651},
            	{"relevant_evt_OF_TA_lost" , 652},
            	{"required_OF_TP" , 653},
            	{"already_S_OF_TP" , 654},
            	{"S_OF_TP" , 655},
            	{"relevant_evt_OF_TP" , 656},
            	{"waiting_for_rep_OF_TP" , 657},
            	{"failF_OF_TP" , 658},
            	{"init_OF_TP" , 659},
            	{"required_OF_TS" , 660},
            	{"already_S_OF_TS" , 661},
            	{"S_OF_TS" , 662},
            	{"relevant_evt_OF_TS" , 663},
            	{"waiting_for_rep_OF_TS" , 664},
            	{"failF_OF_TS" , 665},
            	{"init_OF_TS" , 666},
            	{"required_OF_TS_lost" , 667},
            	{"already_S_OF_TS_lost" , 668},
            	{"S_OF_TS_lost" , 669},
            	{"relevant_evt_OF_TS_lost" , 670},
            	{"required_OF_TS_not_fed" , 671},
            	{"already_S_OF_TS_not_fed" , 672},
            	{"S_OF_TS_not_fed" , 673},
            	{"relevant_evt_OF_TS_not_fed" , 674},
            	{"required_OF_TUA1" , 675},
            	{"already_S_OF_TUA1" , 676},
            	{"S_OF_TUA1" , 677},
            	{"relevant_evt_OF_TUA1" , 678},
            	{"waiting_for_rep_OF_TUA1" , 679},
            	{"failF_OF_TUA1" , 680},
            	{"init_OF_TUA1" , 681},
            	{"required_OF_TUA2" , 682},
            	{"already_S_OF_TUA2" , 683},
            	{"S_OF_TUA2" , 684},
            	{"relevant_evt_OF_TUA2" , 685},
            	{"waiting_for_rep_OF_TUA2" , 686},
            	{"failF_OF_TUA2" , 687},
            	{"init_OF_TUA2" , 688},
            	{"required_OF_TUB1" , 689},
            	{"already_S_OF_TUB1" , 690},
            	{"S_OF_TUB1" , 691},
            	{"relevant_evt_OF_TUB1" , 692},
            	{"waiting_for_rep_OF_TUB1" , 693},
            	{"failF_OF_TUB1" , 694},
            	{"init_OF_TUB1" , 695},
            	{"required_OF_TUB2" , 696},
            	{"already_S_OF_TUB2" , 697},
            	{"S_OF_TUB2" , 698},
            	{"relevant_evt_OF_TUB2" , 699},
            	{"waiting_for_rep_OF_TUB2" , 700},
            	{"failF_OF_TUB2" , 701},
            	{"init_OF_TUB2" , 702},
            	{"required_OF_UE_1" , 703},
            	{"already_S_OF_UE_1" , 704},
            	{"S_OF_UE_1" , 705},
            	{"relevant_evt_OF_UE_1" , 706},
            	{"required_OF_UNIT" , 707},
            	{"already_S_OF_UNIT" , 708},
            	{"S_OF_UNIT" , 709},
            	{"relevant_evt_OF_UNIT" , 710},
            	{"waiting_for_rep_OF_UNIT" , 711},
            	{"failF_OF_UNIT" , 712},
            	{"init_OF_UNIT" , 713},
            	{"required_OF_in_function_house" , 714},
            	{"already_S_OF_in_function_house" , 715},
            	{"S_OF_in_function_house" , 716},
            	{"relevant_evt_OF_in_function_house" , 717},
            	{"waiting_for_rep_OF_in_function_house" , 718},
            	{"failF_OF_in_function_house" , 719},
            	{"init_OF_in_function_house" , 720},
            	{"required_OF_loss_of_houseload_operation" , 721},
            	{"already_S_OF_loss_of_houseload_operation" , 722},
            	{"S_OF_loss_of_houseload_operation" , 723},
            	{"relevant_evt_OF_loss_of_houseload_operation" , 724},
            	{"required_OF_demand_CCF_DG" , 725},
            	{"already_S_OF_demand_CCF_DG" , 726},
            	{"S_OF_demand_CCF_DG" , 727},
            	{"relevant_evt_OF_demand_CCF_DG" , 728},
            	{"waiting_for_rep_OF_demand_CCF_DG" , 729},
            	{"failI_OF_demand_CCF_DG" , 730},
            	{"to_be_fired_OF_demand_CCF_DG" , 731},
            	{"already_standby_OF_demand_CCF_DG" , 732},
            	{"already_required_OF_demand_CCF_DG" , 733},
            	{"required_OF_demand_DGA" , 734},
            	{"already_S_OF_demand_DGA" , 735},
            	{"S_OF_demand_DGA" , 736},
            	{"relevant_evt_OF_demand_DGA" , 737},
            	{"waiting_for_rep_OF_demand_DGA" , 738},
            	{"failI_OF_demand_DGA" , 739},
            	{"to_be_fired_OF_demand_DGA" , 740},
            	{"already_standby_OF_demand_DGA" , 741},
            	{"already_required_OF_demand_DGA" , 742},
            	{"required_OF_demand_DGB" , 743},
            	{"already_S_OF_demand_DGB" , 744},
            	{"S_OF_demand_DGB" , 745},
            	{"relevant_evt_OF_demand_DGB" , 746},
            	{"waiting_for_rep_OF_demand_DGB" , 747},
            	{"failI_OF_demand_DGB" , 748},
            	{"to_be_fired_OF_demand_DGB" , 749},
            	{"already_standby_OF_demand_DGB" , 750},
            	{"already_required_OF_demand_DGB" , 751},
            	{"required_OF_demand_TAC" , 752},
            	{"already_S_OF_demand_TAC" , 753},
            	{"S_OF_demand_TAC" , 754},
            	{"relevant_evt_OF_demand_TAC" , 755},
            	{"waiting_for_rep_OF_demand_TAC" , 756},
            	{"failI_OF_demand_TAC" , 757},
            	{"to_be_fired_OF_demand_TAC" , 758},
            	{"already_standby_OF_demand_TAC" , 759},
            	{"already_required_OF_demand_TAC" , 760},
            	{"required_OF_loss_of_supply_by_DGA" , 761},
            	{"already_S_OF_loss_of_supply_by_DGA" , 762},
            	{"S_OF_loss_of_supply_by_DGA" , 763},
            	{"relevant_evt_OF_loss_of_supply_by_DGA" , 764},
            	{"required_OF_loss_of_supply_by_DGA_and_TAC" , 765},
            	{"already_S_OF_loss_of_supply_by_DGA_and_TAC" , 766},
            	{"S_OF_loss_of_supply_by_DGA_and_TAC" , 767},
            	{"relevant_evt_OF_loss_of_supply_by_DGA_and_TAC" , 768},
            	{"required_OF_loss_of_supply_by_DGB" , 769},
            	{"already_S_OF_loss_of_supply_by_DGB" , 770},
            	{"S_OF_loss_of_supply_by_DGB" , 771},
            	{"relevant_evt_OF_loss_of_supply_by_DGB" , 772},
            	{"required_OF_loss_of_supply_by_GEV" , 773},
            	{"already_S_OF_loss_of_supply_by_GEV" , 774},
            	{"S_OF_loss_of_supply_by_GEV" , 775},
            	{"relevant_evt_OF_loss_of_supply_by_GEV" , 776},
            	{"required_OF_loss_of_supply_by_LGD" , 777},
            	{"already_S_OF_loss_of_supply_by_LGD" , 778},
            	{"S_OF_loss_of_supply_by_LGD" , 779},
            	{"relevant_evt_OF_loss_of_supply_by_LGD" , 780},
            	{"required_OF_loss_of_supply_by_LGF" , 781},
            	{"already_S_OF_loss_of_supply_by_LGF" , 782},
            	{"S_OF_loss_of_supply_by_LGF" , 783},
            	{"relevant_evt_OF_loss_of_supply_by_LGF" , 784},
            	{"required_OF_loss_of_supply_by_LGR" , 785},
            	{"already_S_OF_loss_of_supply_by_LGR" , 786},
            	{"S_OF_loss_of_supply_by_LGR" , 787},
            	{"relevant_evt_OF_loss_of_supply_by_LGR" , 788},
            	{"required_OF_loss_of_supply_by_TA" , 789},
            	{"already_S_OF_loss_of_supply_by_TA" , 790},
            	{"S_OF_loss_of_supply_by_TA" , 791},
            	{"relevant_evt_OF_loss_of_supply_by_TA" , 792},
            	{"required_OF_loss_of_supply_by_TA1" , 793},
            	{"already_S_OF_loss_of_supply_by_TA1" , 794},
            	{"S_OF_loss_of_supply_by_TA1" , 795},
            	{"relevant_evt_OF_loss_of_supply_by_TA1" , 796},
            	{"required_OF_loss_of_supply_by_TAC" , 797},
            	{"already_S_OF_loss_of_supply_by_TAC" , 798},
            	{"S_OF_loss_of_supply_by_TAC" , 799},
            	{"relevant_evt_OF_loss_of_supply_by_TAC" , 800},
            	{"required_OF_loss_of_supply_by_TS" , 801},
            	{"already_S_OF_loss_of_supply_by_TS" , 802},
            	{"S_OF_loss_of_supply_by_TS" , 803},
            	{"relevant_evt_OF_loss_of_supply_by_TS" , 804},
            	{"required_OF_loss_of_supply_by_TS1" , 805},
            	{"already_S_OF_loss_of_supply_by_TS1" , 806},
            	{"S_OF_loss_of_supply_by_TS1" , 807},
            	{"relevant_evt_OF_loss_of_supply_by_TS1" , 808},
            	{"required_OF_loss_of_supply_by_UNIT" , 809},
            	{"already_S_OF_loss_of_supply_by_UNIT" , 810},
            	{"S_OF_loss_of_supply_by_UNIT" , 811},
            	{"relevant_evt_OF_loss_of_supply_by_UNIT" , 812},
            	{"required_OF_on_demand_house" , 813},
            	{"already_S_OF_on_demand_house" , 814},
            	{"S_OF_on_demand_house" , 815},
            	{"relevant_evt_OF_on_demand_house" , 816},
            	{"waiting_for_rep_OF_on_demand_house" , 817},
            	{"failI_OF_on_demand_house" , 818},
            	{"to_be_fired_OF_on_demand_house" , 819},
            	{"already_standby_OF_on_demand_house" , 820},
            	{"already_required_OF_on_demand_house" , 821},
            	{"at_work_OF_repair_constraint" , 822}},

//            std::map<std::string, size_t> mFigarofloatelementindex =
                                            { {"S_OF_UE_1_Fail" , 0}},
//            std::map<std::string, size_t> mFigarofloatelementindex =
                { },

//            std::map<std::string, size_t> mFigarointelementindex =
                {
            	{"priority_OF_OPTIONS" , 0},
            	{"nb_avail_repairmen_OF_repair_constraint" , 1}},

//            std::map<std::string, size_t> mFigaroenumelementindex =
                                            { },
//            std::map<std::string, size_t> failure_variable_names =
                
                {"S_OF_UE_1_Fail"},
//            std::set<std::string> enum_variables_names =
                { },

//            std::set<std::string> float_variables_names =
                { },
//                std::string const topevent=
                "S_OF_UE_1",
//                static int const numBoolState =
                823 ,
//                static int const numBoolFailureState =
                1 ,
//                static int const numFloatState =
                0 ,
//                static int const numIntState =
                2 ,
//                static int const numEnumState =
                0 ,
//                bool ins_transition_found =
                false){}
            /* ---------- CODING ENUMERATED VARIABLES STATES ------------ */
            enum enum_status {};

            
//            std::array<bool, numBoolState> boolState;
//            std::array<bool, numBoolState> backupBoolState;
//            std::array<float, numFloatState> floatState;
//            std::array<float, numFloatState> backupFloatState;
//            std::array<int, numIntState> intState;
//            std::array<int, numIntState> backupIntState;
//            std::array<int, numEnumState> enumState;
//            std::array<int, numEnumState> backupEnumState;
//

            bool REINITIALISATION_OF_priority_OF_OPTIONS ;
            bool REINITIALISATION_OF_S_OF_auto_exclusions ;
            bool REINITIALISATION_OF_required_OF_AND_3 ;
            bool REINITIALISATION_OF_S_OF_AND_3 ;
            bool REINITIALISATION_OF_relevant_evt_OF_AND_3 ;
            bool REINITIALISATION_OF_required_OF_BATTERY_A_lost ;
            bool REINITIALISATION_OF_S_OF_BATTERY_A_lost ;
            bool REINITIALISATION_OF_relevant_evt_OF_BATTERY_A_lost ;
            bool REINITIALISATION_OF_required_OF_BATTERY_B_lost ;
            bool REINITIALISATION_OF_S_OF_BATTERY_B_lost ;
            bool REINITIALISATION_OF_relevant_evt_OF_BATTERY_B_lost ;
            bool REINITIALISATION_OF_required_OF_BATT_A1 ;
            bool REINITIALISATION_OF_S_OF_BATT_A1 ;
            bool REINITIALISATION_OF_relevant_evt_OF_BATT_A1 ;
            bool REINITIALISATION_OF_required_OF_BATT_A2 ;
            bool REINITIALISATION_OF_S_OF_BATT_A2 ;
            bool REINITIALISATION_OF_relevant_evt_OF_BATT_A2 ;
            bool REINITIALISATION_OF_required_OF_BATT_B1 ;
            bool REINITIALISATION_OF_S_OF_BATT_B1 ;
            bool REINITIALISATION_OF_relevant_evt_OF_BATT_B1 ;
            bool REINITIALISATION_OF_required_OF_BATT_B2 ;
            bool REINITIALISATION_OF_S_OF_BATT_B2 ;
            bool REINITIALISATION_OF_relevant_evt_OF_BATT_B2 ;
            bool REINITIALISATION_OF_required_OF_CB_LGD2_unable ;
            bool REINITIALISATION_OF_S_OF_CB_LGD2_unable ;
            bool REINITIALISATION_OF_relevant_evt_OF_CB_LGD2_unable ;
            bool REINITIALISATION_OF_required_OF_CB_LGF2_unable ;
            bool REINITIALISATION_OF_S_OF_CB_LGF2_unable ;
            bool REINITIALISATION_OF_relevant_evt_OF_CB_LGF2_unable ;
            bool REINITIALISATION_OF_required_OF_CB_LHA12_unable ;
            bool REINITIALISATION_OF_S_OF_CB_LHA12_unable ;
            bool REINITIALISATION_OF_relevant_evt_OF_CB_LHA12_unable ;
            bool REINITIALISATION_OF_required_OF_CB_LHA3_unable ;
            bool REINITIALISATION_OF_S_OF_CB_LHA3_unable ;
            bool REINITIALISATION_OF_relevant_evt_OF_CB_LHA3_unable ;
            bool REINITIALISATION_OF_required_OF_CB_LHB12_unable ;
            bool REINITIALISATION_OF_S_OF_CB_LHB12_unable ;
            bool REINITIALISATION_OF_relevant_evt_OF_CB_LHB12_unable ;
            bool REINITIALISATION_OF_required_OF_CCF_DG ;
            bool REINITIALISATION_OF_S_OF_CCF_DG ;
            bool REINITIALISATION_OF_relevant_evt_OF_CCF_DG ;
            bool REINITIALISATION_OF_required_OF_CCF_GEV_LGR ;
            bool REINITIALISATION_OF_S_OF_CCF_GEV_LGR ;
            bool REINITIALISATION_OF_relevant_evt_OF_CCF_GEV_LGR ;
            bool REINITIALISATION_OF_required_OF_DGA_long ;
            bool REINITIALISATION_OF_S_OF_DGA_long ;
            bool REINITIALISATION_OF_relevant_evt_OF_DGA_long ;
            bool REINITIALISATION_OF_required_OF_DGA_lost ;
            bool REINITIALISATION_OF_S_OF_DGA_lost ;
            bool REINITIALISATION_OF_relevant_evt_OF_DGA_lost ;
            bool REINITIALISATION_OF_required_OF_DGA_short ;
            bool REINITIALISATION_OF_S_OF_DGA_short ;
            bool REINITIALISATION_OF_relevant_evt_OF_DGA_short ;
            bool REINITIALISATION_OF_required_OF_DGB_long ;
            bool REINITIALISATION_OF_S_OF_DGB_long ;
            bool REINITIALISATION_OF_relevant_evt_OF_DGB_long ;
            bool REINITIALISATION_OF_required_OF_DGB_lost ;
            bool REINITIALISATION_OF_S_OF_DGB_lost ;
            bool REINITIALISATION_OF_relevant_evt_OF_DGB_lost ;
            bool REINITIALISATION_OF_required_OF_DGB_short ;
            bool REINITIALISATION_OF_S_OF_DGB_short ;
            bool REINITIALISATION_OF_relevant_evt_OF_DGB_short ;
            bool REINITIALISATION_OF_required_OF_GEV ;
            bool REINITIALISATION_OF_S_OF_GEV ;
            bool REINITIALISATION_OF_relevant_evt_OF_GEV ;
            bool REINITIALISATION_OF_required_OF_GRID ;
            bool REINITIALISATION_OF_S_OF_GRID ;
            bool REINITIALISATION_OF_relevant_evt_OF_GRID ;
            bool REINITIALISATION_OF_required_OF_LBA ;
            bool REINITIALISATION_OF_S_OF_LBA ;
            bool REINITIALISATION_OF_relevant_evt_OF_LBA ;
            bool REINITIALISATION_OF_required_OF_LBA_by_line1_lost ;
            bool REINITIALISATION_OF_S_OF_LBA_by_line1_lost ;
            bool REINITIALISATION_OF_relevant_evt_OF_LBA_by_line1_lost ;
            bool REINITIALISATION_OF_required_OF_LBA_by_line2_lost ;
            bool REINITIALISATION_OF_S_OF_LBA_by_line2_lost ;
            bool REINITIALISATION_OF_relevant_evt_OF_LBA_by_line2_lost ;
            bool REINITIALISATION_OF_required_OF_LBA_by_others_lost ;
            bool REINITIALISATION_OF_S_OF_LBA_by_others_lost ;
            bool REINITIALISATION_OF_relevant_evt_OF_LBA_by_others_lost ;
            bool REINITIALISATION_OF_required_OF_LBA_lost ;
            bool REINITIALISATION_OF_S_OF_LBA_lost ;
            bool REINITIALISATION_OF_relevant_evt_OF_LBA_lost ;
            bool REINITIALISATION_OF_required_OF_LBA_not_fed ;
            bool REINITIALISATION_OF_S_OF_LBA_not_fed ;
            bool REINITIALISATION_OF_relevant_evt_OF_LBA_not_fed ;
            bool REINITIALISATION_OF_required_OF_LBB ;
            bool REINITIALISATION_OF_S_OF_LBB ;
            bool REINITIALISATION_OF_relevant_evt_OF_LBB ;
            bool REINITIALISATION_OF_required_OF_LBB_by_line1_lost ;
            bool REINITIALISATION_OF_S_OF_LBB_by_line1_lost ;
            bool REINITIALISATION_OF_relevant_evt_OF_LBB_by_line1_lost ;
            bool REINITIALISATION_OF_required_OF_LBB_by_line2_lost ;
            bool REINITIALISATION_OF_S_OF_LBB_by_line2_lost ;
            bool REINITIALISATION_OF_relevant_evt_OF_LBB_by_line2_lost ;
            bool REINITIALISATION_OF_required_OF_LBB_lost ;
            bool REINITIALISATION_OF_S_OF_LBB_lost ;
            bool REINITIALISATION_OF_relevant_evt_OF_LBB_lost ;
            bool REINITIALISATION_OF_required_OF_LBB_not_fed ;
            bool REINITIALISATION_OF_S_OF_LBB_not_fed ;
            bool REINITIALISATION_OF_relevant_evt_OF_LBB_not_fed ;
            bool REINITIALISATION_OF_required_OF_LGA ;
            bool REINITIALISATION_OF_S_OF_LGA ;
            bool REINITIALISATION_OF_relevant_evt_OF_LGA ;
            bool REINITIALISATION_OF_required_OF_LGB ;
            bool REINITIALISATION_OF_S_OF_LGB ;
            bool REINITIALISATION_OF_relevant_evt_OF_LGB ;
            bool REINITIALISATION_OF_required_OF_LGD ;
            bool REINITIALISATION_OF_S_OF_LGD ;
            bool REINITIALISATION_OF_relevant_evt_OF_LGD ;
            bool REINITIALISATION_OF_required_OF_LGD_not_fed ;
            bool REINITIALISATION_OF_S_OF_LGD_not_fed ;
            bool REINITIALISATION_OF_relevant_evt_OF_LGD_not_fed ;
            bool REINITIALISATION_OF_required_OF_LGE ;
            bool REINITIALISATION_OF_S_OF_LGE ;
            bool REINITIALISATION_OF_relevant_evt_OF_LGE ;
            bool REINITIALISATION_OF_required_OF_LGF ;
            bool REINITIALISATION_OF_S_OF_LGF ;
            bool REINITIALISATION_OF_relevant_evt_OF_LGF ;
            bool REINITIALISATION_OF_required_OF_LGF_not_fed ;
            bool REINITIALISATION_OF_S_OF_LGF_not_fed ;
            bool REINITIALISATION_OF_relevant_evt_OF_LGF_not_fed ;
            bool REINITIALISATION_OF_required_OF_LGR ;
            bool REINITIALISATION_OF_S_OF_LGR ;
            bool REINITIALISATION_OF_relevant_evt_OF_LGR ;
            bool REINITIALISATION_OF_required_OF_LHA ;
            bool REINITIALISATION_OF_S_OF_LHA ;
            bool REINITIALISATION_OF_relevant_evt_OF_LHA ;
            bool REINITIALISATION_OF_required_OF_LHA_and_LHB_lost ;
            bool REINITIALISATION_OF_S_OF_LHA_and_LHB_lost ;
            bool REINITIALISATION_OF_relevant_evt_OF_LHA_and_LHB_lost ;
            bool REINITIALISATION_OF_required_OF_LHA_lost ;
            bool REINITIALISATION_OF_S_OF_LHA_lost ;
            bool REINITIALISATION_OF_relevant_evt_OF_LHA_lost ;
            bool REINITIALISATION_OF_required_OF_LHA_not_fed ;
            bool REINITIALISATION_OF_S_OF_LHA_not_fed ;
            bool REINITIALISATION_OF_relevant_evt_OF_LHA_not_fed ;
            bool REINITIALISATION_OF_required_OF_LHB ;
            bool REINITIALISATION_OF_S_OF_LHB ;
            bool REINITIALISATION_OF_relevant_evt_OF_LHB ;
            bool REINITIALISATION_OF_required_OF_LHB_lost ;
            bool REINITIALISATION_OF_S_OF_LHB_lost ;
            bool REINITIALISATION_OF_relevant_evt_OF_LHB_lost ;
            bool REINITIALISATION_OF_required_OF_LHB_not_fed ;
            bool REINITIALISATION_OF_S_OF_LHB_not_fed ;
            bool REINITIALISATION_OF_relevant_evt_OF_LHB_not_fed ;
            bool REINITIALISATION_OF_required_OF_LKE ;
            bool REINITIALISATION_OF_S_OF_LKE ;
            bool REINITIALISATION_OF_relevant_evt_OF_LKE ;
            bool REINITIALISATION_OF_required_OF_LKI ;
            bool REINITIALISATION_OF_S_OF_LKI ;
            bool REINITIALISATION_OF_relevant_evt_OF_LKI ;
            bool REINITIALISATION_OF_required_OF_LLA ;
            bool REINITIALISATION_OF_S_OF_LLA ;
            bool REINITIALISATION_OF_relevant_evt_OF_LLA ;
            bool REINITIALISATION_OF_required_OF_LLD ;
            bool REINITIALISATION_OF_S_OF_LLD ;
            bool REINITIALISATION_OF_relevant_evt_OF_LLD ;
            bool REINITIALISATION_OF_required_OF_OR_14 ;
            bool REINITIALISATION_OF_S_OF_OR_14 ;
            bool REINITIALISATION_OF_relevant_evt_OF_OR_14 ;
            bool REINITIALISATION_OF_required_OF_RC_CB_LGD2 ;
            bool REINITIALISATION_OF_S_OF_RC_CB_LGD2 ;
            bool REINITIALISATION_OF_relevant_evt_OF_RC_CB_LGD2 ;
            bool REINITIALISATION_OF_to_be_fired_OF_RC_CB_LGD2 ;
            bool REINITIALISATION_OF_required_OF_RC_CB_LGD2_ ;
            bool REINITIALISATION_OF_S_OF_RC_CB_LGD2_ ;
            bool REINITIALISATION_OF_relevant_evt_OF_RC_CB_LGD2_ ;
            bool REINITIALISATION_OF_required_OF_RC_CB_LGF2 ;
            bool REINITIALISATION_OF_S_OF_RC_CB_LGF2 ;
            bool REINITIALISATION_OF_relevant_evt_OF_RC_CB_LGF2 ;
            bool REINITIALISATION_OF_to_be_fired_OF_RC_CB_LGF2 ;
            bool REINITIALISATION_OF_required_OF_RC_CB_LGF2_ ;
            bool REINITIALISATION_OF_S_OF_RC_CB_LGF2_ ;
            bool REINITIALISATION_OF_relevant_evt_OF_RC_CB_LGF2_ ;
            bool REINITIALISATION_OF_required_OF_RC_CB_LHA2 ;
            bool REINITIALISATION_OF_S_OF_RC_CB_LHA2 ;
            bool REINITIALISATION_OF_relevant_evt_OF_RC_CB_LHA2 ;
            bool REINITIALISATION_OF_to_be_fired_OF_RC_CB_LHA2 ;
            bool REINITIALISATION_OF_required_OF_RC_CB_LHA2_ ;
            bool REINITIALISATION_OF_S_OF_RC_CB_LHA2_ ;
            bool REINITIALISATION_OF_relevant_evt_OF_RC_CB_LHA2_ ;
            bool REINITIALISATION_OF_required_OF_RC_CB_LHA3 ;
            bool REINITIALISATION_OF_S_OF_RC_CB_LHA3 ;
            bool REINITIALISATION_OF_relevant_evt_OF_RC_CB_LHA3 ;
            bool REINITIALISATION_OF_to_be_fired_OF_RC_CB_LHA3 ;
            bool REINITIALISATION_OF_required_OF_RC_CB_LHA3_ ;
            bool REINITIALISATION_OF_S_OF_RC_CB_LHA3_ ;
            bool REINITIALISATION_OF_relevant_evt_OF_RC_CB_LHA3_ ;
            bool REINITIALISATION_OF_required_OF_RC_CB_LHB2 ;
            bool REINITIALISATION_OF_S_OF_RC_CB_LHB2 ;
            bool REINITIALISATION_OF_relevant_evt_OF_RC_CB_LHB2 ;
            bool REINITIALISATION_OF_to_be_fired_OF_RC_CB_LHB2 ;
            bool REINITIALISATION_OF_required_OF_RC_CB_LHB2_ ;
            bool REINITIALISATION_OF_S_OF_RC_CB_LHB2_ ;
            bool REINITIALISATION_OF_relevant_evt_OF_RC_CB_LHB2_ ;
            bool REINITIALISATION_OF_required_OF_RDA1 ;
            bool REINITIALISATION_OF_S_OF_RDA1 ;
            bool REINITIALISATION_OF_relevant_evt_OF_RDA1 ;
            bool REINITIALISATION_OF_required_OF_RDA2 ;
            bool REINITIALISATION_OF_S_OF_RDA2 ;
            bool REINITIALISATION_OF_relevant_evt_OF_RDA2 ;
            bool REINITIALISATION_OF_required_OF_RDB1 ;
            bool REINITIALISATION_OF_S_OF_RDB1 ;
            bool REINITIALISATION_OF_relevant_evt_OF_RDB1 ;
            bool REINITIALISATION_OF_required_OF_RDB2 ;
            bool REINITIALISATION_OF_S_OF_RDB2 ;
            bool REINITIALISATION_OF_relevant_evt_OF_RDB2 ;
            bool REINITIALISATION_OF_required_OF_RO_CB_LHA1 ;
            bool REINITIALISATION_OF_S_OF_RO_CB_LHA1 ;
            bool REINITIALISATION_OF_relevant_evt_OF_RO_CB_LHA1 ;
            bool REINITIALISATION_OF_to_be_fired_OF_RO_CB_LHA1 ;
            bool REINITIALISATION_OF_required_OF_RO_CB_LHA1_ ;
            bool REINITIALISATION_OF_S_OF_RO_CB_LHA1_ ;
            bool REINITIALISATION_OF_relevant_evt_OF_RO_CB_LHA1_ ;
            bool REINITIALISATION_OF_required_OF_RO_CB_LHA2 ;
            bool REINITIALISATION_OF_S_OF_RO_CB_LHA2 ;
            bool REINITIALISATION_OF_relevant_evt_OF_RO_CB_LHA2 ;
            bool REINITIALISATION_OF_to_be_fired_OF_RO_CB_LHA2 ;
            bool REINITIALISATION_OF_required_OF_RO_CB_LHA2_ ;
            bool REINITIALISATION_OF_S_OF_RO_CB_LHA2_ ;
            bool REINITIALISATION_OF_relevant_evt_OF_RO_CB_LHA2_ ;
            bool REINITIALISATION_OF_required_OF_RO_CB_LHB1 ;
            bool REINITIALISATION_OF_S_OF_RO_CB_LHB1 ;
            bool REINITIALISATION_OF_relevant_evt_OF_RO_CB_LHB1 ;
            bool REINITIALISATION_OF_to_be_fired_OF_RO_CB_LHB1 ;
            bool REINITIALISATION_OF_required_OF_RO_CB_LHB1_ ;
            bool REINITIALISATION_OF_S_OF_RO_CB_LHB1_ ;
            bool REINITIALISATION_OF_relevant_evt_OF_RO_CB_LHB1_ ;
            bool REINITIALISATION_OF_required_OF_SH_CB_GEV ;
            bool REINITIALISATION_OF_S_OF_SH_CB_GEV ;
            bool REINITIALISATION_OF_relevant_evt_OF_SH_CB_GEV ;
            bool REINITIALISATION_OF_required_OF_SH_CB_LBA1 ;
            bool REINITIALISATION_OF_S_OF_SH_CB_LBA1 ;
            bool REINITIALISATION_OF_relevant_evt_OF_SH_CB_LBA1 ;
            bool REINITIALISATION_OF_required_OF_SH_CB_LBA2 ;
            bool REINITIALISATION_OF_S_OF_SH_CB_LBA2 ;
            bool REINITIALISATION_OF_relevant_evt_OF_SH_CB_LBA2 ;
            bool REINITIALISATION_OF_required_OF_SH_CB_LBB1 ;
            bool REINITIALISATION_OF_S_OF_SH_CB_LBB1 ;
            bool REINITIALISATION_OF_relevant_evt_OF_SH_CB_LBB1 ;
            bool REINITIALISATION_OF_required_OF_SH_CB_LBB2 ;
            bool REINITIALISATION_OF_S_OF_SH_CB_LBB2 ;
            bool REINITIALISATION_OF_relevant_evt_OF_SH_CB_LBB2 ;
            bool REINITIALISATION_OF_required_OF_SH_CB_LGA ;
            bool REINITIALISATION_OF_S_OF_SH_CB_LGA ;
            bool REINITIALISATION_OF_relevant_evt_OF_SH_CB_LGA ;
            bool REINITIALISATION_OF_required_OF_SH_CB_LGB ;
            bool REINITIALISATION_OF_S_OF_SH_CB_LGB ;
            bool REINITIALISATION_OF_relevant_evt_OF_SH_CB_LGB ;
            bool REINITIALISATION_OF_required_OF_SH_CB_LGD1 ;
            bool REINITIALISATION_OF_S_OF_SH_CB_LGD1 ;
            bool REINITIALISATION_OF_relevant_evt_OF_SH_CB_LGD1 ;
            bool REINITIALISATION_OF_required_OF_SH_CB_LGD2 ;
            bool REINITIALISATION_OF_S_OF_SH_CB_LGD2 ;
            bool REINITIALISATION_OF_relevant_evt_OF_SH_CB_LGD2 ;
            bool REINITIALISATION_OF_required_OF_SH_CB_LGE1 ;
            bool REINITIALISATION_OF_S_OF_SH_CB_LGE1 ;
            bool REINITIALISATION_OF_relevant_evt_OF_SH_CB_LGE1 ;
            bool REINITIALISATION_OF_required_OF_SH_CB_LGF1 ;
            bool REINITIALISATION_OF_S_OF_SH_CB_LGF1 ;
            bool REINITIALISATION_OF_relevant_evt_OF_SH_CB_LGF1 ;
            bool REINITIALISATION_OF_required_OF_SH_CB_LGF2 ;
            bool REINITIALISATION_OF_S_OF_SH_CB_LGF2 ;
            bool REINITIALISATION_OF_relevant_evt_OF_SH_CB_LGF2 ;
            bool REINITIALISATION_OF_required_OF_SH_CB_LHA1 ;
            bool REINITIALISATION_OF_S_OF_SH_CB_LHA1 ;
            bool REINITIALISATION_OF_relevant_evt_OF_SH_CB_LHA1 ;
            bool REINITIALISATION_OF_required_OF_SH_CB_LHA2 ;
            bool REINITIALISATION_OF_S_OF_SH_CB_LHA2 ;
            bool REINITIALISATION_OF_relevant_evt_OF_SH_CB_LHA2 ;
            bool REINITIALISATION_OF_required_OF_SH_CB_LHA3 ;
            bool REINITIALISATION_OF_S_OF_SH_CB_LHA3 ;
            bool REINITIALISATION_OF_relevant_evt_OF_SH_CB_LHA3 ;
            bool REINITIALISATION_OF_required_OF_SH_CB_LHB1 ;
            bool REINITIALISATION_OF_S_OF_SH_CB_LHB1 ;
            bool REINITIALISATION_OF_relevant_evt_OF_SH_CB_LHB1 ;
            bool REINITIALISATION_OF_required_OF_SH_CB_LHB2 ;
            bool REINITIALISATION_OF_S_OF_SH_CB_LHB2 ;
            bool REINITIALISATION_OF_relevant_evt_OF_SH_CB_LHB2 ;
            bool REINITIALISATION_OF_required_OF_SH_CB_RDA1 ;
            bool REINITIALISATION_OF_S_OF_SH_CB_RDA1 ;
            bool REINITIALISATION_OF_relevant_evt_OF_SH_CB_RDA1 ;
            bool REINITIALISATION_OF_required_OF_SH_CB_RDA2 ;
            bool REINITIALISATION_OF_S_OF_SH_CB_RDA2 ;
            bool REINITIALISATION_OF_relevant_evt_OF_SH_CB_RDA2 ;
            bool REINITIALISATION_OF_required_OF_SH_CB_RDB1 ;
            bool REINITIALISATION_OF_S_OF_SH_CB_RDB1 ;
            bool REINITIALISATION_OF_relevant_evt_OF_SH_CB_RDB1 ;
            bool REINITIALISATION_OF_required_OF_SH_CB_RDB2 ;
            bool REINITIALISATION_OF_S_OF_SH_CB_RDB2 ;
            bool REINITIALISATION_OF_relevant_evt_OF_SH_CB_RDB2 ;
            bool REINITIALISATION_OF_required_OF_SH_CB_TUA1 ;
            bool REINITIALISATION_OF_S_OF_SH_CB_TUA1 ;
            bool REINITIALISATION_OF_relevant_evt_OF_SH_CB_TUA1 ;
            bool REINITIALISATION_OF_required_OF_SH_CB_TUA2 ;
            bool REINITIALISATION_OF_S_OF_SH_CB_TUA2 ;
            bool REINITIALISATION_OF_relevant_evt_OF_SH_CB_TUA2 ;
            bool REINITIALISATION_OF_required_OF_SH_CB_TUB1 ;
            bool REINITIALISATION_OF_S_OF_SH_CB_TUB1 ;
            bool REINITIALISATION_OF_relevant_evt_OF_SH_CB_TUB1 ;
            bool REINITIALISATION_OF_required_OF_SH_CB_TUB2 ;
            bool REINITIALISATION_OF_S_OF_SH_CB_TUB2 ;
            bool REINITIALISATION_OF_relevant_evt_OF_SH_CB_TUB2 ;
            bool REINITIALISATION_OF_required_OF_SH_CB_line_GEV ;
            bool REINITIALISATION_OF_S_OF_SH_CB_line_GEV ;
            bool REINITIALISATION_OF_relevant_evt_OF_SH_CB_line_GEV ;
            bool REINITIALISATION_OF_required_OF_SH_CB_line_LGR ;
            bool REINITIALISATION_OF_S_OF_SH_CB_line_LGR ;
            bool REINITIALISATION_OF_relevant_evt_OF_SH_CB_line_LGR ;
            bool REINITIALISATION_OF_required_OF_SH_GEV_or_LGR ;
            bool REINITIALISATION_OF_S_OF_SH_GEV_or_LGR ;
            bool REINITIALISATION_OF_relevant_evt_OF_SH_GEV_or_LGR ;
            bool REINITIALISATION_OF_required_OF_SUBSTATION ;
            bool REINITIALISATION_OF_S_OF_SUBSTATION ;
            bool REINITIALISATION_OF_relevant_evt_OF_SUBSTATION ;
            bool REINITIALISATION_OF_required_OF_TA ;
            bool REINITIALISATION_OF_S_OF_TA ;
            bool REINITIALISATION_OF_relevant_evt_OF_TA ;
            bool REINITIALISATION_OF_required_OF_TAC ;
            bool REINITIALISATION_OF_S_OF_TAC ;
            bool REINITIALISATION_OF_relevant_evt_OF_TAC ;
            bool REINITIALISATION_OF_required_OF_TA_lost ;
            bool REINITIALISATION_OF_S_OF_TA_lost ;
            bool REINITIALISATION_OF_relevant_evt_OF_TA_lost ;
            bool REINITIALISATION_OF_required_OF_TP ;
            bool REINITIALISATION_OF_S_OF_TP ;
            bool REINITIALISATION_OF_relevant_evt_OF_TP ;
            bool REINITIALISATION_OF_required_OF_TS ;
            bool REINITIALISATION_OF_S_OF_TS ;
            bool REINITIALISATION_OF_relevant_evt_OF_TS ;
            bool REINITIALISATION_OF_required_OF_TS_lost ;
            bool REINITIALISATION_OF_S_OF_TS_lost ;
            bool REINITIALISATION_OF_relevant_evt_OF_TS_lost ;
            bool REINITIALISATION_OF_required_OF_TS_not_fed ;
            bool REINITIALISATION_OF_S_OF_TS_not_fed ;
            bool REINITIALISATION_OF_relevant_evt_OF_TS_not_fed ;
            bool REINITIALISATION_OF_required_OF_TUA1 ;
            bool REINITIALISATION_OF_S_OF_TUA1 ;
            bool REINITIALISATION_OF_relevant_evt_OF_TUA1 ;
            bool REINITIALISATION_OF_required_OF_TUA2 ;
            bool REINITIALISATION_OF_S_OF_TUA2 ;
            bool REINITIALISATION_OF_relevant_evt_OF_TUA2 ;
            bool REINITIALISATION_OF_required_OF_TUB1 ;
            bool REINITIALISATION_OF_S_OF_TUB1 ;
            bool REINITIALISATION_OF_relevant_evt_OF_TUB1 ;
            bool REINITIALISATION_OF_required_OF_TUB2 ;
            bool REINITIALISATION_OF_S_OF_TUB2 ;
            bool REINITIALISATION_OF_relevant_evt_OF_TUB2 ;
            bool REINITIALISATION_OF_required_OF_UE_1 ;
            bool REINITIALISATION_OF_S_OF_UE_1 ;
            bool REINITIALISATION_OF_relevant_evt_OF_UE_1 ;
            bool REINITIALISATION_OF_required_OF_UNIT ;
            bool REINITIALISATION_OF_S_OF_UNIT ;
            bool REINITIALISATION_OF_relevant_evt_OF_UNIT ;
            bool REINITIALISATION_OF_required_OF_in_function_house ;
            bool REINITIALISATION_OF_S_OF_in_function_house ;
            bool REINITIALISATION_OF_relevant_evt_OF_in_function_house ;
            bool REINITIALISATION_OF_required_OF_loss_of_houseload_operation ;
            bool REINITIALISATION_OF_S_OF_loss_of_houseload_operation ;
            bool REINITIALISATION_OF_relevant_evt_OF_loss_of_houseload_operation ;
            bool REINITIALISATION_OF_required_OF_demand_CCF_DG ;
            bool REINITIALISATION_OF_S_OF_demand_CCF_DG ;
            bool REINITIALISATION_OF_relevant_evt_OF_demand_CCF_DG ;
            bool REINITIALISATION_OF_to_be_fired_OF_demand_CCF_DG ;
            bool REINITIALISATION_OF_required_OF_demand_DGA ;
            bool REINITIALISATION_OF_S_OF_demand_DGA ;
            bool REINITIALISATION_OF_relevant_evt_OF_demand_DGA ;
            bool REINITIALISATION_OF_to_be_fired_OF_demand_DGA ;
            bool REINITIALISATION_OF_required_OF_demand_DGB ;
            bool REINITIALISATION_OF_S_OF_demand_DGB ;
            bool REINITIALISATION_OF_relevant_evt_OF_demand_DGB ;
            bool REINITIALISATION_OF_to_be_fired_OF_demand_DGB ;
            bool REINITIALISATION_OF_required_OF_demand_TAC ;
            bool REINITIALISATION_OF_S_OF_demand_TAC ;
            bool REINITIALISATION_OF_relevant_evt_OF_demand_TAC ;
            bool REINITIALISATION_OF_to_be_fired_OF_demand_TAC ;
            bool REINITIALISATION_OF_required_OF_loss_of_supply_by_DGA ;
            bool REINITIALISATION_OF_S_OF_loss_of_supply_by_DGA ;
            bool REINITIALISATION_OF_relevant_evt_OF_loss_of_supply_by_DGA ;
            bool REINITIALISATION_OF_required_OF_loss_of_supply_by_DGA_and_TAC ;
            bool REINITIALISATION_OF_S_OF_loss_of_supply_by_DGA_and_TAC ;
            bool REINITIALISATION_OF_relevant_evt_OF_loss_of_supply_by_DGA_and_TAC ;
            bool REINITIALISATION_OF_required_OF_loss_of_supply_by_DGB ;
            bool REINITIALISATION_OF_S_OF_loss_of_supply_by_DGB ;
            bool REINITIALISATION_OF_relevant_evt_OF_loss_of_supply_by_DGB ;
            bool REINITIALISATION_OF_required_OF_loss_of_supply_by_GEV ;
            bool REINITIALISATION_OF_S_OF_loss_of_supply_by_GEV ;
            bool REINITIALISATION_OF_relevant_evt_OF_loss_of_supply_by_GEV ;
            bool REINITIALISATION_OF_required_OF_loss_of_supply_by_LGD ;
            bool REINITIALISATION_OF_S_OF_loss_of_supply_by_LGD ;
            bool REINITIALISATION_OF_relevant_evt_OF_loss_of_supply_by_LGD ;
            bool REINITIALISATION_OF_required_OF_loss_of_supply_by_LGF ;
            bool REINITIALISATION_OF_S_OF_loss_of_supply_by_LGF ;
            bool REINITIALISATION_OF_relevant_evt_OF_loss_of_supply_by_LGF ;
            bool REINITIALISATION_OF_required_OF_loss_of_supply_by_LGR ;
            bool REINITIALISATION_OF_S_OF_loss_of_supply_by_LGR ;
            bool REINITIALISATION_OF_relevant_evt_OF_loss_of_supply_by_LGR ;
            bool REINITIALISATION_OF_required_OF_loss_of_supply_by_TA ;
            bool REINITIALISATION_OF_S_OF_loss_of_supply_by_TA ;
            bool REINITIALISATION_OF_relevant_evt_OF_loss_of_supply_by_TA ;
            bool REINITIALISATION_OF_required_OF_loss_of_supply_by_TA1 ;
            bool REINITIALISATION_OF_S_OF_loss_of_supply_by_TA1 ;
            bool REINITIALISATION_OF_relevant_evt_OF_loss_of_supply_by_TA1 ;
            bool REINITIALISATION_OF_required_OF_loss_of_supply_by_TAC ;
            bool REINITIALISATION_OF_S_OF_loss_of_supply_by_TAC ;
            bool REINITIALISATION_OF_relevant_evt_OF_loss_of_supply_by_TAC ;
            bool REINITIALISATION_OF_required_OF_loss_of_supply_by_TS ;
            bool REINITIALISATION_OF_S_OF_loss_of_supply_by_TS ;
            bool REINITIALISATION_OF_relevant_evt_OF_loss_of_supply_by_TS ;
            bool REINITIALISATION_OF_required_OF_loss_of_supply_by_TS1 ;
            bool REINITIALISATION_OF_S_OF_loss_of_supply_by_TS1 ;
            bool REINITIALISATION_OF_relevant_evt_OF_loss_of_supply_by_TS1 ;
            bool REINITIALISATION_OF_required_OF_loss_of_supply_by_UNIT ;
            bool REINITIALISATION_OF_S_OF_loss_of_supply_by_UNIT ;
            bool REINITIALISATION_OF_relevant_evt_OF_loss_of_supply_by_UNIT ;
            bool REINITIALISATION_OF_required_OF_on_demand_house ;
            bool REINITIALISATION_OF_S_OF_on_demand_house ;
            bool REINITIALISATION_OF_relevant_evt_OF_on_demand_house ;
            bool REINITIALISATION_OF_to_be_fired_OF_on_demand_house ;
            
		/* ---------- DECLARATION OF CONSTANTS ------------ */
			bool const initiator_OF_LLA = false;
			int const rep_priority_OF_LBA_by_line2_lost = 1;
			double const lambda_OF_SH_CB_TUB2 = 5e-07;
			bool const failI_FROZEN_OF_RC_CB_LGF2 = false;
			bool const transmit_relevance_OF_LKE = true;
			bool const transmit_relevance_OF_RO_CB_LHB1 = true;
			std::string const calculate_required_OF_LHB_lost = "fn_fathers_and_trig";
			std::string const calculate_required_OF_DGA_lost = "fn_fathers_and_trig";
			bool const initiator_OF_TA = false;
			bool const transmit_relevance_OF_LHA = true;
			bool const failF_FROZEN_OF_SH_CB_LGD1 = false;
			bool const force_relevant_events_OF_LBA_not_fed = false;
			int const rep_priority_OF_LLD = 1;
			bool const force_relevant_events_OF_LGF = false;
			std::string const calculate_required_OF_LKI = "fn_fathers_and_trig";
			bool const failF_FROZEN_OF_SH_CB_LGD2 = false;
			bool const failF_FROZEN_OF_SH_CB_LHA3 = false;
			bool const force_relevant_events_OF_TS = false;
			bool const transmit_relevance_OF_LHA_not_fed = true;
			double const mu_OF_TUA1 = 0.1;
			bool const force_relevant_events_OF_RDA1 = false;
			bool const force_relevant_events_OF_RO_CB_LHA1 = false;
			int const rep_priority_OF_TAC = 1;
			bool const initiator_OF_SH_CB_LHA3 = false;
			bool const failF_FROZEN_OF_UNIT = false;
			std::string const calculate_required_OF_CB_LHB12_unable = "fn_fathers_and_trig";
			double const lambda_OF_LLD = 5e-07;
			bool const failF_FROZEN_OF_DGA_long = false;
			bool const repairable_system_OF_OPTIONS = true;
			bool const initiator_OF_LHB = false;
			bool const transmit_relevance_OF_SH_CB_RDA1 = true;
			bool const force_relevant_events_OF_LGA = false;
			bool const force_relevant_events_OF_RC_CB_LHA3 = false;
			std::string const calculate_required_OF_RC_CB_LGF2_ = "fn_fathers_and_trig";
			double const mu_OF_SH_CB_TUB1 = 0.2;
			int const rep_priority_OF_in_function_house = 1;
			std::string const trigger_kind_OF_t_6_1 = "fn_fathers_and_trig";
			int const rep_priority_OF_DGB_short = 1;
			int const rep_priority_OF_SH_CB_LGE1 = 1;
			std::string const calculate_required_OF_demand_DGB = "fn_fathers_and_trig";
			std::string const calculate_required_OF_RDA2 = "fn_fathers_and_trig";
			bool const transmit_relevance_OF_AND_3 = true;
			bool const force_relevant_events_OF_CCF_DG = false;
			double const lambda_OF_CCF_DG = 5e-05;
			bool const transmit_relevance_OF_DGA_short = true;
			double const mu_OF_RO_CB_LHB1 = 0.2;
			bool const force_relevant_events_OF_LLA = false;
			bool const failI_FROZEN_OF_RO_CB_LHA2 = false;
			bool const transmit_relevance_OF_LBA_by_line1_lost = true;
			int const rep_priority_OF_LGD = 1;
			bool const transmit_relevance_OF_LGE = true;
			double const lambda_OF_BATT_A2 = 2;
			double const lambda_OF_LBA = 5e-07;
			std::string const calculate_required_OF_RC_CB_LGF2 = "fn_fathers_and_trig";
			double const lambda_OF_SH_CB_LGE1 = 5e-07;
			bool const initiator_OF_SH_CB_RDB1 = false;
			bool const force_relevant_events_OF_TA = false;
			bool const transmit_relevance_OF_SH_CB_TUA1 = true;
			bool const transmit_relevance_OF_RC_CB_LHA2_ = true;
			std::string const calculate_required_OF_loss_of_supply_by_GEV = "fn_fathers_and_trig";
			std::string const calculate_required_OF_BATT_B1 = "fn_fathers_and_trig";
			double const agg_lambda_OF_LBA_by_line2_lost = 4.2e-06;
			bool const initiator_OF_BATT_A1 = false;
			double const lambda_OF_SH_CB_LBA1 = 1e-06;
			double const lambda_OF_TUB2 = 2e-07;
			bool const failF_FROZEN_OF_SH_CB_LGF2 = false;
			int const rep_priority_OF_LBB = 1;
			double const mu_OF_SH_CB_RDA1 = 0.2;
			std::string const calculate_required_OF_CB_LHA3_unable = "fn_fathers_and_trig";
			int const rep_priority_OF_LBB_by_line1_lost = 1;
			bool const force_relevant_events_OF_SH_CB_LHA3 = false;
			std::string const trigger_kind_OF_t_1_1 = "fn_fathers_and_trig";
			double const mu_OF_DGA_short = 0.1;
			bool const force_relevant_events_OF_on_demand_house = false;
			bool const transmit_relevance_OF_SH_CB_GEV = true;
			bool const force_relevant_events_OF_LHB = false;
			bool const failI_FROZEN_OF_RO_CB_LHB1 = false;
			bool const initiator_OF_SH_CB_LGF2 = false;
			double const mu_OF_LBA_by_line1_lost = 0.1;
			double const mu_OF_TAC = 0.005;
			std::string const step_down_OF_CB_LGF2_unable = "rep_any";
			bool const force_relevant_events_OF_loss_of_supply_by_LGF = false;
			double const lambda_OF_LBB = 5e-07;
			std::string const calculate_required_OF_LBB_by_line2_lost = "fn_fathers_and_trig";
			bool const failF_FROZEN_OF_RDB2 = false;
			bool const transmit_relevance_OF_SH_CB_TUB2 = true;
			bool const force_relevant_events_OF_UNIT = false;
			std::string const calculate_required_OF_LBB_not_fed = "fn_fathers_and_trig";
			std::string const step_down_OF_CB_LGD2_unable = "rep_any";
			std::string const calculate_required_OF_SH_CB_RDB1 = "fn_fathers_and_trig";
			std::string const trigger_kind_OF_t_3 = "fn_fathers_and_trig";
			bool const force_relevant_events_OF_LHA_lost = false;
			std::string const step_down_OF_CB_LHA12_unable = "rep_any";
			std::string const calculate_required_OF_RO_CB_LHA2 = "fn_fathers_and_trig";
			double const mu_OF_SH_CB_TUA1 = 0.2;
			std::string const calculate_required_OF_GRID = "fn_fathers_and_trig";
			bool const initiator_OF_RDB2 = false;
			bool const failF_FROZEN_OF_TP = false;
			std::string const calculate_required_OF_BATTERY_A_lost = "fn_fathers_and_trig";
			std::string const calculate_required_OF_BATT_A1 = "fn_fathers_and_trig";
			bool const force_relevant_events_OF_loss_of_houseload_operation = false;
			bool const transmit_relevance_OF_loss_of_supply_by_DGA_and_TAC = true;
			bool const initiator_OF_TP = false;
			double const agg_mu_OF_LBB_by_line2_lost = 0.09882353;
			bool const failI_FROZEN_OF_RC_CB_LGD2 = false;
			double const mu_OF_SH_CB_GEV = 0.2;
			bool const transmit_relevance_OF_loss_of_supply_by_TAC = true;
			int const rep_priority_OF_RDA2 = 1;
			int const rep_priority_OF_SH_CB_LBB2 = 1;
			int const rep_priority_OF_SUBSTATION = 1;
			bool const initiator_OF_SH_CB_RDA2 = false;
			double const mu_OF_SH_CB_TUB2 = 0.2;
			bool const force_relevant_events_OF_SH_CB_LGF2 = false;
			std::string const calculate_required_OF_RO_CB_LHB1 = "fn_fathers_and_trig";
			bool const failF_FROZEN_OF_RDB1 = false;
			int const rep_priority_OF_RC_CB_LGF2 = 1;
			bool const transmit_relevance_OF_in_function_house = true;
			bool const initiator_OF_TUB2 = false;
			bool const initiator_OF_SH_CB_LGB = false;
			bool const transmit_relevance_OF_LBA = true;
			int const rep_priority_OF_BATT_B1 = 1;
			bool const force_relevant_events_OF_RDB2 = false;
			bool const transmit_relevance_OF_TUB1 = true;
			bool const failF_FROZEN_OF_SH_CB_RDB2 = false;
			std::string const calculate_required_OF_RC_CB_LHB2_ = "fn_fathers_and_trig";
			int const rep_priority_OF_SH_CB_LHA1 = 1;
			bool const initiator_OF_BATT_B2 = false;
			double const mu_OF_SUBSTATION = 0.05;
			bool const failF_FROZEN_OF_BATT_A2 = false;
			std::string const calculate_required_OF_SH_CB_line_LGR = "fn_fathers_and_trig";
			bool const initiator_OF_SH_CB_LHB1 = false;
			int const rep_priority_OF_LGB = 1;
			double const lambda_OF_GEV = 2e-05;
			bool const failI_FROZEN_OF_demand_DGA = false;
			std::string const calculate_required_OF_SH_CB_LGA = "fn_fathers_and_trig";
			bool const force_relevant_events_OF_TP = false;
			bool const transmit_relevance_OF_RO_CB_LHA2_ = true;
			std::string const calculate_required_OF_RC_CB_LGD2 = "fn_fathers_and_trig";
			bool const transmit_relevance_OF_CCF_GEV_LGR = true;
			std::string const calculate_required_OF_AND_3 = "fn_fathers_and_trig";
			std::string const calculate_required_OF_SH_CB_line_GEV = "fn_fathers_and_trig";
			std::string const calculate_required_OF_DGA_short = "fn_fathers_and_trig";
			std::string const calculate_required_OF_LBA_by_line1_lost = "fn_fathers_and_trig";
			bool const transmit_relevance_OF_LBB_by_line1_lost = true;
			bool const force_relevant_events_OF_SH_CB_RDA2 = false;
			int const rep_priority_OF_GRID = 1;
			std::string const calculate_required_OF_LGE = "fn_fathers_and_trig";
			bool const failF_FROZEN_OF_LGR = false;
			bool const failF_FROZEN_OF_SH_CB_TUA1 = false;
			std::string const calculate_required_OF_RC_CB_LHA2_ = "fn_fathers_and_trig";
			double const mu_OF_TUB1 = 0.1;
			bool const force_relevant_events_OF_TUB2 = false;
			bool const force_relevant_events_OF_SH_CB_LGB = false;
			bool const transmit_relevance_OF_LBA_not_fed = true;
			bool const failI_FROZEN_OF_demand_CCF_DG = false;
			bool const transmit_relevance_OF_LHB_lost = true;
			bool const initiator_OF_SH_CB_LHA2 = false;
			bool const transmit_relevance_OF_DGA_lost = true;
			bool const initiator_OF_SH_CB_LBB1 = false;
			int const rep_priority_OF_TUA1 = 1;
			double const mu_OF_RDA1 = 0.3333333;
			double const mu_OF_LLD = 0.02;
			std::string const calculate_required_OF_TA_lost = "always_true";
			bool const transmit_relevance_OF_LKI = true;
			bool const force_relevant_events_OF_TS_not_fed = false;
			int const rep_priority_OF_SH_CB_TUB1 = 1;
			bool const force_relevant_events_OF_BATT_B2 = false;
			bool const failF_FROZEN_OF_TUA2 = false;
			std::string const calculate_required_OF_SH_CB_GEV = "fn_fathers_and_trig";
			bool const force_relevant_events_OF_SH_CB_LHB1 = false;
			bool const transmit_relevance_OF_LGA = true;
			double const lambda_OF_LGF = 2e-07;
			std::string const calculate_required_OF_demand_DGA = "fn_fathers_and_trig";
			int const rep_priority_OF_LKE = 1;
			int const rep_priority_OF_RO_CB_LHB1 = 1;
			bool const force_relevant_events_OF_RC_CB_LHA2 = false;
			bool const transmit_relevance_OF_CB_LHB12_unable = true;
			std::string const calculate_required_OF_LGD_not_fed = "fn_fathers_and_trig";
			int const rep_priority_OF_LHA = 1;
			bool const failF_FROZEN_OF_SH_CB_TUB2 = false;
			bool const initiator_OF_TUA2 = false;
			bool const aggregation_OF_LBB_by_line1_lost = true;
			bool const transmit_relevance_OF_LLA = true;
			bool const transmit_relevance_OF_demand_DGB = true;
			bool const transmit_relevance_OF_LBA_lost = true;
			double const lambda_OF_RDB1 = 1e-06;
			bool const transmit_relevance_OF_TA = true;
			std::string const calculate_required_OF_SH_CB_LBB1 = "fn_fathers_and_trig";
			double const mu_OF_LGD = 0.02;
			bool const transmit_relevance_OF_loss_of_supply_by_LGD = true;
			double const lambda_OF_TA = 5e-06;
			bool const transmit_relevance_OF_RC_CB_LGF2 = true;
			bool const force_relevant_events_OF_SH_CB_LHA2 = false;
			std::string const calculate_required_OF_demand_CCF_DG = "fn_fathers_and_trig";
			std::string const calculate_required_OF_loss_of_supply_by_DGA_and_TAC = "fn_fathers_and_trig";
			int const rep_priority_OF_SH_CB_RDA1 = 1;
			bool const force_relevant_events_OF_LGR = false;
			bool const transmit_relevance_OF_loss_of_supply_by_GEV = true;
			bool const initiator_OF_GEV = false;
			bool const failI_FROZEN_OF_demand_TAC = false;
			bool const failF_FROZEN_OF_LLD = false;
			bool const transmit_relevance_OF_BATT_B1 = true;
			int const rep_priority_OF_RC_CB_LGD2 = 1;
			double const lambda_OF_SH_CB_line_GEV = 1e-07;
			std::string const calculate_required_OF_loss_of_supply_by_TAC = "fn_fathers_and_trig";
			double const lambda_OF_SH_CB_RDB2 = 1e-06;
			int const rep_priority_OF_SH_CB_line_GEV = 1;
			int const rep_priority_OF_DGA_short = 1;
			std::string const calculate_required_OF_LBA_lost = "always_true";
			bool const transmit_relevance_OF_on_demand_house = true;
			bool const transmit_relevance_OF_LHB = true;
			double const lambda_OF_SH_CB_RDA1 = 1e-06;
			int const rep_priority_OF_LBA_by_line1_lost = 1;
			double const lambda_OF_SH_CB_LGA = 5e-07;
			int const rep_priority_OF_LGE = 1;
			bool const force_relevant_events_OF_TUA2 = false;
			std::string const calculate_required_OF_SH_CB_LBA2 = "fn_fathers_and_trig";
			bool const failF_FROZEN_OF_in_function_house = false;
			std::string const trimming_option_OF_OPTIONS = "maximum";
			std::string const trigger_kind_OF_t_7_1 = "fn_fathers_and_trig";
			bool const initiator_OF_TAC = false;
			bool const failF_FROZEN_OF_DGB_short = false;
			bool const failF_FROZEN_OF_SH_CB_LGE1 = false;
			double const lambda_OF_BATT_B1 = 2;
			bool const transmit_relevance_OF_LBB_by_line2_lost = true;
			bool const failI_FROZEN_OF_RO_CB_LHA1 = false;
			bool const transmit_relevance_OF_LBB_not_fed = true;
			double const agg_lambda_OF_LBA_by_line1_lost = 3.9e-06;
			double const mu_OF_RC_CB_LGF2 = 0.2;
			bool const transmit_relevance_OF_SH_CB_RDB1 = true;
			std::string const calculate_required_OF_DGB_long = "fn_fathers_and_trig";
			std::string const calculate_required_OF_TUB1 = "fn_fathers_and_trig";
			bool const transmit_relevance_OF_RO_CB_LHA2 = true;
			bool const transmit_relevance_OF_GRID = true;
			bool const failF_FROZEN_OF_LGD = false;
			bool const initiator_OF_SH_CB_LHB2 = false;
			double const lambda_OF_SH_CB_GEV = 1e-07;
			std::string const trigger_kind_OF_t_6 = "fn_fathers_and_trig";
			bool const failI_FROZEN_OF_RC_CB_LHA3 = false;
			std::string const calculate_required_OF_loss_of_supply_by_DGB = "fn_fathers_and_trig";
			int const rep_priority_OF_SH_CB_GEV = 1;
			std::string const calculate_required_OF_RC_CB_LGD2_ = "fn_fathers_and_trig";
			std::string const when_to_check_OF_RC_CB_LGF2 = "not_req_to_req";
			bool const For_IandAB_method_OF___ARBRE__EIRM = false;
			bool const transmit_relevance_OF_BATT_A1 = true;
			std::string const calculate_required_OF_RO_CB_LHA2_ = "fn_fathers_and_trig";
			bool const force_relevant_events_OF_GEV = false;
			std::string const calculate_required_OF_CCF_GEV_LGR = "fn_fathers_and_trig";
			std::string const calculate_required_OF_demand_TAC = "fn_fathers_and_trig";
			bool const force_relevant_events_OF_RC_CB_LHA3_ = false;
			bool const initiator_OF_SH_CB_LGF1 = false;
			std::string const calculate_required_OF_SH_CB_TUA2 = "fn_fathers_and_trig";
			bool const initiator_OF_LGD = false;
			bool const force_relevant_events_OF_loss_of_supply_by_TA = false;
			bool const initiator_OF_SH_CB_LBA1 = false;
			bool const force_relevant_events_OF_RC_CB_LHB2 = false;
			bool const failF_FROZEN_OF_LBB = false;
			bool const force_relevant_events_OF_LLD = false;
			double const mu_OF_LBB_by_line2_lost = 0.1;
			bool const force_relevant_events_OF_LGF_not_fed = false;
			bool const aggregation_OF_LBB_by_line2_lost = true;
			std::string const calculate_required_OF_LBA_not_fed = "fn_fathers_and_trig";
			bool const force_relevant_events_OF_TAC = false;
			double const mu_OF_GRID = 0.1;
			std::string const calculate_required_OF_LGF = "fn_fathers_and_trig";
			std::string const calculate_required_OF_TS = "fn_fathers_and_trig";
			double const mu_OF_SH_CB_RDB1 = 0.2;
			int const rep_priority_OF_demand_CCF_DG = 1;
			double const mu_OF_RO_CB_LHA2 = 0.2;
			std::string const calculate_required_OF_RDA1 = "fn_fathers_and_trig";
			double const mu_OF_BATT_A1 = 0.1;
			double const mu_OF_RDB2 = 0.3333333;
			bool const force_relevant_events_OF_LHA_and_LHB_lost = false;
			bool const force_relevant_events_OF_DGB_short = false;
			bool const force_relevant_events_OF_SH_CB_LGE1 = false;
			bool const failI_FROZEN_OF_on_demand_house = false;
			bool const force_relevant_events_OF_SH_CB_LHB2 = false;
			std::string const when_to_check_OF_RO_CB_LHA2 = "not_req_to_req";
			bool const initiator_OF_SH_CB_LGD1 = false;
			std::string const calculate_required_OF_LGA = "fn_fathers_and_trig";
			std::string const calculate_required_OF_RC_CB_LHA3 = "fn_fathers_and_trig";
			bool const initiator_OF_SH_CB_LGD2 = false;
			bool const transmit_relevance_OF_RC_CB_LHB2_ = true;
			double const mu_OF_LGB = 0.02;
			bool const transmit_relevance_OF_SH_CB_line_LGR = true;
			std::string const calculate_required_OF_CCF_DG = "fn_fathers_and_trig";
			bool const force_relevant_events_OF_SH_CB_LGF1 = false;
			bool const force_relevant_events_OF_LGD = false;
			bool const force_relevant_events_OF_SH_CB_LBA1 = false;
			bool const transmit_relevance_OF_SH_CB_LGA = true;
			int const rep_priority_OF_SH_CB_LBA2 = 1;
			bool const force_relevant_events_OF_RO_CB_LHB1_ = false;
			std::string const calculate_required_OF_LLA = "fn_fathers_and_trig";
			int const rep_priority_OF_LBA = 1;
			bool const transmit_relevance_OF_RC_CB_LGD2 = true;
			bool const force_relevant_events_OF_loss_of_supply_by_UNIT = false;
			bool const failF_FROZEN_OF_RDA2 = false;
			bool const transmit_relevance_OF_SH_CB_line_GEV = true;
			bool const initiator_OF_UNIT = false;
			bool const failF_FROZEN_OF_SH_CB_LBB2 = false;
			bool const initiator_OF_DGA_long = false;
			double const lambda_OF_DGB_long = 0.0005;
			std::string const calculate_required_OF_TA = "fn_fathers_and_trig";
			bool const failF_FROZEN_OF_SUBSTATION = false;
			int const rep_priority_OF_TUB1 = 1;
			std::string const calculate_required_OF_loss_of_supply_by_LGD = "fn_fathers_and_trig";
			double const mu_OF_RDB1 = 0.3333333;
			std::string const when_to_check_OF_RO_CB_LHB1 = "not_req_to_req";
			double const mu_OF_in_function_house = 0.05;
			bool const failF_FROZEN_OF_BATT_B1 = false;
			int const rep_priority_OF_CCF_GEV_LGR = 1;
			int const rep_priority_OF_demand_TAC = 1;
			std::string const calculate_required_OF_SH_CB_LHA3 = "fn_fathers_and_trig";
			bool const transmit_relevance_OF_BATT_B2 = true;
			std::string const calculate_required_OF_on_demand_house = "fn_fathers_and_trig";
			bool const transmit_relevance_OF_SH_CB_LHB1 = true;
			std::string const calculate_required_OF_LHB = "fn_fathers_and_trig";
			double const mu_OF_SH_CB_line_LGR = 0.2;
			std::string const trigger_kind_OF_t_2 = "fn_fathers_and_trig";
			bool const force_relevant_events_OF_SH_CB_LGD1 = false;
			bool const force_relevant_events_OF_BATTERY_B_lost = false;
			double const lambda_OF_UNIT = 0.0001;
			bool const force_relevant_events_OF_SH_CB_LGD2 = false;
			double const mu_OF_SH_CB_LGA = 0.2;
			bool const failF_FROZEN_OF_SH_CB_LHA1 = false;
			double const gamma_OF_demand_DGB = 0.002;
			bool const transmit_relevance_OF_RC_CB_LHA2 = true;
			std::string const calculate_required_OF_loss_of_supply_by_LGF = "fn_fathers_and_trig";
			double const mu_OF_RC_CB_LGD2 = 0.2;
			bool const failF_FROZEN_OF_LGB = false;
			bool const force_relevant_events_OF_OR_14 = false;
			bool const force_relevant_events_OF_CB_LGF2_unable = false;
			double const lambda_OF_SH_CB_TUA2 = 5e-07;
			std::string const calculate_required_OF_UE_1 = "fn_fathers_and_trig";
			double const mu_OF_SH_CB_line_GEV = 0.2;
			double const gamma_OF_RO_CB_LHB1 = 0.0002;
			double const mu_OF_LKE = 0.02;
			bool const force_relevant_events_OF_DGB_lost = false;
			double const mu_OF_TUB2 = 0.1;
			bool const transmit_relevance_OF_demand_DGA = true;
			std::string const calculate_required_OF_LHA_lost = "fn_fathers_and_trig";
			bool const force_relevant_events_OF_CB_LGD2_unable = false;
			int const rep_priority_OF_LGF = 1;
			bool const initiator_OF_SH_CB_LHA1 = false;
			double const mu_OF_LHA = 0.02;
			bool const transmit_relevance_OF_LGD_not_fed = true;
			std::string const when_to_check_OF_RC_CB_LGD2 = "not_req_to_req";
			bool const force_relevant_events_OF_CB_LHA12_unable = false;
			bool const force_relevant_events_OF_DGA_long = false;
			bool const aggregation_OF_LBA_by_line1_lost = true;
			bool const initiator_OF_LGB = false;
			int const rep_priority_OF_RDA1 = 1;
			int const rep_priority_OF_LKI = 1;
			bool const force_relevant_events_OF_RC_CB_LGF2_ = false;
			std::string const step_down_OF_CB_LHB12_unable = "rep_any";
			bool const failF_FROZEN_OF_GRID = false;
			double const mu_OF_BATT_B2 = 0.1;
			bool const transmit_relevance_OF_SH_CB_LHA2 = true;
			std::string const calculate_required_OF_loss_of_houseload_operation = "fn_fathers_and_trig";
			bool const force_relevant_events_OF_SH_CB_LBB2 = false;
			bool const transmit_relevance_OF_SH_CB_LBB1 = true;
			int const rep_priority_OF_LGA = 1;
			bool const force_relevant_events_OF_SUBSTATION = false;
			bool const force_relevant_events_OF_loss_of_supply_by_DGA = false;
			double const lambda_OF_LGR = 2e-05;
			double const mu_OF_SH_CB_LHB1 = 0.2;
			bool const transmit_relevance_OF_demand_CCF_DG = true;
			double const lambda_OF_LKI = 5e-07;
			int const rep_priority_OF_CCF_DG = 1;
			bool const failF_FROZEN_OF_TUA1 = false;
			double const gamma_OF_RC_CB_LGD2 = 0.0002;
			int const rep_priority_OF_LLA = 1;
			bool const force_relevant_events_OF_LBB_lost = false;
			double const mu_OF_CCF_DG = 0.0025;
			double const mu_OF_RC_CB_LHA2 = 0.2;
			int const rep_priority_OF_demand_DGB = 1;
			double const mu_OF_CCF_GEV_LGR = 0.005;
			std::string const calculate_required_OF_SH_CB_LGF2 = "fn_fathers_and_trig";
			bool const force_relevant_events_OF_loss_of_supply_by_TS = false;
			bool const failF_FROZEN_OF_SH_CB_TUB1 = false;
			bool const initiator_OF_TUA1 = false;
			int const rep_priority_OF_TA = 1;
			bool const force_relevant_events_OF_LHB_not_fed = false;
			double const mu_OF_LGE = 0.02;
			bool const failF_FROZEN_OF_LKE = false;
			double const lambda_OF_LLA = 5e-07;
			bool const initiator_OF_SH_CB_TUB1 = false;
			std::string const when_to_check_OF_demand_DGA = "not_req_to_req";
			double const lambda_OF_RDA2 = 1e-06;
			bool const force_relevant_events_OF_SH_CB_LHA1 = false;
			bool const failF_FROZEN_OF_LHA = false;
			double const lambda_OF_SH_CB_LBB2 = 1e-06;
			bool const transmit_relevance_OF_SH_CB_LBA2 = true;
			bool const force_relevant_events_OF_LGB = false;
			std::string const calculate_required_OF_RDB2 = "fn_fathers_and_trig";
			std::string const step_down_OF_CB_LHA3_unable = "rep_any";
			double const mu_OF_SH_CB_LHA2 = 0.2;
			double const lambda_OF_SUBSTATION = 1e-06;
			bool const initiator_OF_LKE = false;
			bool const initiator_OF_RDB1 = false;
			double const mu_OF_SH_CB_LBB1 = 0.2;
			double const lambda_OF_TUA2 = 2e-07;
			std::string const trigger_kind_OF_t_4 = "fn_fathers_and_trig";
			int const rep_priority_OF_on_demand_house = 1;
			bool const initiator_OF_LHA = false;
			bool const transmit_relevance_OF_DGB_long = true;
			int const rep_priority_OF_LHB = 1;
			std::string const calculate_required_OF_TP = "fn_fathers_and_trig";
			bool const Profil1_OF___ARBRE__EIRM = true;
			bool const failI_FROZEN_OF_RC_CB_LHA2 = false;
			bool const force_relevant_events_OF_SH_GEV_or_LGR = false;
			bool const transmit_relevance_OF_RC_CB_LHA3_ = true;
			bool const transmit_relevance_OF_loss_of_supply_by_DGB = true;
			bool const failF_FROZEN_OF_SH_CB_RDA1 = false;
			bool const transmit_relevance_OF_RC_CB_LGD2_ = true;
			bool const force_relevant_events_OF_loss_of_supply_by_LGR = false;
			double const lambda_OF_SH_CB_LHA3 = 5e-07;
			std::string const when_to_check_OF_demand_CCF_DG = "not_req_to_req";
			bool const force_relevant_events_OF_TS_lost = false;
			bool const initiator_OF_SH_CB_RDB2 = false;
			bool const transmit_relevance_OF_demand_TAC = true;
			std::string const calculate_required_OF_SH_CB_RDA2 = "fn_fathers_and_trig";
			bool const transmit_relevance_OF_SH_CB_TUA2 = true;
			bool const initiator_OF_BATT_A2 = false;
			int const rep_priority_OF_LBB_by_line2_lost = 1;
			bool const failF_FROZEN_OF_SH_CB_line_GEV = false;
			bool const failF_FROZEN_OF_DGA_short = false;
			bool const initiator_OF_SH_CB_RDA1 = false;
			int const rep_priority_OF_SH_CB_RDB1 = 1;
			bool const force_relevant_events_OF_TUA1 = false;
			bool const force_relevant_events_OF_LBA_by_others_lost = false;
			std::string const calculate_required_OF_TUB2 = "fn_fathers_and_trig";
			std::string const calculate_required_OF_SH_CB_LGB = "fn_fathers_and_trig";
			int const rep_priority_OF_RO_CB_LHA2 = 1;
			double const mu_OF_demand_DGB = 0.005;
			double const mu_OF_SH_CB_LBA2 = 0.2;
			bool const failF_FROZEN_OF_LGE = false;
			bool const force_relevant_events_OF_SH_CB_TUB1 = false;
			double const mu_OF_TA = 0.005;
			int const rep_priority_OF_BATT_A1 = 1;
			double const lambda_OF_SH_CB_RDB1 = 1e-06;
			bool const transmit_relevance_OF_LGF = true;
			bool const transmit_relevance_OF_TS = true;
			std::string const calculate_required_OF_TS_not_fed = "fn_fathers_and_trig";
			bool const force_relevant_events_OF_LKE = false;
			bool const force_relevant_events_OF_RDB1 = false;
			std::string const calculate_required_OF_BATT_B2 = "fn_fathers_and_trig";
			bool const initiator_OF_LGR = false;
			double const lambda_OF_TS = 5e-06;
			bool const transmit_relevance_OF_RDA1 = true;
			std::string const calculate_required_OF_SH_CB_LHB1 = "fn_fathers_and_trig";
			bool const initiator_OF_SH_CB_TUA1 = false;
			bool const force_relevant_events_OF_LHA = false;
			bool const transmit_relevance_OF_RO_CB_LHA1 = true;
			std::string const calculate_required_OF_RC_CB_LHA2 = "fn_fathers_and_trig";
			bool const force_relevant_events_OF_LHA_not_fed = false;
			bool const transmit_relevance_OF_SH_CB_LGF1 = true;
			bool const force_relevant_events_OF_loss_of_supply_by_TS1 = false;
			bool const failF_FROZEN_OF_SH_CB_GEV = false;
			bool const transmit_relevance_OF_RC_CB_LHA3 = true;
			bool const transmit_relevance_OF_SH_CB_LBA1 = true;
			double const mu_OF_SH_CB_TUA2 = 0.2;
			bool const force_relevant_events_OF_SH_CB_RDB2 = false;
			bool const transmit_relevance_OF_CCF_DG = true;
			bool const force_relevant_events_OF_BATT_A2 = false;
			double const lambda_OF_SH_CB_LGF2 = 5e-07;
			std::string const when_to_check_OF_demand_TAC = "not_req_to_req";
			double const mu_OF_LBA = 0.02;
			bool const force_relevant_events_OF_SH_CB_RDA1 = false;
			double const lambda_OF_LGD = 2e-07;
			std::string const calculate_required_OF_SH_CB_LHA2 = "fn_fathers_and_trig";
			bool const initiator_OF_SH_CB_TUB2 = false;
			double const agg_lambda_OF_LBB_by_line2_lost = 4.2e-06;
			std::string const calculate_required_OF_LGR = "fn_fathers_and_trig";
			std::string const trigger_kind_OF_t_5_1 = "fn_fathers_and_trig";
			double const lambda_OF_SH_CB_line_LGR = 1e-07;
			bool const trimming_OF_OPTIONS = true;
			double const mu_OF_RO_CB_LHA1 = 0.2;
			int const rep_priority_OF_SH_CB_line_LGR = 1;
			int const rep_priority_OF_SH_CB_LGA = 1;
			bool const initiator_OF_LBA_by_line2_lost = false;
			bool const failI_FROZEN_OF_RC_CB_LHB2 = false;
			bool const force_relevant_events_OF_SH_CB_TUA1 = false;
			bool const transmit_relevance_OF_SH_CB_LGD1 = true;
			double const mu_OF_SH_CB_LGF1 = 0.2;
			double const gamma_OF_demand_DGA = 0.002;
			bool const transmit_relevance_OF_BATTERY_B_lost = true;
			int const rep_priority_OF_SH_CB_RDA2 = 1;
			double const mu_OF_RC_CB_LHA3 = 0.2;
			double const mu_OF_SH_CB_LBA1 = 0.2;
			bool const transmit_relevance_OF_SH_CB_LHA3 = true;
			bool const force_relevant_events_OF_SH_CB_TUA2 = false;
			double const mu_OF_LBB = 0.02;
			std::string const calculate_required_OF_TUA2 = "fn_fathers_and_trig";
			bool const transmit_relevance_OF_DGB_lost = true;
			bool const initiator_OF_LLD = false;
			int const rep_priority_OF_SH_CB_LGB = 1;
			int const rep_priority_OF_TUB2 = 1;
			bool const transmit_relevance_OF_loss_of_supply_by_LGF = true;
			bool const transmit_relevance_OF_UE_1 = true;
			bool const transmit_relevance_OF_UNIT = true;
			bool const transmit_relevance_OF_DGA_long = true;
			bool const failF_FROZEN_OF_SH_CB_LBA2 = false;
			bool const transmit_relevance_OF_LHA_lost = true;
			bool const failF_FROZEN_OF_LBA = false;
			std::string const when_to_check_OF_demand_DGB = "not_req_to_req";
			double const lambda_OF_SH_CB_LGB = 5e-07;
			int const rep_priority_OF_BATT_B2 = 1;
			double const mu_OF_RDA2 = 0.3333333;
			double const lambda_OF_DGB_short = 0.002;
			double const mu_OF_LKI = 0.02;
			int const rep_priority_OF_SH_CB_LHB1 = 1;
			bool const force_relevant_events_OF_SH_CB_TUB2 = false;
			bool const initiator_OF_in_function_house = false;
			bool const initiator_OF_DGB_short = false;
			bool const initiator_OF_SH_CB_LGE1 = false;
			bool const initiator_OF_LBA = false;
			bool const failF_FROZEN_OF_TUB1 = false;
			std::string const calculate_required_OF_GEV = "fn_fathers_and_trig";
			std::string const calculate_required_OF_loss_of_supply_by_TA = "fn_fathers_and_trig";
			std::string const calculate_required_OF_RC_CB_LHA3_ = "fn_fathers_and_trig";
			int const rep_priority_OF_RC_CB_LHA2 = 1;
			bool const transmit_relevance_OF_loss_of_houseload_operation = true;
			double const mu_OF_LGA = 0.02;
			double const mu_OF_SH_CB_LHA3 = 0.2;
			bool const force_relevant_events_OF_LBA_by_line2_lost = false;
			int const rep_priority_OF_demand_DGA = 1;
			std::string const calculate_required_OF_RC_CB_LHB2 = "fn_fathers_and_trig";
			double const lambda_OF_SH_CB_LHB1 = 5e-07;
			bool const failF_FROZEN_OF_CCF_GEV_LGR = false;
			std::string const calculate_required_OF_LLD = "fn_fathers_and_trig";
			double const mu_OF_UNIT = 0.1;
			bool const transmit_relevance_OF_loss_of_supply_by_TS = true;
			std::string const calculate_required_OF_LGF_not_fed = "fn_fathers_and_trig";
			bool const transmit_relevance_OF_LHB_not_fed = true;
			std::string const when_to_check_OF_on_demand_house = "not_req_to_req";
			bool const transmit_relevance_OF_SH_CB_LGF2 = true;
			std::string const calculate_required_OF_TAC = "fn_fathers_and_trig";
			double const agg_mu_OF_LBA_by_line2_lost = 0.09882353;
			int const rep_priority_OF_SH_CB_LHA2 = 1;
			int const rep_priority_OF_SH_CB_LBB1 = 1;
			bool const force_relevant_events_OF_UE_1 = true;
			double const lambda_OF_BATT_A1 = 2;
			double const gamma_OF_RC_CB_LGF2 = 0.0002;
			bool const initiator_OF_LBB = false;
			bool const initiator_OF_LBB_by_line1_lost = false;
			bool const failF_FROZEN_OF_LGF = false;
			std::string const calculate_required_OF_LHA_and_LHB_lost = "fn_fathers_and_trig";
			std::string const calculate_required_OF_DGB_short = "fn_fathers_and_trig";
			std::string const calculate_required_OF_SH_CB_LHB2 = "fn_fathers_and_trig";
			bool const failF_FROZEN_OF_RDA1 = false;
			bool const transmit_relevance_OF_RDB2 = true;
			std::string const calculate_required_OF_RO_CB_LHA1 = "fn_fathers_and_trig";
			double const lambda_OF_SH_CB_LHA2 = 5e-07;
			bool const force_relevant_events_OF_in_function_house = false;
			double const mu_OF_demand_CCF_DG = 0.0025;
			bool const failF_FROZEN_OF_LKI = false;
			double const lambda_OF_SH_CB_LBB1 = 1e-06;
			bool const force_relevant_events_OF_LBA = false;
			double const gamma_OF_demand_TAC = 0.002;
			double const lambda_OF_LGB = 2e-07;
			std::string const calculate_required_OF_SH_CB_LGF1 = "fn_fathers_and_trig";
			std::string const calculate_required_OF_LGD = "fn_fathers_and_trig";
			std::string const calculate_required_OF_SH_CB_LBA1 = "fn_fathers_and_trig";
			bool const failF_FROZEN_OF_LGA = false;
			bool const transmit_relevance_OF_TP = true;
			bool const transmit_relevance_OF_loss_of_supply_by_LGR = true;
			std::string const calculate_required_OF_RO_CB_LHB1_ = "fn_fathers_and_trig";
			bool const force_relevant_events_OF_RO_CB_LHA1_ = false;
			double const lambda_OF_TP = 5e-06;
			std::string const calculate_required_OF_loss_of_supply_by_UNIT = "fn_fathers_and_trig";
			bool const failF_FROZEN_OF_CCF_DG = false;
			double const mu_OF_SH_CB_LGF2 = 0.2;
			bool const failF_FROZEN_OF_LLA = false;
			double const mu_OF_demand_DGA = 0.005;
			bool const transmit_relevance_OF_SH_CB_RDA2 = true;
			bool const failF_FROZEN_OF_TA = false;
			double const gamma_OF_RO_CB_LHA2 = 0.0002;
			int const rep_priority_OF_DGB_long = 1;
			bool const force_relevant_events_OF_TA_lost = false;
			bool const force_relevant_events_OF_loss_of_supply_by_TA1 = false;
			bool const transmit_relevance_OF_TUB2 = true;
			double const lambda_OF_SH_CB_LBA2 = 1e-06;
			bool const transmit_relevance_OF_SH_CB_LGB = true;
			int const rep_priority_OF_GEV = 1;
			bool const force_relevant_events_OF_LBB = false;
			bool const initiator_OF_RDA2 = false;
			bool const force_relevant_events_OF_LBB_by_line1_lost = false;
			bool const initiator_OF_SH_CB_LBB2 = false;
			bool const transmit_relevance_OF_RDB1 = true;
			double const lambda_OF_TUA1 = 2e-07;
			bool const initiator_OF_SUBSTATION = false;
			std::string const calculate_required_OF_SH_CB_LGD1 = "fn_fathers_and_trig";
			int const rep_priority_OF_RC_CB_LHB2 = 1;
			bool const transmit_relevance_OF_TS_not_fed = true;
			std::string const calculate_required_OF_BATTERY_B_lost = "fn_fathers_and_trig";
			std::string const calculate_required_OF_SH_CB_LGD2 = "fn_fathers_and_trig";
			int const rep_priority_OF_SH_CB_TUA2 = 1;
			bool const force_relevant_events_OF_LHB_lost = false;
			bool const force_relevant_events_OF_DGA_lost = false;
			std::string const calculate_required_OF_OR_14 = "fn_fathers_and_trig";
			bool const transmit_relevance_OF_loss_of_supply_by_TS1 = true;
			std::string const calculate_required_OF_CB_LGF2_unable = "fn_fathers_and_trig";
			bool const failF_FROZEN_OF_LHB = false;
			std::string const calculate_required_OF_DGB_lost = "fn_fathers_and_trig";
			bool const initiator_OF_BATT_B1 = false;
			bool const force_relevant_events_OF_LKI = false;
			std::string const calculate_required_OF_CB_LGD2_unable = "fn_fathers_and_trig";
			bool const transmit_relevance_OF_SH_CB_RDB2 = true;
			double const lambda_OF_CCF_GEV_LGR = 1e-06;
			double const lambda_OF_LHA = 2e-07;
			bool const transmit_relevance_OF_BATT_A2 = true;
			std::string const calculate_required_OF_CB_LHA12_unable = "fn_fathers_and_trig";
			std::string const calculate_required_OF_DGA_long = "fn_fathers_and_trig";
			double const mu_OF_SH_CB_RDA2 = 0.2;
			int const rep_priority_OF_TS = 1;
			double const mu_OF_SH_CB_LGB = 0.2;
			double const lambda_OF_BATT_B2 = 2;
			bool const force_relevant_events_OF_CB_LHB12_unable = false;
			int const rep_priority_OF_SH_CB_LHB2 = 1;
			std::string const calculate_required_OF_SH_CB_LBB2 = "fn_fathers_and_trig";
			bool const failF_FROZEN_OF_SH_CB_RDB1 = false;
			int const rep_priority_OF_RO_CB_LHA1 = 1;
			std::string const calculate_required_OF_SUBSTATION = "fn_fathers_and_trig";
			bool const force_relevant_events_OF_demand_DGB = false;
			std::string const calculate_required_OF_loss_of_supply_by_DGA = "fn_fathers_and_trig";
			bool const force_relevant_events_OF_RDA2 = false;
			bool const initiator_OF_LBB_by_line2_lost = false;
			bool const force_relevant_events_OF_LBA_lost = false;
			int const rep_priority_OF_SH_CB_LGF1 = 1;
			double const lambda_OF_TAC = 0.001;
			std::string const trigger_kind_OF_t_2_1 = "fn_fathers_and_trig";
			int const rep_priority_OF_RC_CB_LHA3 = 1;
			bool const transmit_relevance_OF_LGR = true;
			double const mu_OF_DGB_long = 0.005;
			bool const failF_FROZEN_OF_BATT_A1 = false;
			double const lambda_OF_RDA1 = 1e-06;
			int const rep_priority_OF_SH_CB_LBA1 = 1;
			double const lambda_OF_SH_CB_LHB2 = 5e-07;
			std::string const trigger_kind_OF_t_1 = "fn_fathers_and_trig";
			double const agg_mu_OF_LBB_by_line1_lost = 0.08533333;
			bool const initiator_OF_GRID = false;
			bool const force_relevant_events_OF_loss_of_supply_by_LGD = false;
			std::string const calculate_required_OF_loss_of_supply_by_TS = "fn_fathers_and_trig";
			bool const force_relevant_events_OF_RC_CB_LGF2 = false;
			double const mu_OF_BATT_A2 = 0.1;
			double const mu_OF_SH_CB_RDB2 = 0.2;
			std::string const calculate_required_OF_LHB_not_fed = "fn_fathers_and_trig";
			double const lambda_OF_SH_CB_LGF1 = 5e-07;
			bool const force_relevant_events_OF_loss_of_supply_by_GEV = false;
			std::string const repair_time_approx_OF_OPTIONS = "weighed_average";
			double const mu_OF_demand_TAC = 0.005;
			bool const force_relevant_events_OF_BATT_B1 = false;
			double const lambda_OF_LGE = 2e-07;
			std::string const calculate_required_OF_SH_CB_LHA1 = "fn_fathers_and_trig";
			std::string const when_to_check_OF_RC_CB_LHA2 = "not_req_to_req";
			bool const transmit_relevance_OF_TUA2 = true;
			std::string const calculate_required_OF_LGB = "fn_fathers_and_trig";
			bool const force_relevant_events_OF_CB_LHA3_unable = false;
			int const rep_priority_OF_SH_CB_LGD1 = 1;
			double const mu_OF_TS = 0.005;
			int const rep_priority_OF_SH_CB_LGD2 = 1;
			double const mu_OF_LGR = 0.2;
			int const rep_priority_OF_SH_CB_LHA3 = 1;
			std::string const calculate_required_OF_SH_GEV_or_LGR = "fn_fathers_and_trig";
			bool const force_relevant_events_OF_LBB_by_line2_lost = false;
			std::string const calculate_required_OF_loss_of_supply_by_LGR = "fn_fathers_and_trig";
			bool const force_relevant_events_OF_LBB_not_fed = false;
			std::string const calculate_required_OF_TS_lost = "fn_fathers_and_trig";
			bool const force_relevant_events_OF_SH_CB_RDB1 = false;
			bool const transmit_relevance_OF_LBA_by_line2_lost = true;
			bool const transmit_relevance_OF_GEV = true;
			double const gamma_OF_RC_CB_LHA2 = 0.0002;
			bool const transmit_relevance_OF_loss_of_supply_by_TA = true;
			bool const force_relevant_events_OF_RO_CB_LHA2 = false;
			bool const failF_FROZEN_OF_SH_CB_line_LGR = false;
			double const lambda_OF_DGA_long = 0.0005;
			double const lambda_OF_SH_CB_LGD1 = 5e-07;
			bool const force_relevant_events_OF_GRID = false;
			int const rep_priority_OF_UNIT = 1;
			double const lambda_OF_SH_CB_LGD2 = 5e-07;
			double const mu_OF_TUA2 = 0.1;
			int const rep_priority_OF_DGA_long = 1;
			bool const failF_FROZEN_OF_SH_CB_LGA = false;
			std::string const calculate_required_OF_TUA1 = "fn_fathers_and_trig";
			std::string const calculate_required_OF_LBA_by_others_lost = "fn_fathers_and_trig";
			bool const force_relevant_events_OF_BATTERY_A_lost = false;
			bool const force_relevant_events_OF_BATT_A1 = false;
			bool const transmit_relevance_OF_RC_CB_LHB2 = true;
			bool const failF_FROZEN_OF_SH_CB_RDA2 = false;
			bool const transmit_relevance_OF_LLD = true;
			bool const initiator_OF_SH_CB_line_LGR = false;
			bool const transmit_relevance_OF_LGF_not_fed = true;
			std::string const calculate_required_OF_SH_CB_TUB1 = "fn_fathers_and_trig";
			bool const initiator_OF_SH_CB_LGA = false;
			double const mu_OF_on_demand_house = 0.05;
			bool const failF_FROZEN_OF_TUB2 = false;
			bool const failF_FROZEN_OF_SH_CB_LGB = false;
			bool const transmit_relevance_OF_TAC = true;
			double const lambda_OF_DGA_short = 0.002;
			bool const initiator_OF_SH_CB_line_GEV = false;
			std::string const calculate_required_OF_LKE = "fn_fathers_and_trig";
			bool const initiator_OF_DGA_short = false;
			std::string const calculate_required_OF_RDB1 = "fn_fathers_and_trig";
			double const mu_OF_LBA_by_line2_lost = 0.1;
			std::string const calculate_required_OF_LHA = "fn_fathers_and_trig";
			double const gamma_OF_demand_CCF_DG = 0.0002;
			double const gamma_OF_on_demand_house = 0.2;
			bool const initiator_OF_LBA_by_line1_lost = false;
			bool const transmit_relevance_OF_LHA_and_LHB_lost = true;
			bool const transmit_relevance_OF_DGB_short = true;
			bool const transmit_relevance_OF_SH_CB_LGE1 = true;
			bool const initiator_OF_LGE = false;
			bool const force_relevant_events_OF_RO_CB_LHB1 = false;
			bool const failF_FROZEN_OF_BATT_B2 = false;
			double const mu_OF_GEV = 0.2;
			std::string const calculate_required_OF_LHA_not_fed = "fn_fathers_and_trig";
			bool const aggregation_OF_LBA_by_line2_lost = true;
			bool const transmit_relevance_OF_SH_CB_LHB2 = true;
			bool const failF_FROZEN_OF_SH_CB_LHB1 = false;
			std::string const calculate_required_OF_loss_of_supply_by_TS1 = "fn_fathers_and_trig";
			std::string const trigger_kind_OF_t_5 = "fn_fathers_and_trig";
			int const rep_priority_OF_SH_CB_LGF2 = 1;
			bool const transmit_relevance_OF_RO_CB_LHA1_ = true;
			std::string const calculate_required_OF_SH_CB_RDB2 = "fn_fathers_and_trig";
			double const mu_OF_RC_CB_LHB2 = 0.2;
			bool const transmit_relevance_OF_LGD = true;
			std::string const calculate_required_OF_BATT_A2 = "fn_fathers_and_trig";
			bool const transmit_relevance_OF_RO_CB_LHB1_ = true;
			std::string const calculate_required_OF_SH_CB_RDA1 = "fn_fathers_and_trig";
			bool const transmit_relevance_OF_loss_of_supply_by_UNIT = true;
			bool const force_relevant_events_OF_RC_CB_LHB2_ = false;
			std::string const when_to_check_OF_RC_CB_LHB2 = "not_req_to_req";
			bool const force_relevant_events_OF_SH_CB_line_LGR = false;
			bool const initiator_OF_SH_CB_GEV = false;
			double const mu_OF_DGA_long = 0.005;
			int const rep_priority_OF_RDB2 = 1;
			bool const force_relevant_events_OF_SH_CB_LGA = false;
			double const lambda_OF_SH_CB_LHA1 = 5e-07;
			bool const transmit_relevance_OF_TA_lost = true;
			double const lambda_OF_TUB1 = 2e-07;
			bool const transmit_relevance_OF_loss_of_supply_by_TA1 = true;
			double const mu_OF_DGB_short = 0.1;
			bool const force_relevant_events_OF_RC_CB_LGD2 = false;
			bool const transmit_relevance_OF_LBB = true;
			bool const force_relevant_events_OF_AND_3 = false;
			bool const failF_FROZEN_OF_SH_CB_LHA2 = false;
			bool const force_relevant_events_OF_SH_CB_line_GEV = false;
			std::string const trigger_kind_OF_t_7 = "fn_fathers_and_trig";
			bool const force_relevant_events_OF_DGA_short = false;
			bool const failF_FROZEN_OF_SH_CB_LBB1 = false;
			double const mu_OF_SH_CB_LGE1 = 0.2;
			int const rep_priority_OF_TP = 1;
			std::string const calculate_required_OF_SH_CB_TUA1 = "fn_fathers_and_trig";
			double const lambda_OF_RDB2 = 1e-06;
			double const mu_OF_SH_CB_LHB2 = 0.2;
			bool const force_relevant_events_OF_LBA_by_line1_lost = false;
			bool const force_relevant_events_OF_LGE = false;
			double const gamma_OF_RC_CB_LHB2 = 0.0002;
			bool const force_relevant_events_OF_RC_CB_LHA2_ = false;
			double const agg_lambda_OF_LBB_by_line1_lost = 3.2e-06;
			bool const transmit_relevance_OF_SH_CB_LGD2 = true;
			std::string const when_to_check_OF_RO_CB_LHA1 = "not_req_to_req";
			bool const transmit_relevance_OF_OR_14 = true;
			bool const transmit_relevance_OF_CB_LGF2_unable = true;
			std::string const when_to_check_OF_RC_CB_LHA3 = "not_req_to_req";
			bool const transmit_relevance_OF_CB_LGD2_unable = true;
			double const mu_OF_LBB_by_line1_lost = 0.1;
			double const lambda_OF_SH_CB_RDA2 = 1e-06;
			int const rep_priority_OF_RDB1 = 1;
			bool const transmit_relevance_OF_CB_LHA12_unable = true;
			bool const force_relevant_events_OF_SH_CB_GEV = false;
			double const mu_OF_LGF = 0.02;
			std::string const calculate_required_OF_SH_CB_TUB2 = "fn_fathers_and_trig";
			double const lambda_OF_SH_CB_TUB1 = 5e-07;
			bool const transmit_relevance_OF_RC_CB_LGF2_ = true;
			bool const force_relevant_events_OF_demand_DGA = false;
			bool const transmit_relevance_OF_RDA2 = true;
			double const gamma_OF_RO_CB_LHA1 = 0.0002;
			bool const force_relevant_events_OF_LGD_not_fed = false;
			bool const transmit_relevance_OF_SH_CB_LBB2 = true;
			bool const initiator_OF_SH_CB_LBA2 = false;
			bool const failF_FROZEN_OF_DGB_long = false;
			double const lambda_OF_LGA = 2e-07;
			double const lambda_OF_LKE = 5e-07;
			std::string const calculate_required_OF_LBA_by_line2_lost = "fn_fathers_and_trig";
			bool const failF_FROZEN_OF_GEV = false;
			bool const transmit_relevance_OF_SUBSTATION = true;
			bool const transmit_relevance_OF_loss_of_supply_by_DGA = true;
			int const rep_priority_OF_SH_CB_RDB2 = 1;
			double const gamma_OF_RC_CB_LHA3 = 0.0002;
			double const mu_OF_SH_CB_LGD1 = 0.2;
			int const rep_priority_OF_BATT_A2 = 1;
			double const mu_OF_SH_CB_LGD2 = 0.2;
			double const mu_OF_TP = 0.005;
			bool const initiator_OF_DGB_long = false;
			bool const force_relevant_events_OF_SH_CB_LBB1 = false;
			bool const transmit_relevance_OF_LBB_lost = true;
			bool const initiator_OF_TUB1 = false;
			bool const failF_FROZEN_OF_SH_CB_TUA2 = false;
			double const lambda_OF_in_function_house = 0.1;
			bool const force_relevant_events_OF_demand_CCF_DG = false;
			double const mu_OF_LLA = 0.02;
			bool const force_relevant_events_OF_loss_of_supply_by_DGA_and_TAC = false;
			bool const initiator_OF_CCF_GEV_LGR = false;
			bool const failF_FROZEN_OF_TAC = false;
			bool const initiator_OF_SH_CB_TUA2 = false;
			double const agg_mu_OF_LBA_by_line1_lost = 0.078;
			bool const transmit_relevance_OF_CB_LHA3_unable = true;
			bool const force_relevant_events_OF_loss_of_supply_by_TAC = false;
			bool const transmit_relevance_OF_SH_CB_LHA1 = true;
			int const rep_priority_OF_LGR = 1;
			bool const transmit_relevance_OF_LGB = true;
			int const rep_priority_OF_SH_CB_TUA1 = 1;
			std::string const calculate_required_OF_in_function_house = "fn_fathers_and_trig";
			bool const failF_FROZEN_OF_TS = false;
			double const mu_OF_SH_CB_LBB2 = 0.2;
			double const lambda_OF_GRID = 1e-05;
			std::string const calculate_required_OF_LBA = "fn_fathers_and_trig";
			std::string const calculate_required_OF_LBB_lost = "always_true";
			std::string const calculate_required_OF_SH_CB_LGE1 = "fn_fathers_and_trig";
			bool const failF_FROZEN_OF_SH_CB_LHB2 = false;
			double const lambda_OF_LHB = 2e-07;
			bool const Battery_exhausted_OF___ARBRE__EIRM = false;
			bool const force_relevant_events_OF_SH_CB_LBA2 = false;
			bool const initiator_OF_TS = false;
			bool const initiator_OF_LGF = false;
			bool const transmit_relevance_OF_SH_GEV_or_LGR = true;
			double const mu_OF_BATT_B1 = 0.1;
			std::string const calculate_required_OF_RO_CB_LHA1_ = "fn_fathers_and_trig";
			double const lambda_OF_SH_CB_TUA1 = 5e-07;
			double const mu_OF_LHB = 0.02;
			bool const failF_FROZEN_OF_SH_CB_LGF1 = false;
			std::string const calculate_required_OF_UNIT = "fn_fathers_and_trig";
			bool const initiator_OF_RDA1 = false;
			int const rep_priority_OF_TUA2 = 1;
			bool const failF_FROZEN_OF_SH_CB_LBA1 = false;
			bool const transmit_relevance_OF_TS_lost = true;
			bool const initiator_OF_LKI = false;
			bool const force_relevant_events_OF_DGB_long = false;
			bool const force_relevant_events_OF_TUB1 = false;
			bool const transmit_relevance_OF_BATTERY_A_lost = true;
			bool const force_relevant_events_OF_loss_of_supply_by_DGB = false;
			bool const initiator_OF_LGA = false;
			bool const force_relevant_events_OF_RC_CB_LGD2_ = false;
			int const rep_priority_OF_SH_CB_TUB2 = 1;
			bool const transmit_relevance_OF_LBA_by_others_lost = true;
			double const mu_OF_SH_CB_LHA1 = 0.2;
			bool const transmit_relevance_OF_TUA1 = true;
			bool const force_relevant_events_OF_RO_CB_LHA2_ = false;
			bool const failI_FROZEN_OF_demand_DGB = false;
			std::string const calculate_required_OF_loss_of_supply_by_TA1 = "fn_fathers_and_trig";
			bool const Without_long_fail_of_offsite_power_OF___ARBRE__EIRM = false;
			bool const force_relevant_events_OF_CCF_GEV_LGR = false;
			std::string const calculate_required_OF_LBB = "fn_fathers_and_trig";
			bool const force_relevant_events_OF_demand_TAC = false;
			bool const initiator_OF_CCF_DG = false;
			std::string const calculate_required_OF_LBB_by_line1_lost = "fn_fathers_and_trig";
			bool const transmit_relevance_OF_SH_CB_TUB1 = true;
		
/* ---------- DECLARATION OF OCCURRENCE RULES FIRING FLAGS ------------ */
            bool FIRE_xx10_OF_BATT_A1;
            bool FIRE_xx11_OF_BATT_A1;
            bool FIRE_xx10_OF_BATT_A2;
            bool FIRE_xx11_OF_BATT_A2;
            bool FIRE_xx10_OF_BATT_B1;
            bool FIRE_xx11_OF_BATT_B1;
            bool FIRE_xx10_OF_BATT_B2;
            bool FIRE_xx11_OF_BATT_B2;
            bool FIRE_xx10_OF_CCF_DG;
            bool FIRE_xx11_OF_CCF_DG;
            bool FIRE_xx10_OF_CCF_GEV_LGR;
            bool FIRE_xx11_OF_CCF_GEV_LGR;
            bool FIRE_xx10_OF_DGA_long;
            bool FIRE_xx11_OF_DGA_long;
            bool FIRE_xx10_OF_DGA_short;
            bool FIRE_xx11_OF_DGA_short;
            bool FIRE_xx10_OF_DGB_long;
            bool FIRE_xx11_OF_DGB_long;
            bool FIRE_xx10_OF_DGB_short;
            bool FIRE_xx11_OF_DGB_short;
            bool FIRE_xx10_OF_GEV;
            bool FIRE_xx11_OF_GEV;
            bool FIRE_xx10_OF_GRID;
            bool FIRE_xx11_OF_GRID;
            bool FIRE_xx10_OF_LBA;
            bool FIRE_xx11_OF_LBA;
            bool FIRE_xx32_OF_LBA_by_line1_lost;
            bool FIRE_xx33_OF_LBA_by_line1_lost;
            bool FIRE_xx32_OF_LBA_by_line2_lost;
            bool FIRE_xx33_OF_LBA_by_line2_lost;
            bool FIRE_xx10_OF_LBB;
            bool FIRE_xx11_OF_LBB;
            bool FIRE_xx32_OF_LBB_by_line1_lost;
            bool FIRE_xx33_OF_LBB_by_line1_lost;
            bool FIRE_xx32_OF_LBB_by_line2_lost;
            bool FIRE_xx33_OF_LBB_by_line2_lost;
            bool FIRE_xx10_OF_LGA;
            bool FIRE_xx11_OF_LGA;
            bool FIRE_xx10_OF_LGB;
            bool FIRE_xx11_OF_LGB;
            bool FIRE_xx10_OF_LGD;
            bool FIRE_xx11_OF_LGD;
            bool FIRE_xx11_OF_LGE;
            bool FIRE_xx10_OF_LGF;
            bool FIRE_xx11_OF_LGF;
            bool FIRE_xx10_OF_LGR;
            bool FIRE_xx11_OF_LGR;
            bool FIRE_xx10_OF_LHA;
            bool FIRE_xx11_OF_LHA;
            bool FIRE_xx10_OF_LHB;
            bool FIRE_xx11_OF_LHB;
            bool FIRE_xx11_OF_LKE;
            bool FIRE_xx11_OF_LKI;
            bool FIRE_xx11_OF_LLA;
            bool FIRE_xx11_OF_LLD;
            bool FIRE_xx23_OF_RC_CB_LGD2_INS_55;
            bool FIRE_xx23_OF_RC_CB_LGD2_INS_56;
            bool FIRE_xx24_OF_RC_CB_LGD2;
            bool FIRE_xx23_OF_RC_CB_LGF2_INS_58;
            bool FIRE_xx23_OF_RC_CB_LGF2_INS_59;
            bool FIRE_xx24_OF_RC_CB_LGF2;
            bool FIRE_xx23_OF_RC_CB_LHA2_INS_61;
            bool FIRE_xx23_OF_RC_CB_LHA2_INS_62;
            bool FIRE_xx24_OF_RC_CB_LHA2;
            bool FIRE_xx23_OF_RC_CB_LHA3_INS_64;
            bool FIRE_xx23_OF_RC_CB_LHA3_INS_65;
            bool FIRE_xx24_OF_RC_CB_LHA3;
            bool FIRE_xx23_OF_RC_CB_LHB2_INS_67;
            bool FIRE_xx23_OF_RC_CB_LHB2_INS_68;
            bool FIRE_xx24_OF_RC_CB_LHB2;
            bool FIRE_xx11_OF_RDA1;
            bool FIRE_xx11_OF_RDA2;
            bool FIRE_xx11_OF_RDB1;
            bool FIRE_xx11_OF_RDB2;
            bool FIRE_xx23_OF_RO_CB_LHA1_INS_74;
            bool FIRE_xx23_OF_RO_CB_LHA1_INS_75;
            bool FIRE_xx24_OF_RO_CB_LHA1;
            bool FIRE_xx23_OF_RO_CB_LHA2_INS_77;
            bool FIRE_xx23_OF_RO_CB_LHA2_INS_78;
            bool FIRE_xx24_OF_RO_CB_LHA2;
            bool FIRE_xx23_OF_RO_CB_LHB1_INS_80;
            bool FIRE_xx23_OF_RO_CB_LHB1_INS_81;
            bool FIRE_xx24_OF_RO_CB_LHB1;
            bool FIRE_xx10_OF_SH_CB_GEV;
            bool FIRE_xx11_OF_SH_CB_GEV;
            bool FIRE_xx10_OF_SH_CB_LBA1;
            bool FIRE_xx11_OF_SH_CB_LBA1;
            bool FIRE_xx11_OF_SH_CB_LBA2;
            bool FIRE_xx10_OF_SH_CB_LBB1;
            bool FIRE_xx11_OF_SH_CB_LBB1;
            bool FIRE_xx11_OF_SH_CB_LBB2;
            bool FIRE_xx10_OF_SH_CB_LGA;
            bool FIRE_xx11_OF_SH_CB_LGA;
            bool FIRE_xx10_OF_SH_CB_LGB;
            bool FIRE_xx11_OF_SH_CB_LGB;
            bool FIRE_xx10_OF_SH_CB_LGD1;
            bool FIRE_xx11_OF_SH_CB_LGD1;
            bool FIRE_xx10_OF_SH_CB_LGD2;
            bool FIRE_xx11_OF_SH_CB_LGD2;
            bool FIRE_xx11_OF_SH_CB_LGE1;
            bool FIRE_xx10_OF_SH_CB_LGF1;
            bool FIRE_xx11_OF_SH_CB_LGF1;
            bool FIRE_xx10_OF_SH_CB_LGF2;
            bool FIRE_xx11_OF_SH_CB_LGF2;
            bool FIRE_xx10_OF_SH_CB_LHA1;
            bool FIRE_xx11_OF_SH_CB_LHA1;
            bool FIRE_xx10_OF_SH_CB_LHA2;
            bool FIRE_xx11_OF_SH_CB_LHA2;
            bool FIRE_xx10_OF_SH_CB_LHA3;
            bool FIRE_xx11_OF_SH_CB_LHA3;
            bool FIRE_xx10_OF_SH_CB_LHB1;
            bool FIRE_xx11_OF_SH_CB_LHB1;
            bool FIRE_xx10_OF_SH_CB_LHB2;
            bool FIRE_xx11_OF_SH_CB_LHB2;
            bool FIRE_xx11_OF_SH_CB_RDA1;
            bool FIRE_xx11_OF_SH_CB_RDA2;
            bool FIRE_xx11_OF_SH_CB_RDB1;
            bool FIRE_xx11_OF_SH_CB_RDB2;
            bool FIRE_xx11_OF_SH_CB_TUA1;
            bool FIRE_xx11_OF_SH_CB_TUA2;
            bool FIRE_xx11_OF_SH_CB_TUB1;
            bool FIRE_xx11_OF_SH_CB_TUB2;
            bool FIRE_xx10_OF_SH_CB_line_GEV;
            bool FIRE_xx11_OF_SH_CB_line_GEV;
            bool FIRE_xx10_OF_SH_CB_line_LGR;
            bool FIRE_xx11_OF_SH_CB_line_LGR;
            bool FIRE_xx10_OF_SUBSTATION;
            bool FIRE_xx11_OF_SUBSTATION;
            bool FIRE_xx10_OF_TA;
            bool FIRE_xx11_OF_TA;
            bool FIRE_xx10_OF_TAC;
            bool FIRE_xx11_OF_TAC;
            bool FIRE_xx10_OF_TP;
            bool FIRE_xx11_OF_TP;
            bool FIRE_xx10_OF_TS;
            bool FIRE_xx11_OF_TS;
            bool FIRE_xx11_OF_TUA1;
            bool FIRE_xx11_OF_TUA2;
            bool FIRE_xx11_OF_TUB1;
            bool FIRE_xx11_OF_TUB2;
            bool FIRE_xx10_OF_UNIT;
            bool FIRE_xx11_OF_UNIT;
            bool FIRE_xx10_OF_in_function_house;
            bool FIRE_xx11_OF_in_function_house;
            bool FIRE_xx23_OF_demand_CCF_DG_INS_144;
            bool FIRE_xx23_OF_demand_CCF_DG_INS_145;
            bool FIRE_xx24_OF_demand_CCF_DG;
            bool FIRE_xx23_OF_demand_DGA_INS_147;
            bool FIRE_xx23_OF_demand_DGA_INS_148;
            bool FIRE_xx24_OF_demand_DGA;
            bool FIRE_xx23_OF_demand_DGB_INS_150;
            bool FIRE_xx23_OF_demand_DGB_INS_151;
            bool FIRE_xx24_OF_demand_DGB;
            bool FIRE_xx23_OF_demand_TAC_INS_153;
            bool FIRE_xx23_OF_demand_TAC_INS_154;
            bool FIRE_xx24_OF_demand_TAC;
            bool FIRE_xx23_OF_on_demand_house_INS_156;
            bool FIRE_xx23_OF_on_demand_house_INS_157;
            bool FIRE_xx24_OF_on_demand_house;

            int S_OF_auto_exclusions = 0 ;
            int required_OF_AND_3 = 1 ;
            int already_S_OF_AND_3 = 2 ;
            int S_OF_AND_3 = 3 ;
            int relevant_evt_OF_AND_3 = 4 ;
            int required_OF_BATTERY_A_lost = 5 ;
            int already_S_OF_BATTERY_A_lost = 6 ;
            int S_OF_BATTERY_A_lost = 7 ;
            int relevant_evt_OF_BATTERY_A_lost = 8 ;
            int required_OF_BATTERY_B_lost = 9 ;
            int already_S_OF_BATTERY_B_lost = 10 ;
            int S_OF_BATTERY_B_lost = 11 ;
            int relevant_evt_OF_BATTERY_B_lost = 12 ;
            int required_OF_BATT_A1 = 13 ;
            int already_S_OF_BATT_A1 = 14 ;
            int S_OF_BATT_A1 = 15 ;
            int relevant_evt_OF_BATT_A1 = 16 ;
            int waiting_for_rep_OF_BATT_A1 = 17 ;
            int failF_OF_BATT_A1 = 18 ;
            int init_OF_BATT_A1 = 19 ;
            int required_OF_BATT_A2 = 20 ;
            int already_S_OF_BATT_A2 = 21 ;
            int S_OF_BATT_A2 = 22 ;
            int relevant_evt_OF_BATT_A2 = 23 ;
            int waiting_for_rep_OF_BATT_A2 = 24 ;
            int failF_OF_BATT_A2 = 25 ;
            int init_OF_BATT_A2 = 26 ;
            int required_OF_BATT_B1 = 27 ;
            int already_S_OF_BATT_B1 = 28 ;
            int S_OF_BATT_B1 = 29 ;
            int relevant_evt_OF_BATT_B1 = 30 ;
            int waiting_for_rep_OF_BATT_B1 = 31 ;
            int failF_OF_BATT_B1 = 32 ;
            int init_OF_BATT_B1 = 33 ;
            int required_OF_BATT_B2 = 34 ;
            int already_S_OF_BATT_B2 = 35 ;
            int S_OF_BATT_B2 = 36 ;
            int relevant_evt_OF_BATT_B2 = 37 ;
            int waiting_for_rep_OF_BATT_B2 = 38 ;
            int failF_OF_BATT_B2 = 39 ;
            int init_OF_BATT_B2 = 40 ;
            int required_OF_CB_LGD2_unable = 41 ;
            int already_S_OF_CB_LGD2_unable = 42 ;
            int S_OF_CB_LGD2_unable = 43 ;
            int relevant_evt_OF_CB_LGD2_unable = 44 ;
            int required_OF_CB_LGF2_unable = 45 ;
            int already_S_OF_CB_LGF2_unable = 46 ;
            int S_OF_CB_LGF2_unable = 47 ;
            int relevant_evt_OF_CB_LGF2_unable = 48 ;
            int required_OF_CB_LHA12_unable = 49 ;
            int already_S_OF_CB_LHA12_unable = 50 ;
            int S_OF_CB_LHA12_unable = 51 ;
            int relevant_evt_OF_CB_LHA12_unable = 52 ;
            int required_OF_CB_LHA3_unable = 53 ;
            int already_S_OF_CB_LHA3_unable = 54 ;
            int S_OF_CB_LHA3_unable = 55 ;
            int relevant_evt_OF_CB_LHA3_unable = 56 ;
            int required_OF_CB_LHB12_unable = 57 ;
            int already_S_OF_CB_LHB12_unable = 58 ;
            int S_OF_CB_LHB12_unable = 59 ;
            int relevant_evt_OF_CB_LHB12_unable = 60 ;
            int required_OF_CCF_DG = 61 ;
            int already_S_OF_CCF_DG = 62 ;
            int S_OF_CCF_DG = 63 ;
            int relevant_evt_OF_CCF_DG = 64 ;
            int waiting_for_rep_OF_CCF_DG = 65 ;
            int failF_OF_CCF_DG = 66 ;
            int init_OF_CCF_DG = 67 ;
            int required_OF_CCF_GEV_LGR = 68 ;
            int already_S_OF_CCF_GEV_LGR = 69 ;
            int S_OF_CCF_GEV_LGR = 70 ;
            int relevant_evt_OF_CCF_GEV_LGR = 71 ;
            int waiting_for_rep_OF_CCF_GEV_LGR = 72 ;
            int failF_OF_CCF_GEV_LGR = 73 ;
            int init_OF_CCF_GEV_LGR = 74 ;
            int required_OF_DGA_long = 75 ;
            int already_S_OF_DGA_long = 76 ;
            int S_OF_DGA_long = 77 ;
            int relevant_evt_OF_DGA_long = 78 ;
            int waiting_for_rep_OF_DGA_long = 79 ;
            int failF_OF_DGA_long = 80 ;
            int init_OF_DGA_long = 81 ;
            int required_OF_DGA_lost = 82 ;
            int already_S_OF_DGA_lost = 83 ;
            int S_OF_DGA_lost = 84 ;
            int relevant_evt_OF_DGA_lost = 85 ;
            int required_OF_DGA_short = 86 ;
            int already_S_OF_DGA_short = 87 ;
            int S_OF_DGA_short = 88 ;
            int relevant_evt_OF_DGA_short = 89 ;
            int waiting_for_rep_OF_DGA_short = 90 ;
            int failF_OF_DGA_short = 91 ;
            int init_OF_DGA_short = 92 ;
            int required_OF_DGB_long = 93 ;
            int already_S_OF_DGB_long = 94 ;
            int S_OF_DGB_long = 95 ;
            int relevant_evt_OF_DGB_long = 96 ;
            int waiting_for_rep_OF_DGB_long = 97 ;
            int failF_OF_DGB_long = 98 ;
            int init_OF_DGB_long = 99 ;
            int required_OF_DGB_lost = 100 ;
            int already_S_OF_DGB_lost = 101 ;
            int S_OF_DGB_lost = 102 ;
            int relevant_evt_OF_DGB_lost = 103 ;
            int required_OF_DGB_short = 104 ;
            int already_S_OF_DGB_short = 105 ;
            int S_OF_DGB_short = 106 ;
            int relevant_evt_OF_DGB_short = 107 ;
            int waiting_for_rep_OF_DGB_short = 108 ;
            int failF_OF_DGB_short = 109 ;
            int init_OF_DGB_short = 110 ;
            int required_OF_GEV = 111 ;
            int already_S_OF_GEV = 112 ;
            int S_OF_GEV = 113 ;
            int relevant_evt_OF_GEV = 114 ;
            int waiting_for_rep_OF_GEV = 115 ;
            int failF_OF_GEV = 116 ;
            int init_OF_GEV = 117 ;
            int required_OF_GRID = 118 ;
            int already_S_OF_GRID = 119 ;
            int S_OF_GRID = 120 ;
            int relevant_evt_OF_GRID = 121 ;
            int waiting_for_rep_OF_GRID = 122 ;
            int failF_OF_GRID = 123 ;
            int init_OF_GRID = 124 ;
            int required_OF_LBA = 125 ;
            int already_S_OF_LBA = 126 ;
            int S_OF_LBA = 127 ;
            int relevant_evt_OF_LBA = 128 ;
            int waiting_for_rep_OF_LBA = 129 ;
            int failF_OF_LBA = 130 ;
            int init_OF_LBA = 131 ;
            int required_OF_LBA_by_line1_lost = 132 ;
            int already_S_OF_LBA_by_line1_lost = 133 ;
            int S_OF_LBA_by_line1_lost = 134 ;
            int relevant_evt_OF_LBA_by_line1_lost = 135 ;
            int waiting_for_rep_OF_LBA_by_line1_lost = 136 ;
            int failAG_OF_LBA_by_line1_lost = 137 ;
            int init_OF_LBA_by_line1_lost = 138 ;
            int required_OF_LBA_by_line2_lost = 139 ;
            int already_S_OF_LBA_by_line2_lost = 140 ;
            int S_OF_LBA_by_line2_lost = 141 ;
            int relevant_evt_OF_LBA_by_line2_lost = 142 ;
            int waiting_for_rep_OF_LBA_by_line2_lost = 143 ;
            int failAG_OF_LBA_by_line2_lost = 144 ;
            int init_OF_LBA_by_line2_lost = 145 ;
            int required_OF_LBA_by_others_lost = 146 ;
            int already_S_OF_LBA_by_others_lost = 147 ;
            int S_OF_LBA_by_others_lost = 148 ;
            int relevant_evt_OF_LBA_by_others_lost = 149 ;
            int required_OF_LBA_lost = 150 ;
            int already_S_OF_LBA_lost = 151 ;
            int S_OF_LBA_lost = 152 ;
            int relevant_evt_OF_LBA_lost = 153 ;
            int required_OF_LBA_not_fed = 154 ;
            int already_S_OF_LBA_not_fed = 155 ;
            int S_OF_LBA_not_fed = 156 ;
            int relevant_evt_OF_LBA_not_fed = 157 ;
            int required_OF_LBB = 158 ;
            int already_S_OF_LBB = 159 ;
            int S_OF_LBB = 160 ;
            int relevant_evt_OF_LBB = 161 ;
            int waiting_for_rep_OF_LBB = 162 ;
            int failF_OF_LBB = 163 ;
            int init_OF_LBB = 164 ;
            int required_OF_LBB_by_line1_lost = 165 ;
            int already_S_OF_LBB_by_line1_lost = 166 ;
            int S_OF_LBB_by_line1_lost = 167 ;
            int relevant_evt_OF_LBB_by_line1_lost = 168 ;
            int waiting_for_rep_OF_LBB_by_line1_lost = 169 ;
            int failAG_OF_LBB_by_line1_lost = 170 ;
            int init_OF_LBB_by_line1_lost = 171 ;
            int required_OF_LBB_by_line2_lost = 172 ;
            int already_S_OF_LBB_by_line2_lost = 173 ;
            int S_OF_LBB_by_line2_lost = 174 ;
            int relevant_evt_OF_LBB_by_line2_lost = 175 ;
            int waiting_for_rep_OF_LBB_by_line2_lost = 176 ;
            int failAG_OF_LBB_by_line2_lost = 177 ;
            int init_OF_LBB_by_line2_lost = 178 ;
            int required_OF_LBB_lost = 179 ;
            int already_S_OF_LBB_lost = 180 ;
            int S_OF_LBB_lost = 181 ;
            int relevant_evt_OF_LBB_lost = 182 ;
            int required_OF_LBB_not_fed = 183 ;
            int already_S_OF_LBB_not_fed = 184 ;
            int S_OF_LBB_not_fed = 185 ;
            int relevant_evt_OF_LBB_not_fed = 186 ;
            int required_OF_LGA = 187 ;
            int already_S_OF_LGA = 188 ;
            int S_OF_LGA = 189 ;
            int relevant_evt_OF_LGA = 190 ;
            int waiting_for_rep_OF_LGA = 191 ;
            int failF_OF_LGA = 192 ;
            int init_OF_LGA = 193 ;
            int required_OF_LGB = 194 ;
            int already_S_OF_LGB = 195 ;
            int S_OF_LGB = 196 ;
            int relevant_evt_OF_LGB = 197 ;
            int waiting_for_rep_OF_LGB = 198 ;
            int failF_OF_LGB = 199 ;
            int init_OF_LGB = 200 ;
            int required_OF_LGD = 201 ;
            int already_S_OF_LGD = 202 ;
            int S_OF_LGD = 203 ;
            int relevant_evt_OF_LGD = 204 ;
            int waiting_for_rep_OF_LGD = 205 ;
            int failF_OF_LGD = 206 ;
            int init_OF_LGD = 207 ;
            int required_OF_LGD_not_fed = 208 ;
            int already_S_OF_LGD_not_fed = 209 ;
            int S_OF_LGD_not_fed = 210 ;
            int relevant_evt_OF_LGD_not_fed = 211 ;
            int required_OF_LGE = 212 ;
            int already_S_OF_LGE = 213 ;
            int S_OF_LGE = 214 ;
            int relevant_evt_OF_LGE = 215 ;
            int waiting_for_rep_OF_LGE = 216 ;
            int failF_OF_LGE = 217 ;
            int init_OF_LGE = 218 ;
            int required_OF_LGF = 219 ;
            int already_S_OF_LGF = 220 ;
            int S_OF_LGF = 221 ;
            int relevant_evt_OF_LGF = 222 ;
            int waiting_for_rep_OF_LGF = 223 ;
            int failF_OF_LGF = 224 ;
            int init_OF_LGF = 225 ;
            int required_OF_LGF_not_fed = 226 ;
            int already_S_OF_LGF_not_fed = 227 ;
            int S_OF_LGF_not_fed = 228 ;
            int relevant_evt_OF_LGF_not_fed = 229 ;
            int required_OF_LGR = 230 ;
            int already_S_OF_LGR = 231 ;
            int S_OF_LGR = 232 ;
            int relevant_evt_OF_LGR = 233 ;
            int waiting_for_rep_OF_LGR = 234 ;
            int failF_OF_LGR = 235 ;
            int init_OF_LGR = 236 ;
            int required_OF_LHA = 237 ;
            int already_S_OF_LHA = 238 ;
            int S_OF_LHA = 239 ;
            int relevant_evt_OF_LHA = 240 ;
            int waiting_for_rep_OF_LHA = 241 ;
            int failF_OF_LHA = 242 ;
            int init_OF_LHA = 243 ;
            int required_OF_LHA_and_LHB_lost = 244 ;
            int already_S_OF_LHA_and_LHB_lost = 245 ;
            int S_OF_LHA_and_LHB_lost = 246 ;
            int relevant_evt_OF_LHA_and_LHB_lost = 247 ;
            int required_OF_LHA_lost = 248 ;
            int already_S_OF_LHA_lost = 249 ;
            int S_OF_LHA_lost = 250 ;
            int relevant_evt_OF_LHA_lost = 251 ;
            int required_OF_LHA_not_fed = 252 ;
            int already_S_OF_LHA_not_fed = 253 ;
            int S_OF_LHA_not_fed = 254 ;
            int relevant_evt_OF_LHA_not_fed = 255 ;
            int required_OF_LHB = 256 ;
            int already_S_OF_LHB = 257 ;
            int S_OF_LHB = 258 ;
            int relevant_evt_OF_LHB = 259 ;
            int waiting_for_rep_OF_LHB = 260 ;
            int failF_OF_LHB = 261 ;
            int init_OF_LHB = 262 ;
            int required_OF_LHB_lost = 263 ;
            int already_S_OF_LHB_lost = 264 ;
            int S_OF_LHB_lost = 265 ;
            int relevant_evt_OF_LHB_lost = 266 ;
            int required_OF_LHB_not_fed = 267 ;
            int already_S_OF_LHB_not_fed = 268 ;
            int S_OF_LHB_not_fed = 269 ;
            int relevant_evt_OF_LHB_not_fed = 270 ;
            int required_OF_LKE = 271 ;
            int already_S_OF_LKE = 272 ;
            int S_OF_LKE = 273 ;
            int relevant_evt_OF_LKE = 274 ;
            int waiting_for_rep_OF_LKE = 275 ;
            int failF_OF_LKE = 276 ;
            int init_OF_LKE = 277 ;
            int required_OF_LKI = 278 ;
            int already_S_OF_LKI = 279 ;
            int S_OF_LKI = 280 ;
            int relevant_evt_OF_LKI = 281 ;
            int waiting_for_rep_OF_LKI = 282 ;
            int failF_OF_LKI = 283 ;
            int init_OF_LKI = 284 ;
            int required_OF_LLA = 285 ;
            int already_S_OF_LLA = 286 ;
            int S_OF_LLA = 287 ;
            int relevant_evt_OF_LLA = 288 ;
            int waiting_for_rep_OF_LLA = 289 ;
            int failF_OF_LLA = 290 ;
            int init_OF_LLA = 291 ;
            int required_OF_LLD = 292 ;
            int already_S_OF_LLD = 293 ;
            int S_OF_LLD = 294 ;
            int relevant_evt_OF_LLD = 295 ;
            int waiting_for_rep_OF_LLD = 296 ;
            int failF_OF_LLD = 297 ;
            int init_OF_LLD = 298 ;
            int required_OF_OR_14 = 299 ;
            int already_S_OF_OR_14 = 300 ;
            int S_OF_OR_14 = 301 ;
            int relevant_evt_OF_OR_14 = 302 ;
            int required_OF_RC_CB_LGD2 = 303 ;
            int already_S_OF_RC_CB_LGD2 = 304 ;
            int S_OF_RC_CB_LGD2 = 305 ;
            int relevant_evt_OF_RC_CB_LGD2 = 306 ;
            int waiting_for_rep_OF_RC_CB_LGD2 = 307 ;
            int failI_OF_RC_CB_LGD2 = 308 ;
            int to_be_fired_OF_RC_CB_LGD2 = 309 ;
            int already_standby_OF_RC_CB_LGD2 = 310 ;
            int already_required_OF_RC_CB_LGD2 = 311 ;
            int required_OF_RC_CB_LGD2_ = 312 ;
            int already_S_OF_RC_CB_LGD2_ = 313 ;
            int S_OF_RC_CB_LGD2_ = 314 ;
            int relevant_evt_OF_RC_CB_LGD2_ = 315 ;
            int required_OF_RC_CB_LGF2 = 316 ;
            int already_S_OF_RC_CB_LGF2 = 317 ;
            int S_OF_RC_CB_LGF2 = 318 ;
            int relevant_evt_OF_RC_CB_LGF2 = 319 ;
            int waiting_for_rep_OF_RC_CB_LGF2 = 320 ;
            int failI_OF_RC_CB_LGF2 = 321 ;
            int to_be_fired_OF_RC_CB_LGF2 = 322 ;
            int already_standby_OF_RC_CB_LGF2 = 323 ;
            int already_required_OF_RC_CB_LGF2 = 324 ;
            int required_OF_RC_CB_LGF2_ = 325 ;
            int already_S_OF_RC_CB_LGF2_ = 326 ;
            int S_OF_RC_CB_LGF2_ = 327 ;
            int relevant_evt_OF_RC_CB_LGF2_ = 328 ;
            int required_OF_RC_CB_LHA2 = 329 ;
            int already_S_OF_RC_CB_LHA2 = 330 ;
            int S_OF_RC_CB_LHA2 = 331 ;
            int relevant_evt_OF_RC_CB_LHA2 = 332 ;
            int waiting_for_rep_OF_RC_CB_LHA2 = 333 ;
            int failI_OF_RC_CB_LHA2 = 334 ;
            int to_be_fired_OF_RC_CB_LHA2 = 335 ;
            int already_standby_OF_RC_CB_LHA2 = 336 ;
            int already_required_OF_RC_CB_LHA2 = 337 ;
            int required_OF_RC_CB_LHA2_ = 338 ;
            int already_S_OF_RC_CB_LHA2_ = 339 ;
            int S_OF_RC_CB_LHA2_ = 340 ;
            int relevant_evt_OF_RC_CB_LHA2_ = 341 ;
            int required_OF_RC_CB_LHA3 = 342 ;
            int already_S_OF_RC_CB_LHA3 = 343 ;
            int S_OF_RC_CB_LHA3 = 344 ;
            int relevant_evt_OF_RC_CB_LHA3 = 345 ;
            int waiting_for_rep_OF_RC_CB_LHA3 = 346 ;
            int failI_OF_RC_CB_LHA3 = 347 ;
            int to_be_fired_OF_RC_CB_LHA3 = 348 ;
            int already_standby_OF_RC_CB_LHA3 = 349 ;
            int already_required_OF_RC_CB_LHA3 = 350 ;
            int required_OF_RC_CB_LHA3_ = 351 ;
            int already_S_OF_RC_CB_LHA3_ = 352 ;
            int S_OF_RC_CB_LHA3_ = 353 ;
            int relevant_evt_OF_RC_CB_LHA3_ = 354 ;
            int required_OF_RC_CB_LHB2 = 355 ;
            int already_S_OF_RC_CB_LHB2 = 356 ;
            int S_OF_RC_CB_LHB2 = 357 ;
            int relevant_evt_OF_RC_CB_LHB2 = 358 ;
            int waiting_for_rep_OF_RC_CB_LHB2 = 359 ;
            int failI_OF_RC_CB_LHB2 = 360 ;
            int to_be_fired_OF_RC_CB_LHB2 = 361 ;
            int already_standby_OF_RC_CB_LHB2 = 362 ;
            int already_required_OF_RC_CB_LHB2 = 363 ;
            int required_OF_RC_CB_LHB2_ = 364 ;
            int already_S_OF_RC_CB_LHB2_ = 365 ;
            int S_OF_RC_CB_LHB2_ = 366 ;
            int relevant_evt_OF_RC_CB_LHB2_ = 367 ;
            int required_OF_RDA1 = 368 ;
            int already_S_OF_RDA1 = 369 ;
            int S_OF_RDA1 = 370 ;
            int relevant_evt_OF_RDA1 = 371 ;
            int waiting_for_rep_OF_RDA1 = 372 ;
            int failF_OF_RDA1 = 373 ;
            int init_OF_RDA1 = 374 ;
            int required_OF_RDA2 = 375 ;
            int already_S_OF_RDA2 = 376 ;
            int S_OF_RDA2 = 377 ;
            int relevant_evt_OF_RDA2 = 378 ;
            int waiting_for_rep_OF_RDA2 = 379 ;
            int failF_OF_RDA2 = 380 ;
            int init_OF_RDA2 = 381 ;
            int required_OF_RDB1 = 382 ;
            int already_S_OF_RDB1 = 383 ;
            int S_OF_RDB1 = 384 ;
            int relevant_evt_OF_RDB1 = 385 ;
            int waiting_for_rep_OF_RDB1 = 386 ;
            int failF_OF_RDB1 = 387 ;
            int init_OF_RDB1 = 388 ;
            int required_OF_RDB2 = 389 ;
            int already_S_OF_RDB2 = 390 ;
            int S_OF_RDB2 = 391 ;
            int relevant_evt_OF_RDB2 = 392 ;
            int waiting_for_rep_OF_RDB2 = 393 ;
            int failF_OF_RDB2 = 394 ;
            int init_OF_RDB2 = 395 ;
            int required_OF_RO_CB_LHA1 = 396 ;
            int already_S_OF_RO_CB_LHA1 = 397 ;
            int S_OF_RO_CB_LHA1 = 398 ;
            int relevant_evt_OF_RO_CB_LHA1 = 399 ;
            int waiting_for_rep_OF_RO_CB_LHA1 = 400 ;
            int failI_OF_RO_CB_LHA1 = 401 ;
            int to_be_fired_OF_RO_CB_LHA1 = 402 ;
            int already_standby_OF_RO_CB_LHA1 = 403 ;
            int already_required_OF_RO_CB_LHA1 = 404 ;
            int required_OF_RO_CB_LHA1_ = 405 ;
            int already_S_OF_RO_CB_LHA1_ = 406 ;
            int S_OF_RO_CB_LHA1_ = 407 ;
            int relevant_evt_OF_RO_CB_LHA1_ = 408 ;
            int required_OF_RO_CB_LHA2 = 409 ;
            int already_S_OF_RO_CB_LHA2 = 410 ;
            int S_OF_RO_CB_LHA2 = 411 ;
            int relevant_evt_OF_RO_CB_LHA2 = 412 ;
            int waiting_for_rep_OF_RO_CB_LHA2 = 413 ;
            int failI_OF_RO_CB_LHA2 = 414 ;
            int to_be_fired_OF_RO_CB_LHA2 = 415 ;
            int already_standby_OF_RO_CB_LHA2 = 416 ;
            int already_required_OF_RO_CB_LHA2 = 417 ;
            int required_OF_RO_CB_LHA2_ = 418 ;
            int already_S_OF_RO_CB_LHA2_ = 419 ;
            int S_OF_RO_CB_LHA2_ = 420 ;
            int relevant_evt_OF_RO_CB_LHA2_ = 421 ;
            int required_OF_RO_CB_LHB1 = 422 ;
            int already_S_OF_RO_CB_LHB1 = 423 ;
            int S_OF_RO_CB_LHB1 = 424 ;
            int relevant_evt_OF_RO_CB_LHB1 = 425 ;
            int waiting_for_rep_OF_RO_CB_LHB1 = 426 ;
            int failI_OF_RO_CB_LHB1 = 427 ;
            int to_be_fired_OF_RO_CB_LHB1 = 428 ;
            int already_standby_OF_RO_CB_LHB1 = 429 ;
            int already_required_OF_RO_CB_LHB1 = 430 ;
            int required_OF_RO_CB_LHB1_ = 431 ;
            int already_S_OF_RO_CB_LHB1_ = 432 ;
            int S_OF_RO_CB_LHB1_ = 433 ;
            int relevant_evt_OF_RO_CB_LHB1_ = 434 ;
            int required_OF_SH_CB_GEV = 435 ;
            int already_S_OF_SH_CB_GEV = 436 ;
            int S_OF_SH_CB_GEV = 437 ;
            int relevant_evt_OF_SH_CB_GEV = 438 ;
            int waiting_for_rep_OF_SH_CB_GEV = 439 ;
            int failF_OF_SH_CB_GEV = 440 ;
            int init_OF_SH_CB_GEV = 441 ;
            int required_OF_SH_CB_LBA1 = 442 ;
            int already_S_OF_SH_CB_LBA1 = 443 ;
            int S_OF_SH_CB_LBA1 = 444 ;
            int relevant_evt_OF_SH_CB_LBA1 = 445 ;
            int waiting_for_rep_OF_SH_CB_LBA1 = 446 ;
            int failF_OF_SH_CB_LBA1 = 447 ;
            int init_OF_SH_CB_LBA1 = 448 ;
            int required_OF_SH_CB_LBA2 = 449 ;
            int already_S_OF_SH_CB_LBA2 = 450 ;
            int S_OF_SH_CB_LBA2 = 451 ;
            int relevant_evt_OF_SH_CB_LBA2 = 452 ;
            int waiting_for_rep_OF_SH_CB_LBA2 = 453 ;
            int failF_OF_SH_CB_LBA2 = 454 ;
            int init_OF_SH_CB_LBA2 = 455 ;
            int required_OF_SH_CB_LBB1 = 456 ;
            int already_S_OF_SH_CB_LBB1 = 457 ;
            int S_OF_SH_CB_LBB1 = 458 ;
            int relevant_evt_OF_SH_CB_LBB1 = 459 ;
            int waiting_for_rep_OF_SH_CB_LBB1 = 460 ;
            int failF_OF_SH_CB_LBB1 = 461 ;
            int init_OF_SH_CB_LBB1 = 462 ;
            int required_OF_SH_CB_LBB2 = 463 ;
            int already_S_OF_SH_CB_LBB2 = 464 ;
            int S_OF_SH_CB_LBB2 = 465 ;
            int relevant_evt_OF_SH_CB_LBB2 = 466 ;
            int waiting_for_rep_OF_SH_CB_LBB2 = 467 ;
            int failF_OF_SH_CB_LBB2 = 468 ;
            int init_OF_SH_CB_LBB2 = 469 ;
            int required_OF_SH_CB_LGA = 470 ;
            int already_S_OF_SH_CB_LGA = 471 ;
            int S_OF_SH_CB_LGA = 472 ;
            int relevant_evt_OF_SH_CB_LGA = 473 ;
            int waiting_for_rep_OF_SH_CB_LGA = 474 ;
            int failF_OF_SH_CB_LGA = 475 ;
            int init_OF_SH_CB_LGA = 476 ;
            int required_OF_SH_CB_LGB = 477 ;
            int already_S_OF_SH_CB_LGB = 478 ;
            int S_OF_SH_CB_LGB = 479 ;
            int relevant_evt_OF_SH_CB_LGB = 480 ;
            int waiting_for_rep_OF_SH_CB_LGB = 481 ;
            int failF_OF_SH_CB_LGB = 482 ;
            int init_OF_SH_CB_LGB = 483 ;
            int required_OF_SH_CB_LGD1 = 484 ;
            int already_S_OF_SH_CB_LGD1 = 485 ;
            int S_OF_SH_CB_LGD1 = 486 ;
            int relevant_evt_OF_SH_CB_LGD1 = 487 ;
            int waiting_for_rep_OF_SH_CB_LGD1 = 488 ;
            int failF_OF_SH_CB_LGD1 = 489 ;
            int init_OF_SH_CB_LGD1 = 490 ;
            int required_OF_SH_CB_LGD2 = 491 ;
            int already_S_OF_SH_CB_LGD2 = 492 ;
            int S_OF_SH_CB_LGD2 = 493 ;
            int relevant_evt_OF_SH_CB_LGD2 = 494 ;
            int waiting_for_rep_OF_SH_CB_LGD2 = 495 ;
            int failF_OF_SH_CB_LGD2 = 496 ;
            int init_OF_SH_CB_LGD2 = 497 ;
            int required_OF_SH_CB_LGE1 = 498 ;
            int already_S_OF_SH_CB_LGE1 = 499 ;
            int S_OF_SH_CB_LGE1 = 500 ;
            int relevant_evt_OF_SH_CB_LGE1 = 501 ;
            int waiting_for_rep_OF_SH_CB_LGE1 = 502 ;
            int failF_OF_SH_CB_LGE1 = 503 ;
            int init_OF_SH_CB_LGE1 = 504 ;
            int required_OF_SH_CB_LGF1 = 505 ;
            int already_S_OF_SH_CB_LGF1 = 506 ;
            int S_OF_SH_CB_LGF1 = 507 ;
            int relevant_evt_OF_SH_CB_LGF1 = 508 ;
            int waiting_for_rep_OF_SH_CB_LGF1 = 509 ;
            int failF_OF_SH_CB_LGF1 = 510 ;
            int init_OF_SH_CB_LGF1 = 511 ;
            int required_OF_SH_CB_LGF2 = 512 ;
            int already_S_OF_SH_CB_LGF2 = 513 ;
            int S_OF_SH_CB_LGF2 = 514 ;
            int relevant_evt_OF_SH_CB_LGF2 = 515 ;
            int waiting_for_rep_OF_SH_CB_LGF2 = 516 ;
            int failF_OF_SH_CB_LGF2 = 517 ;
            int init_OF_SH_CB_LGF2 = 518 ;
            int required_OF_SH_CB_LHA1 = 519 ;
            int already_S_OF_SH_CB_LHA1 = 520 ;
            int S_OF_SH_CB_LHA1 = 521 ;
            int relevant_evt_OF_SH_CB_LHA1 = 522 ;
            int waiting_for_rep_OF_SH_CB_LHA1 = 523 ;
            int failF_OF_SH_CB_LHA1 = 524 ;
            int init_OF_SH_CB_LHA1 = 525 ;
            int required_OF_SH_CB_LHA2 = 526 ;
            int already_S_OF_SH_CB_LHA2 = 527 ;
            int S_OF_SH_CB_LHA2 = 528 ;
            int relevant_evt_OF_SH_CB_LHA2 = 529 ;
            int waiting_for_rep_OF_SH_CB_LHA2 = 530 ;
            int failF_OF_SH_CB_LHA2 = 531 ;
            int init_OF_SH_CB_LHA2 = 532 ;
            int required_OF_SH_CB_LHA3 = 533 ;
            int already_S_OF_SH_CB_LHA3 = 534 ;
            int S_OF_SH_CB_LHA3 = 535 ;
            int relevant_evt_OF_SH_CB_LHA3 = 536 ;
            int waiting_for_rep_OF_SH_CB_LHA3 = 537 ;
            int failF_OF_SH_CB_LHA3 = 538 ;
            int init_OF_SH_CB_LHA3 = 539 ;
            int required_OF_SH_CB_LHB1 = 540 ;
            int already_S_OF_SH_CB_LHB1 = 541 ;
            int S_OF_SH_CB_LHB1 = 542 ;
            int relevant_evt_OF_SH_CB_LHB1 = 543 ;
            int waiting_for_rep_OF_SH_CB_LHB1 = 544 ;
            int failF_OF_SH_CB_LHB1 = 545 ;
            int init_OF_SH_CB_LHB1 = 546 ;
            int required_OF_SH_CB_LHB2 = 547 ;
            int already_S_OF_SH_CB_LHB2 = 548 ;
            int S_OF_SH_CB_LHB2 = 549 ;
            int relevant_evt_OF_SH_CB_LHB2 = 550 ;
            int waiting_for_rep_OF_SH_CB_LHB2 = 551 ;
            int failF_OF_SH_CB_LHB2 = 552 ;
            int init_OF_SH_CB_LHB2 = 553 ;
            int required_OF_SH_CB_RDA1 = 554 ;
            int already_S_OF_SH_CB_RDA1 = 555 ;
            int S_OF_SH_CB_RDA1 = 556 ;
            int relevant_evt_OF_SH_CB_RDA1 = 557 ;
            int waiting_for_rep_OF_SH_CB_RDA1 = 558 ;
            int failF_OF_SH_CB_RDA1 = 559 ;
            int init_OF_SH_CB_RDA1 = 560 ;
            int required_OF_SH_CB_RDA2 = 561 ;
            int already_S_OF_SH_CB_RDA2 = 562 ;
            int S_OF_SH_CB_RDA2 = 563 ;
            int relevant_evt_OF_SH_CB_RDA2 = 564 ;
            int waiting_for_rep_OF_SH_CB_RDA2 = 565 ;
            int failF_OF_SH_CB_RDA2 = 566 ;
            int init_OF_SH_CB_RDA2 = 567 ;
            int required_OF_SH_CB_RDB1 = 568 ;
            int already_S_OF_SH_CB_RDB1 = 569 ;
            int S_OF_SH_CB_RDB1 = 570 ;
            int relevant_evt_OF_SH_CB_RDB1 = 571 ;
            int waiting_for_rep_OF_SH_CB_RDB1 = 572 ;
            int failF_OF_SH_CB_RDB1 = 573 ;
            int init_OF_SH_CB_RDB1 = 574 ;
            int required_OF_SH_CB_RDB2 = 575 ;
            int already_S_OF_SH_CB_RDB2 = 576 ;
            int S_OF_SH_CB_RDB2 = 577 ;
            int relevant_evt_OF_SH_CB_RDB2 = 578 ;
            int waiting_for_rep_OF_SH_CB_RDB2 = 579 ;
            int failF_OF_SH_CB_RDB2 = 580 ;
            int init_OF_SH_CB_RDB2 = 581 ;
            int required_OF_SH_CB_TUA1 = 582 ;
            int already_S_OF_SH_CB_TUA1 = 583 ;
            int S_OF_SH_CB_TUA1 = 584 ;
            int relevant_evt_OF_SH_CB_TUA1 = 585 ;
            int waiting_for_rep_OF_SH_CB_TUA1 = 586 ;
            int failF_OF_SH_CB_TUA1 = 587 ;
            int init_OF_SH_CB_TUA1 = 588 ;
            int required_OF_SH_CB_TUA2 = 589 ;
            int already_S_OF_SH_CB_TUA2 = 590 ;
            int S_OF_SH_CB_TUA2 = 591 ;
            int relevant_evt_OF_SH_CB_TUA2 = 592 ;
            int waiting_for_rep_OF_SH_CB_TUA2 = 593 ;
            int failF_OF_SH_CB_TUA2 = 594 ;
            int init_OF_SH_CB_TUA2 = 595 ;
            int required_OF_SH_CB_TUB1 = 596 ;
            int already_S_OF_SH_CB_TUB1 = 597 ;
            int S_OF_SH_CB_TUB1 = 598 ;
            int relevant_evt_OF_SH_CB_TUB1 = 599 ;
            int waiting_for_rep_OF_SH_CB_TUB1 = 600 ;
            int failF_OF_SH_CB_TUB1 = 601 ;
            int init_OF_SH_CB_TUB1 = 602 ;
            int required_OF_SH_CB_TUB2 = 603 ;
            int already_S_OF_SH_CB_TUB2 = 604 ;
            int S_OF_SH_CB_TUB2 = 605 ;
            int relevant_evt_OF_SH_CB_TUB2 = 606 ;
            int waiting_for_rep_OF_SH_CB_TUB2 = 607 ;
            int failF_OF_SH_CB_TUB2 = 608 ;
            int init_OF_SH_CB_TUB2 = 609 ;
            int required_OF_SH_CB_line_GEV = 610 ;
            int already_S_OF_SH_CB_line_GEV = 611 ;
            int S_OF_SH_CB_line_GEV = 612 ;
            int relevant_evt_OF_SH_CB_line_GEV = 613 ;
            int waiting_for_rep_OF_SH_CB_line_GEV = 614 ;
            int failF_OF_SH_CB_line_GEV = 615 ;
            int init_OF_SH_CB_line_GEV = 616 ;
            int required_OF_SH_CB_line_LGR = 617 ;
            int already_S_OF_SH_CB_line_LGR = 618 ;
            int S_OF_SH_CB_line_LGR = 619 ;
            int relevant_evt_OF_SH_CB_line_LGR = 620 ;
            int waiting_for_rep_OF_SH_CB_line_LGR = 621 ;
            int failF_OF_SH_CB_line_LGR = 622 ;
            int init_OF_SH_CB_line_LGR = 623 ;
            int required_OF_SH_GEV_or_LGR = 624 ;
            int already_S_OF_SH_GEV_or_LGR = 625 ;
            int S_OF_SH_GEV_or_LGR = 626 ;
            int relevant_evt_OF_SH_GEV_or_LGR = 627 ;
            int required_OF_SUBSTATION = 628 ;
            int already_S_OF_SUBSTATION = 629 ;
            int S_OF_SUBSTATION = 630 ;
            int relevant_evt_OF_SUBSTATION = 631 ;
            int waiting_for_rep_OF_SUBSTATION = 632 ;
            int failF_OF_SUBSTATION = 633 ;
            int init_OF_SUBSTATION = 634 ;
            int required_OF_TA = 635 ;
            int already_S_OF_TA = 636 ;
            int S_OF_TA = 637 ;
            int relevant_evt_OF_TA = 638 ;
            int waiting_for_rep_OF_TA = 639 ;
            int failF_OF_TA = 640 ;
            int init_OF_TA = 641 ;
            int required_OF_TAC = 642 ;
            int already_S_OF_TAC = 643 ;
            int S_OF_TAC = 644 ;
            int relevant_evt_OF_TAC = 645 ;
            int waiting_for_rep_OF_TAC = 646 ;
            int failF_OF_TAC = 647 ;
            int init_OF_TAC = 648 ;
            int required_OF_TA_lost = 649 ;
            int already_S_OF_TA_lost = 650 ;
            int S_OF_TA_lost = 651 ;
            int relevant_evt_OF_TA_lost = 652 ;
            int required_OF_TP = 653 ;
            int already_S_OF_TP = 654 ;
            int S_OF_TP = 655 ;
            int relevant_evt_OF_TP = 656 ;
            int waiting_for_rep_OF_TP = 657 ;
            int failF_OF_TP = 658 ;
            int init_OF_TP = 659 ;
            int required_OF_TS = 660 ;
            int already_S_OF_TS = 661 ;
            int S_OF_TS = 662 ;
            int relevant_evt_OF_TS = 663 ;
            int waiting_for_rep_OF_TS = 664 ;
            int failF_OF_TS = 665 ;
            int init_OF_TS = 666 ;
            int required_OF_TS_lost = 667 ;
            int already_S_OF_TS_lost = 668 ;
            int S_OF_TS_lost = 669 ;
            int relevant_evt_OF_TS_lost = 670 ;
            int required_OF_TS_not_fed = 671 ;
            int already_S_OF_TS_not_fed = 672 ;
            int S_OF_TS_not_fed = 673 ;
            int relevant_evt_OF_TS_not_fed = 674 ;
            int required_OF_TUA1 = 675 ;
            int already_S_OF_TUA1 = 676 ;
            int S_OF_TUA1 = 677 ;
            int relevant_evt_OF_TUA1 = 678 ;
            int waiting_for_rep_OF_TUA1 = 679 ;
            int failF_OF_TUA1 = 680 ;
            int init_OF_TUA1 = 681 ;
            int required_OF_TUA2 = 682 ;
            int already_S_OF_TUA2 = 683 ;
            int S_OF_TUA2 = 684 ;
            int relevant_evt_OF_TUA2 = 685 ;
            int waiting_for_rep_OF_TUA2 = 686 ;
            int failF_OF_TUA2 = 687 ;
            int init_OF_TUA2 = 688 ;
            int required_OF_TUB1 = 689 ;
            int already_S_OF_TUB1 = 690 ;
            int S_OF_TUB1 = 691 ;
            int relevant_evt_OF_TUB1 = 692 ;
            int waiting_for_rep_OF_TUB1 = 693 ;
            int failF_OF_TUB1 = 694 ;
            int init_OF_TUB1 = 695 ;
            int required_OF_TUB2 = 696 ;
            int already_S_OF_TUB2 = 697 ;
            int S_OF_TUB2 = 698 ;
            int relevant_evt_OF_TUB2 = 699 ;
            int waiting_for_rep_OF_TUB2 = 700 ;
            int failF_OF_TUB2 = 701 ;
            int init_OF_TUB2 = 702 ;
            int required_OF_UE_1 = 703 ;
            int already_S_OF_UE_1 = 704 ;
            int S_OF_UE_1 = 705 ;
            int relevant_evt_OF_UE_1 = 706 ;
            int required_OF_UNIT = 707 ;
            int already_S_OF_UNIT = 708 ;
            int S_OF_UNIT = 709 ;
            int relevant_evt_OF_UNIT = 710 ;
            int waiting_for_rep_OF_UNIT = 711 ;
            int failF_OF_UNIT = 712 ;
            int init_OF_UNIT = 713 ;
            int required_OF_in_function_house = 714 ;
            int already_S_OF_in_function_house = 715 ;
            int S_OF_in_function_house = 716 ;
            int relevant_evt_OF_in_function_house = 717 ;
            int waiting_for_rep_OF_in_function_house = 718 ;
            int failF_OF_in_function_house = 719 ;
            int init_OF_in_function_house = 720 ;
            int required_OF_loss_of_houseload_operation = 721 ;
            int already_S_OF_loss_of_houseload_operation = 722 ;
            int S_OF_loss_of_houseload_operation = 723 ;
            int relevant_evt_OF_loss_of_houseload_operation = 724 ;
            int required_OF_demand_CCF_DG = 725 ;
            int already_S_OF_demand_CCF_DG = 726 ;
            int S_OF_demand_CCF_DG = 727 ;
            int relevant_evt_OF_demand_CCF_DG = 728 ;
            int waiting_for_rep_OF_demand_CCF_DG = 729 ;
            int failI_OF_demand_CCF_DG = 730 ;
            int to_be_fired_OF_demand_CCF_DG = 731 ;
            int already_standby_OF_demand_CCF_DG = 732 ;
            int already_required_OF_demand_CCF_DG = 733 ;
            int required_OF_demand_DGA = 734 ;
            int already_S_OF_demand_DGA = 735 ;
            int S_OF_demand_DGA = 736 ;
            int relevant_evt_OF_demand_DGA = 737 ;
            int waiting_for_rep_OF_demand_DGA = 738 ;
            int failI_OF_demand_DGA = 739 ;
            int to_be_fired_OF_demand_DGA = 740 ;
            int already_standby_OF_demand_DGA = 741 ;
            int already_required_OF_demand_DGA = 742 ;
            int required_OF_demand_DGB = 743 ;
            int already_S_OF_demand_DGB = 744 ;
            int S_OF_demand_DGB = 745 ;
            int relevant_evt_OF_demand_DGB = 746 ;
            int waiting_for_rep_OF_demand_DGB = 747 ;
            int failI_OF_demand_DGB = 748 ;
            int to_be_fired_OF_demand_DGB = 749 ;
            int already_standby_OF_demand_DGB = 750 ;
            int already_required_OF_demand_DGB = 751 ;
            int required_OF_demand_TAC = 752 ;
            int already_S_OF_demand_TAC = 753 ;
            int S_OF_demand_TAC = 754 ;
            int relevant_evt_OF_demand_TAC = 755 ;
            int waiting_for_rep_OF_demand_TAC = 756 ;
            int failI_OF_demand_TAC = 757 ;
            int to_be_fired_OF_demand_TAC = 758 ;
            int already_standby_OF_demand_TAC = 759 ;
            int already_required_OF_demand_TAC = 760 ;
            int required_OF_loss_of_supply_by_DGA = 761 ;
            int already_S_OF_loss_of_supply_by_DGA = 762 ;
            int S_OF_loss_of_supply_by_DGA = 763 ;
            int relevant_evt_OF_loss_of_supply_by_DGA = 764 ;
            int required_OF_loss_of_supply_by_DGA_and_TAC = 765 ;
            int already_S_OF_loss_of_supply_by_DGA_and_TAC = 766 ;
            int S_OF_loss_of_supply_by_DGA_and_TAC = 767 ;
            int relevant_evt_OF_loss_of_supply_by_DGA_and_TAC = 768 ;
            int required_OF_loss_of_supply_by_DGB = 769 ;
            int already_S_OF_loss_of_supply_by_DGB = 770 ;
            int S_OF_loss_of_supply_by_DGB = 771 ;
            int relevant_evt_OF_loss_of_supply_by_DGB = 772 ;
            int required_OF_loss_of_supply_by_GEV = 773 ;
            int already_S_OF_loss_of_supply_by_GEV = 774 ;
            int S_OF_loss_of_supply_by_GEV = 775 ;
            int relevant_evt_OF_loss_of_supply_by_GEV = 776 ;
            int required_OF_loss_of_supply_by_LGD = 777 ;
            int already_S_OF_loss_of_supply_by_LGD = 778 ;
            int S_OF_loss_of_supply_by_LGD = 779 ;
            int relevant_evt_OF_loss_of_supply_by_LGD = 780 ;
            int required_OF_loss_of_supply_by_LGF = 781 ;
            int already_S_OF_loss_of_supply_by_LGF = 782 ;
            int S_OF_loss_of_supply_by_LGF = 783 ;
            int relevant_evt_OF_loss_of_supply_by_LGF = 784 ;
            int required_OF_loss_of_supply_by_LGR = 785 ;
            int already_S_OF_loss_of_supply_by_LGR = 786 ;
            int S_OF_loss_of_supply_by_LGR = 787 ;
            int relevant_evt_OF_loss_of_supply_by_LGR = 788 ;
            int required_OF_loss_of_supply_by_TA = 789 ;
            int already_S_OF_loss_of_supply_by_TA = 790 ;
            int S_OF_loss_of_supply_by_TA = 791 ;
            int relevant_evt_OF_loss_of_supply_by_TA = 792 ;
            int required_OF_loss_of_supply_by_TA1 = 793 ;
            int already_S_OF_loss_of_supply_by_TA1 = 794 ;
            int S_OF_loss_of_supply_by_TA1 = 795 ;
            int relevant_evt_OF_loss_of_supply_by_TA1 = 796 ;
            int required_OF_loss_of_supply_by_TAC = 797 ;
            int already_S_OF_loss_of_supply_by_TAC = 798 ;
            int S_OF_loss_of_supply_by_TAC = 799 ;
            int relevant_evt_OF_loss_of_supply_by_TAC = 800 ;
            int required_OF_loss_of_supply_by_TS = 801 ;
            int already_S_OF_loss_of_supply_by_TS = 802 ;
            int S_OF_loss_of_supply_by_TS = 803 ;
            int relevant_evt_OF_loss_of_supply_by_TS = 804 ;
            int required_OF_loss_of_supply_by_TS1 = 805 ;
            int already_S_OF_loss_of_supply_by_TS1 = 806 ;
            int S_OF_loss_of_supply_by_TS1 = 807 ;
            int relevant_evt_OF_loss_of_supply_by_TS1 = 808 ;
            int required_OF_loss_of_supply_by_UNIT = 809 ;
            int already_S_OF_loss_of_supply_by_UNIT = 810 ;
            int S_OF_loss_of_supply_by_UNIT = 811 ;
            int relevant_evt_OF_loss_of_supply_by_UNIT = 812 ;
            int required_OF_on_demand_house = 813 ;
            int already_S_OF_on_demand_house = 814 ;
            int S_OF_on_demand_house = 815 ;
            int relevant_evt_OF_on_demand_house = 816 ;
            int waiting_for_rep_OF_on_demand_house = 817 ;
            int failI_OF_on_demand_house = 818 ;
            int to_be_fired_OF_on_demand_house = 819 ;
            int already_standby_OF_on_demand_house = 820 ;
            int already_required_OF_on_demand_house = 821 ;
            int at_work_OF_repair_constraint = 822 ;


            int priority_OF_OPTIONS = 0 ;
            int nb_avail_repairmen_OF_repair_constraint = 1 ;



            /* ---------- DECLARATION OF FUNCTIONS ------------ */
            void init();
            void saveCurrentState();
            void printState();
            void fireOccurrence(int numFire);
            std::vector<std::tuple<int, double, std::string, int>> showFireableOccurrences();
            void runOnceInteractionStep_check_priorities();
            void runOnceInteractionStep_initialization();
            void runOnceInteractionStep_propagate_effect_S();
            void runOnceInteractionStep_propagate_effect_required();
            void runOnceInteractionStep_propagate_leaves();
            void runOnceInteractionStep_tops();
            int compareStates();
            void doReinitialisations();
            void runInteractions();
            void printstatetuple();
            void fireinsttransitiongroup(std::string);
            int_fast64_t stateSize() const;
            bool figaromodelhasinstransitions();
            void runOnceInteractionStep_default_step();
        };
    }
}
