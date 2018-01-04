#ifndef QUADCUBE_H
#define QUADCUBE_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include "LinearAlg.h"

typedef struct quadCube {
	int size;
	int nsize;
	vec3 *points;
	vec3 *normals;
	int vertexNumber;
} quadCube;

void createCube(int divisions, quadCube *newQuadCube);

#endif