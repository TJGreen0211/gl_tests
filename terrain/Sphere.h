#ifndef SPHERE_H
#define SPHERE_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include "LinearAlg.h"

typedef struct sphere {
	int size;
	int nsize;
	vec3 *points;
	vec3 *normals;
	int vertexNumber;
} sphere;

sphere tetrahedron(int count, sphere *s);

#endif