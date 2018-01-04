#ifndef OBJ_H
#define OBJ_H

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "LinearAlg.h"

typedef struct obj{
	vec3 *points;
	vec3 *normals;
	int vertexNumber;
	int size;
	int nsize;
} obj;

obj ObjLoadModel(char *fname);

#endif