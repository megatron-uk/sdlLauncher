// Load a Windows bitmap file from disk
int imageLoadBMP(FILE *log, char *src, struct agnostic_bitmap *bmp);

// Convert an in-memory bitmap from imageLoadBMP to Atari .IMG planar format
int imageBMP2Bitplane(FILE *log, struct agnostic_bitmap *bmp, int free);