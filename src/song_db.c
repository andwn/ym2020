#include <genesis.h>
#include "timer.h"
#include "song_db.h"
#include "resources.h"

const song_db_def song_db[NUM_SONGS] = {
//		{ NULL,     &SPR_YM20, &PAL_YM20, "Silence",                    "N/A" },
		{ BGM_AETA, &SPR_AETA, &PAL_AETA, &TS_AETA, &PAL_T_AETA, "I'm All Ears to Adventure!", "RigidatoMS",       {0,1,55,19}, },
		{ BGM_ANTR, &SPR_ANTR, &PAL_ANTR, &TS_ANTR, &PAL_T_ANTR, "A Night to Remember",        "Tobikomi",         {0,3,21,49}, },
		{ BGM_BAF,  &SPR_BAF,  &PAL_BAF,  &TS_BAF,  &PAL_T_BAF,  "Bolt Action Fighter",        "Raijin",           {0,2, 9,19}, },
		{ BGM_BK,   &SPR_BK,   &PAL_BK,   &TS_BK,   &PAL_T_BK,   "Be Kind",                    "Simon Hutchison",  {0,3,17,49}, },
		{ BGM_BM,   &SPR_BM,   &PAL_BM,   &TS_BM,   &PAL_T_BM,   "Bloody Mary",                "Voido McMerc",     {0,2,46,29}, },
		{ BGM_CTC,  &SPR_CTC,  &PAL_CTC,  &TS_CTC,  &PAL_T_CTC,  "Chippy the Cat 3",           "Laz",              {0,5,47,49}, },
		{ BGM_DS,   &SPR_DS,   &PAL_DS,   &TS_DS,   &PAL_T_DS,   "Dragon Slayer",              "Ten-ichi",         {0,2,48,49}, },
		{ BGM_Haven,&SPR_Haven,&PAL_Haven,&TS_Haven,&PAL_T_Haven,"Haven is a Place on Earth",  "Robyn & Patrickm", {0,2,33,49}, },
		{ BGM_CNV,  &SPR_CNV,  &PAL_CNV,  &TS_CNV,  &PAL_T_CNV,  "Night Vision",               "Catears",          {0,2,39,49}, },
		{ BGM_ML,   &SPR_ML,   &PAL_ML,   &TS_ML,   &PAL_T_ML,   "MiNd-LoCK",                  "179092",           {0,5,24,49}, },
		{ BGM_MR,   &SPR_MR,   &PAL_MR,   &TS_MR,   &PAL_T_MR,   "Mega Racer",                 "SuperJet Spade",   {0,2,41,29}, },
		{ BGM_MV,   &SPR_MV,   &PAL_MV,   &TS_MV,   &PAL_T_MV,   "Maiden Voyage",              "Dya",              {0,4,48,49}, },
		{ BGM_ROD,  &SPR_ROD,  &PAL_ROD,  &TS_ROD,  &PAL_T_ROD,  "Reliquary of Dreams",        "th4 d34d",         {0,4,19,49}, },
		{ BGM_SCA,  &SPR_SCA,  &PAL_SCA,  &TS_SCA,  &PAL_T_SCA,  "Sunken City Awakens",        "zygrunt",          {0,3,27,49}, },
		{ BGM_STTE, &SPR_STTE, &PAL_STTE, &TS_STTE, &PAL_T_STTE, "Straight to the End",        "Cyan",             {0,3,33,29}, },
		{ BGM_Tigon,&SPR_Tigon,&PAL_Tigon,&TS_Tigon,&PAL_T_Tigon,"Tigon",                      "Pulsing",          {0,0,57,29}, },
		{ BGM_Tube, &SPR_Tube, &PAL_Tube, &TS_Tube, &PAL_T_Tube, "Beat 'em up, champ!",        "Biruhan",          {0,1,45,29}, },
		{ BGM_UN,   &SPR_UN,   &PAL_UN,   &TS_UN,   &PAL_T_UN,   "Urban Ninja",                "Jredd",            {0,2,16,49}, },
		{ BGM_VH,   &SPR_VH,   &PAL_VH,   &TS_VH,   &PAL_T_VH,   "Virtua Heaven",              "Nullsleep",        {0,3,56,49}, },
		{ BGM_WTCS, &SPR_WTCS, &PAL_WTCS, &TS_WTCS, &PAL_T_WTCS, "While the City Sleeps",      "Retro Asobi",      {0,1,36,49}, },
};
