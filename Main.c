#define PHALLOC_IMPLEMENTATION // Only define PHALLOC_IMPLEMENTATION in once in one translation unit
#define PHALLOC_DEBUG
#define PHALLOC_EZ_NAMES

#include "PHALLOC.h"

int main()
{
	Pha_Init();

	printf_s("Hello, World!\n");

	size_t* arr1 = Malloc(size_t, sizeof(size_t) * 3);
	size_t* arr2 = Calloc(size_t, 4);
	arr1 = ReAlloc(size_t, arr1, sizeof(size_t) * 2);
	Free(arr1);
	Free(arr2);

	Pha_Dump(stdout);

	Pha_Close();

	return 0;
}