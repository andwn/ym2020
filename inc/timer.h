typedef struct {
	uint8_t h, m, s, f;
} Timer;

extern uint16_t timer_tick(Timer *t);
extern void timer_draw(const Timer *t, uint16_t x, uint16_t y);
extern void timer_draw_bar(const Timer *t, const Timer *len, uint16_t x, uint16_t y);
extern void timer_reset(Timer *t);
