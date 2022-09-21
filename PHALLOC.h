#ifndef _PHALLOC_H_
#define _PHALLOC_H_

//					 SEE BOTTOM OF FILE FOR LICENSE INFORMATION						//

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
|			PHA_FREE(memBlock)			-> Free(memBlock)							 |
|																					 |
|	#define PHALLOC_WARN_DIRE	// Dump(ostream) only dumps not-freed memory		 |
*//*--------------------------------------------------------------------------------*/

#pragma region phalloc_internal_setting_macros
// Version defs, not necessary but probably good to have ffr.
#define PHALLOC_VERSION_MAJOR 1
#define PHALLOC_VERSION_MINOR 1
#define PHALLOC_VERSION_REVISION 2
#define PHALLOC_VERSION_STRING "1.1.2"
#define PHALLOC_VERSION_NUM(major, minor, revision) (((major) << 16) | ((minor) << 8) | (revision))
#define PHALLOC_VERSION PHALLOC_VERSION_NUM(PHALLOC_VERSION_MAJOR, PHALLOC_VERSION_MINOR, PHALLOC_VERSION_REVISION)

#ifdef PHALLOC_EZ_NAMES
	#ifdef Malloc
	#error Malloc macro already defined. Please remove definition or undefine PHALLOC_EZ_NAMES
	#endif

	#ifdef Calloc
	#error Calloc macro already defined. Please remove definition or undefine PHALLOC_EZ_NAMES
	#endif

	#ifdef ReAlloc
	#error ReAlloc macro already defined. Please remove definition or undefine PHALLOC_EZ_NAMES
	#endif

	#ifdef Free
	#error Free macro already defined. Please remove definition or undefine PHALLOC_EZ_NAMES
	#endif
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
#pragma endregion

#ifdef __cplusplus
extern "C"
{
#endif
	#include "stdio.h"
	#include "stdlib.h"
	#include "string.h"

	#define PHA_BOOL	char
	#define PHA_FALSE	0
	#define PHA_TRUE	!PHA_FALSE

	#define PHA_DEF static inline

	#ifdef _CRT_INSECURE_DEPRECATE
	#define PHA_FPRINTF(stream, string, ...) fprintf_s(stream, string, __VA_ARGS__)
	#else
	#define PHA_FPRINTF(stream, string, ...) fprintf(stream, string, __VA_ARGS__)
	#endif

	#ifdef _CRT_INSECURE_DEPRECATE
	#define PHA_STRCPY(destination, source) strcpy_s(destination, sizeof(destination), source)
	#else
	#define PHA_STRCPY(destination, source) strcpy(destination, source)
	#endif

	#ifdef PHALLOC_DEBUG
	typedef struct
	{
		const char* file;
		PHA_BOOL freed, reallocated;
		int line;
	} mem_instance;

	typedef struct
	{
		void* mem;
		mem_instance instance;
	} void_ptr_mem_instance_vector; // lol

	extern void_ptr_mem_instance_vector* Pha_Internal_instanceVector;
	extern size_t Pha_Internal_instanceVectorLength;
	extern size_t Pha_Internal_instanceVectorSize;

	PHA_DEF void Pha_Internal_InstanceVector_Add(void_ptr_mem_instance_vector obj);

	PHA_DEF void Pha_Internal_InstanceVector_Erase(void* key);

	PHA_DEF mem_instance* Pha_Internal_InstanceVector_Find(void* key);
	#endif

	PHA_DEF void Pha_Dump(FILE* stream);

	PHA_DEF void Pha_Init();

	PHA_DEF void Pha_Close();

	PHA_DEF void* Pha_Internal_Malloc(size_t bytes, const char* file, int line, size_t sizeOfType);

	PHA_DEF void* Pha_Internal_Calloc(size_t numElements, const char* file, int  line, size_t sizeOfType);

	PHA_DEF void* Pha_Internal_ReAlloc(void* memBlock, size_t bytes, const char* file, int line, size_t sizeOfType);

	PHA_DEF void Pha_Internal_Free(void* memBlock);

	#ifdef PHALLOC_IMPLEMENTATION
		#ifdef PHALLOC_DEBUG
		void_ptr_mem_instance_vector* Pha_Internal_instanceVector;
		size_t Pha_Internal_instanceVectorLength;
		size_t Pha_Internal_instanceVectorSize;

		// DO NOT CALL Pha_Internal_InstanceVector_Add(void*)
		PHA_DEF void Pha_Internal_InstanceVector_Add(void_ptr_mem_instance_vector obj)
		{
			if (Pha_Internal_instanceVector == NULL)
			{
				PHA_FPRINTF(stderr, "PHALLOC ERROR: A PHALLOC library function was called before Pha_Init()\n");
				exit(EXIT_FAILURE);
			}

			if (Pha_Internal_instanceVectorSize + 1 >= Pha_Internal_instanceVectorLength)
			{
				// Must be done this way in order to prevent a warning. The warning wouldn't have mattered as the program is to crash if realloc(void*, size_t) returned NULL, which would have free'd the whole heap
				void* temp = realloc((void*)Pha_Internal_instanceVector, (Pha_Internal_instanceVectorLength + (Pha_Internal_instanceVectorLength / 2)) * sizeof(void_ptr_mem_instance_vector));
				if (temp != NULL)
				{
					Pha_Internal_instanceVector = (void_ptr_mem_instance_vector*)temp;
				}
				else
				{
					PHA_FPRINTF(stderr, "PHALLOC ERROR: Pha_Internal_InstanceVector_Add(void_ptr_mem_instance_vector) failed to reallocate vector\n");
					exit(EXIT_FAILURE);
				}
			}
			Pha_Internal_instanceVector[Pha_Internal_instanceVectorSize] = obj;
			Pha_Internal_instanceVectorSize++;
		}

		// DO NOT CALL Pha_Internal_InstanceVector_Erase(void*)
		PHA_DEF void Pha_Internal_InstanceVector_Erase(void* key)
		{
			if (Pha_Internal_instanceVector == NULL)
			{
				PHA_FPRINTF(stderr, "PHALLOC ERROR: A PHALLOC library function was called before Pha_Init()\n");
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

		// DO NOT CALL Pha_Internal_InstanceVector_Find(void*)
		PHA_DEF mem_instance* Pha_Internal_InstanceVector_Find(void* key)
		{
			if (Pha_Internal_instanceVector == NULL)
			{
				PHA_FPRINTF(stderr, "PHALLOC ERROR: A PHALLOC library function was called before Pha_Init()\n");
				exit(EXIT_FAILURE);
			}

			for (size_t i = 0; i < Pha_Internal_instanceVectorLength; i++)
			{
				if (key == Pha_Internal_instanceVector[i].mem)
					return &Pha_Internal_instanceVector[i].instance;
			}

			return NULL;
		}

		// Dumps collected data into a given stream with the format:
		// "(pointer): Allocated/Reallocated in (filepath) on line (line). Was/Was not freed."
		PHA_DEF void Pha_Dump(FILE* stream)
		{
			if (stream == NULL)
			{
				PHA_FPRINTF(stderr, "PHALLOC ERROR: Failed to dump to stream! Attempting to output to stderr...\n");
				stream = stderr;
			}

			for (size_t i = 0; i < Pha_Internal_instanceVectorSize; i++)
			{
				char allocOrRealloc[12] = "Allocated";

				if (Pha_Internal_instanceVector[i].instance.reallocated)
					PHA_STRCPY(allocOrRealloc, "Reallocated");

				char freedOrNot[8] = "Was not";
				#ifndef PHALLOC_WARN_DIRE
				if (Pha_Internal_instanceVector[i].instance.freed)
					PHA_STRCPY(freedOrNot, "Was");
				#endif

				PHA_FPRINTF(stream, "%p: %s in %s on line %i. %s freed.\n", Pha_Internal_instanceVector[i].mem, allocOrRealloc, Pha_Internal_instanceVector[i].instance.file, Pha_Internal_instanceVector[i].instance.line, freedOrNot);
			}
		}

		// Initalizes library's internal list. Must be called before using any library functions
		PHA_DEF void Pha_Init()
		{
			Pha_Internal_instanceVector = (void_ptr_mem_instance_vector*)calloc(4, sizeof(void_ptr_mem_instance_vector));
			if (Pha_Internal_instanceVector == NULL)
			{
				PHA_FPRINTF(stderr, "PHALLOC ERROR: Pha_Init() failed to allocate vector\n");
				exit(EXIT_FAILURE);
			}
			Pha_Internal_instanceVectorLength = 4;
			Pha_Internal_instanceVectorSize = 0;
		}

		// Frees the library's internal list.
		PHA_DEF void Pha_Close()
		{
			free(Pha_Internal_instanceVector);
		}
		#else
		// Dumps collected data into a given stream with the format:
		// "(pointer): Allocated/Reallocated in (filepath) on line (line). Was/Was not freed."
		PHA_DEF void Pha_Dump(FILE* stream) {	}

		// Initalizes library's internal list. Must be called before using any library functions if PHALLOC_DEBUG is defined
		PHA_DEF void Pha_Init() {	}

		// Frees the library's internal list.
		PHA_DEF void Pha_Close() {	}
		#endif

	// DO NOT CALL Pha_Internal_Malloc(size_t, const char*, size_t, size_t), USE PHA_MALLOC(typename, number) MACRO INSTEAD
	PHA_DEF void* Pha_Internal_Malloc(size_t bytes, const char* file, int line, size_t sizeOfType)
	{
		#ifndef PHALLOC_SPEED
		if (bytes % sizeOfType != 0) // Would a cast work?
		{
			PHA_FPRINTF(stderr, "PHALLOC ERROR: Truncated Memory Cast! Tried to PHA_MALLOC %llu bytes in %s on line %i, which could not be cast to a type of size %llu\n", bytes, file, line, sizeOfType);
			exit(EXIT_FAILURE);
		}

		void* mem = malloc(bytes);
		if (!mem)
		{
			PHA_FPRINTF(stderr, "PHALLOC ERROR: Out of Memory! Tried to PHA_MALLOC %llu bytes in %s on line %i\n", bytes, file, line);
			exit(EXIT_FAILURE);
		}

		#ifdef PHALLOC_DEBUG
		Pha_Internal_InstanceVector_Add((void_ptr_mem_instance_vector) { mem, (mem_instance) { file, PHA_FALSE, PHA_FALSE, line } });
		#endif

		return mem;
		#else
		return malloc(bytes);
		#endif
	}

	// DO NOT CALL Pha_Internal_Calloc(size_t, const char*, size_t, size_t), USE PHA_CALLOC(typename, number) MACRO INSTEAD
	PHA_DEF void* Pha_Internal_Calloc(size_t numElements, const char* file, int line, size_t sizeOfType)
	{
		#ifndef PHALLOC_SPEED
		void* mem = calloc(numElements, sizeOfType);
		if (!mem)
		{
			PHA_FPRINTF(stderr, "PHALLOC ERROR: Out of Memory! Tried to PHA_CALLOC %llu bytes in %s on line %i\n", numElements, file, line);
			exit(EXIT_FAILURE);
		}

		#ifdef PHALLOC_DEBUG
		Pha_Internal_InstanceVector_Add((void_ptr_mem_instance_vector) { mem, (mem_instance) { file, PHA_FALSE, PHA_FALSE, line } });
		#endif

		return mem;
		#else
		return calloc(numElements, sizeOfType);
		#endif
	}

	// DO NOT CALL Pha_Internal_ReAlloc(void*, size_t, const char*, size_t, size_t), USE PHA_REALLOC(typename, type*, number) MACRO INSTEAD
	PHA_DEF void* Pha_Internal_ReAlloc(void* memBlock, size_t bytes, const char* file, int line, size_t sizeOfType)
	{
		#ifndef PHALLOC_SPEED
		#ifdef PHALLOC_DEBUG
		Pha_Internal_InstanceVector_Erase(memBlock);
		#endif

		if (bytes % sizeOfType != 0) // Would a cast work?
		{
			PHA_FPRINTF(stderr, "PHALLOC ERROR: Truncated Memory Cast! Tried to PHA_REALLOC pointer at address %p to a new size of %llu bytes in %s on line %i, which could not be cast to a type of size %llu\n", memBlock, bytes, file, line, sizeOfType);
			exit(EXIT_FAILURE);
		}

		void* reallocdMem = realloc(memBlock, bytes);
		if (!reallocdMem)
		{
			PHA_FPRINTF(stderr, "PHALLOC ERROR: Failed Reallocation! Tried to PHA_REALLOC pointer at address %p to a new size of %llu bytes in %s on line %i\n", memBlock, bytes, file, line);
			exit(EXIT_FAILURE);
		}

		#ifdef PHALLOC_DEBUG
		Pha_Internal_InstanceVector_Add((void_ptr_mem_instance_vector) { reallocdMem, (mem_instance) { file, PHA_FALSE, PHA_TRUE, line } });
		#endif

		return reallocdMem;
		#else
		return realloc(memBlock, bytes);
		#endif
	}

	// DO NOT CALL Pha_Internal_Free(void*), USE PHA_FREE(typename, type*) MACRO INSTEAD
	PHA_DEF void Pha_Internal_Free(void* memBlock)
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
	#endif

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

	#undef PHA_STRCPY

	#undef PHA_FPRINTF

	#undef PHA_DEF

	#undef PHA_BOOL
	#undef PHA_FALSE
	#undef PHA_TRUE

#ifdef __cplusplus
}	
#endif

#endif

/*
MIT License

Copyright (c) 2022 ItIsIndeedMeOpticalStarline

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/