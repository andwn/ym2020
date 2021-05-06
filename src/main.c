#include <genesis.h>
#include "marquee.h"
#include "song_db.h"
#include "timer.h"
#include "resources.h"

/* JOYPAD WRAPPER */

static uint16_t joystate, oldstate;

void joy_update() {
	oldstate = joystate;
	joystate = JOY_readJoypad(0);
}

uint16_t joy_pressed(uint16_t b) {
    return (joystate & b) && (~oldstate & b);
}
uint16_t joy_released(uint16_t b) {
    return (~joystate & b) && (oldstate & b);
}
uint16_t joy_down(uint16_t b) {
    return joystate & b;
}

/* TRACK ORDER */

uint16_t track_repeat;
uint16_t track_order[NUM_SONGS];

void order_linear() {
	for(uint16_t i = 0; i < NUM_SONGS; i++) {
		track_order[i] = i;
	}
}

void order_shuffle() {
	for(uint16_t i = NUM_SONGS; --i;) {
		uint16_t p = random() % NUM_SONGS;
		uint16_t hold = track_order[i-1];
		track_order[i-1] = track_order[p];
		track_order[p] = hold;
	}
}

/* ANIMATION */

#define BOX1_CENTER 26
#define BOX1_LEFT   (BOX1_CENTER - 64)
#define BOX1_RIGHT  (BOX1_CENTER + 64)

#define BOX2_CENTER 256
#define BOX2_LEFT   (BOX2_CENTER - 64)
#define BOX2_RIGHT  (BOX2_CENTER + 64)

uint16_t mode_index, wheel_index, box1_index, box2_index;
uint16_t boxes_moving, wheel_time;
int16_t boxes_xspeed;

void boxes_move_left() {
	boxes_moving = TRUE;
	boxes_xspeed = 2;
	wheel_time = 0;
}

void boxes_move_right() {
	boxes_moving = TRUE;
	boxes_xspeed = -2;
	wheel_time = 0;
}

void boxes_swap(uint16_t track) {
	uint16_t prev = track > 0 ? track - 1 : NUM_SONGS - 1;
	uint16_t next = track < NUM_SONGS - 1 ? track + 1 : 0;
	SPR_loadAllFrames(song_db[prev].spr, box1_index, NULL);
	SPR_loadAllFrames(song_db[next].spr, box2_index, NULL);
	VDP_setPalette(PAL2, song_db[prev].pal->data);
	VDP_setPalette(PAL3, song_db[next].pal->data);
}

void boxes_update(Sprite *box[], Sprite *wheel[], uint16_t track) {
	if(!boxes_moving) return;
	// Animate wheels
	if(wheel_time) {
		wheel_time--;
	} else {
		for(uint16_t i = 0; i < 8; i++) {
			if(boxes_xspeed > 0) {
				if(--wheel[i]->frameInd < 0) wheel[i]->frameInd = 3;
			} else {
				if(++wheel[i]->frameInd > 3) wheel[i]->frameInd = 0;
			}
			SPR_setVRAMTileIndex(wheel[i], wheel_index + wheel[i]->frameInd * 4);
		}
		wheel_time = 8;
	}
	// Move boxes
	for(uint16_t i = 0; i < 2; i++) {
		SPR_setPosition(box[i], SPR_getPositionX(box[i]) + boxes_xspeed, SPR_getPositionY(box[i]));
	}
	// Box reached monitor? Load new ones
	if(boxes_xspeed > 0) {
		if(SPR_getPositionX(box[0]) >= BOX1_RIGHT) {
			SPR_setPosition(box[0], BOX1_LEFT, SPR_getPositionY(box[0]));
			SPR_setPosition(box[1], BOX2_LEFT, SPR_getPositionY(box[1]));
			boxes_swap(track);
			
		}
	} else {
		if(SPR_getPositionX(box[0]) <= BOX1_LEFT) {
			SPR_setPosition(box[0], BOX1_RIGHT, SPR_getPositionY(box[0]));
			SPR_setPosition(box[1], BOX2_RIGHT, SPR_getPositionY(box[1]));
			boxes_swap(track);
		}
	}
	// Box reached center? Stop moving
	if(SPR_getPositionX(box[0]) == BOX1_CENTER) boxes_moving = FALSE;
}

/* MAIN */

uint16_t cycle_mode(uint16_t mode, uint16_t mode_index) {
	if(++mode > 2) mode = 0;
	VDP_setTileMapXY(BG_B, TILE_ATTR_FULL(PAL0,0,0,0,mode_index   + (mode<<1)), 33, 21);
	VDP_setTileMapXY(BG_B, TILE_ATTR_FULL(PAL0,0,0,0,mode_index+1 + (mode<<1)), 34, 21);
	switch(mode) {
		case 0: // Shuffle
		order_shuffle();
		track_repeat = FALSE;
		break;
		case 1: // Repeat
		order_linear();
		track_repeat = TRUE;
		break;
		case 2: // Linear
		order_linear();
		track_repeat = FALSE;
		break;
	}
	return mode;
}

void play_track(uint16_t track) {
	if(song_db[track].data) {
		XGM_startPlay(song_db[track].data);
	} else {
		XGM_pausePlay();
	}
}

void pause_track() {
	XGM_pausePlay();
	VDP_drawText("  = PAUSED =  ", 24, 2);
}


const uint16_t black_pal[16] = {
		0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000,
		0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000
};
const uint16_t text_pal[16] = {
		0x000, 0x222, 0x444, 0x666, 0x888, 0xAAA, 0xCCC, 0xEEE,
		0x000, 0x222, 0x444, 0x666, 0x888, 0xAAA, 0xCCC, 0xEEE
};
const uint16_t title_pal[16] = {
		0x000, 0x226, 0x222, 0x440, 0x248, 0x64E, 0xCA4, 0x2CE,
		0xEEE, 0xEEE, 0xEEE, 0xEEE, 0xEEE, 0xEEE, 0xEEE, 0xEEE
};
const uint16_t back_pal[16] = {
		0x000, 0x228, 0x442, 0x842, 0x448, 0x64C, 0x664, 0x866,
		0xA82, 0x68A, 0x8A6, 0xCAA, 0x2AE, 0xCCE, 0xEEA, 0xAEE
};

void main_title() {
	SYS_disableInts();
	VDP_drawImage(BG_B, &IMG_Title, 0, 0);
	VDP_setPalette(PAL0, PAL_Title.data);
	VDP_setPalette(PAL1, text_pal);
	SYS_enableInts();
	
	while(TRUE) {
		joy_update();
		if(joy_pressed(BUTTON_A|BUTTON_B|BUTTON_C|BUTTON_START)) break;
		SYS_doVBlankProcess();
	}
}

void main_player() {
	SYS_disableInts();
	VDP_setPalette(PAL0, black_pal);
	VDP_setPalette(PAL1, black_pal);
	curTileInd = TILE_USERINDEX;
	// Load graphics
	mode_index = TILE_USERINDEX + IMG_Back.tileset->numTile;
	wheel_index = mode_index + TS_Mode.numTile;
	VDP_drawImage(BG_B, &IMG_Back, 0, 0);
	VDP_loadTileSet(&TS_Mode, mode_index, DMA);
	SPR_loadAllFrames(&SPR_Wheel, wheel_index, &box1_index);
	box1_index += wheel_index;
	SPR_loadAllFrames(&SPR_WTCS, box1_index, &box2_index);
	box2_index += box1_index;
	SPR_loadAllFrames(&SPR_AETA, box2_index, NULL);
	// Sprites for wheels
	Sprite *wheel[8] = {
		SPR_addSprite(&SPR_Wheel, 1,  136, TILE_ATTR(PAL0, 0, 0, 0)),
		SPR_addSprite(&SPR_Wheel, 21, 136, TILE_ATTR(PAL0, 0, 0, 0)),
		SPR_addSprite(&SPR_Wheel, 41, 136, TILE_ATTR(PAL0, 0, 0, 0)),
		SPR_addSprite(&SPR_Wheel, 61, 136, TILE_ATTR(PAL0, 0, 0, 0)),

		SPR_addSprite(&SPR_Wheel, 284, 136, TILE_ATTR(PAL0, 0, 0, 0)),
		SPR_addSprite(&SPR_Wheel, 264, 136, TILE_ATTR(PAL0, 0, 0, 0)),
		SPR_addSprite(&SPR_Wheel, 244, 136, TILE_ATTR(PAL0, 0, 0, 0)),
		SPR_addSprite(&SPR_Wheel, 304, 136, TILE_ATTR(PAL0, 0, 0, 0)),
	};
	for(uint16_t i = 0; i < 8; i++) {
		SPR_setAutoTileUpload(wheel[i], FALSE);
		SPR_setVRAMTileIndex(wheel[i], wheel_index + (i&1) * 4);
		wheel[i]->frameInd = i&1;
	}
	// Sprites for boxes
	Sprite *box[2] = {
		SPR_addSprite(&SPR_WTCS, BOX1_CENTER, 80, TILE_ATTR(PAL2, 0, 0, 0)),
		SPR_addSprite(&SPR_AETA, BOX2_CENTER, 80, TILE_ATTR(PAL3, 0, 0, 0)),
	};
	for(uint16_t i = 0; i < 2; i++) {
		SPR_setAutoTileUpload(box[i], FALSE);
		SPR_setVRAMTileIndex(box[i], i ? box2_index : box1_index);
	}
	// Load palettes
	VDP_setPalette(PAL0, PAL_Back.data);
	VDP_setPalette(PAL1, text_pal);
	VDP_setPalette(PAL2, PAL_WTCS.data);
	VDP_setPalette(PAL3, PAL_AETA.data);
	

	// Default mode repeat, set silence track, fill in marquees
	boxes_moving = FALSE;
	uint16_t paused = 0;
	uint16_t mode = 0;
	mode = cycle_mode(mode, mode_index);
	uint16_t track = 0;
	uint16_t prev_track = 0;
	Marquee m_playing, m_selected;
	marquee_init(&m_playing, 24, 2, 14, track);
	marquee_init(&m_selected, 13, 21, 14, track);
	Timer playtime;
	timer_reset(&playtime);
	// Draw "Now Playing" text in upper right
	VDP_drawText("Now Playing:", 23, 1);
	// IT BEGINS
	play_track(track);
	SYS_enableInts();
	
	while(TRUE) {
		joy_update();
		if(!boxes_moving) {
			if(joy_pressed(BUTTON_LEFT)) {
				if(--track >= NUM_SONGS) track = NUM_SONGS - 1;
				boxes_move_left();
				marquee_set_track(&m_selected, track);
			} else if(joy_pressed(BUTTON_RIGHT)) {
				if(++track >= NUM_SONGS) track = 0;
				boxes_move_right();
				marquee_set_track(&m_selected, track);
			}
			if(joy_pressed(BUTTON_A)) {
				if(track != prev_track) {
					prev_track = track;
					paused = 0;
					play_track(track);
					marquee_set_track(&m_playing, track);
					timer_reset(&playtime);
				} else if(paused) {
					paused = 0;
					play_track(track);
					marquee_set_track(&m_playing, track);
				} else {
					paused = 1;
					pause_track();
				}
			} else if(joy_pressed(BUTTON_B)) {
				play_track(0);
				marquee_set_track(&m_playing, 0);
			} else if(joy_pressed(BUTTON_C)) {
				mode = cycle_mode(mode, mode_index);
			}
		}
		if(!paused) {
			marquee_update(&m_playing);
			timer_tick(&playtime);
		}
		marquee_update(&m_selected);
		boxes_update(box, wheel, track);
		SPR_update();
		SYS_doVBlankProcess();
	}

	for(uint16_t i = 0; i < 8; i++) SPR_releaseSprite(wheel[i]);
}

void main_credits() {

}

int main() {
    SPR_init();
	VDP_setTextPalette(PAL1);

	main_title();
	while(1) {
		main_player();
		main_credits();
	}
}
