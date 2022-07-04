#ifndef _PHALLOC_H_
#define _PHALLOC_H_

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

#include "stdio.h"
#include "stdlib.h"

#ifdef __cplusplus
extern "C"
{
#endif

	#define PHA_BOOL	char
	#define PHA_FALSE	0
	#define PHA_TRUE	!PHA_FALSE

	#ifdef PHALLOC_DEBUG
	typedef enum
	{
		AT_UNKNWN,
		AT_MALLOC,
		AT_CALLOC,

		AT_NUM
	} alloctype_t;

	typedef struct
	{
		const char* file;
		PHA_BOOL freed;
		size_t id;
		size_t line;
		void* memory;
		size_t memSize;
		alloctype_t type;
	} meminstance_t;

	typedef struct listnode_s
	{
		meminstance_t data;
		size_t index;
		struct listnode_s* next;
	} listnode_t;

	typedef struct
	{
		listnode_t* first;
		size_t length;
	} list_t;

	static size_t pha_internal_memInstanceIndex = 0;
	static list_t* pha_internal_memInstanceList;

	static inline meminstance_t* Pha_Internal_List_At(list_t* list, size_t index)
	{
		if (index >= list->length)
			return NULL;

		listnode_t* currNode = list->first;

		while (index != 0)
		{
			currNode = currNode->next;
			index--;
		}

		return &currNode->data;
	}

	static inline list_t* Pha_Internal_List_Create()
	{
		list_t* list = (list_t*)calloc(1, sizeof(list_t));
		if (!list)
		{
			fprintf_s(stderr, "PHALLOC ERROR: Failed to create list\n");
			exit(EXIT_FAILURE);
		}

		list->first = NULL;
		list->length = 0;
		return list;
	}

	static inline PHA_BOOL Pha_Internal_List_Empty(list_t* list)
	{
		if (list->length == 0)
			return PHA_TRUE;

		return PHA_FALSE;
	}

	static inline void Pha_Internal_ListNode_Delete(list_t* list, listnode_t* node)
	{
		free(node);
		node = NULL;
		list->length--;
	}

	static inline void Pha_Internal_List_Delete(list_t* list)
	{
		if (Pha_Internal_List_Empty(list))
			return;

		listnode_t* currNode = list->first;
		listnode_t* nextNode = NULL;

		while (list->length) // Pha_Internal_ListNode_Delete decrements list->length, no need for manual decrementation
		{
			if (currNode != NULL)
			{
				nextNode = currNode->next;
				Pha_Internal_ListNode_Delete(list, currNode);
			}
		}
	}

	static inline listnode_t* Pha_Internal_ListNode_Create(list_t* list)
	{
		listnode_t* node = (listnode_t*)calloc(1, sizeof(listnode_t));
		if (!node)
		{
			fprintf_s(stderr, "PHALLOC ERROR: Failed to creaate list node\n");
			exit(EXIT_FAILURE);
		}

		node->index = list->length;
		list->length++;
		node->next = NULL;
		return node;
	}

	static inline void Pha_Internal_List_Insert(list_t* list, meminstance_t element, size_t index)
	{
	}

	static inline meminstance_t* Pha_Internal_List_Pop(list_t* list)
	{
	}

	static inline void Pha_Internal_List_Push(list_t* list, meminstance_t element)
	{
	}

	static inline void Pha_Internal_List_Remove(list_t* list, size_t index)
	{
	}

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
		#endif

		void* mem = malloc(bytes);

		#ifndef PHALLOC_SPEED
		if (!mem)
		{
			fprintf_s(stderr, "PHALLOC ERROR: Out of Memory! Tried to PHA_MALLOC %llu bytes in %s on line %llu\n", bytes, file, line);
			exit(EXIT_FAILURE);
		}
		#endif

		#ifdef PHALLOC_DEBUG
		Pha_Internal_List_Push(pha_internal_memInstanceList, (meminstance_t){ file, PHA_FALSE, pha_internal_memInstanceIndex, line, mem, AT_MALLOC });
		pha_internal_memInstanceIndex++;
		#endif

		return mem;
	}

	#ifdef PHALLOC_EZ_NAMES
	// Allocates a block of memory into code of a specific size and returns a typed pointer to the memory
	// if PHALLOC_DEBUG is defined, will record if memory is being freed and where it is being stored
	// --------------------------------------------------------------------------------
	// typename type:		type of the allocated memory
	// number bytes:		number of bytes to be allocated
	#define Malloc(type, bytes) Pha_Internal_Malloc(bytes, __FILE__, __LINE__, sizeof(type))
	#else
	// Allocates a block of memory into code of a specific size and returns a typed pointer to the memory
	// if PHALLOC_DEBUG is defined, will record if memory is being freed and where it is being stored
	// --------------------------------------------------------------------------------
	// typename type:		type of the allocated memory
	// number bytes:		number of bytes to be allocated
	#define PHA_MALLOC(type, bytes) Pha_Internal_Malloc(bytes, __FILE__, __LINE__, sizeof(type))
	#endif

	// DO NOT CALL Pha_Internal_Calloc(size_t, const char*, size_t, size_t), USE PHA_CALLOC(typename, number) MACRO INSTEAD
	static inline void* Pha_Internal_Calloc(size_t numElements, const char* file, size_t line, size_t sizeOfType)
	{
		void* mem = calloc(numElements, sizeOfType);

		#ifndef PHALLOC_SPEED
		if (!mem)
		{
			fprintf_s(stderr, "PHALLOC ERROR: Out of Memory! Tried to PHA_CALLOC %llu bytes in %s on line %llu\n", numElements, file, line);
			exit(EXIT_FAILURE);
		}
		#endif

		#ifdef PHALLOC_DEBUG
		Pha_Internal_List_Push(pha_internal_memInstanceList, (meminstance_t){ file, PHA_FALSE, pha_internal_memInstanceIndex, line, mem, AT_CALLOC });
		pha_internal_memInstanceIndex++;
		#endif

		return mem;
	}

	#ifdef PHALLOC_EZ_NAMES
	// Allocates a block of memory into code of a specific size, initalizes all values to zero, and returns a typed pointer to the memory
	// if PHALLOC_DEBUG is defined, will record if memory is being freed and where it was allocated
	// --------------------------------------------------------------------------------
	// typename type:		type of the allocated memory
	// number numElements:	number of elements to be allocated
	#define Calloc(type, numElements) (type*)Pha_Internal_Calloc(numElements, __FILE__, __LINE__, sizeof(type))
	#else
	// Allocates a block of memory into code of a specific size, initalizes all values to zero, and returns a typed pointer to the memory
	// if PHALLOC_DEBUG is defined, will record if memory is being freed and where it was allocated
	// --------------------------------------------------------------------------------
	// typename type:		type of the allocated memory
	// number numElements:	number of elements to be allocated
	#define PHA_CALLOC(type, numElements) (type*)Pha_Internal_Calloc(numElements, __FILE__, __LINE__, sizeof(type))
	#endif

	// DO NOT CALL Pha_Internal_ReAlloc(void*, size_t, const char*, size_t, size_t), USE PHA_REALLOC(typename, type*, number) MACRO INSTEAD
	static inline void* Pha_Internal_ReAlloc(void* memBlock, size_t bytes, const char* file, size_t line, size_t sizeOfType)
	{
		#ifndef PHALLOC_SPEED
		if (bytes % sizeOfType != 0) // Would a cast work?
		{
			fprintf_s(stderr, "PHALLOC ERROR: Truncated Memory Cast! Tried to PHA_REALLOC pointer at address %p to a new size of %llu bytes in %s on line %llu, which could not be cast to a type of size %llu\n", memBlock, bytes, file, line, sizeOfType);
			exit(EXIT_FAILURE);
		}
		#endif

		void* reallocdMem = realloc(memBlock, bytes);

		#ifndef PHALLOC_SPEED
		if (!reallocdMem)
		{
			fprintf_s(stderr, "PHALLOC ERROR: Failed Reallocation! Tried to PHA_REALLOC pointer at address %p to a new size of %llu bytes in %s on line %llu\n", memBlock, bytes, file, line);	
			exit(EXIT_FAILURE);
		}
		#endif

		return reallocdMem;
	}

	#ifdef PHALLOC_EZ_NAMES
	// Re-Allocates a block of memory to a new size and returns a typed pointer to the memory
	// --------------------------------------------------------------------------------
	// typename type:		type of the allocated memory
	// type* memBlock:		pointer to the allocated memory
	// number bytes:		number of bytes to be allocated
	#define ReAlloc(type, memBlock, bytes) (type*)Pha_Internal_ReAlloc((void*)memBlock, bytes, __FILE__, __LINE__, sizeof(type))
	#else
	// Re-Allocates a block of memory to a new size and returns a typed pointer to the memory
	// --------------------------------------------------------------------------------
	// typename type:		type of the allocated memory
	// type* memBlock:		pointer to the allocated memory
	// number bytes:		number of bytes to be allocated
	#define PHA_REALLOC(type, memBlock, bytes) (type*)Pha_Internal_ReAlloc((void*)memBlock, bytes, __FILE__, __LINE__, sizeof(type))
	#endif

	// DO NOT CALL Pha_Internal_Free(void*), USE PHA_FREE(typename, type*) MACRO INSTEAD
	static inline void Pha_Internal_Free(void* memBlock)
	{
		#ifdef PHALLOC_DEBUG
		for (size_t i = 0; i < pha_internal_memInstanceList->length; i++)
		{
			if (Pha_Internal_List_At(pha_internal_memInstanceList, i)->id == 0)
			{
				Pha_Internal_List_At(pha_internal_memInstanceList, i)->freed = PHA_TRUE;
				break;
			}
		}
		#endif

		free(memBlock);
		memBlock = NULL;
	}

	#ifdef PHALLOC_EZ_NAMES
	// Frees a block of allocated memory
	// --------------------------------------------------------------------------------
	// type* memBlock:		pointer to the allocated memory
	#define Free(memBlock) Pha_Internal_Free((void*)memBlock)
	#else
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