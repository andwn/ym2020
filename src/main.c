#include <genesis.h>
#include "marquee.h"
#include "timer.h"
#include "song_db.h"
#include "resources.h"

#define BACK_INDEX  TILE_USER_INDEX
#define BACKC_INDEX 762
#define TITLE_INDEX 880
#define VIS_INDEX 	1148

/* JOYPAD WRAPPER */

static uint16_t joystate, oldstate;

void joy_update() {
	oldstate = joystate;
	joystate = JOY_readJoypad(0);
}

uint16_t joy_pressed(uint16_t b) {
    return (joystate & b) && !(oldstate & b);
}
uint16_t joy_released(uint16_t b) {
    return !(joystate & b) && (oldstate & b);
}
uint16_t joy_down(uint16_t b) {
    return joystate & b;
}

/* TRACK ORDER */

uint16_t track_repeat;
uint16_t track_order[NUM_SONGS];

static const uint8_t *song_dat(u16 ind) { return song_db[track_order[ind]].data; }
static const SpriteDefinition *song_spr(u16 ind) { return song_db[track_order[ind]].spr; }
static const uint16_t *song_pal(u16 ind) { return song_db[track_order[ind]].pal->data; }
static const TileSet *song_ts(u16 ind) { return song_db[track_order[ind]].ts; }
static const uint16_t *song_pal_t(u16 ind) { return song_db[track_order[ind]].pal_t->data; }
static const char *song_artist(u16 ind) { return song_db[track_order[ind]].artist; }
static const char *song_name(u16 ind) { return song_db[track_order[ind]].name; }
static const Timer *song_len(u16 ind) { return &song_db[track_order[ind]].len; }

void order_linear() {
	for(uint16_t i = 0; i < NUM_SONGS; i++) {
		track_order[i] = i;
	}
}

void order_shuffle() {
	order_linear();
	for(uint16_t i = NUM_SONGS; --i;) {
		uint16_t p = random() % NUM_SONGS;
		uint16_t hold = track_order[i-1];
		track_order[i-1] = track_order[p];
		track_order[p] = hold;
	}
}

uint16_t find_song(uint16_t id) {
	for(uint16_t i = 0; i < NUM_SONGS; i++) {
		if(track_order[i] == id) return i;
	}
	return 0;
}

/* ANIMATION */

#define BOX1_CENTER 26
#define BOX1_LEFT   (BOX1_CENTER - 64)
#define BOX1_RIGHT  (BOX1_CENTER + 64)

#define BOX2_CENTER 256
#define BOX2_LEFT   (BOX2_CENTER - 64)
#define BOX2_RIGHT  (BOX2_CENTER + 64)

uint16_t mode_index, wheel_index, box1_index, box2_index, progress_index, screen_index, belt_index;
uint16_t boxes_moving, wheel_time, belt_x;
int16_t boxes_xspeed;


void screen_refresh(uint16_t track) {
	PAL_setPalette(PAL1, song_pal_t(track), DMA);
	VDP_loadTileSet(song_ts(track), screen_index, DMA);
	uint16_t yy = 0;
	for(uint16_t y = 0; y < 5; y++) {
		for(uint16_t x = 0; x < 4; x++) {
			VDP_setTileMapXY(BG_A, TILE_ATTR_FULL(PAL1,1,0,0,screen_index + yy + x), 15 + x, 9 + y);
		}
		yy += 4;
	}
}

void belt_draw(uint16_t full) {
	uint16_t belt[40];
	uint16_t off = belt_x & 15;
	uint16_t rep[2] = { 
		TILE_ATTR_FULL(PAL0,0,0,0,off > 7 ? belt_index : belt_index + 1 + off),
		TILE_ATTR_FULL(PAL0,0,0,0,off < 8 ? belt_index : belt_index - 7 + off)
	};
	for(uint16_t x = 0; x < 40; x += 2) {
		belt[x] = rep[0];
		belt[x+1] = rep[1];
	}
	if(full) {
		VDP_setTileMapDataRow(BG_A, belt, 22, 0, 40, DMA);
	} else {
		VDP_setTileMapDataRow(BG_A, belt, 16, 0, 12, DMA);
		VDP_setTileMapDataRow(BG_A, belt, 16, 28, 12, DMA);
	}
}

void belt_reset(uint16_t full) {
	belt_x = 0;
	belt_draw(full);
}

void boxes_move_left() {
	boxes_moving = TRUE;
	boxes_xspeed = 4;
	wheel_time = 0;
}

void boxes_move_right() {
	boxes_moving = TRUE;
	boxes_xspeed = -4;
	wheel_time = 0;
}

void boxes_swap(uint16_t track) {
	uint16_t prev = track > 0 ? track - 1 : NUM_SONGS - 1;
	uint16_t next = track < NUM_SONGS - 1 ? track + 1 : 0;
	SPR_loadAllFrames(song_spr(prev), box1_index, NULL);
	SPR_loadAllFrames(song_spr(next), box2_index, NULL);
	//VDP_setPalette(PAL2, song_pal(prev));
	//VDP_setPalette(PAL3, song_pal(next));
	// No dots pls
	DMA_queueDma(DMA_CRAM, (void*)song_pal(prev), PAL2<<5, 16, 2);
	DMA_queueDma(DMA_CRAM, (void*)song_pal(next), PAL3<<5, 16, 2);
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
		wheel_time = 6;
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
	if(SPR_getPositionX(box[0]) == BOX1_CENTER) {
		boxes_moving = FALSE;
		screen_refresh(track);
	} else {
		uint16_t data[4*5];
		for(uint16_t i = 0; i < 4*5; i++) {
			data[i] = random();
		}
		VDP_setTileMapDataRect(BG_A, data, 15, 9, 4, 5, 4, DMA);
	}
	// Move conveyor belts
	belt_x -= boxes_xspeed;
	belt_draw(FALSE);
}

/* VISUALIZER */

#define VIS_X 21
#define VIS_Y 10

uint8_t vis_state[8];

void vis_clear() {
	for(uint16_t i = 0; i < 8; i++) {
		vis_state[i] = 0;
	}
}

void vis_raise_bar(uint8_t ind) {
	vis_state[ind] = 16 << 2;
}

void vis_update() {
	for(uint16_t i = 0; i < 8; i++) {
		if(vis_state[i]) vis_state[i]--;
	}
	// Map vis bar values to tiles. Look at res/ts/vis.png before reading this and it'll make a lot more sense
	for(uint16_t i = 0; i < 4; i++) {
		uint16_t vis_tile[2] = {0};
		uint16_t temp_tile = 0;
		uint8_t l_state = vis_state[i<<1];
		uint8_t r_state = vis_state[(i<<1) + 1];
		temp_tile = (l_state >> 2) * 9;
		if(temp_tile > 8 * 9) {
			temp_tile -= 8 * 9;
			vis_tile[1] += 8 * 9;
		} else {
			vis_tile[1] += temp_tile;
			temp_tile = 0;
		}
		vis_tile[0] = (r_state >> 2);
		if(vis_tile[0] > 8) {
			vis_tile[0] -= 8;
			vis_tile[1] += 8;
		} else {
			vis_tile[1] += vis_tile[0];
			vis_tile[0] = 0;
		}
		vis_tile[0] += temp_tile;
		VDP_setTileMapXY(BG_A, TILE_ATTR_FULL(PAL0,1,0,0,VIS_INDEX + vis_tile[0]), VIS_X + i, VIS_Y);
		VDP_setTileMapXY(BG_A, TILE_ATTR_FULL(PAL0,1,0,0,VIS_INDEX + vis_tile[1]), VIS_X + i, VIS_Y + 1);
	}
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
	XGM_startPlay(song_dat(track));
}

void pause_track() {
	XGM_pausePlay();
	VDP_drawText("  = PAUSED =  ", 24, 2);
}

void resume_track(uint16_t track) {
	(void)track;
	XGM_resumePlay(/*song_dat(track)*/);
}


const uint16_t black_pal[16] = {
	0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000,
	0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000
};
const uint16_t text_pal[16] = {
	0x000, 0x222, 0x444, 0x666, 0x888, 0xAAA, 0xCCC, 0xEEE,
	0x000, 0x222, 0x444, 0x666, 0x888, 0xAAA, 0xCCC, 0xEEE
};

// Title pal cycle, black_pal -> low -> med -> PAL_Title.data
const uint16_t title_lit_low[16] = {
	0x000,
	0x000, 0x000, 0x204, 0x000,
	0x000, 0x000, 0x420, 0x000,
	0x000, 0x000, 0x048, 0x000,
	0x000, 0x000, 0x000
};
const uint16_t title_lit_med[16] = {
	0x000,
	0x000, 0x204, 0x64C, 0x222,
	0x000, 0x420, 0xA84, 0x222,
	0x000, 0x048, 0x4CC, 0x222,
	0x000, 0x000, 0x000
};
#define OFF  black_pal
#define LOW  title_lit_low
#define MED  title_lit_med
#define FULL PAL_Title.data
#define PINK   1
#define BLUE   5
#define YELLOW 9
#define LIGHT(col, lev) DMA_queueDma(DMA_CRAM, (void*)&lev[col], col<<1, 4, 2)

void main_title() {
	uint16_t cycle_ticks = 0;
	SYS_disableInts();
	VDP_setEnable(FALSE);
	// There is a memory leak or fragmentation messing with tileset decompression,
	// So I load them all up front since there is plenty of VRAM
	VDP_loadTileSet(IMG_Back.tileset, BACK_INDEX, DMA);
	VDP_loadTileSet(IMG_BackC.tileset, BACKC_INDEX, DMA);
	VDP_loadTileSet(IMG_Title.tileset, TITLE_INDEX, DMA);
	VDP_setTileMapEx(BG_B, IMG_Title.tilemap, TITLE_INDEX, 0, 0, 0, 0, IMG_Title.tilemap->w, IMG_Title.tilemap->h, DMA);
	//VDP_drawImage(BG_B, &IMG_Title, 0, 0);
	PAL_setPalette(PAL0, black_pal /*PAL_Title.data*/, DMA);
	PAL_setPalette(PAL1, black_pal /*text_pal*/, DMA);
	VDP_drawText("PRESS START", 15, 20);
	VDP_setEnable(TRUE);
	SYS_enableInts();
	
	while(TRUE) {
		joy_update();
		if(joy_pressed(BUTTON_A|BUTTON_B|BUTTON_C|BUTTON_START)) break;
		if(++cycle_ticks >= 360) cycle_ticks = 0;
			switch(cycle_ticks) {
				/* Pink */
				case 6:   LIGHT(PINK, LOW); break; // Smooth on/off 3 times
				case 12:  LIGHT(PINK, MED); break;
				case 18:  LIGHT(PINK, FULL); break;
				case 24:  LIGHT(PINK, MED); break;
				case 28:  LIGHT(PINK, LOW); break;
				case 32:  LIGHT(PINK, MED); break;
				case 38:  LIGHT(PINK, FULL); break;
				//case 44:  LIGHT(PINK, MED); break;
				case 48:  LIGHT(PINK, LOW); break;
				case 52:  LIGHT(PINK, MED); break;
				case 56:  LIGHT(PINK, FULL); break;

				//case 80:  LIGHT(PINK, LOW); break; // Flicker once
				case 82:  LIGHT(PINK, MED); break;
				case 84:  LIGHT(PINK, FULL); break;

				//case 202: LIGHT(PINK, LOW); break; // Flicker at end
				case 204: LIGHT(PINK, MED); break;
				case 206: LIGHT(PINK, FULL); break;

				/* Blue */
				case 23+6:   LIGHT(BLUE, LOW); break; // Smooth on/off 3 times
				case 23+12:  LIGHT(BLUE, MED); break;
				case 23+18:  LIGHT(BLUE, FULL); break;
				case 23+24:  LIGHT(BLUE, MED); break;
				case 23+28:  LIGHT(BLUE, LOW); break;
				case 23+32:  LIGHT(BLUE, MED); break;
				case 23+38:  LIGHT(BLUE, FULL); break;
				//case 23+44:  LIGHT(BLUE, MED); break;
				case 23+48:  LIGHT(BLUE, LOW); break;
				case 23+52:  LIGHT(BLUE, MED); break;
				case 23+56:  LIGHT(BLUE, FULL); break;

				//case 302: LIGHT(BLUE, LOW); break; // Flicker at end
				case 304: LIGHT(BLUE, MED); break;
				case 306: LIGHT(BLUE, FULL); break;

				/* Yellow */
				case 44+6:   LIGHT(YELLOW, LOW); break; // Smooth on/off 3 times
				case 45+12:  LIGHT(YELLOW, MED); break;
				case 45+18:  LIGHT(YELLOW, FULL); break;
				case 45+24:  LIGHT(YELLOW, MED); break;
				case 45+28:  LIGHT(YELLOW, LOW); break;
				case 45+32:  LIGHT(YELLOW, MED); break;
				case 45+38:  LIGHT(YELLOW, FULL); break;
				//case 45+44:  LIGHT(YELLOW, MED); break;
				case 45+48:  LIGHT(YELLOW, LOW); break;
				case 45+52:  LIGHT(YELLOW, MED); break;
				case 45+56:  LIGHT(YELLOW, FULL); break;

				//case 197: LIGHT(YELLOW, LOW); break; // Flicker at end
				case 199: LIGHT(YELLOW, MED); break;
				case 201: LIGHT(YELLOW, FULL); break;

				/* Press Start */
				case 88: PAL_setPalette(PAL1, text_pal, CPU); break;
			}
		
		SYS_doVBlankProcess();
	}
}

void main_player() {
	SYS_disableInts();
	PAL_setPalette(PAL0, black_pal, CPU);
	PAL_setPalette(PAL1, black_pal, CPU);
	VDP_setEnable(FALSE);
	VDP_clearPlane(BG_A, TRUE);
	
	boxes_moving = FALSE;
	uint16_t paused = 0;
	uint16_t track = 0;
	uint16_t prev_track = 0;
	
	VDP_setTileMapEx(BG_B, IMG_Back.tilemap, BACK_INDEX, 0, 0, 0, 0, IMG_Back.tilemap->w, IMG_Back.tilemap->h, DMA);
	mode_index = BACK_INDEX + IMG_Back.tileset->numTile;
	wheel_index = mode_index + TS_Mode.numTile;
	VDP_loadTileSet(&TS_Mode, mode_index, DMA);
	SPR_loadAllFrames(&SPR_Wheel, wheel_index, &box1_index);
	// Set mode to shuffle before loading boxes
	uint16_t mode = 2;
	mode = cycle_mode(mode, mode_index);
	uint16_t prev = track > 0 ? track - 1 : NUM_SONGS - 1;
	uint16_t next = track < NUM_SONGS - 1 ? track + 1 : 0;
	box1_index += wheel_index;
	SPR_loadAllFrames(song_spr(prev), box1_index, &box2_index);
	box2_index += box1_index;
	SPR_loadAllFrames(song_spr(next), box2_index, &progress_index);
	progress_index += box2_index;
	VDP_loadTileSet(&TS_Progress, progress_index, DMA);
	screen_index = progress_index + 10;
	belt_index = screen_index + 20;
	VDP_loadTileSet(&TS_Belt, belt_index, DMA);
	belt_reset(FALSE);
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
		SPR_addSprite(song_spr(prev), BOX1_CENTER, 80, TILE_ATTR(PAL2, 0, 0, 0)),
		SPR_addSprite(song_spr(next), BOX2_CENTER, 80, TILE_ATTR(PAL3, 0, 0, 0)),
	};
	for(uint16_t i = 0; i < 2; i++) {
		SPR_setAutoTileUpload(box[i], FALSE);
		SPR_setVRAMTileIndex(box[i], i ? box2_index : box1_index);
	}
	// Vis bars
	VDP_loadTileSet(&TS_Vis, VIS_INDEX, DMA);
	
	screen_refresh(track);
	Marquee m_playing, m_selected;
	marquee_init(&m_playing, 24, 2, 14);
	marquee_init(&m_selected, 13, 21, 14);
	marquee_set_track(&m_playing, song_artist(track), song_name(track));
	marquee_set_track(&m_selected, song_artist(track), song_name(track));
	Timer playtime;
	timer_reset(&playtime);
	timer_draw(&playtime, 21, 13);
	timer_draw_bar(&playtime, song_len(prev_track), 14, 15);
	const Timer *tracklen = song_len(track);
	timer_draw(tracklen, 21, 14);
	VDP_drawText("/", 20, 14);
	vis_clear();
	// Draw "Now Playing" text in upper right
	VDP_drawText("Now Playing:", 23, 1);
	VDP_drawText("Previous", 2, 8);
	VDP_drawText("Next", 32, 8);
	
	VDP_setEnable(TRUE);
	SYS_enableInts();
	// Get the sprites on screen ASAP or it'll flicker
	SPR_update();
	SYS_doVBlankProcess();
	
	PAL_setPalette(PAL0, PAL_Back.data, DMA);
	//VDP_setPalette(PAL1, text_pal);
	PAL_setPalette(PAL2, song_pal(prev), DMA);
	PAL_setPalette(PAL3, song_pal(next), DMA);
	
	// IT BEGINS
	play_track(track);
	
	while(TRUE) {
		joy_update();
		if(!boxes_moving) { // Don't listen for inputs during box move animation
			if(joy_pressed(BUTTON_LEFT)) {
				if(--track >= NUM_SONGS) track = NUM_SONGS - 1;
				boxes_move_left();
				marquee_set_track(&m_selected, song_artist(track), song_name(track));
			} else if(joy_pressed(BUTTON_RIGHT)) {
				if(++track >= NUM_SONGS) track = 0;
				boxes_move_right();
				marquee_set_track(&m_selected, song_artist(track), song_name(track));
			}
			if(joy_pressed(BUTTON_A)) {
				// Play a new track, or pause/resume the current one
				if(track != prev_track || (!XGM_isPlaying() && !paused)) {
					prev_track = track;
					paused = 0;
					play_track(track);
					marquee_set_track(&m_playing, song_artist(track), song_name(track));
					timer_reset(&playtime);
					timer_draw(&playtime, 21, 13);
					timer_draw_bar(&playtime, song_len(prev_track), 14, 15);
					tracklen = song_len(prev_track);
					timer_draw(tracklen, 21, 14);
				} else if(paused) {
					paused = 0;
					resume_track(track);
					marquee_set_track(&m_playing, song_artist(track), song_name(track));
				} else {
					paused = 1;
					pause_track();
				}
			} else if(joy_pressed(BUTTON_B)) {
				// Stop the track, make sure paused is 0 so next play will start from beginning
				paused = 0;
				XGM_pausePlay();
				marquee_set_track(&m_playing, "N/A", "N/A");
				timer_reset(&playtime);
				timer_draw(&playtime, 21, 13);
				timer_draw_bar(&playtime, song_len(prev_track), 14, 15);
				timer_draw(tracklen, 21, 14);
			} else if(joy_pressed(BUTTON_C)) {
				// Cycle play mode, keep the currently playing track selected when shuffling
				uint16_t old_id = track_order[prev_track];
				mode = cycle_mode(mode, mode_index);
				prev_track = find_song(old_id);
				track = prev_track;
				boxes_swap(prev_track);
				screen_refresh(prev_track);
			} else if(joy_pressed(BUTTON_START)) {
				// Go to credits screen
				XGM_pausePlay();
				SYS_doVBlankProcess();
				break;
			}
		}
		if(!paused && XGM_isPlaying()) {
			const Timer *len = song_len(prev_track);
			if(mode != 1 && ((playtime.m > len->m) || (playtime.m == len->m && playtime.s > len->s)
				|| (playtime.m == len->m && playtime.s == len->s && playtime.f > len->f))) {
				// Track finished playing
				if(track == prev_track) {
					// Scroll automatically if the currently selected track is also the playing track
					if(++track >= NUM_SONGS) track = 0;
					boxes_move_right();
					marquee_set_track(&m_selected, song_artist(track), song_name(track));
				}
				// Change now playing display and reset timer
				if(++prev_track >= NUM_SONGS) prev_track = 0;
				play_track(prev_track);
				marquee_set_track(&m_playing, song_artist(prev_track), song_name(prev_track));
				timer_reset(&playtime);
				timer_draw(&playtime, 21, 13);
				timer_draw_bar(&playtime, song_len(prev_track), 14, 15);
				tracklen = song_len(track);
				timer_draw(tracklen, 21, 14);
			} else if(timer_tick(&playtime)) {
				// Redraw play time once per second
				timer_draw(&playtime, 21, 13);
				timer_draw_bar(&playtime, song_len(prev_track), 14, 15);
			}
			marquee_update(&m_playing);
			if(playtime.f & 1) vis_raise_bar(random() & 7);
		}
		vis_update();
		marquee_update(&m_selected);
		boxes_update(box, wheel, track);
		SPR_update();
		SYS_doVBlankProcess();
	}

	for(uint16_t i = 0; i < 8; i++) SPR_releaseSprite(wheel[i]);
	for(uint16_t i = 0; i < 2; i++) SPR_releaseSprite(box[i]);
	
	SPR_update();
	SYS_doVBlankProcess();
}

void main_credits() {
	SYS_disableInts();
	PAL_setPalette(PAL0, black_pal, CPU);
	PAL_setPalette(PAL1, black_pal, CPU);
	VDP_setEnable(FALSE);
	
	VDP_setTileMapEx(BG_B, IMG_BackC.tilemap, BACKC_INDEX, 0, 0, 0, 0, IMG_Back.tilemap->w, IMG_Back.tilemap->h, DMA);
	VDP_clearPlane(BG_A, TRUE);
	Sprite *wheel[16];
	for(uint16_t i = 0; i < 16; i++) {
		wheel[i] = SPR_addSprite(&SPR_Wheel, 1 + i*20,  184, TILE_ATTR(PAL0, 0, 0, 0));
		SPR_setAutoTileUpload(wheel[i], FALSE);
		SPR_setVRAMTileIndex(wheel[i], wheel_index + (i&1) * 4);
		wheel[i]->frameInd = i&1;
	}
	Sprite *box[4];
	for(uint16_t i = 0; i < 4; i++) {
		box[i] = SPR_addSprite(&SPR_YM20, 40 + i*88, 128, TILE_ATTR(PAL2, 0, 0, 0));
		SPR_setAutoTileUpload(box[i], FALSE);
		SPR_setVRAMTileIndex(box[i], box1_index);
	}
	SPR_loadAllFrames(&SPR_YM20, box1_index, NULL);
	belt_reset(TRUE);
	
	VDP_setEnable(TRUE);
	SYS_enableInts();
	SPR_update();
	SYS_doVBlankProcess();

	VDP_drawText("YM2020", 			17, 2);
	VDP_drawText("Producer", 		2,  4);
	VDP_drawText("Art & Design", 	2,  6);
	VDP_drawText("Programmer", 		2,  8);
	VDP_drawText("Hardware", 		2,  10);
	VDP_drawText("Addt'l Hardware", 2,  12);
	VDP_drawText("   Catskull", 	24, 4);
	VDP_drawText("  Drew Wise", 	24, 6);
	VDP_drawText(" Andy Grind", 	24, 8);
	VDP_drawText("Retro Stage", 	24, 10);
	VDP_drawText(" Orgia Mode", 	24, 12);
	
	PAL_setPalette(PAL0, PAL_Back.data, CPU);
	PAL_setPalette(PAL1, text_pal, CPU);
	PAL_setPalette(PAL2, PAL_YM20.data, CPU);
	
	XGM_startPlay(BGM_Fireworks);

	while(TRUE) {
		joy_update();
		if(belt_x > 10 && joy_pressed(BUTTON_A|BUTTON_B|BUTTON_C|BUTTON_START)) break;

		// Animate wheels
		if(wheel_time) {
			wheel_time--;
		} else {
			for(uint16_t i = 0; i < 16; i++) {
				if(++wheel[i]->frameInd > 3) wheel[i]->frameInd = 0;
				SPR_setVRAMTileIndex(wheel[i], wheel_index + wheel[i]->frameInd * 4);
			}
			wheel_time = 8;
		}
		// Move boxes
		for(uint16_t i = 0; i < 4; i++) {
			int16_t x = SPR_getPositionX(box[i]);
			int16_t y = SPR_getPositionY(box[i]);
			if(x <= -40) {
				SPR_setPosition(box[i], 320, y);
			} else {
				SPR_setPosition(box[i], x - 1, y);
			}
		}
		// Move conveyor belt
		belt_x += 1;
		belt_draw(TRUE);
		
		SPR_update();
		SYS_doVBlankProcess();
	}

	for(uint16_t i = 0; i < 16; i++) SPR_releaseSprite(wheel[i]);
	for(uint16_t i = 0; i < 4; i++) SPR_releaseSprite(box[i]);
	
	SPR_update();
	SYS_doVBlankProcess();
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
