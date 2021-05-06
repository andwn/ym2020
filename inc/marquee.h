typedef struct {
	uint16_t xpos, ypos;
	uint16_t index, ticks;
	uint16_t full_len;
	uint16_t disp_len;
	char full_text[65];
	char disp_text[41];
} Marquee;

extern void marquee_refresh(Marquee *m);
extern void marquee_update(Marquee *m);
extern void marquee_set_track(Marquee *m, uint16_t track);
extern void marquee_init(Marquee *m, uint16_t x, uint16_t y, uint16_t len, uint16_t track);
