#ifndef _PHALLOC_HPP_
#define _PHALLOC_HPP_

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
|			PHA_FREE(type, memBlock)			-> Free(type memBlock)				 |
|																					 |
|	#define PHALLOC_WARN_DIRE	// Dump(ostream) only dumps not-freed memory		 |
*//*--------------------------------------------------------------------------------*/

#pragma region phalloc_internal_setting_macros
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

#include "stdlib.h"

#include <iostream>

#ifdef PHALLOC_DEBUG 
#include <map> 
#endif

namespace pha
{
	constexpr size_t VERSION_MAJOR = 1;
	constexpr size_t VERSION_MINOR = 1;
	constexpr size_t VERSION_REVISION = 2;
	constexpr const char* VERSION_CSTRING = "1.1.2";

	// Generates a version id based on input. Returns current version id by default.
	static inline constexpr size_t VersionNumber(size_t major = VERSION_MAJOR, size_t minor = VERSION_MINOR, size_t revision = VERSION_REVISION) { return ((major << 16) | (minor << 8) | revision); }

	namespace itl
	{
		#ifdef PHALLOC_DEBUG
		struct mem_instance
		{
			const char* file;
			bool freed, reallocated;
			int line;
		};

		extern std::map<void*, mem_instance> instanceList;
		#endif

		template<typename T>
		static inline T* Malloc(size_t bytes, const char* file, int line);

		template<typename T>
		static inline T* Calloc(size_t numElements, const char* file, int line);

		template<typename T>
		static inline T* ReAlloc(T* memBlock, size_t bytes, const char* file, int line);

		static inline void Free(void* memBlock);
	}

	#ifdef PHALLOC_DEBUG
	static inline void Dump(std::ostream& stream = std::clog);
	#endif

	#ifdef PHALLOC_IMPLEMENTATION
	namespace itl
	{
		#ifdef PHALLOC_DEBUG
		// DO NOT USE instanceList
		std::map<void*, mem_instance> instanceList;
		#endif

		// DO NOT CALL Malloc<typename>(size_t, const char*, size_t), USE PHA_MALLOC(typename, number) MACRO INSTEAD
		template<typename T>
		static inline T* Malloc(size_t bytes, const char* file, size_t line)
		{
			#ifndef PHALLOC_SPEED
			if (bytes % sizeof(T) != 0) // Would a cast work?
			{
				std::cerr << "PHALLOC ERROR: Truncated Memory Cast! Tried to PHA_MALLOC " << bytes << " bytes in " << file << " on line " << line << " which could not be cast to a(n) " << typeid(T).name() << "which has a size of " << sizeof(T) << std::endl;
				exit(EXIT_FAILURE);
			}

			void* mem = malloc(bytes);
			if (!mem)
			{
				std::cerr << "PHALLOC ERROR: Out of Memory! Tried to PHA_MALLOC " << bytes << " bytes in " << file << " on line " << line << std::endl;
				exit(EXIT_FAILURE);
			}

			#ifdef PHALLOC_DEBUG
			instanceList.insert(std::pair<void*, mem_instance>(mem, mem_instance{ file, false, false, line }));
			#endif

			return static_cast<T*>(mem);
			#else
			return static_cast<T*>(malloc(bytes));
			#endif
		}

		// DO NOT CALL Calloc<typename>(size_t, const char*, size_t), USE PHA_CALLOC(typename, number) MACRO INSTEAD
		template<typename T>
		static inline T* Calloc(size_t numElements, const char* file, size_t line)
		{
			#ifndef PHALLOC_SPEED
			void* mem = calloc(numElements, sizeof(T));
			if (!mem)
			{
				std::cerr << "PHALLOC ERROR: Out of Memory! Tried to PHA_CALLOC " << sizeof(T) * numElements << " bytes in " << file << " on line " << line << std::endl;
				exit(EXIT_FAILURE);
			}

			#ifdef PHALLOC_DEBUG
			instanceList.insert(std::pair<void*, mem_instance>(mem, mem_instance{ file, false, false, line }));
			#endif

			return static_cast<T*>(mem);
			#else
			return static_cast<T*>(calloc(numElements, sizeof(T)));
			#endif
		}

		// DO NOT CALL ReAlloc<typename>(T*, size_t, const char*, size_t), USE PHA_REALLOC(typename, type*, number) MACRO INSTEAD
		template<typename T>
		static inline T* ReAlloc(T* memBlock, size_t bytes, const char* file, size_t line)
		{
			#ifdef PHALLOC_DEBUG
			instanceList.erase(static_cast<void*>(memBlock));
			#endif

			#ifndef PHALLOC_SPEED
			if (bytes % sizeof(T) != 0) // Would a cast work?
			{
				std::cerr << "PHALLOC ERROR: Truncated Memory Cast! Tried to PHA_REALLOC pointer at address " << memBlock << " to a new size of " << bytes << " bytes in " << file << " on line " << line << ", which could not be cast back to a(n)" << typeid(T).name() << "which has a size of " << sizeof(T) << std::endl;
				exit(EXIT_FAILURE);
			}

			void* reallocdMem = realloc(static_cast<void*>(memBlock), bytes);
			if (!reallocdMem)
			{
				std::cerr << "PHALLOC ERROR: Failed Reallocation! Tried to PHA_REALLOC pointer at address " << memBlock << " to a new size of " << bytes << " bytes in " << file << " on line " << line << std::endl;
				exit(EXIT_FAILURE);
			}

			#ifdef PHALLOC_DEBUG
			instanceList.insert(std::pair<void*, mem_instance>(reallocdMem, mem_instance{ file, false, true, line }));
			#endif

			return static_cast<T*>(reallocdMem);
			#else
			return static_cast<T*>(realloc(static_cast<void*>(memBlock)), bytes);
			#endif
		}

		// DO NOT CALL Free(void*), USE PHA_FREE(typename, type*) MACRO INSTEAD
		static inline void Free(void* memBlock)
		{
			#ifdef PHALLOC_DEBUG
				#ifdef PHALLOC_WARN_DIRE
				instanceList.erase(memBlock);
				#else
				instanceList.at(memBlock).freed = true;
				#endif
			#endif

			free(memBlock);
			memBlock = NULL;
		}
	}

		#ifdef PHALLOC_DEBUG
		// Dumps collected data into a given stream with the format:
		// "(pointer): Allocated/Reallocated in (filepath) on line (line). Was/Was not freed."
		static inline void Dump(std::ostream& stream)
		{
			bool doClog = false;
			if (!stream.good())
			{
				std::cerr << "PHALLOC ERROR: Failed to dump to stream! Attempting to output to std::clog..." << std::endl;
				doClog = true;
			}

			for (const std::pair<void*, itl::mem_instance>& inst : itl::instanceList)
			{
				char allocOrRealloc[12] = "Allocated";
				if (inst.second.reallocated)
					strcpy_s(allocOrRealloc, sizeof(allocOrRealloc), "Reallocated");

				char freedOrNot[8] = "Was not";
				#ifndef PHALLOC_WARN_DIRE
				if (inst.second.freed)
					strcpy_s(freedOrNot, sizeof(freedOrNot), "Was");
				#endif

				if (doClog)
					std::clog << inst.first << ": " << allocOrRealloc << " in " << inst.second.file << " on line " << inst.second.line << ". " << freedOrNot << " freed." << std::endl;
				else
					stream << inst.first << ": " << allocOrRealloc << " in " << inst.second.file << " on line " << inst.second.line << ". " << freedOrNot << " freed." << std::endl;
			}
		}
		#endif
	#endif


	#ifdef PHALLOC_EZ_NAMES
	// Allocates a block of memory into code of a specific size and returns a typed pointer to the memory
	// if PHALLOC_DEBUG is defined, will record if memory is being freed and where it is being stored
	// --------------------------------------------------------------------------------
	// typename type:		type of the allocated memory
	// number bytes:		number of bytes to be allocated
	#define Malloc(type, bytes) pha::itl::Malloc<type>(bytes, __FILE__, __LINE__)

	// Allocates a block of memory into code of a specific size, initalizes all values to zero, and returns a typed pointer to the memory
	// if PHALLOC_DEBUG is defined, will record if memory is being freed and where it was allocated
	// --------------------------------------------------------------------------------
	// typename type:		type of the allocated memory
	// number numElements:	number of elements to be allocated
	#define Calloc(type, numElements) pha::itl::Calloc<type>(numElements, __FILE__, __LINE__)

	// Re-Allocates a block of memory to a new size and returns a typed pointer to the memory
	// --------------------------------------------------------------------------------
	// typename type:		type of the allocated memory
	// type* memBlock:		pointer to the allocated memory
	// number bytes:		number of bytes to be allocated
	#define ReAlloc(type, memBlock, bytes) pha::itl::ReAlloc<type>(memBlock, bytes, __FILE__, __LINE__)

	// Frees a block of allocated memory
	// --------------------------------------------------------------------------------
	// typename type:		type of the allocated memory
	// type* memBlock:		pointer to the allocated memory
	#define Free(memBlock) pha::itl::Free(static_cast<void*>(memBlock))
	#else
	// Allocates a block of memory into code of a specific size and returns a typed pointer to the memory
	// if PHALLOC_DEBUG is defined, will record if memory is being freed and where it is being stored
	// --------------------------------------------------------------------------------
	// typename type:		type of the allocated memory
	// number bytes:		number of bytes to be allocated
	#define PHA_MALLOC(type, bytes) pha::itl::Malloc<type>(bytes, __FILE__, __LINE__)

	// Allocates a block of memory into code of a specific size, initalizes all values to zero, and returns a typed pointer to the memory
	// if PHALLOC_DEBUG is defined, will record if memory is being freed and where it was allocated
	// --------------------------------------------------------------------------------
	// typename type:		type of the allocated memory
	// number numElements:	number of elements to be allocated
	#define PHA_CALLOC(type, numElements) pha::itl::Calloc<type>(numElements, __FILE__, __LINE__)

	// Re-Allocates a block of memory to a new size and returns a typed pointer to the memory
	// --------------------------------------------------------------------------------
	// typename type:		type of the allocated memory
	// type* memBlock:		pointer to the allocated memory
	// number bytes:		number of bytes to be allocated
	#define PHA_REALLOC(type, memBlock, bytes) pha::itl::ReAlloc<type>(memBlock, bytes, __FILE__, __LINE__)

	// Frees a block of allocated memory
	// --------------------------------------------------------------------------------
	// typename type:		type of the allocated memory
	// type* memBlock:		pointer to the allocated memory
	#define PHA_FREE(memBlock) pha::itl::Free(static_cast<void*>(memBlock))
	#endif
}

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