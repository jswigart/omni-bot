#include "PrecompCommon.h"
#include <iostream>
#include <fstream>
#include "Omni-Bot_Types.h"
#include "Omni-Bot_BitFlags.h"

#include "DebugConsole.h"
#include "DebugWindow.h"

//////////////////////////////////////////////////////////////////////////
namespace Dbg
{
	//////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////
	std::ostream& msg(std::ostream& os)
	{  
		static_cast<DebugStreamBuffer*>(os.rdbuf())->SetMessageType(kNormal);
		return os;
	}
	std::ostream& info(std::ostream& os)
	{
		static_cast<DebugStreamBuffer*>(os.rdbuf())->SetMessageType(kInfo);
		return os;
	}
	std::ostream& warning(std::ostream& os)
	{  
		static_cast<DebugStreamBuffer*>(os.rdbuf())->SetMessageType(kWarning);
		return os;
	}
	std::ostream& error(std::ostream& os)
	{  
		static_cast<DebugStreamBuffer*>(os.rdbuf())->SetMessageType(kError);
		return os;
	}
	std::ostream& debug(std::ostream& os)
	{
		static_cast<DebugStreamBuffer*>(os.rdbuf())->SetMessageType(kDebug);
		return os;
	}

	//////////////////////////////////////////////////////////////////////////

	DebugStream dout;

	DebugStreamBuffer::DebugStreamBuffer() :
		m_Type(kNormal)
	{
		std::filebuf::open("NUL", std::ios::out); 
		m_Filter.SetFlag(kNormal);
		m_Filter.SetFlag(kInfo);
		m_Filter.SetFlag(kWarning);
		m_Filter.SetFlag(kError);
	}

	DebugStreamBuffer::~DebugStreamBuffer()
	{
	}

	void DebugStreamBuffer::open(const char fname[])
	{
		close();
		std::filebuf::open(fname ? fname : "NUL", std::ios::out | std::ios::app | std::ios::trunc);
		//_win.open(fname);
	} 

	void DebugStreamBuffer::close() 
	{
		//_win.close(); 
		std::filebuf::close();
	}

	int DebugStreamBuffer::sync()
	{
		if(m_Filter.CheckFlag(m_Type))
		{
			obColor col = COLOR::WHITE;
			switch(m_Type)
			{
			case kNormal:
				break;
			case kInfo:
				col = COLOR::LAVENDER;
				break;
			case kWarning:
				col = COLOR::ORANGE;
				break;
			case kError:
				col = COLOR::RED;
				break;
			case kDebug:
				col = COLOR::YELLOW;
				break;
			case kScript:
				col = COLOR::CYAN;
				break;
			}

			String str;
			int count = pptr() - pbase();
			str.insert(0, pbase(), count);
			Utils::StringTrimCharacters(str, "\r\n");
#ifdef ENABLE_DEBUG_WINDOW
			DW.Console.AddLine(str);
#endif
		}
		return std::filebuf::sync(); 
	}

	//////////////////////////////////////////////////////////////////////////

	DebugStream::DebugStream() : 
		std::ostream(new DebugStreamBuffer())
	{
		
	}

	DebugStream::~DebugStream() 
	{ 
		delete rdbuf(); 
	}

	void DebugStream::open(const char fname[]) 
	{
		//_buf.open(fname); 
	}

	void DebugStream::close() 
	{ 
		//_buf.close(); 
	}
};
