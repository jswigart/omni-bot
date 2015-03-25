////////////////////////////////////////////////////////////////////////////////
//
// $LastChangedBy: jswigart $
// $LastChangedDate: 2010-08-28 00:12:05 -0500 (Sat, 28 Aug 2010) $
// $LastChangedRevision: 32 $
//
////////////////////////////////////////////////////////////////////////////////

#ifndef __RECAST_INTERFACES_H__
#define __RECAST_INTERFACES_H__

#include "DebugDraw.h"
#include "Recast.h"
#include "RecastDump.h"
#include "Timer.h"
#include "FileSystem.h"

// Recast build context.
class RecastBuildContext : public rcContext
{
public:
	RecastBuildContext();
	virtual ~RecastBuildContext();

	// Dumps the log to stdout.
	void dumpLog(const char* format, ...);
	// Returns number of log messages.
	int getLogCount() const;
	// Returns log message text.
	const char* getLogText(const int i) const;

protected:
	// Virtual functions for custom implementations.
	virtual void doResetLog();
	virtual void doLog(const rcLogCategory /*category*/, const char* /*msg*/, const int /*len*/);
	virtual void doResetTimers();
	virtual void doStartTimer(const rcTimerLabel /*label*/);
	virtual void doStopTimer(const rcTimerLabel /*label*/);
	virtual int doGetAccumulatedTime(const rcTimerLabel /*label*/) const;

private:
	Timer			 mtimers[RC_MAX_TIMERS];
	float			 maccTime[RC_MAX_TIMERS];

	struct Msg
	{
		rcLogCategory	mCategory;
		std::string		mMsg;
	};
	std::vector< Msg >  messages;
};

// stdio file implementation.
class rcFileIO : public duFileIO
{
	File mfile;
	int mmode;
public:
	rcFileIO();
	virtual ~rcFileIO();
	bool openForWrite(const char* path);
	bool openForRead(const char* path);
	virtual bool isWriting() const;
	virtual bool isReading() const;
	virtual bool write(const void* ptr, const size_t size);
	virtual bool read(void* ptr, const size_t size);
};

#endif
