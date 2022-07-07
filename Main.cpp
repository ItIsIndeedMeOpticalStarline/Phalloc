#define PHALLOC_DEBUG
#define PHALLOC_EZ_NAMES

#include "PHALLOC.hpp"

int main()
{
	printf_s("Hello, World!\n");

	size_t* arr1 = Malloc(size_t, sizeof(size_t) * 3);
	size_t* arr2 = Calloc(size_t, 4);
	arr1 = ReAlloc(size_t, arr1, sizeof(size_t) * 2);
	Free(arr1);
	Free(arr2);

	pha::Dump(std::cout);

	return 0;
}