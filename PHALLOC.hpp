#ifndef _PHALLOC_HPP_
#define _PHALLOC_HPP_

/*---------------------------CONFIGURABLE MACRO SETINGS---------------------------*//*
|	#define PHALLOC_DEBUG		// Dump debug info into phalloc log.txt at the end	 |
|								of runtime											 |
|	#define PHALLOC_SPEED		// Remove all safety checks. Not reccomended unless	 |
|								extensive programming debug work as been done		 |
|	#define PHALLOC_EZ_NAMES	// Replace macro names with shorter variants		 |
|			PHA_MALLOC(type, bytes)				-> Malloc(type, bytes)				 |
|			PHA_CALLOC(type, numElements)		-> Calloc(type, numElements)		 |
|			PHA_REALLOC(type, memBlock, bytes)  -> ReAlloc(type, memBlock, bytes)	 |
|			PHA_FREE(type, memBlock)			-> Free(type memBlock)				 |
*//*--------------------------------------------------------------------------------*/

#ifdef PHALLOC_DEBUG 
#include <list> 
#endif

#include "stdlib.h"

#include <iostream>

namespace pha
{
	#ifdef PHALLOC_DEBUG
	struct mem_instance
	{
		const char* file;
		bool freed;
		size_t id;
		size_t line;
	};

	static std::list<mem_instance> Internal_MemTable;
	#endif

	// DO NOT CALL Internal_Malloc<typename>(size_t, const char*, size_t), USE PHA_MALLOC(typename, number) MACRO INSTEAD
	template<typename T>
	static inline T* Internal_Malloc(size_t bytes, const char* file, size_t line)
	{
		#ifndef PHALLOC_SPEED
		if (bytes % sizeof(T) != 0) // Would a cast work?
		{
			std::cerr << "PHALLOC ERROR: Truncated Memory Cast! Tried to PHA_MALLOC " << bytes << " bytes in " << file << " on line " << line << "which could not be cast to a(n)" << typeid(T).name() << std::endl;
			exit(EXIT_FAILURE);
		}

		void* mem = malloc(bytes);
		if (!mem)
		{
			std::cerr << "PHALLOC ERROR: Out of Memory! Tried to PHA_MALLOC " << bytes << " bytes in " << file << " on line " << line << std::endl;
			exit(EXIT_FAILURE);
		}

		return static_cast<T*>(mem);
		#else
		return static_cast<T*>(malloc(bytes));
		#endif
	}

	#ifdef PHALLOC_EZ_NAMES
	// Allocates a block of memory into code of a specific size and returns a typed pointer to the memory
	// if PHALLOC_DEBUG is defined, will record if memory is being freed and where it is being stored
	// --------------------------------------------------------------------------------
	// typename type:		type of the allocated memory
	// number bytes:		number of bytes to be allocated
	#define Malloc(type, bytes) pha::Internal_Malloc<type>(bytes, __FILE__, __LINE__)
	#else
	// Allocates a block of memory into code of a specific size and returns a typed pointer to the memory
	// if PHALLOC_DEBUG is defined, will record if memory is being freed and where it is being stored
	// --------------------------------------------------------------------------------
	// typename type:		type of the allocated memory
	// number bytes:		number of bytes to be allocated
	#define PHA_MALLOC(type, bytes) pha::Internal_Malloc<type>(bytes, __FILE__, __LINE__)
	#endif


	// DO NOT CALL Internal_Calloc<typename>(size_t, const char*, size_t), USE PHA_CALLOC(typename, number) MACRO INSTEAD
	template<typename T>
	static inline T* Internal_Calloc(size_t numElements, const char* file, size_t line) 
	{ 
		#ifndef PHALLOC_SPEED
		void* mem = calloc(numElements, sizeof(T));
		if (!mem)
		{
			std::cerr << "PHALLOC ERROR: Out of Memory! Tried to PHA_CALLOC " << bytes << " bytes in " << file << " on line " << line << std::endl;
			exit(EXIT_FAILURE);
		}

		return static_cast<T*>(mem); 
		#else
		return static_cast<T*>(calloc(numElements, sizeof(T)));
		#endif
	}

	#ifdef PHALLOC_EZ_NAMES
	// Allocates a block of memory into code of a specific size, initalizes all values to zero, and returns a typed pointer to the memory
	// if PHALLOC_DEBUG is defined, will record if memory is being freed and where it was allocated
	// --------------------------------------------------------------------------------
	// typename type:		type of the allocated memory
	// number numElements:	number of elements to be allocated
	#define Calloc(type, numElements) pha::Internal_Calloc<type>(numElements, __FILE__, __LINE__)
	#else
	// Allocates a block of memory into code of a specific size, initalizes all values to zero, and returns a typed pointer to the memory
	// if PHALLOC_DEBUG is defined, will record if memory is being freed and where it was allocated
	// --------------------------------------------------------------------------------
	// typename type:		type of the allocated memory
	// number numElements:	number of elements to be allocated
	#define PHA_CALLOC(type, numElements) pha::Internal_Calloc<type>(numElements, __FILE__, __LINE__)
	#endif

	// DO NOT CALL Internal_ReAlloc<typename>(T*, size_t, const char*, size_t), USE PHA_REALLOC(typename, type*, number) MACRO INSTEAD
	template<typename T>
	static inline T* Internal_ReAlloc(T* memBlock, size_t bytes, const char* file, size_t line)
	{
		#ifndef PHALLOC_SPEED
		if (bytes % sizeof(T) != 0) // Would a cast work?
		{
			std::cerr << "PHALLOC ERROR: Truncated Memory Cast! Tried to PHA_REALLOC pointer at address " memBlock << " to a new size of " << bytes << " bytes in " << file << " on line " << line << ", which could not be cast back to a(n)" << typeid(T).name() << std::endl;
			exit(EXIT_FAILURE);
		}

		void* reallocdMem = realloc(static_cast<void*>(memBlock), bytes);
		if (!reallocdMem)
		{
			std::cerr << "PHALLOC ERROR: Failed Reallocation! Tried to PHA_REALLOC pointer at address " memBlock << " to a new size of " << bytes << " bytes in " << file << " on line " << line << std::endl;
			exit(EXIT_FAILURE);
		}

		return static_cast<T*>(reallocdMem); 
		#else
		return static_cast<T*>(realloc(static_cast<void*>(memBlock)), bytes);
		#endif
	}

	#ifdef PHALLOC_EZ_NAMES
	// Re-Allocates a block of memory to a new size and returns a typed pointer to the memory
	// --------------------------------------------------------------------------------
	// typename type:		type of the allocated memory
	// type* memBlock:		pointer to the allocated memory
	// number bytes:		number of bytes to be allocated
	#define ReAlloc(type, memBlock, bytes) pha::Internal_ReAlloc<type>(memBlock, bytes, __FILE__, __LINE__)
	#else
	// Re-Allocates a block of memory to a new size and returns a typed pointer to the memory
	// --------------------------------------------------------------------------------
	// typename type:		type of the allocated memory
	// type* memBlock:		pointer to the allocated memory
	// number bytes:		number of bytes to be allocated
	#define PHA_REALLOC(type, memBlock, bytes) pha::Internal_ReAlloc<type>(memBlock, bytes, __FILE__, __LINE__)
	#endif

	// DO NOT CALL Internal_Free<typename>(T*), USE PHA_FREE(typename, type*) MACRO INSTEAD
	template<typename T>
	static inline void Internal_Free(T* memBlock)
	{
		free(static_cast<void*>(memBlock));
		memBlock = NULL;
	}

	#ifdef PHALLOC_EZ_NAMES
	// Frees a block of allocated memory
	// --------------------------------------------------------------------------------
	// typename type:		type of the allocated memory
	// type* memBlock:		pointer to the allocated memory
	#define Free(type, memBlock), pha::Internal_Free<type>(memBlock)
	#else
	// Frees a block of allocated memory
	// --------------------------------------------------------------------------------
	// typename type:		type of the allocated memory
	// type* memBlock:		pointer to the allocated memory
	#define PHA_FREE(type, memBlock), pha::Internal_Free<type>(memBlock)
	#endif
}

#endif