#include <genesis.h>
#include "song_db.h"
#include "resources.h"

const song_db_def song_db[NUM_SONGS] = {
		{ NULL,     &SPR_YM20, &PAL_YM20, "Silence",                    "N/A" },
		{ BGM_AETA, &SPR_AETA, &PAL_AETA, "I'm All Ears to Adventure!", "RigidatoMS" },
		{ BGM_ANTR, &SPR_ANTR, &PAL_ANTR, "A Night to Remember",        "Tobikomi" },
		{ BGM_BAF,  &SPR_BAF,  &PAL_BAF,  "Bolt Action Fighter",        "Raijin" },
		{ BGM_BK,   &SPR_BK,   &PAL_BK,   "Be Kind",                    "Simon Hutchison" },
		{ BGM_BM,   &SPR_BM,   &PAL_BM,   "Bloody Mary",                "Voido McMerc" },
		{ BGM_CTC,  &SPR_CTC,  &PAL_CTC,  "Chippy the Cat 3",           "Laz" },
		{ BGM_DS,   &SPR_DS,   &PAL_DS,   "Dragon Slayer",              "Ten-ichi" },
		{ BGM_Haven,&SPR_Haven,&PAL_Haven,"Haven is a Place on Earth",  "Robyn & Patrickm" },
		{ BGM_CNV,  &SPR_CNV,  &PAL_CNV,  "Night Vision",               "Catears" },
		{ BGM_ML,   &SPR_ML,   &PAL_ML,   "MiNd-LoCK",                  "179092" },
		{ BGM_MR,   &SPR_MR,   &PAL_MR,   "Mega Racer",                 "SuperJet Spade" },
		{ BGM_MV,   &SPR_MV,   &PAL_MV,   "Maiden Voyage",              "Dya" },
		{ BGM_ROD,  &SPR_ROD,  &PAL_ROD,  "Reliquary of Dreams",        "th4 d34d" },
		{ BGM_SCA,  &SPR_SCA,  &PAL_SCA,  "Sunken City Awakens",        "zygrunt" },
		{ BGM_STTE, &SPR_STTE, &PAL_STTE, "Straight to the End",        "Cyan" },
		{ BGM_Tigon,&SPR_Tigon,&PAL_Tigon,"Tigon",                      "Pulsing" },
		{ BGM_Tube, &SPR_Tube, &PAL_Tube, "Beat 'em up, champ!",        "Biruhan" },
		{ BGM_UN,   &SPR_UN,   &PAL_UN,   "Urban Ninja",                "Jredd" },
		{ BGM_VH,   &SPR_VH,   &PAL_VH,   "Virtua Heaven",              "Nullsleep" },
		{ BGM_WTCS, &SPR_WTCS, &PAL_WTCS, "While the City Sleeps",      "Retro Asobi" },
};
