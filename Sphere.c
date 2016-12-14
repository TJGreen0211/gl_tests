#include "Sphere.h"
int NumTimesToSubdivide;
int NumTriangles;
int NumVertices;

vec3 *triPoints;
vec3 *triNorms;
vec2 *tex;

sphere newSphere;

int Index = 0;
void triangle(vec3 a, vec3 b, vec3 c)
{
	vec3 one, two;
	one.x = b.x - a.x;
	one.y = b.y - a.y;
	one.z = b.z - a.z;
	
	two.x = c.x - b.x;
	two.y = c.y - b.y;
	two.z = c.z - b.z;

	vec3 normal = normalizevec3(crossvec3(one, two));
	triNorms[Index] = normal; triPoints[Index] = a; Index++;
	triNorms[Index] = normal; triPoints[Index] = b; Index++;
	triNorms[Index] = normal; triPoints[Index] = c; Index++;
}

vec3 unitCircle(vec3 p)
{
	float length = p.x*p.x + p.y*p.y + p.z*p.z;
	vec3 t = {0.0, 0.0, 0.0};
	if(length > 0)
	{
		t.x = p.x / sqrt(length);
		t.y = p.y / sqrt(length);
		t.z = p.z / sqrt(length);
	}
	return t;
}

void divideTriangle(vec3 a, vec3 b, vec3 c, int count)
{
	if(count > 0) {
		vec3 v1 = unitCircle(addvec3(a, b));
		vec3 v2 = unitCircle(addvec3(a, c));
		vec3 v3 = unitCircle(addvec3(b, c));
		divideTriangle(  a, v1, v2, count - 1 );
        divideTriangle(  c, v2, v3, count - 1 );
        divideTriangle(  b, v3, v1, count - 1 );
        divideTriangle( v1, v3, v2, count - 1 );
	}
	else {
		triangle(a, b, c);
	}
}

sphere tetrahedron(int count)
{
	NumTimesToSubdivide = count;
	NumTriangles = pow(4, count+1);
	NumVertices = 3 * NumTriangles;
	
	int mallocVertSize = NumVertices*sizeof(triPoints[0]);
	int mallocNormSize = NumVertices*sizeof(triNorms[0]);
	
	triPoints = malloc(mallocVertSize);
	triNorms = malloc(mallocNormSize);
	
	vec3 v[4] = {
		{ 0.0, 0.0, 1.0 },
		{ 0.0, 0.942809, -0.333333},
		{ -0.816497, -0.471405, -0.333333 },
		{ 0.816497, -0.471405, -0.333333 }
    };
    
    divideTriangle( v[0], v[1], v[2], count );
    divideTriangle( v[3], v[2], v[1], count );
    divideTriangle( v[0], v[3], v[1], count );
    divideTriangle( v[0], v[2], v[3], count );
    
    newSphere.points = malloc(mallocVertSize);
    newSphere.normals = malloc(mallocNormSize);
    newSphere.vertexNumber = NumVertices;
    newSphere.size = mallocVertSize;
    newSphere.nsize = mallocNormSize;
    for(int i = 0; i < NumVertices; i++)
	{
		newSphere.points[i] = triPoints[i];
		newSphere.normals[i] = triNorms[i];
	}
	free(triPoints);
	free(triNorms);
	return newSphere;
}