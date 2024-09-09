#include "score.h"

#include <stdio.h>

static const char* score_file = "score";

int read_score() {
    FILE* file = fopen(score_file, "rb");

    if (!file) return 0;

    int ret = 0;
    fread(&ret, sizeof(ret), 1, file);

    fclose(file);
    return ret;
}

void write_score(int score) {
    FILE* file = fopen(score_file, "wb");

    if (!file) return;

    fwrite(&score, sizeof(score), 1, file);
    fclose(file);
}