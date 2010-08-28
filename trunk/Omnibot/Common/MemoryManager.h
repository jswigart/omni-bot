////////////////////////////////////////////////////////////////////////////////
// 
// $LastChangedBy$
// $LastChangedDate$
// $LastChangedRevision$
//
////////////////////////////////////////////////////////////////////////////////

#ifndef __MEMORYMANAGER_H__
#define __MEMORYMANAGER_H__

#include "Omni-Bot_BasicTypes.h"
#include <list>

#ifndef _DEBUG
#define MemMalloc(_mgr, _numbytes) (_mgr._Malloc(_numbytes, 0, 0))
#else
#define MemMalloc(_mgr, _numbytes) (_mgr._Malloc(_numbytes, __LINE__, __FILE__))
#endif

class MemoryManager
{
public:

	enum
	{
		FILENAME_SIZE = 16
	};

	typedef struct
	{
		obuint32	m_Size		: 31;
		obuint32	m_IsActive	: 1;
		void*		m_pBlock;
#if _DEBUG 
		obuint32	m_LineNum;
		char		m_FileName[FILENAME_SIZE];
#endif
	} MemNodeT;

	bool InitHeap(obuint32 _memorysize);
	void ShutdownHeap();

	void *_Malloc(obuint32 _numBytes, obuint32 _line, char *_file);
	void Free(void *_memory);

	void MemDumpInfo();

	MemoryManager();
	~MemoryManager();
private:
	void _MergeBlocks();

	void*		m_Heap;
	obuint32	m_HeapSize;

	typedef std::list<MemNodeT> MemoryNodes;
	MemoryNodes	m_MemoryNodes;
};

#endif
