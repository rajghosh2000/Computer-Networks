#include <stdio.h>
#define MAX_CLIENTS 100000

struct fileName{
    char fname[10];
};

struct fileName fN[MAX_CLIENTS];

FILE *fp[MAX_CLIENTS];
