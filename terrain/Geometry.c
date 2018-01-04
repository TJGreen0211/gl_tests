#include "Geometry.h"

const float PI = 3.14159265359;
const float degToRad = PI / 180.0;

ring createRing(int numDivides, float innerRad, float outerRad) {
	ring newRing;
	int index = 0;
	
	newRing.vertexNumber = 360*6;
	newRing.size = newRing.vertexNumber*sizeof(vec3);
	newRing.nsize = newRing.vertexNumber*sizeof(vec3);
	newRing.texsize = newRing.vertexNumber*sizeof(vec2);
	newRing.points = malloc(newRing.size);
	newRing.normals = malloc(newRing.nsize);
	newRing.texCoords = malloc(newRing.texsize);
	vec3 one, two, normal;
	
	for(int i = 0; i < 360; i++) {
		float deg = i * degToRad;
		newRing.points[index].x = cos(deg)*outerRad;
		newRing.points[index].y = sin(deg)*outerRad;
		newRing.points[index].z = 0.0;
		newRing.texCoords[index].x = 1.0;
		newRing.texCoords[index].y = atan(newRing.points[index].y/newRing.points[index].x);
		
		newRing.points[index+1].x = cos(deg)*innerRad;
		newRing.points[index+1].y = sin(deg)*innerRad;
		newRing.points[index+1].z = 0.0;
		newRing.texCoords[index+1].x = 0.0;
		newRing.texCoords[index+1].y = atan(newRing.points[index+1].y/newRing.points[index+1].x);
		
		newRing.points[index+2].x = cos((i+1) * degToRad)*outerRad;
		newRing.points[index+2].y = sin((i+1) * degToRad)*outerRad;
		newRing.points[index+2].z = 0.0;
		newRing.texCoords[index+2].x = 1.0;
		newRing.texCoords[index+2].y = atan(newRing.points[index+2].y/newRing.points[index+2].x);
		
		one.x = newRing.points[index+1].x - newRing.points[index].x;
		one.y = newRing.points[index+1].y - newRing.points[index].y;
		one.z = newRing.points[index+1].z - newRing.points[index].z;
	
		two.x = newRing.points[index+2].x - newRing.points[index+1].x;
		two.y = newRing.points[index+2].y - newRing.points[index+1].y;
		two.z = newRing.points[index+2].z - newRing.points[index+1].z;
		
		normal = normalizevec3(crossvec3(one, two));
		
		newRing.normals[index] = normal;
		newRing.normals[index+1] = normal;
		newRing.normals[index+2] = normal;
		
		newRing.points[index+3].x = cos((i+1) * degToRad)*outerRad;
		newRing.points[index+3].y = sin((i+1) * degToRad)*outerRad;
		newRing.points[index+3].z = 0.0;
		newRing.texCoords[index+3].x = 1.0;
		newRing.texCoords[index+3].y = atan(newRing.points[index+3].y/newRing.points[index+3].x);
		
		newRing.points[index+4].x = cos((i) * degToRad)*innerRad;
		newRing.points[index+4].y = sin((i) * degToRad)*innerRad;
		newRing.points[index+4].z = 0.0;
		newRing.texCoords[index+4].x = 0.0;
		newRing.texCoords[index+4].y = atan(newRing.points[index+4].y/newRing.points[index+4].x);
		
		newRing.points[index+5].x = cos((i+1) * degToRad)*innerRad;
		newRing.points[index+5].y = sin((i+1) * degToRad)*innerRad;
		newRing.points[index+5].z = 0.0;
		newRing.texCoords[index+5].x = 0.0;
		newRing.texCoords[index+5].y = atan(newRing.points[index+5].y/newRing.points[index+5].x);
		
		one.x = newRing.points[index+1].x - newRing.points[index].x;
		one.y = newRing.points[index+1].y - newRing.points[index].y;
		one.z = newRing.points[index+1].z - newRing.points[index].z;
	
		two.x = newRing.points[index+2].x - newRing.points[index+1].x;
		two.y = newRing.points[index+2].y - newRing.points[index+1].y;
		two.z = newRing.points[index+2].z - newRing.points[index+1].z;
		
		normal = normalizevec3(crossvec3(one, two));
		
		newRing.normals[index+3] = normal;
		newRing.normals[index+4] = normal;
		newRing.normals[index+5] = normal;
		
		//printf("inner: %f, %f, %f\n", newRing.points[index].x, newRing.points[index].y, newRing.points[index].z);
		//printf("outer: %f, %f, %f\n", newRing.points[index+1].x, newRing.points[index+1].y, newRing.points[index+1].z);
		
		index += 6;
	}
	
	return newRing;
}


vec3 *generateTangents(int vertexNumber, vec3 *points, vec3 *tangent)
{	
	vec3 edge1, edge2, deltaUV1, deltaUV2;
	
	for(int i = 0; i < vertexNumber; i+=3)
	{
		edge1.x = points[i+1].x - points[i].x;
		edge1.y = points[i+1].y - points[i].y;
		edge1.z = points[i+1].z - points[i].z;
		edge2.x = points[i+2].x - points[i].x;
		edge2.y = points[i+2].y - points[i].y;
		edge2.z = points[i+2].z - points[i].z;
		
		deltaUV1.x = points[i+1].x - points[i].x;
		deltaUV1.y = points[i+1].y - points[i].y;
		deltaUV2.x = points[i+2].x - points[i].x;
		deltaUV2.y = points[i+2].y - points[i].y;
		
		float f = 1.0 / (deltaUV1.x * deltaUV2.y - deltaUV2.x * deltaUV1.y);
		tangent[i].x = f  * (deltaUV2.y * edge1.x - deltaUV1.y * edge2.x);
		tangent[i].y = f  * (deltaUV2.y * edge1.y - deltaUV1.y * edge2.y);
		tangent[i].z = f  * (deltaUV2.y * edge1.z - deltaUV1.y * edge2.z);
		tangent[i] = normalizevec3(tangent[i]);
		for(int j = i; j < i+3; j++)
			tangent[j] = tangent[i];
	}	
	return tangent;	
}

vec3 *generateSmoothNormals(vec3 vna[], vec3 *vertices, vec3 *normals, int size) {
	vec3 vn;
	for(int i = 0; i < size; i++) {
		vec3 tempvn = {0.0, 0.0, 0.0};
		vn = vertices[i];
		for(int j = 0; j < size; j++) {
			if(vn.x == vertices[j].x && vn.y == vertices[j].y && vn.z == vertices[j].z) {
				tempvn = plusequalvec3(tempvn, normals[j]);
			}
		}
		vna[i] = normalizevec3(tempvn);
	}
	return vna;
}
