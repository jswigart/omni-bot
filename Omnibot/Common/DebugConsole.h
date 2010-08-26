
#ifndef __DEBUGCONSOLE_H__
#define __DEBUGCONSOLE_H__

namespace Dbg
{
	class DebugStreamBuffer : public std::filebuf
	{
	public:
		
		void open(const char fname[]);
		void close();
		virtual int sync();

		void SetMessageType(MessageType _type) { m_Type = _type; }

		DebugStreamBuffer();
		~DebugStreamBuffer();
	private:
		MessageType		m_Type;
		BitFlag32		m_Filter;
	};

	class DebugStream : public std::ostream
	{
	public:
		
		DebugStream();
		~DebugStream();
		void open(const char fname[] = 0);
		void close();
	private:
		
	};

	extern DebugStream dout;

	void DrawProfiler(const Vector2f &_pos);

	//////////////////////////////////////////////////////////////////////////
	// Manipulators

	std::ostream& msg(std::ostream& os);
	std::ostream& info(std::ostream& os);
	std::ostream& warning(std::ostream& os);
	std::ostream& error(std::ostream& os);
	std::ostream& debug(std::ostream& os);
};

#endif
