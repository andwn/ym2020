#define NUM_SONGS 21

typedef struct {
	const uint8_t *data;
	const SpriteDefinition *spr;
	const Palette *pal;
	char name[32];
	char artist[24];
} song_db_def;

extern const song_db_def song_db[NUM_SONGS];
