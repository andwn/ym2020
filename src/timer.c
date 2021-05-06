#include <genesis.h>
#include "timer.h"

void timer_tick(Timer *t) {
	if(++t->f >= (SYS_isPAL() ? 50 : 60)) {
		if(++t->s >= 60) {
			if(++t->m >= 60) {
				if(t->h == 99) return;
				t->h++;
				t->m = 0;
			}
			t->s = 0;
		}
		t->f = 0;
	}
}

void timer_reset(Timer *t) {
	t->h = t->m = t->s = t->f = 0;
}
