////////////////////////////////////////////////////////////////////////////////
//
// $LastChangedBy$
// $LastChangedDate$
// $LastChangedRevision$
//
////////////////////////////////////////////////////////////////////////////////

#include "PathPlannerBase.h"
#include "FileSystem.h"
#include "IGame.h"

#include "RenderBuffer.h"

PathPlannerBase::PathPlannerBase()
{
	mNavigationBounds.Min = Vector3f( -1000.0f, -1000.0f, -1000.0f );
	mNavigationBounds.Max = Vector3f(  1000.0f,  1000.0f,  1000.0f );
}

PathPlannerBase::~PathPlannerBase()
{
}

void PathPlannerBase::InitCommands()
{
	SetEx("nav_logfailedpath", "Saves info about failed path attempts for debugging.",
		this, &PathPlannerBase::cmdLogFailedPaths);
	SetEx("nav_showfailedpath", "Render a failed path by its index.",
		this, &PathPlannerBase::cmdShowFailedPaths);
	SetEx("nav_benchmarkpathfinder", "Render a failed path by its index.",
		this, &PathPlannerBase::cmdBenchmarkPathFind);
	SetEx("nav_benchmarkgetnavpoint", "Render a failed path by its index.",
		this, &PathPlannerBase::cmdBenchmarkGetNavPoint);
	SetEx("nav_resaveall", "Re-save all nav files to the newest file format..",
		this, &PathPlannerBase::cmdResaveNav);
}

void PathPlannerBase::cmdLogFailedPaths(const StringVector &_args)
{
	if(!m_PlannerFlags.CheckFlag(NAV_VIEW))
		return;

	const char *strUsage[] =
	{
		"nav_logfailedpath enable[bool]"
		"> enable: Enable failed path logging. true/false/on/off/1/0",
	};

	CHECK_NUM_PARAMS(_args, 2, strUsage);
	CHECK_BOOL_PARAM(bEnable, 1, strUsage);

	m_PlannerFlags.SetFlag(NAV_SAVEFAILEDPATHS, bEnable);

	EngineFuncs::ConsoleMessage(va("nav_logfailedpath %s", bEnable ? "enabled" : "disabled"));
}

void PathPlannerBase::cmdShowFailedPaths(const StringVector &_args)
{
	if(!m_PlannerFlags.CheckFlag(NAV_VIEW))
		return;

	const char *strUsage[] =
	{
		"nav_showfailedpath #"
		"> #: Index of path to toggle rendering.",
		"> enable: Enable nav rendering. true/false/on/off/1/0",
		"",
	};

	if(_args.size() != 3)
	{
		PRINT_USAGE(strUsage);

		EngineFuncs::ConsoleMessage("Failed Paths");
		EngineFuncs::ConsoleMessage("------------");
		int iIndex = 0;
		FailedPathList::iterator it = m_FailedPathList.begin(), itEnd = m_FailedPathList.end();
		for(;it != itEnd; ++it)
		{
			FailedPath &fp = (*it);
			EngineFuncs::ConsoleMessage(
				va("%d: (%.2f,%.2f,%.2f) to (%.2f,%.2f,%.2f) %s",
				iIndex,
				fp.m_Start.X(), fp.m_Start.Y(), fp.m_Start.Z(),
				fp.m_End.X(), fp.m_End.Y(), fp.m_End.Z(),
				fp.m_Render ? "rendering" : "not rendering"));
			++iIndex;
		}
		return;
	}

	CHECK_INT_PARAM(index, 1, strUsage);
	CHECK_INT_PARAM(enable, 1, strUsage);
	if(index >= (int)m_FailedPathList.size() || index < 0)
	{
		if(!m_FailedPathList.empty())
			EngineFuncs::ConsoleMessage(va("Invalid Index, must be 0-%d", m_FailedPathList.size()));
		else
			EngineFuncs::ConsoleMessage("No failed paths to render.");
		return;
	}

	FailedPathList::iterator it = m_FailedPathList.begin();
	std::advance(it, index);
	(*it).m_Render = enable!=0;
}

void PathPlannerBase::_BenchmarkPathFinder(const StringVector &_args)
{
	EngineFuncs::ConsoleMessage("Benchmark Not Implemented!");
}
void PathPlannerBase::_BenchmarkGetNavPoint(const StringVector &_args)
{
	EngineFuncs::ConsoleMessage("Benchmark Not Implemented!");
}

void PathPlannerBase::cmdBenchmarkPathFind(const StringVector &_args)
{
	if(!m_PlannerFlags.CheckFlag(NAV_VIEW))
		return;

	_BenchmarkPathFinder(_args);
}

void PathPlannerBase::cmdBenchmarkGetNavPoint(const StringVector &_args)
{
	if(!m_PlannerFlags.CheckFlag(NAV_VIEW))
		return;

	_BenchmarkGetNavPoint(_args);
}

void PathPlannerBase::cmdResaveNav(const StringVector &_args)
{
	if(!m_PlannerFlags.CheckFlag(NAV_VIEW))
		return;

	DirectoryList wpFiles;
	FileSystem::FindAllFiles("nav/", wpFiles, va( ".*%s", _GetNavFileExtension().c_str() ).c_str() );
	for(obuint32 i = 0; i < wpFiles.size(); ++i)
	{
		const std::string &mapname = fs::basename(wpFiles[i]);

		bool bGood = false;
		if(Load(mapname))
		{
			bGood = true;
			Save(mapname);
		}

		EngineFuncs::ConsoleMessage(va("Resaving %s, %s", mapname.c_str(),bGood?"success":"failed"));
	}

	// reload current map wps
	Load(std::string(gEngineFuncs->GetMapName()));
}

void PathPlannerBase::AddFailedPath(const Vector3f &_start, const Vector3f &_end)
{
	FailedPath fp;
	fp.m_Start = _start;
	fp.m_End = _end;
	fp.m_Render = false;
	m_FailedPathList.push_back(fp);
	EngineFuncs::ConsoleMessage(va("Added failed path to log, view with nav_showfailedpath %d",
		m_FailedPathList.size()));
}

bool PathPlannerBase::Load(bool _dl)
{
	return Load(gEngineFuncs->GetMapName(),_dl);
}

void PathPlannerBase::RenderFailedPaths()
{
	Prof(RenderFailedPaths);

	if(!m_PlannerFlags.CheckFlag(NAV_SAVEFAILEDPATHS))
		return;

	for ( FailedPathList::iterator it = m_FailedPathList.begin(); 
		it != m_FailedPathList.end(); 
		++it )
	{
		FailedPath &fp = (*it);
		if(fp.m_Render)
		{
			AABB local;
			Vector3f pos;
			GameEntity ge = Utils::GetLocalEntity();
			if ( ge.IsValid() && EngineFuncs::EntityPosition( ge, pos ) && EngineFuncs::EntityLocalAABB( ge, local ) )
			{
				RenderBuffer::AddAABB( local.TranslateCopy( fp.m_Start ), COLOR::GREEN );
				RenderBuffer::AddAABB( local.TranslateCopy( fp.m_End ), COLOR::RED );
			}
		}
	}
}