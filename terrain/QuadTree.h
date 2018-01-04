#ifndef QUADTREE_H
#define QUADTREE_H

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "LinearAlg.h"

typedef struct quadtreePoint {
	double x;
	double y;
} quadtreePoint;

typedef struct quadtreeBounds {
	quadtreePoint *nw;
	quadtreePoint *se;
	double width;
	double height;
} quadtreeBounds;

typedef struct quadtreeNode {
	struct quadtreeNode *ne;
	struct quadtreeNode *nw;
	struct quadtreeNode *se;
	struct quadtreeNode *sw;
	quadtreeBounds *bounds;
	quadtreePoint *point;
	void *key;
} quadtreeNode;

typedef struct quadtree {
	quadtreeNode *root;
	void (*keyFree)(void *key);
	unsigned int length;
} quadtree;

quadtreePoint *quadtreePointNew(double x, double y);
void quadtreePointFree(quadtreePoint *point);

quadtreeBounds *quadtreeBoundsNew();
void quadtreeBoundsExtend(quadtreeBounds *bounds, double x, double y);
void quadtreeBoundsFree(quadtreeBounds *bounds);

quadtreeNode *quadtreeNodeNew();
void quadtreeNodeFree(quadtreeNode *node, void (*valueFree)(void*));
void quadtreeNodeReset(quadtreeNode *node, void (*keyFree)(void*));
int quadtreeNodeIsleaf(quadtreeNode *node);
int quadtreeNodeIspointer(quadtreeNode *node);
int quadtreeNodeIsempty(quadtreeNode *node);

quadtreeNode *quadtreeNodeWithBounds(double minx, double miny, double maxx, double maxy);

quadtree *quadtreeNew(double minx, double miny, double maxx, double maxy);
void quadtreeFree(quadtree *tree);

quadtreePoint *quadtreeSearch(quadtree *tree, double x, double y);
int quadtreeInsert(quadtree *tree, double x, double y, void *key);
void quadtreeWalk(quadtreeNode *root, void (*descent)(quadtreeNode *node), void (*ascent)(quadtreeNode *node));

#endif