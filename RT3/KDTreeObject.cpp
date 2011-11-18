
#include "KDTreeObject.h"
#include <stdlib.h>
#include <stack>
#include <time.h>

using namespace std;

KDTreeNodeToRefine::KDTreeNodeToRefine(int* o, int s, Vector _min, Vector _max, int _pIndex, int _depth)
:objects(o), size(s), min(_min), max(_max), pIndex(_pIndex), depth(_depth)
{
}

KDTreeNodeToRefine::~KDTreeNodeToRefine()
{
	if(objects) delete[] objects;
}

/*
	KD-Tree Node Todo
	Used for intersecting with the KDTree
*/
KDTreeNodeTodo::KDTreeNodeTodo(int i, float tn, float tf):index(i), tnear(tn), tfar(tf)
{
}

BoundEdge::BoundEdge():x(0), objIndex(-1), start(-1) {
}

BoundEdge::BoundEdge(float _x, bool isStart, int index):x(_x), objIndex(index) {
	start = (isStart ? 0 : 1);
}

int BoundEdge::compare(const void* a, const void* b) {
	BoundEdge *ba = (BoundEdge*)a, *bb = (BoundEdge*)b;
	if(ba->x == bb->x) {
		return bb->start - ba->start;
	}
	return ((ba->x < bb->x) ? -1 : 1);
}

/*
	MailBox
*/
MailBox::MailBox():object(NULL)
{
}
MailBox::MailBox(Object3d* o):object(o), lastRayId(-1)
{
}
MailBox::~MailBox()
{
	if(object)
		delete object;
}
int MailBox::RayId = 0;
unsigned long MailBox::testCount = 0;

/*
	KD-Tree Node
*/
void KDTreeNode::printCosts(float costs[], float bestcost, int size)
{
	for(int i = 0; i < size; i+=100) {
		float avg = 0.0;
		for(int j = i; (j < i+100 && j < size); j++)
			avg += costs[j];
		avg /= 100.0;
		printf("%f",avg);
	}
	printf("Best Cost = %f", bestcost);
}

void KDTreeNode::MakeLeaf(KDTreeNodeToRefine* nodeToRefine)
{
	int N = nodeToRefine->size;

	setLeafFlag();

	nObjects = N;
	iMailBoxes = new int[N];
	if(iMailBoxes == NULL) {
		printf("Out of memory to allocate for mailboxes");
	}

	for(int i = 0; i < N; i++) {
		iMailBoxes[i] = nodeToRefine->objects[i];
	}
}

KDTreeNode::KDTreeNode(KDTreeNodeToRefine* nodeToRefine, KDTreeObject* treeObj, int index, KDTreeNodeToRefine** leftNode, KDTreeNodeToRefine** rightNode)
{
	if(nodeToRefine == NULL) return;

	// Update Right Child index of the node
	if(nodeToRefine->pIndex != -1) {
		treeObj->nodes[nodeToRefine->pIndex]->rIndex = index;
	}

	int N = nodeToRefine->size;

	// Leaf node:
	if(nodeToRefine->depth >= treeObj->MAXDEPTH || N <= treeObj->MINOBJS) {
		MakeLeaf(nodeToRefine);
		return;
	}

	Vector min = nodeToRefine->min, max = nodeToRefine->max;
	Vector delta = max - min;
	int axis = (delta.x > delta.y ? (delta.x > delta.z ? 0 : 2) : (delta.y > delta.z ? 1 : 2));

	// Non-leaf node:
	iMailBoxes = NULL;

	const int otherAxis[3][2] = {{1,2}, {0, 2}, {0, 1}};
	const int n1 = otherAxis[axis][0], n2 = otherAxis[axis][1];

	const int N2 = N << 1;
	BoundEdge* bounds = new BoundEdge[N2];

	for(int i = 0, bi = 0; i < N; i++) {
		int oi = nodeToRefine->objects[i];
		bounds[bi++] = BoundEdge(treeObj->mins[oi][axis], true, oi);
		bounds[bi++] = BoundEdge(treeObj->maxs[oi][axis], false, oi);
	}
	qsort(bounds, N2, sizeof(BoundEdge), BoundEdge::compare);

	float x1 = min[axis], x2 = max[axis];
	float splitpos, cost, bestsplitpos = FLT_MAX, bestcost = FLT_MAX;
	float Sl, Sr, pl, pr, l1 = delta[n1], l2 = delta[n2], l3 = x2 - x1;
	float constS = 2 * (l1 * l2 + l2 * l3 + l3 * l1); // Total surface area of the node
	float cA = 2 * l1 * l2, cL = 2 * (l1 + l2);
	float invS = 1.0f / (constS);
	int Nr = N, Nl = 0, bestOffset = -1;
	int tT = treeObj->tTraversal, tI = treeObj->tIntersection;
	float eB = treeObj->emptyBonus;

	//float* costs = new float[2*N];

	// Find best split position
	BoundEdge* be = bounds;
	for(int i=0; i < N2; i++, be++) {
		if(be->start == 0) Nr--;

		splitpos = be->x;
		if(splitpos > x1 && splitpos < x2) {
			Sl = cA + (splitpos-x1) * cL;
			Sr = cA + (x2-splitpos) * cL;
			pl = Sl * invS;
			pr = Sr * invS;
			
			if(Nl == 0 || Nr == 0) {
						cost = tT + (1-eB)*tI*(pl*Nl + pr*Nr);
			} else {
						cost = tT + tI*(pl*Nl + pr*Nr);
			}
		//	costs[i] = cost;

			if(cost < bestcost) {
				bestcost = cost;
				bestsplitpos = splitpos;
				bestOffset = i;
			}
		}
		if(be->start == 1) Nl++;
	}

	//if(nodeToRefine->depth == 1)
	//	printCosts(costs, bestcost, 2*N);
	//delete[] costs;

	splitPosition = bestsplitpos;

	// Create leaf if splitting is costlier
	if(bestOffset == -1 || splitPosition <= x1 || splitPosition >= x2) {
		MakeLeaf(nodeToRefine);
		return;
	} else {
		Nl = 0, Nr = 0;

		for(int i = 0; i < bestOffset; i++) {
			if(bounds[i].start == 0) {
				Nl++;
			}
		}
		for(int i = bestOffset+1; i < N2; i++) {
			if(bounds[i].start == 1) {
				Nr++;
			}
		}

		int* lObjs = (Nl >= 0 ? new int[Nl] : NULL);
		int* rObjs = (Nr >= 0 ? new int[Nr] : NULL);

		for(int i = 0, j = 0; i < bestOffset; i++) {
			if(bounds[i].start == 0) {
				lObjs[j++] = bounds[i].objIndex;
			}
		}
		for(int i = bestOffset+1, j = 0; i < N*2; i++) {
			if(bounds[i].start == 1) {
				rObjs[j++] = bounds[i].objIndex;
			}
		}

		Vector midmin = min, midmax = max;
		midmin[axis] = splitPosition;
		midmax[axis] = splitPosition;

		setSplitAxis(axis);
		rIndex = -1;

		if(Nl >= 0) {
			*leftNode = new KDTreeNodeToRefine(lObjs, Nl, min, midmax, -1, nodeToRefine->depth+1);
		}
		if(Nr >= 0) {
			*rightNode = new KDTreeNodeToRefine(rObjs, Nr, midmin, max, index, nodeToRefine->depth+1);
		}
	}
	delete[] bounds;
}

KDTreeNode::~KDTreeNode()
{
	if(isLeaf() && iMailBoxes != NULL) {
		delete[] iMailBoxes;
	}
}

/*
	KD-Tree
*/
int KDTreeObject::nextNodeIndex()
{
	return lastNodeIndex++;
}

int KDTreeObject::MINOBJS = 64;

KDTreeObject::KDTreeObject(vector<Object3d*> o):Object3d(NULL)
{
	clock_t t = clock();
	printf("\nCreating Kd-Tree...");
	if(o.size() < 1) {
		nodes = NULL;
		return;
	}

	MAXDEPTH = (int)(8.5001f + 1.3 * log((float)o.size()) / log(2.0));
	tIntersection = 80;
	tTraversal = 1;
	emptyBonus = 0.25;
	lastNodeIndex = 0;

	int N = o.size();

	nObjects = N;
	objects = new MailBox*[N];
	for(int i = 0; i < N; i++) {
		objects[i] = new MailBox(o[i]);
	}

	mins = new Vector[N];
	maxs = new Vector[N];

	Vector a, b;
	int k = 0;
	int *objsToAdd = new int[N];

	for(int i = 0; i < N; i++) {
		objects[i]->object->GetBounds(a, b);
		_min.x = (a.x < _min.x ? a.x : _min.x);
		_min.y = (a.y < _min.y ? a.y : _min.y);
		_min.z = (a.z < _min.z ? a.z : _min.z);

		_max.x = (b.x > _max.x ? b.x : _max.x);
		_max.y = (b.y > _max.y ? b.y : _max.y);
		_max.z = (b.z > _max.z ? b.z : _max.z);

		mins[i] = Vector(a);
		maxs[i] = Vector(b);
		objsToAdd[k] = k++;
	}

	/*
		Tree Creation
	*/
	nodesSize = 512;
	nodes = new KDTreeNode*[nodesSize];

	stack<KDTreeNodeToRefine*> nodesToDo;
	nodesToDo.push(new KDTreeNodeToRefine(objsToAdd, N, _min, _max, -1, 1));

	while(!nodesToDo.empty()) {
		KDTreeNodeToRefine *nodeToDo = nodesToDo.top(); nodesToDo.pop();
		KDTreeNodeToRefine *leftToDo = NULL, *rightToDo = NULL;

		int index = nextNodeIndex();
		if(index >= nodesSize) {
			int newNodesSize = nodesSize + 512;
			KDTreeNode** newNodes = new KDTreeNode*[newNodesSize];
			for(int i = 0; i < nodesSize; i++) {
				newNodes[i] = nodes[i];
			}
			delete[] nodes;
			nodes = newNodes;
			nodesSize = newNodesSize;
		}

		nodes[index] = new KDTreeNode(nodeToDo, this, index, &leftToDo, &rightToDo);
		delete nodeToDo;

		if(rightToDo != NULL) {
			nodesToDo.push(rightToDo);
		}
		if(leftToDo != NULL) {
			nodesToDo.push(leftToDo);
		}
	}
	t = clock() - t;
	printf("done in %.3fs. Kd-tree contains %d nodes", t / (float)(CLOCKS_PER_SEC), lastNodeIndex);
	/*
		KD-Tree test
	List<int>^ l = new List<int>();
	for(int i = 0; i < nodeCount; i++) {
		KDTreeNode^ n = nodes[i];
		if(n->iMailBoxes != NULL) {
			for(int j = 0; j < n->iMailBoxes->Length; j++) {
				int mb = n->iMailBoxes[j];
				if(l->BinarySearch(mb) < 0) {
					l->Add(mb);
					l->Sort();
				}
			}
		}
	}
	l->Sort();

	System::String^ miss = "Missing: ";
	int prev, cur, i = 0;
	for(int i = 1; i < l->Count; i++) {
		prev = l[i-1];
		cur = l[i];
		for(int x = prev+1; x <= cur-1; x++) {
			miss = miss + ", " + x;
		}
	}

	System::Windows::Forms::MessageBox::Show(miss);
	*/
}

void KDTreeObject::GetBounds(Vector &min, Vector &max)
{
	min = _min;
	max = _max;
	if(transform) transform->TransformBounds(min,max);
}

bool KDTreeObject::IntersectBox(Vector &min, Vector &max, Vector &D, Vector &O, float &tnear, float &tfar)
{
	float t0 = tnear, t1 = tfar;

	for(int i = 0; i < 3; i++) {
		float invD = 1.0f / D[i];
		float tn = (min[i] - O[i]) * invD;
		float tf = (max[i] - O[i]) * invD;

		if(tn > tf) {
			float t = tn;
			tn = tf;
			tf = t;
		}
		t0 = (tn > t0) ? tn : t0;
		t1 = (tf < t1) ? tf : t1;
		if(t0 > t1) return false;
	}

	tnear = t0;
	tfar = t1;
	return true;
}

bool KDTreeObject::Intersect(Ray& r, Intersection& intersection)
{
	Ray ray = (transform ? transform->InvTransformRay(r) : r);

	Vector D = ray.D, invD = Vector(1.0f/D.x, 1.0f/D.y, 1.0f/D.z), O = ray.O;
	float tnear = ray.mint, tfar = ray.maxt;

	if(!IntersectBox(_min, _max, D, O, tnear, tfar)) {
		return false;
	}

	int rayId = MailBox::RayId++;
	stack<KDTreeNodeTodo> nodesTodo;
	nodesTodo.push(KDTreeNodeTodo(0, tnear, tfar));

	bool intersected = false;
	while(!nodesTodo.empty()) {
		KDTreeNodeTodo nodeTodo = nodesTodo.top(); nodesTodo.pop();
		if(nodeTodo.index == -1) continue;

		tnear = nodeTodo.tnear;
		tfar = nodeTodo.tfar;

		if(ray.maxt < tnear) break;

		KDTreeNode* node = nodes[nodeTodo.index];

		if(node->isLeaf()) {
			for(int i = 0; i < node->nObjects; i++) {
				MailBox* mb = objects[node->iMailBoxes[i]];
				if(mb->lastRayId != rayId) {
					MailBox::testCount++;
					if(mb->object->Intersect(ray,intersection)) {
						mb->lastRayId = rayId;
						intersected = true;
					}
				}
			}
		} else {
			//nodesTodo.push(new KDTreeNodeTodo(node->lIndex, tnear, tfar));
			//nodesTodo.push(new KDTreeNodeTodo(node->rIndex, tnear, tfar));

			int axis = node->getSplitAxis();
			float invDx = invD[axis], Ox = O[axis];
			float splitpos = node->splitPosition;
			float t = (splitpos - Ox) * invDx;

			int first, second;
			bool leftFirst = (Ox < splitpos) || (Ox == splitpos && invDx < 0);
			if(leftFirst) {
				first = nodeTodo.index+1;
				second = node->rIndex;
			} else {
				first = node->rIndex;
				second = nodeTodo.index+1;
			}
			if(t > tfar || t <= 0) {
				nodesTodo.push(KDTreeNodeTodo(first, tnear, tfar));
			} else if (t < tnear) {
				nodesTodo.push(KDTreeNodeTodo(second, tnear, tfar));
			} else {
				nodesTodo.push(KDTreeNodeTodo(second, t, tfar));
				nodesTodo.push(KDTreeNodeTodo(first, tnear, t));
			}
		}
	}
	return intersected;
}

bool KDTreeObject::IntersectP(Ray& r)
{
	Ray ray = (transform ? transform->InvTransformRay(r) : r);

	Vector D = ray.D, invD = Vector(1.0f/D.x, 1.0f/D.y, 1.0f/D.z), O = ray.O;
	float tnear = ray.mint, tfar = ray.maxt;

	if(!IntersectBox(_min, _max, D, O, tnear, tfar)) {
		return false;
	}

	int rayId = MailBox::RayId++;
	stack<KDTreeNodeTodo> nodesTodo;
	nodesTodo.push(KDTreeNodeTodo(0, tnear, tfar));

	while(!nodesTodo.empty()) {
		KDTreeNodeTodo nodeTodo = nodesTodo.top(); nodesTodo.pop();
		if(nodeTodo.index == -1) continue;

		tnear = nodeTodo.tnear;
		tfar = nodeTodo.tfar;

		if(ray.maxt < tnear) break;

		KDTreeNode* node = nodes[nodeTodo.index];

		if(node->isLeaf()) {
			for(int i = 0; i < node->nObjects; i++) {
				MailBox* mb = objects[node->iMailBoxes[i]];
				if(mb->lastRayId != rayId) {
					MailBox::testCount++;
					if(mb->object->IntersectP(ray)) {
						mb->lastRayId = rayId;
						return true;
					}
				}
			}
		} else {
			//nodesTodo.push(new KDTreeNodeTodo(node->lIndex, tnear, tfar));
			//nodesTodo.push(new KDTreeNodeTodo(node->rIndex, tnear, tfar));

			int axis = node->getSplitAxis();
			float invDx = invD[axis], Ox = O[axis];
			float splitpos = node->splitPosition;
			float t = (splitpos - Ox) * invDx;

			int first, second;
			bool leftFirst = (Ox < splitpos) || (Ox == splitpos && invDx < 0);
			if(leftFirst) {
				first = nodeTodo.index+1;
				second = node->rIndex;
			} else {
				first = node->rIndex;
				second = nodeTodo.index+1;
			}
			if(t > tfar || t <= 0) {
				nodesTodo.push(KDTreeNodeTodo(first, tnear, tfar));
			} else if (t < tnear) {
				nodesTodo.push(KDTreeNodeTodo(second, tnear, tfar));
			} else {
				nodesTodo.push(KDTreeNodeTodo(second, t, tfar));
				nodesTodo.push(KDTreeNodeTodo(first, tnear, t));
			}
		}
	}

	return false;
}

KDTreeObject::~KDTreeObject(void)
{
	if(nodes != NULL) {
		while(lastNodeIndex > 0) {
			delete nodes[--lastNodeIndex];
		}
		delete[] nodes;
	}
	if(objects) {
		for(int i = 0; i < nObjects; i++)
			delete objects[i];
		delete[] objects;
	}
	if(maxs) delete[] maxs;
	if(mins) delete[] mins;
}