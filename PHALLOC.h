#ifndef _PHALLOC_H_
#define _PHALLOC_H_

/*---------------------------CONFIGURABLE MACRO SETINGS---------------------------*//*
|	#define PHALLOC_DEBUG		// Record debug information which can be output	with |
|								Dump(outstream)										 |
|																					 |
|	#define PHALLOC_SPEED		// Remove all safety checks. Not reccomended unless	 |
|								extensive programming debug work as been done		 |
|																					 |
|	#define PHALLOC_EZ_NAMES	// Replace macro names with shorter variants		 |
|			PHA_MALLOC(type, bytes)				-> Malloc(type, bytes)				 |
|			PHA_CALLOC(type, numElements)		-> Calloc(type, numElements)		 |
|			PHA_REALLOC(type, memBlock, bytes)  -> ReAlloc(type, memBlock, bytes)	 |
|			PHA_FREE(type, memBlock)			-> Free(type memBlock)				 |
|																					 |
|	#define PHALLOC_WARN_DIRE	// Dump(ostream) only dumps not-freed memory		 |
*//*--------------------------------------------------------------------------------*/

#ifdef Malloc
#undef PHALLOC_EZ_NAMES
#error Malloc macro already defined. Please remove definition or undefine PHALLOC_EZ_NAMES
#endif

#ifdef Calloc
#undef PHALLOC_EZ_NAMES
#error Calloc macro already defined. Please remove definition or undefine PHALLOC_EZ_NAMES
#endif

#ifdef ReAlloc
#undef PHALLOC_EZ_NAMES
#error ReAlloc macro already defined. Please remove definition or undefine PHALLOC_EZ_NAMES
#endif

#ifdef Free
#undef PHALLOC_EZ_NAMES
#error Free macro already defined. Please remove definition or undefine PHALLOC_EZ_NAMES
#endif

#ifdef PHALLOC_SPEED
	#ifdef PHALLOC_DEBUG
	#undef PHALLOC_DEBUG
	#endif

	#ifdef PHALLOC_WARN_DIRE
	#undef PHALLOC_WARN_DIRE
	#endif
#endif


#ifdef PHALLOC_WARN_DIRE
	#ifndef PHALLOC_DEBUG
	#define PHALLOC_DEBUG
	#endif
#endif


#include "stdio.h"
#include "stdlib.h"
#include "string.h"

#ifdef __cplusplus
extern "C"
{
#endif

	#define PHA_BOOL	char
	#define PHA_FALSE	0
	#define PHA_TRUE	!PHA_FALSE

	#ifdef PHALLOC_DEBUG
	typedef struct
	{
		const char* file;
		PHA_BOOL freed, reallocated;
		size_t line;
	} meminstance_t;

	typedef struct
	{
		void* mem;
		meminstance_t instance;
	} voidptrmeminstancevector_t; // lol

	extern voidptrmeminstancevector_t* Pha_Internal_instanceVector;
	extern size_t Pha_Internal_instanceVectorLength;
	extern size_t Pha_Internal_instanceVectorSize;
	#ifdef PHALLOC_IMPLEMENTATION
	voidptrmeminstancevector_t* Pha_Internal_instanceVector;
	size_t Pha_Internal_instanceVectorLength;
	size_t Pha_Internal_instanceVectorSize;
	#endif

	static inline void Pha_Internal_InstanceVector_Add(voidptrmeminstancevector_t obj)
	{
		if (Pha_Internal_instanceVector == NULL)
		{
			fprintf_s(stderr, "PHALLOC ERROR: A PHALLOC library function was called before Pha_Init()\n");
			exit(EXIT_FAILURE);
		}

		if (Pha_Internal_instanceVectorSize + 1 >= Pha_Internal_instanceVectorLength)
		{
			Pha_Internal_instanceVector = (voidptrmeminstancevector_t*)realloc((void*)Pha_Internal_instanceVector, (Pha_Internal_instanceVectorLength + (Pha_Internal_instanceVectorLength / 2)) * sizeof(voidptrmeminstancevector_t));
			if (Pha_Internal_instanceVector == NULL)
			{
				fprintf_s(stderr, "PHALLOC ERROR: Pha_Internal_InstanceVector_Add(voidptrmeminstancevector_t) failed to reallocate vector\n");
				exit(EXIT_FAILURE);
			}
		}
		Pha_Internal_instanceVector[Pha_Internal_instanceVectorSize] = obj;
		Pha_Internal_instanceVectorSize++;
	}

	static inline void Pha_Internal_InstanceVector_Erase(void* key)
	{
		if (Pha_Internal_instanceVector == NULL)
		{
			fprintf_s(stderr, "PHALLOC ERROR: A PHALLOC library function was called before Pha_Init()\n");
			exit(EXIT_FAILURE);
		}

		for (size_t i = 0; i < Pha_Internal_instanceVectorLength; i++)
		{
			if (key == Pha_Internal_instanceVector[i].mem)
			{
				for (size_t j = i + 1; j < Pha_Internal_instanceVectorSize; j++)
				{
					Pha_Internal_instanceVector[j - 1] = Pha_Internal_instanceVector[j];
				}
				break;
			}
		}
		Pha_Internal_instanceVectorSize--;
	}

	static inline meminstance_t* Pha_Internal_InstanceVector_Find(void* key)
	{
		if (Pha_Internal_instanceVector == NULL)
		{
			fprintf_s(stderr, "PHALLOC ERROR: A PHALLOC library function was called before Pha_Init()\n");
			exit(EXIT_FAILURE);
		}

		for (size_t i = 0; i < Pha_Internal_instanceVectorLength; i++)
		{
			if (key == Pha_Internal_instanceVector[i].mem)
				return &Pha_Internal_instanceVector[i].instance;
		}

		return NULL;
	}

	static inline void Pha_Init()
	{
		Pha_Internal_instanceVector = (voidptrmeminstancevector_t*)calloc(4, sizeof(voidptrmeminstancevector_t));
		Pha_Internal_instanceVectorLength = 4;
		Pha_Internal_instanceVectorSize = 0;
	}

	static inline void Pha_Close()
	{
		free(Pha_Internal_instanceVector);
	}
	#else
	static inline void Pha_Init() {	}

	static inline void Pha_Close() {	}
	#endif

	// DO NOT CALL Pha_Internal_Malloc(size_t, const char*, size_t, size_t), USE PHA_MALLOC(typename, number) MACRO INSTEAD
	static inline void* Pha_Internal_Malloc(size_t bytes, const char* file, size_t line, size_t sizeOfType)
	{
		#ifndef PHALLOC_SPEED
		if (bytes % sizeOfType != 0) // Would a cast work?
		{
			fprintf_s(stderr, "PHALLOC ERROR: Truncated Memory Cast! Tried to PHA_MALLOC %llu bytes in %s on line %llu, which could not be cast to a type of size %llu\n", bytes, file, line, sizeOfType);
			exit(EXIT_FAILURE);
		}

		void* mem = malloc(bytes);
		if (!mem)
		{
			fprintf_s(stderr, "PHALLOC ERROR: Out of Memory! Tried to PHA_MALLOC %llu bytes in %s on line %llu\n", bytes, file, line);
			exit(EXIT_FAILURE);
		}

		#ifdef PHALLOC_DEBUG
		Pha_Internal_InstanceVector_Add((voidptrmeminstancevector_t){ mem, (meminstance_t){ file, PHA_FALSE, PHA_FALSE, line } });
		#endif

		return mem;
		#else
		return malloc(bytes);
		#endif
	}

	// DO NOT CALL Pha_Internal_Calloc(size_t, const char*, size_t, size_t), USE PHA_CALLOC(typename, number) MACRO INSTEAD
	static inline void* Pha_Internal_Calloc(size_t numElements, const char* file, size_t line, size_t sizeOfType)
	{
		#ifndef PHALLOC_SPEED
		void* mem = calloc(numElements, sizeOfType);
		if (!mem)
		{
			fprintf_s(stderr, "PHALLOC ERROR: Out of Memory! Tried to PHA_CALLOC %llu bytes in %s on line %llu\n", numElements, file, line);
			exit(EXIT_FAILURE);
		}

		#ifdef PHALLOC_DEBUG
		Pha_Internal_InstanceVector_Add((voidptrmeminstancevector_t) { mem, (meminstance_t) { file, PHA_FALSE, PHA_FALSE, line } });
		#endif

		return mem;
		#else
		return calloc(numElements, sizeOfType);
		#endif
	}

	// DO NOT CALL Pha_Internal_ReAlloc(void*, size_t, const char*, size_t, size_t), USE PHA_REALLOC(typename, type*, number) MACRO INSTEAD
	static inline void* Pha_Internal_ReAlloc(void* memBlock, size_t bytes, const char* file, size_t line, size_t sizeOfType)
	{
		#ifdef PHALLOC_DEBUG
		Pha_Internal_InstanceVector_Erase(memBlock);
		#endif

		#ifndef PHALLOC_SPEED
		if (bytes % sizeOfType != 0) // Would a cast work?
		{
			fprintf_s(stderr, "PHALLOC ERROR: Truncated Memory Cast! Tried to PHA_REALLOC pointer at address %p to a new size of %llu bytes in %s on line %llu, which could not be cast to a type of size %llu\n", memBlock, bytes, file, line, sizeOfType);
			exit(EXIT_FAILURE);
		}

		void* reallocdMem = realloc(memBlock, bytes);
		if (!reallocdMem)
		{
			fprintf_s(stderr, "PHALLOC ERROR: Failed Reallocation! Tried to PHA_REALLOC pointer at address %p to a new size of %llu bytes in %s on line %llu\n", memBlock, bytes, file, line);
			exit(EXIT_FAILURE);
		}

		#ifdef PHALLOC_DEBUG
		Pha_Internal_InstanceVector_Add((voidptrmeminstancevector_t) { reallocdMem, (meminstance_t) { file, PHA_FALSE, PHA_TRUE, line } });
		#endif

		return reallocdMem;
		#else
		return realloc(memBlock, bytes);
		#endif
	}

	// DO NOT CALL Pha_Internal_Free(void*), USE PHA_FREE(typename, type*) MACRO INSTEAD
	static inline void Pha_Internal_Free(void* memBlock)
	{
		#ifdef PHALLOC_DEBUG
			#ifdef PHALLOC_WARN_DIRE
			Pha_Internal_InstanceVector_Erase(memBlock);
			#else
			Pha_Internal_InstanceVector_Find(memBlock)->freed = PHA_TRUE;
			#endif
		#endif

		free(memBlock);
		memBlock = NULL;
	}

	static inline void Pha_Dump(FILE* stream)
	{
		PHA_BOOL doStderr = PHA_FALSE;
		if (stream == NULL)
		{
			fprintf_s(stderr, "PHALLOC ERROR: Failed to dump to stream! Attempting to output to stderr...\n");
			doStderr = PHA_TRUE;
		}

		for (size_t i = 0; i < Pha_Internal_instanceVectorSize; i++)
		{
			char allocOrRealloc[12] = "Allocated";
			if (Pha_Internal_instanceVector[i].instance.reallocated)
				strcpy_s(allocOrRealloc, sizeof(allocOrRealloc), "Reallocated");

			char freedOrNot[8] = "Was not";
			#ifndef PHALLOC_WARN_DIRE
			if (Pha_Internal_instanceVector[i].instance.freed)
				strcpy_s(freedOrNot, sizeof(freedOrNot), "Was");
			#endif

			if (doStderr)
				fprintf_s(stderr, "%p: %s in %s on line %llu. %s freed.\n", Pha_Internal_instanceVector[i].mem, allocOrRealloc, Pha_Internal_instanceVector[i].instance.file, Pha_Internal_instanceVector[i].instance.line, freedOrNot);
			else
				fprintf_s(stream, "%p: %s in %s on line %llu. %s freed.\n", Pha_Internal_instanceVector[i].mem, allocOrRealloc, Pha_Internal_instanceVector[i].instance.file, Pha_Internal_instanceVector[i].instance.line, freedOrNot);
		}
	}

	#ifdef PHALLOC_EZ_NAMES
	// Allocates a block of memory into code of a specific size and returns a typed pointer to the memory
	// if PHALLOC_DEBUG is defined, will record if memory is being freed and where it is being stored
	// --------------------------------------------------------------------------------
	// typename type:		type of the allocated memory
	// number bytes:		number of bytes to be allocated
	#define Malloc(type, bytes) Pha_Internal_Malloc(bytes, __FILE__, __LINE__, sizeof(type))

	// Allocates a block of memory into code of a specific size, initalizes all values to zero, and returns a typed pointer to the memory
	// if PHALLOC_DEBUG is defined, will record if memory is being freed and where it was allocated
	// --------------------------------------------------------------------------------
	// typename type:		type of the allocated memory
	// number numElements:	number of elements to be allocated
	#define Calloc(type, numElements) (type*)Pha_Internal_Calloc(numElements, __FILE__, __LINE__, sizeof(type))

	// Re-Allocates a block of memory to a new size and returns a typed pointer to the memory
	// --------------------------------------------------------------------------------
	// typename type:		type of the allocated memory
	// type* memBlock:		pointer to the allocated memory
	// number bytes:		number of bytes to be allocated
	#define ReAlloc(type, memBlock, bytes) (type*)Pha_Internal_ReAlloc((void*)memBlock, bytes, __FILE__, __LINE__, sizeof(type))

	// Frees a block of allocated memory
	// --------------------------------------------------------------------------------
	// type* memBlock:		pointer to the allocated memory
	#define Free(memBlock) Pha_Internal_Free((void*)memBlock)
	#else
	// Allocates a block of memory into code of a specific size and returns a typed pointer to the memory
	// if PHALLOC_DEBUG is defined, will record if memory is being freed and where it is being stored
	// --------------------------------------------------------------------------------
	// typename type:		type of the allocated memory
	// number bytes:		number of bytes to be allocated
	#define PHA_MALLOC(type, bytes) Pha_Internal_Malloc(bytes, __FILE__, __LINE__, sizeof(type))

	// Allocates a block of memory into code of a specific size, initalizes all values to zero, and returns a typed pointer to the memory
	// if PHALLOC_DEBUG is defined, will record if memory is being freed and where it was allocated
	// --------------------------------------------------------------------------------
	// typename type:		type of the allocated memory
	// number numElements:	number of elements to be allocated
	#define PHA_CALLOC(type, numElements) (type*)Pha_Internal_Calloc(numElements, __FILE__, __LINE__, sizeof(type))

	// Re-Allocates a block of memory to a new size and returns a typed pointer to the memory
	// --------------------------------------------------------------------------------
	// typename type:		type of the allocated memory
	// type* memBlock:		pointer to the allocated memory
	// number bytes:		number of bytes to be allocated
	#define PHA_REALLOC(type, memBlock, bytes) (type*)Pha_Internal_ReAlloc((void*)memBlock, bytes, __FILE__, __LINE__, sizeof(type))

	// Frees a block of allocated memory
	// --------------------------------------------------------------------------------
	// type* memBlock:		pointer to the allocated memory
	#define PHA_FREE(memBlock) Pha_Internal_Free((void*)memBlock)
	#endif

	#undef PHA_BOOL
	#undef PHA_FALSE
	#undef PHA_TRUE

#ifdef __cplusplus
}	
#endif

#endif