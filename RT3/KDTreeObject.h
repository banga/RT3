#pragma once
#include "Object3d.h"

class KDTreeObject;
class KDTreeNode;

class KDTreeNodeToRefine
{
public:
	int *objects;
	int size;
	Vector min, max;
	int pIndex, depth;

	KDTreeNodeToRefine(int* objects, int size, Vector min, Vector max, int parentIndex, int depth);
	~KDTreeNodeToRefine();
};

class KDTreeNodeTodo
{
public:
	int index;
	float tnear, tfar;

	KDTreeNodeTodo(int index, float tnear, float tfar);
};

class BoundEdge {
public:
	float x;
	int start, objIndex;

	BoundEdge();
	BoundEdge(float _x, bool isStart, int index);
	static int compare(const void*, const void*);
};

class MailBox
{
public:
	static int RayId;
	static unsigned long testCount;

	Object3d* object;
	int lastRayId;
	MailBox();
	MailBox(Object3d* o);
	~MailBox();
};

class KDTreeNode
{

public:
	// Used in non-leaf nodes
	union {
		int rIndex;
		int nObjects;
	};
	union {
		float splitPosition;
		struct {
			int padding, flags;
		} flagStruct;
		int* iMailBoxes;
	};

	KDTreeNode(KDTreeNodeToRefine* nodeToRefine, KDTreeObject* treeObj, int index, KDTreeNodeToRefine** leftNode, KDTreeNodeToRefine** rightNode);

	void printCosts(float costs[], float bestcost, int size);
	void MakeLeaf(KDTreeNodeToRefine* nodeToRefine);

	inline int getSplitAxis() {
		return (flagStruct.flags & 3);
	}
	inline void setSplitAxis(int axis) {
		flagStruct.flags = (flagStruct.flags & ~3) | (axis);
	}
	inline void setLeafFlag() {
		flagStruct.flags = (flagStruct.flags | 3);
	}
	inline bool isLeaf() {
		return ((flagStruct.flags & 3) == 3);
	}
	~KDTreeNode(void);
};

class KDTreeObject:public Object3d
{
public:
	static int MINOBJS;
	int MAXDEPTH;
	int tIntersection, tTraversal;
	float emptyBonus;

	// Bounding box
	Vector _min, _max;
	KDTreeNode** nodes;
	int nodesSize, lastNodeIndex;
	MailBox** objects;
	int nObjects;
	Vector* mins;
	Vector* maxs;

	KDTreeObject(vector<Object3d*> o);
	bool Intersect(Ray& ray, Intersection& intersection);
	bool IntersectP(Ray& ray) ;
	virtual void GetBounds(Vector &min, Vector &max) override;
	int nextNodeIndex();
	static bool IntersectBox(Vector &min, Vector &max, Vector &D, Vector &O, float &tnear, float &tfar);

public:
	~KDTreeObject(void);
};
