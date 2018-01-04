#include "QuadTree.h"

#ifndef INFINITY
  // no infinity defined? high chance we are compiling as c89
  #include <float.h>
  #include <math.h>
  #define INFINITY DBL_MAX
  #define fmin(a,b) (((a)<(b))?(a):(b))
  #define fmax(a,b) (((a)>(b))?(a):(b))
#endif

static int insert(quadtree *tree, quadtreeNode *root, quadtreePoint *point, void *key);
static int splitNode(quadtree *tree, quadtreeNode *node);
static int nodeContains(quadtreeNode *outer, quadtreePoint *it);
static quadtreeNode *getQuadrant(quadtreeNode *root, quadtreePoint *point);

quadtreePoint *quadtreePointNew(double x, double y) {
	quadtreePoint *point;
	if(!(point = malloc(sizeof(*point)))) {
		return NULL;
	}
	point->x = x;
	point->y = y;
	return point;
}

void quadtreePointFree(quadtreePoint *point) {
	free(point);
}

void quadtreeBoundsExtend(quadtreeBounds *bounds, double x, double y) {
	bounds->nw->x = fmin(x, bounds->nw->x);
	bounds->nw->x = fmax(y, bounds->nw->y);
	bounds->se->x = fmax(x, bounds->se->x);
	bounds->se->x = fmin(y, bounds->se->y);
	bounds->width = fabs(bounds->nw->x - bounds->se->x);
	bounds->height = fabs(bounds->nw->y - bounds->se->y);
}

void quadtreeBoundsFree(quadtreeBounds *bounds) {
	quadtreePointFree(bounds->nw);
	quadtreePointFree(bounds->se);
	free(bounds);
}

quadtreeBounds *quadtreeBoundsNew() {
	quadtreeBounds *bounds;
	if((bounds = malloc(sizeof(*bounds))) == NULL)
		return NULL;
	bounds->nw = quadtreePointNew(INFINITY, -INFINITY);
	bounds->se = quadtreePointNew(-INFINITY, INFINITY);
	bounds->width = 0;
	bounds->height = 0;
	return bounds;
}

quadtreeNode *quadtreeNodeNew() {
	quadtreeNode *node;
	if(!(node = malloc(sizeof(*node))))
		return NULL;
	node->ne = NULL;
	node->nw = NULL;
	node->se = NULL;
	node->sw = NULL;
	node->point = NULL;
	node->bounds = NULL;
	node->key = NULL;
	return node;
}

void quadtreeNodeFree(quadtreeNode *node, void (*keyFree)(void*)) {
	if(node->nw != NULL) quadtreeNodeFree(node->nw, keyFree);
	if(node->ne != NULL) quadtreeNodeFree(node->ne, keyFree);
	if(node->sw != NULL) quadtreeNodeFree(node->sw, keyFree);
	if(node->se != NULL) quadtreeNodeFree(node->se, keyFree);
	
	quadtreeBoundsFree(node->bounds);
	quadtreeNodeReset(node, keyFree);
	free(node);
}

void quadtreeNodeReset(quadtreeNode *node, void (*keyFree)(void*)) {
	quadtreePointFree(node->point);
	(*keyFree)(node->key);
}

quadtreeNode *quadtreeNodeWithBounds(double minx, double miny, double maxx, double maxy) {
	quadtreeNode *node;
	if(!(node = quadtreeNodeNew())) return NULL;
	if(!(node->bounds = quadtreeBoundsNew())) return NULL;
	quadtreeBoundsExtend(node->bounds, maxx, maxy);
	quadtreeBoundsExtend(node->bounds, minx, miny);
	return node;
}


int quadtreeNodeIsleaf(quadtreeNode *node) {
	return node->point != NULL;
}

int quadtreeNodeIspointer(quadtreeNode *node) {
	return node->nw != NULL
		&& node->ne != NULL
		&& node->sw != NULL
		&& node->se != NULL
		&& !quadtreeNodeIsleaf(node);
}

int quadtreeNodeIsempty(quadtreeNode *node) {
	return node->nw == NULL
		&& node->ne == NULL
		&& node->sw == NULL
		&& node->se == NULL
		&& !quadtreeNodeIsleaf(node);
}

//Static node functions

static int nodeContains(quadtreeNode *outer, quadtreePoint *it) {
	return outer->bounds != NULL
		&& outer->bounds->nw->x <= it->x
		&& outer->bounds->nw->y >= it->y
		&& outer->bounds->se->x >= it->x
		&& outer->bounds->se->y <= it->y;
}

static void elision(void* key){}

static void resetNode(quadtree *tree, quadtreeNode *node) {
	if(tree->keyFree != NULL) {
		quadtreeNodeReset(node, tree->keyFree);
	} else {
		quadtreeNodeReset(node, elision);
	}
}

static quadtreeNode *getQuadrant(quadtreeNode *root, quadtreePoint *point) {
	if(nodeContains(root->nw, point)) return root->nw;
	if(nodeContains(root->ne, point)) return root->ne;
	if(nodeContains(root->sw, point)) return root->sw;
	if(nodeContains(root->se, point)) return root->se;
	return NULL;
}

static int splitNode(quadtree *tree, quadtreeNode *node) {
	quadtreeNode *nw;
	quadtreeNode *ne;
	quadtreeNode *sw;
	quadtreeNode *se;
	quadtreePoint *old;
	void *key;
	
	double x = node->bounds->nw->x;
	double y = node->bounds->nw->y;
	double hw = node->bounds->width / 2;
	double hh = node->bounds->height / 2;
	
	if(!(nw = quadtreeNodeWithBounds(x,    y-hh,   x+hw,   y))) return 0;
	if(!(ne = quadtreeNodeWithBounds(x+hw, y-hh,   x+hw*2, y))) return 0;
	if(!(sw = quadtreeNodeWithBounds(x,    y-hh*2, x+hw,   y-hh))) return 0;
	if(!(se = quadtreeNodeWithBounds(x+hw, y-hh*2, x+hw*2, y-hh))) return 0;
	
	node->nw = nw;
	node->ne = ne;
	node->sw = sw;
	node->se = se;
	
	old = node->point;
	key = node->key;
	node->point = NULL;
	node->key = NULL;
	
	return insert(tree, node, old, key);
}

static quadtreePoint *find(quadtreeNode *node, double x, double y) {
	if(!node) {
		return NULL;
	}
	if(quadtreeNodeIsleaf(node)) {
		if(node->point->x == x && node->point->y == y)
			return node->point;
	} else if(quadtreeNodeIspointer(node)) {
		quadtreePoint test;
		test.x = x;
		test.y = y;
		return find(getQuadrant(node, &test), x, y);
	}
	return NULL;
}

static int insert(quadtree *tree, quadtreeNode *root, quadtreePoint *point, void *key) {
	if(quadtreeNodeIsempty(root)) {
		root->point = point;
		root->key = key;
		return 1;
	} else if(quadtreeNodeIsleaf(root)) {
		if(root->point->x == point->x && root->point->y == point->y) {
			resetNode(tree, root);
			root->point = point;
			root->key = key;
			return 2;
		} else {
			if(!splitNode(tree, root)) {
				return 0;
			}
			return insert(tree, root, point, key);
		}
	} else if(quadtreeNodeIspointer(root)) {
		quadtreeNode *quadrant = getQuadrant(root, point);
		return quadrant == NULL ? 0 : insert(tree, quadrant, point, key);
	}
	return 0;
}

//Interface node functions

quadtree *quadtreeNew(double minx, double miny, double maxx, double maxy) {
	quadtree *tree;
	if(!(tree = malloc(sizeof(*tree))))
		return NULL;
	tree->root = quadtreeNodeWithBounds(minx, miny, maxx, maxy);
	if(!(tree->root))
		return NULL;
	tree->keyFree = NULL;
	tree->length = 0;
	return tree;
}

int quadtreeInsert(quadtree *tree, double x, double y, void *key) {
	quadtreePoint *point;
	int insertStatus;
	
	if(!(point = quadtreePointNew(x, y))) return 0;
	if(!nodeContains(tree->root, point)) {
		quadtreePointFree(point);
		return 0;
	}
	if(!(insertStatus = insert(tree, tree->root, point, key))) {
		quadtreePointFree(point);
		return 0;
	}
	if(insertStatus == 1) tree->length++;
	return insertStatus;
}

quadtreePoint *quadtreeSearch(quadtree *tree, double x, double y) {
	return find(tree->root, x, y);
}

void quadtreeFree(quadtree *tree) {
	if(tree->keyFree != NULL) {
		quadtreeNodeFree(tree->root, tree->keyFree);
	} else {
		quadtreeNodeFree(tree->root, elision);
	}
	free(tree);
}

void quadtreeWalk(quadtreeNode *root, void (*descent)(quadtreeNode *node), void (*ascent)(quadtreeNode *node)) {
	(*descent)(root);
	if(root->nw != NULL) quadtreeWalk(root->nw, descent, ascent);
	if(root->ne != NULL) quadtreeWalk(root->nw, descent, ascent);
	if(root->sw != NULL) quadtreeWalk(root->nw, descent, ascent);
	if(root->se != NULL) quadtreeWalk(root->nw, descent, ascent);
	(*ascent)(root);
}