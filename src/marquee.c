#include <genesis.h>
#include "marquee.h"
#include "timer.h"
#include "song_db.h"

void marquee_refresh(Marquee *m) {
	for(uint16_t i = 0; i < m->disp_len; i++) {
		uint16_t j = m->index + i;
		if(j >= m->full_len) j -= m->full_len;
		m->disp_text[i] = m->full_text[j];
	}
	m->disp_text[m->disp_len] = 0;
	VDP_drawText(m->disp_text, m->xpos, m->ypos);
}

void marquee_update(Marquee *m) {
	if(--m->ticks == 0) {
		if(++m->index >= m->full_len) m->index = 0;
		m->ticks = 30;
		marquee_refresh(m);
	}
}

void marquee_set_track(Marquee *m, const char *artist, const char *name) {
	m->index = 0;
	m->ticks = 60;
	sprintf(m->full_text, "%s - %s  ", artist, name);
	m->full_len = strlen(m->full_text);
	marquee_refresh(m);
}

void marquee_init(Marquee *m, uint16_t x, uint16_t y, uint16_t len) {
	m->xpos = x;
	m->ypos = y;
	m->disp_len = len;
	//marquee_set_track(m, track);
}
