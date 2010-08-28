////////////////////////////////////////////////////////////////////////////////
// 
// $LastChangedBy$
// $LastChangedDate$
// $LastChangedRevision$
//
////////////////////////////////////////////////////////////////////////////////

#include "PrecompCommon.h"
#include "MemoryManager.h"

//////////////////////////////////////////////////////////////////////////

MemoryManager::MemoryManager() :
	m_Heap		(0),
	m_HeapSize	(0)
{
}

MemoryManager::~MemoryManager()
{
	ShutdownHeap();
}

bool MemoryManager::InitHeap(obuint32 _memorysize)
{
	if(!m_Heap)
	{
		m_HeapSize = _memorysize;
		m_Heap = new char[m_HeapSize];

		MemNodeT node;
		memset(&node, 0, sizeof(node));
		node.m_pBlock = m_Heap;
		node.m_Size = m_HeapSize;

		m_MemoryNodes.push_back(node);
	}
	else
	{
		return false;
	}	

	return true;
}

void MemoryManager::ShutdownHeap()
{
	OB_ARRAY_DELETE(m_Heap);
	m_HeapSize = 0;
}

void *MemoryManager::_Malloc(obuint32 _numBytes, obuint32 _line, char *_file)
{
	OBASSERT(m_Heap, "Memory Manager Not Initialized!");

#ifdef _DEBUG
	// Extract just the filename.
	char *pFileName = _file, *pTemp = 0;
	for(;;)
	{
		pTemp = strchr(pFileName, '\\');
		if(!pTemp)
			break;
		pFileName = pTemp+1;
	}
#endif

	// Search the list for the first block that can fit.
	MemoryNodes::iterator mIt = m_MemoryNodes.begin(), mItEnd = m_MemoryNodes.end();
	for(; mIt != mItEnd; ++mIt)
	{
		if(!mIt->m_IsActive)
		{
			if(mIt->m_Size == _numBytes)
			{
				// Perfect fit, make it active and return the block
				mIt->m_IsActive = true;
				return mIt->m_pBlock;
			}
			else if(mIt->m_Size > _numBytes)
			{
				// The block is bigger than requested, so split it.
				obuint32 iBlockSize = mIt->m_Size;

				// Resize this node to fit the requested size.
				mIt->m_Size = _numBytes;
				mIt->m_IsActive = true;

				// Create a new node with the remaining size.
				MemNodeT newnode;
#ifdef _DEBUG
				// Set the debug info.
				Utils::StringCopy(mIt->m_FileName, pFileName, FILENAME_SIZE);
				mIt->m_LineNum = _line;

				// Clear the new nodes debug info.
				newnode.m_FileName[0] = 0;
				newnode.m_LineNum = 0;
#endif
				newnode.m_IsActive = false;
				newnode.m_Size = iBlockSize - _numBytes;
				newnode.m_pBlock = (char*)mIt->m_pBlock + newnode.m_Size;

				void *pReturnBlock = mIt->m_pBlock;
								
				// We're not the last node, so insert instead.
				m_MemoryNodes.insert(++mIt, newnode);
				
				// Return the appropriately requested block
				return pReturnBlock;
			}
		}
	}
	return NULL;
}

void MemoryManager::Free(void *_memory)
{
	MemoryNodes::iterator mIt = m_MemoryNodes.begin(), mItEnd = m_MemoryNodes.end();
	for(; mIt != mItEnd; ++mIt)
	{
		if(mIt->m_pBlock == _memory)
		{
			mIt->m_IsActive = false;
			_MergeBlocks();
			return;
		}
	}
	OBASSERT(0, "Attempted to Free a memory block that doesn't belong to this Memory Manager");
}

void MemoryManager::_MergeBlocks()
{
	MemoryNodes::iterator mIt = m_MemoryNodes.begin();
	for(; mIt != m_MemoryNodes.end(); ++mIt)
	{
		// If it's not active, look ahead for all other inactive nodes in series
		if(!mIt->m_IsActive)
		{
			MemoryNodes::iterator mItTemp = mIt, mInActiveEnd = mIt;
			while(mItTemp != m_MemoryNodes.end())
			{
				// Mark a series of inactive nodes.
				if(!mItTemp->m_IsActive)
					mInActiveEnd = mItTemp;
				else
					break;
				++mItTemp;
			}

			// If we found a range of inactive nodes, merge them.
			if(mIt != mInActiveEnd)
			{
				MemoryNodes::iterator mInActiveFirst = mIt, mInActiveLast = mInActiveEnd;

				// Create a new node from the merged nodes.
				MemNodeT node;
#ifdef _DEBUG
				node.m_FileName[0] = 0;
				node.m_LineNum = 0;
#endif
				node.m_IsActive = false;
				node.m_Size = mInActiveFirst->m_Size;
				node.m_pBlock = mInActiveFirst->m_pBlock;

				// Merge the remaining nodes.
				++mInActiveFirst;
				while(1)
				{
					node.m_Size += mInActiveFirst->m_Size;

					if(mInActiveFirst == mInActiveLast)
						break;

					++mInActiveFirst;
				}

				// Remove the old ones.
				mIt = m_MemoryNodes.erase(mIt, ++mInActiveEnd);

				// Insert the new node.
				mIt = m_MemoryNodes.insert(mIt, node);				
			}
			else
			{
				// Mark this node as available.
				mIt->m_IsActive = false;
#ifdef _DEBUG
				mIt->m_FileName[0] = 0;
				mIt->m_LineNum = 0;
#endif
			}
			break;
		}
	}
}

void MemoryManager::MemDumpInfo()
{
	printf("---------------------------------------------------------------\n");
	printf("  #  Address | File\t\t| Line\t| byte size\t| Used\n");
	printf("---------------------------------------------------------------\n");
#if _DEBUG
	int i = 0;
	MemoryNodes::iterator mIt = m_MemoryNodes.begin(), mItEnd = m_MemoryNodes.end();
	for(; mIt != mItEnd; ++mIt)
	{
		printf("%.4d %p  %16s\t  %.4d\t  %4s\t  %d\n", 
			i++,
			mIt->m_pBlock,
			mIt->m_FileName ? mIt->m_FileName : "",
			mIt->m_LineNum,
			Utils::FormatByteString(mIt->m_Size).c_str(),
			mIt->m_IsActive);
	}
#endif

	printf("\n");
}
