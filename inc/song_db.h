#define NUM_SONGS 20

typedef struct {
	const uint8_t *data;
	const SpriteDefinition *spr;
	const Palette *pal;
	const TileSet *ts;
	const Palette *pal_t;
	char name[32];
	char artist[24];
	Timer len;
} song_db_def;

extern const song_db_def song_db[NUM_SONGS];
