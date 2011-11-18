#include "Defines.h"

#if _USE_MERSENNE
MTRand _randGen;
#endif

#if _MEMORY_MANAGEMENT
MemoryManager MemoryManaged::mManager = MemoryManager();
#endif

bool RTInit() {
	return true;
}

void RTCleanUp() {
}