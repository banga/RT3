#pragma once
#define _USE_MATH_DEFINES
#include <math.h>
#include <float.h>
#include <stdio.h>
#include <time.h>
#include <vector>
using std::vector;

#undef max
#undef min

#undef M_PI
#undef M_1_PI
#define M_PI 3.14159265358979323846f
#define M_1_PI  0.318309886183790671538f
#define M_1_2PI 0.159154943091895335768f
#define M_180_PI 57.295779513082320876798f

#define EPSILON 0.0001f
#define INFINITY FLT_MAX
#define VACUUM_IOR 1

#define POW(x,e) powf(x, e)
#define SQRT(x) POW(x, 0.5f)
#define ATAN(y,x) atan2(y,x)
#define ACOS(x) acos(x)

inline int MOD(int a, int b) {
    int n = a/b;
    a -= n*b;
    if (a < 0) a += b;
    return a;
}

#define _USE_MERSENNE 1

#if _USE_MERSENNE
	#include "MersenneTwister.h"
	extern MTRand _randGen;

	inline int RAND() {
		return _randGen.randInt();
	}
	inline float FRAND() {
		return (float)_randGen();
	}
#else
	#define FRAND() (rand() / (float)(RAND_MAX-1))
	#define RAND() (rand())
#endif

inline float CLAMP(float x, float a, float b) {
	return ((x >= a) ? ((x <= b) ? x : b) : a);
}

inline void SWAP(float& x, float& y) {
	float t = x;
	x = y;
	y = t;
}

const float M_PI_180 = M_PI / 180.0f;

inline float DEG_2_RAD(float d) {
	return d * M_PI_180;
}

inline float RAD_2_DEG(float r) {
	return r * M_180_PI;
}

#define _MEMORY_MANAGEMENT 0

#if _MEMORY_MANAGEMENT
class MemoryManager {
	const unsigned int BLOCK_SIZE;
	unsigned int allocated;
	char *currentBlock;
	vector<char*> usedBlocks;
public:
	MemoryManager(unsigned int block_size = 32768):BLOCK_SIZE(block_size) {
		currentBlock = (char*)malloc(BLOCK_SIZE);
		allocated = 0;
	}
	void* allocate(size_t bytes) {
//		printf("\nRequested %d bytes", (int)bytes);
		bytes = ((bytes+7) & (~7));
		unsigned int newallocated = allocated+bytes;
		if(newallocated > BLOCK_SIZE) {
			usedBlocks.push_back(currentBlock);
			currentBlock = (char*)malloc((bytes > BLOCK_SIZE) ? bytes : BLOCK_SIZE);
			if(currentBlock == NULL) {
				throw "Memory allocation failed";
			}
			allocated = 0;
			newallocated = bytes;
			//printf("\nAllocated new block");
		}
		void *p = (currentBlock + allocated);
		allocated = newallocated;
		//printf("\n%x %u",p,allocated);
		return p;
	}
	~MemoryManager() {
		free (currentBlock);
		for(vector<char*>::iterator it = usedBlocks.begin(); it < usedBlocks.end(); it++) {
			free (*it);
		}
	}
};

class MemoryManaged {
	static MemoryManager mManager;
public:
	void* operator new(size_t size) {
		return mManager.allocate(size);
	}
	void* operator new[](size_t size) {
		return mManager.allocate(size);
	}
	void operator delete(void*) {
	}
	void operator delete[](void*) {
	}
};
#endif

bool RTInit();
void RTCleanUp();
