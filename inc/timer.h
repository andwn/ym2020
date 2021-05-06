typedef struct {
	uint8_t h, m, s, f;
} Timer;

extern void timer_tick(Timer *t);
extern void timer_reset(Timer *t);
