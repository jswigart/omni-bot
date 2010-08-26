////////////////////////////////////////////////////////////////////////////////
// 
// $LastChangedBy: drevil $
// $LastChangedDate: 2008-07-05 03:18:03 -0700 (Sat, 05 Jul 2008) $
// $LastChangedRevision: 3023 $
//
////////////////////////////////////////////////////////////////////////////////

#ifndef __CHUNKEDFILE_H__
#define __CHUNKEDFILE_H__

class ChunkedFile
{
public:

	bool OpenForWrite(const char *_name, bool _append = false);
	bool OpenForRead(const char *_name);
	void Close();

	bool FirstChunk(obuint32 &_chunk, obuint32 &_size);
	bool NextChunk(obuint32 &_chunk, obuint32 &_size);

	bool WriteChunkHdr(obuint32 _chunk);
	/*bool WriteChunkData(obint32 _num);
	bool WriteChunkData(obReal _num);*/
	bool WriteChunkData(const void *_data, obuint32 _size); // remove?

	ChunkedFile();
	~ChunkedFile();
private:
	File	m_File;
	String	m_ChunkData;
};

#endif
