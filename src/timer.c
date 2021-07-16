#include <genesis.h>
#include "timer.h"

extern uint16_t progress_index;

uint16_t timer_tick(Timer *t) {
	uint16_t changed = FALSE;
	if(++t->f >= (SYS_isPAL() ? 50 : 60)) {
		if(++t->s >= 60) {
			if(++t->m >= 60) {
				if(t->h == 99) return FALSE;
				t->h++;
				t->m = 0;
			}
			t->s = 0;
		}
		t->f = 0;
		changed = TRUE;
	}
	return changed;
}

void timer_draw(const Timer *t, uint16_t x, uint16_t y) {
	char str[8];
	sprintf(str, "%02hu:%02hu", t->m, t->s);
	VDP_drawText(str, x, y);
}

static uint16_t map[16];

void timer_draw_bar(const Timer *t, const Timer *len, uint16_t x, uint16_t y) {
	uint16_t t_sec = t->m * 60 + t->s;
	uint16_t len_sec = len->m * 60 + len->s;
	if(t_sec > len_sec) t_sec = len_sec;
	uint16_t progress_px = t_sec * 12 * 8 / len_sec;
	uint16_t progress_tile = progress_px / 8;
	for(uint16_t i = 0; i < 12; i++) {
		if(i < progress_tile) {
			map[i] = TILE_ATTR_FULL(PAL0, 1, 0, 0, progress_index+9);
		}
		if(i == progress_tile) {
			map[i] = TILE_ATTR_FULL(PAL0, 1, 0, 0, progress_index+1+(progress_px&7));
		}
		if(i > progress_tile) {
			map[i] = TILE_ATTR_FULL(PAL0, 1, 0, 0, progress_index);
		}
	}
	VDP_setTileMapDataRow(BG_A, map, y, x, 12, DMA_QUEUE);
}

void timer_reset(Timer *t) {
	t->h = t->m = t->s = t->f = 0;
}
