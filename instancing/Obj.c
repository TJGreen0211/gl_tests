#include "Obj.h"

obj ObjLoadModel(char *fname);
obj newObj;

obj ObjLoadModel(char *fname)
{	
	FILE *fp;
	fp = fopen(fname, "r");
	
	char ch;
	int read;
	float x, y, z;
	int ix, iy, iz;
	int vertCount = 0;
   	int faceCount = 0;
   	int objectCount = 0;
   	int i = 0;
	int j = 0;
	
	if(!fp)
   	{
      	printf("Error while opening the file.\n");
      	exit(1);
   	}
   	
   	
   	
	while((ch = fgetc(fp)) != EOF)
	{
		read = fscanf(fp, "%c",&ch);
		if(read == 1 && strncmp(&ch, "o ", 1) == 0)
		{
			objectCount+=1;
			//printf("OBJECT COUNT: %d\n", objectCount);
		}
		if(read == 1 && ch == 'v')
		{
			fscanf(fp, "%f", &x);
			fscanf(fp, "%f", &y);
			fscanf(fp, "%f", &z);
			vertCount++; 
		}
		if(read == 1 && ch == 'f')
		{
			fscanf(fp, "%d", &ix);
			fscanf(fp, "%d", &iy);
			fscanf(fp, "%d", &iz);
			faceCount++;
		}
	}
	
	rewind(fp);
	
	vec3 verts[vertCount];
	vec3 faces[faceCount];
	int breakCount= 0;
	
	while((ch = fgetc(fp)) != EOF)
	{
		read = fscanf(fp, "%c",&ch);
		if(read == 1 && ch == 'o' && breakCount == 1)
		{
			breakCount = 1;
			
		}
		if(read == 1 && ch == 'v')
		{
			fscanf(fp, "%f", &x);
			fscanf(fp, "%f", &y);
			fscanf(fp, "%f", &z);
			verts[i].x = x;
			verts[i].y = y;
			verts[i].z = z;
			//printf("%f, %f, %f\n", verts[i].x, verts[i].y, verts[i].z);
			i++;
		}
		if(read == 1 && ch == 'f')
		{
			fscanf(fp, "%d", &ix);
			fscanf(fp, "%d", &iy);
			fscanf(fp, "%d", &iz);
			faces[j].x = ix;
			faces[j].y = iy;
			faces[j].z = iz;
			//printf("%f, %f, %f\n", faces[j].x, faces[j].y, faces[j].z);
			j++;
		}
	}
	
	fclose(fp);
	
	vec3 normals[vertCount*3];
	vec3 points[vertCount*3];
	vec3 one, two;
	int Index = 0;
	int a, b, c;
	
	newObj.normals = malloc(faceCount*3*sizeof(normals[0]));
	newObj.points = malloc(faceCount*3*sizeof(points[0]));
	
	for(int i = 0; i < faceCount; i++)
	{
		a = faces[i].x-1;
		b = faces[i].y-1;
		c = faces[i].z-1;
		
		one.x = verts[b].x - verts[a].x;
		one.y = verts[b].y - verts[a].y;
		one.z = verts[b].z - verts[a].z;
	
		two.x = verts[c].x - verts[b].x;
		two.y = verts[c].y - verts[b].y;
		two.z = verts[c].z - verts[b].z;

		vec3 normal = normalizevec3(crossvec3(one, two));

		newObj.normals[Index] = normal; newObj.points[Index] = verts[a]; Index++;
		newObj.normals[Index] = normal; newObj.points[Index] = verts[b]; Index++;
		newObj.normals[Index] = normal; newObj.points[Index] = verts[c]; Index++;
	}
	
	newObj.size = Index*sizeof(points[0]);
	newObj.nsize = Index*sizeof(normals[0]);
	newObj.vertexNumber = Index;
	return newObj;
}