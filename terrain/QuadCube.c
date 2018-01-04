#include "QuadCube.h"

int subdivideFace(quadCube *qc, vec3 start, int divisions, float fdivisions, int index) {
	float offset = 2.0/(fdivisions);
	for(int i = 0; i < divisions; i++) {
		start.x = 1.0;
		for(int j = 0; j < divisions; j++) {
			vec3 face0 = {start.x,   	  start.y,   start.z};
			vec3 face1 = {start.x-offset, start.y-offset, start.z};
			vec3 face2 = {start.x,        start.y-offset, start.z};
			vec3 face3 = {start.x-offset, start.y,   start.z};
	
			if(start.z == -1.0) {
				qc->points[index++] = face2;
				qc->points[index++] = face1;
				qc->points[index++] = face0;
	
				qc->points[index++] = face1;
				qc->points[index++] = face3;
				qc->points[index++] = face0;
			}
			else {
				qc->points[index++] = face0;
				qc->points[index++] = face1;
				qc->points[index++] = face2;
	
				qc->points[index++] = face0;
				qc->points[index++] = face3;
				qc->points[index++] = face1;
			}
			
			start.x = start.x - offset;
		}
		start.y -= offset;
	}
	return index;
}

int subdivideZ(quadCube *qc, vec3 start, int divisions, float fdivisions, int index) {
	float offset = 2.0/(fdivisions);
	for(int i = 0; i < divisions; i++) {
		start.z = 1.0;
		for(int j = 0; j < divisions; j++) {
			vec3 face0 = {start.x, start.y, start.z};
			vec3 face1 = {start.x, start.y-offset, start.z-offset};
			vec3 face2 = {start.x, start.y-offset, start.z};
			vec3 face3 = {start.x, start.y, start.z-offset};
	
			if(start.x == -1.0) {
				qc->points[index++] = face0;
				qc->points[index++] = face1;
				qc->points[index++] = face2;
	
				qc->points[index++] = face0;
				qc->points[index++] = face3;
				qc->points[index++] = face1;
			}
			else {
				qc->points[index++] = face2;
				qc->points[index++] = face1;
				qc->points[index++] = face0;
	
				qc->points[index++] = face1;
				qc->points[index++] = face3;
				qc->points[index++] = face0;
			}
			
			start.z = start.z - offset;
		}
		start.y -= offset;
	}
	return index;
}

int subdivideY(quadCube *qc, vec3 start, int divisions, float fdivisions, int index) {
	float offset = 2.0/(fdivisions);
	for(int i = 0; i < divisions; i++) {
		start.x = 1.0;
		for(int j = 0; j < divisions; j++) {
			vec3 face0 = {start.x, 		  start.y, start.z};
			vec3 face1 = {start.x-offset, start.y, start.z-offset};
			vec3 face2 = {start.x,        start.y, start.z-offset};
			vec3 face3 = {start.x-offset, start.y, start.z};
	
			if(start.y == -1.0) {
				qc->points[index++] = face0;
				qc->points[index++] = face1;
				qc->points[index++] = face2;
	
				qc->points[index++] = face0;
				qc->points[index++] = face3;
				qc->points[index++] = face1;
			}
			else {
				qc->points[index++] = face2;
				qc->points[index++] = face1;
				qc->points[index++] = face0;
	
				qc->points[index++] = face1;
				qc->points[index++] = face3;
				qc->points[index++] = face0;
			}
			
			start.x = start.x - offset;
		}
		start.z -= offset;
	}
	return index;
}

void createCube(int divisions, quadCube *newQuadCube) {
	/*
	a,b,c,d,e,f,g,h
			 (-1,1,1)e-------f(1,1,1)
				   / |	   / |
				 /	 |	 /	 |
	   (-1,1,-1)a-------b(1,1|,-1)
		   (-1,-|1,1)g--|----h(1,-1,1)
				|  /	|   /
				|/		| /
	  (-1,-1,-1)c-------d(1,-1,-1)
	*/
	
	newQuadCube->points = malloc(divisions*divisions*sizeof(vec3)*6*6);
	newQuadCube->normals = malloc(divisions*divisions*sizeof(vec3)*6*6);
	
	int index = 0;
	vec3 start = {1.0, 1.0, -1.0};
	index = subdivideFace(newQuadCube, start, divisions, (float)divisions, index);	
	start.x = 1.0; start.y = 1.0; start.z = 1.0;
	index = subdivideFace(newQuadCube, start, divisions, (float)divisions, index);
	start.x = 1.0; start.y = 1.0; start.z = 1.0;
	index = subdivideZ(newQuadCube, start, divisions, (float)divisions, index);
	start.x = -1.0; start.y = 1.0; start.z = 1.0;
	index = subdivideZ(newQuadCube, start, divisions, (float)divisions, index);
	start.x = 1.0; start.y = -1.0; start.z = 1.0;
	index = subdivideY(newQuadCube, start, divisions, (float)divisions, index);
	start.x = 1.0; start.y = 1.0; start.z = 1.0;
	index = subdivideY(newQuadCube, start, divisions, (float)divisions, index);
	
	newQuadCube->size = index*sizeof(vec3);
	newQuadCube->nsize = index*sizeof(vec3);
	newQuadCube->vertexNumber = index;
	
	for(int i = 0; i < newQuadCube->vertexNumber; i++) {
		//printf("x:%f, y:%f, z:%f\n", newQuadCube.points[i].x, newQuadCube.points[i].y, newQuadCube.points[i].z);
		newQuadCube->points[i] = normalizevec3(newQuadCube->points[i]);
	}
	
	for(int i = 0; i < newQuadCube->vertexNumber; i+=3)
	{
		vec3 one, two;
		
		one.x = newQuadCube->points[i+1].x - newQuadCube->points[i].x;
		one.y = newQuadCube->points[i+1].y - newQuadCube->points[i].y;
		one.z = newQuadCube->points[i+1].z - newQuadCube->points[i].z;
	
		two.x = newQuadCube->points[i+2].x - newQuadCube->points[i+1].x;
		two.y = newQuadCube->points[i+2].y - newQuadCube->points[i+1].y;
		two.z = newQuadCube->points[i+2].z - newQuadCube->points[i+1].z;

		vec3 normal = normalizevec3(crossvec3(one, two));

		newQuadCube->normals[i] = normal; 
		newQuadCube->normals[i+1] = normal;
		newQuadCube->normals[i+2] = normal;
	}
}