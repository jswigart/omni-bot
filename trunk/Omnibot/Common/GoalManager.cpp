////////////////////////////////////////////////////////////////////////////////
// 
// $LastChangedBy$
// $LastChangedDate$
// $LastChangedRevision$
//
////////////////////////////////////////////////////////////////////////////////

#include "PrecompCommon.h"
#include "GoalManager.h"
#include "NavigationFlags.h"
#include "Waypoint.h"
#include "Client.h"
#include "ScriptManager.h"
#include "NavigationManager.h"
#include "MapGoalDatabase.h"

#include "gmUtilityLib.h"

//////////////////////////////////////////////////////////////////////////

enum { MapGoalVersion = 1 };

//////////////////////////////////////////////////////////////////////////

class PriorityGreaterThan
{
public:
	bool operator()(MapGoalPtr _goal1, MapGoalPtr _goal2)
	{
		obReal fG1Priority = m_Client ? 
			_goal1->GetPriorityForClient(m_Client) :
			_goal1->GetDefaultPriority();

		obReal fG2Priority = m_Client ? 
			_goal2->GetPriorityForClient(m_Client) : 
			_goal2->GetDefaultPriority();

		return fG1Priority > fG2Priority;
	}

	// functions for equal_range
	bool operator()(const obReal _priority, MapGoalPtr _goal2)
	{
		obReal fG2Priority = m_Client ? 
			_goal2->GetPriorityForClient(m_Client) : 
			_goal2->GetDefaultPriority();
		return _priority == fG2Priority;
	}

	PriorityGreaterThan(Client *_client) : m_Client(_client) {}
private:
	Client *m_Client;

	PriorityGreaterThan();
};

class PriorityLessThan
{
public:
	bool operator()(MapGoalPtr _goal1, MapGoalPtr _goal2)
	{
		obReal fG1Priority = m_Client ? 
			_goal1->GetPriorityForClient(m_Client) :
			_goal1->GetDefaultPriority();
		obReal fG2Priority = m_Client ? 
			_goal2->GetPriorityForClient(m_Client) : 
			_goal2->GetDefaultPriority();

		return fG1Priority < fG2Priority;
	}

	PriorityLessThan(Client *_client) : m_Client(_client) {}
private:
	Client *m_Client;
	PriorityLessThan();
};

//////////////////////////////////////////////////////////////////////////

GoalManager::Query::Query(obuint32 _type, Client *_client)
	: m_GoalType(_type)
	, m_Client(_client)
	, m_Team(0)
	, m_RoleMask(-1)
	, m_TagName(0)
	, m_SortType(SORT_BIAS)
	, m_SkipNoInProgressSlots(true)
	, m_SkipNoInUseSlots(true)
	, m_SkipDelayed(true)
	, m_SkipInUse(true)
	, m_CheckInRadius(false)
	, m_Error(QueryOk)
{
	Bot(_client);
}

GoalManager::Query &GoalManager::Query::Type(obuint32 _type)
{
	m_GoalType = _type; 
	return *this; 
}

GoalManager::Query &GoalManager::Query::Team(int _team) 
{
	m_Team = _team; 
	return *this; 
}

GoalManager::Query &GoalManager::Query::Bot(Client *_client) 
{ 
	if(_client)
	{
		m_Client = _client; 
		m_Team = _client->GetTeam(); 
		m_RoleMask = _client->GetRoleMask();
	}
	return *this; 
}

GoalManager::Query &GoalManager::Query::TagName(const char *_tagname) 
{
	m_TagName = _tagname; 
	return *this; 
}

GoalManager::Query &GoalManager::Query::Sort(SortType _sort) 
{
	m_SortType = _sort; 
	return *this; 
}

GoalManager::Query &GoalManager::Query::Expression(const char *_exp) 
{
	if(_exp)
	{
		try
		{
			m_NameExp = boost::regex(_exp, REGEX_OPTIONS);
		}
		catch(const std::exception&e)
		{
			e;
			m_Error = QueryBadNameExpression;
		}
	}
	return *this; 
}

GoalManager::Query &GoalManager::Query::Group(const char *_group) 
{
	if(_group)
	{
		try
		{
			m_GroupExp = boost::regex(_group, REGEX_OPTIONS);
		}
		catch(const std::exception&e)
		{
			e;
			m_Error = QueryBadGroupExpression;
		}
	}
	return *this; 
}

GoalManager::Query &GoalManager::Query::RoleMask(obuint32 _i) 
{
	m_RoleMask = BitFlag32(_i); 
	return *this; 
}

//GoalManager::Query &GoalManager::Query::WeaponReq(int *_weaponIds, int _numids)
//{
//	m_WeaponIds = _weaponIds;
//	m_NumWeaponIds = _numids;
//	return *this; 
//}

GoalManager::Query &GoalManager::Query::Ent(GameEntity _ent)
{
	m_Entity = _ent;
	return *this; 
}

GoalManager::Query &GoalManager::Query::CheckInRadius(const Vector3f & pos, float radius)
{
	m_Position = pos;
	m_Radius = radius;
	m_CheckInRadius = true;
	return *this;
}

GoalManager::Query &GoalManager::Query::NoFilters()
{
	m_SkipDelayed = false;
	m_SkipNoInProgressSlots = false;
	m_SkipNoInUseSlots = false;
	m_SkipInUse = false;
	m_RoleMask = BitFlag32(-1);
	return *this;
}

GoalManager::Query &GoalManager::Query::SkipDelayed(bool _skip)
{
	m_SkipDelayed = _skip;
	return *this;
}

GoalManager::Query &GoalManager::Query::SkipNoInProgress(bool _skip)
{
	m_SkipNoInProgressSlots = _skip;
	return *this;
}

GoalManager::Query &GoalManager::Query::SkipNoInUse(bool _skip)
{
	m_SkipNoInUseSlots = _skip;
	return *this;
}
GoalManager::Query &GoalManager::Query::SkipInUse(bool _skip)
{
	m_SkipInUse = _skip;
	return *this;
}
bool GoalManager::Query::GetBest(MapGoalPtr &_mg) 
{
	if(!m_List.empty())
	{
		_mg = m_List.front();
		return true;
	}
	return false; 
}

void GoalManager::Query::DefaultGlobalQuery()
{
	m_SkipDelayed = false;
	m_SkipNoInProgressSlots = false;
	m_SkipNoInUseSlots = false;
}

void GoalManager::Query::FromTable(gmMachine *a_machine, gmTableObject *a_table)
{
	{
		if(const char *GroupName = a_table->Get(a_machine,"Group").GetCStringSafe(0))
			Group(GroupName);
	}
	{
		gmVariable var = a_table->Get(a_machine,"Role");
		if(var.IsInt())
			RoleMask(var.GetInt());
	}
	{
		gmVariable var = a_table->Get(a_machine,"Team");
		if(var.IsInt())
			Team(var.GetInt());
	}
	{
		gmVariable var = a_table->Get(a_machine,"SkipDelayed");
		if(var.IsInt())
			SkipDelayed(var.GetInt()!=0);
	}
	{
		gmVariable var = a_table->Get(a_machine,"SkipNoInProgress");
		if(var.IsInt())
			SkipNoInProgress(var.GetInt()!=0);
	}
	{
		gmVariable var = a_table->Get(a_machine,"SkipNoInUse");
		if(var.IsInt())
			SkipNoInUse(var.GetInt()!=0);
	}
	{
		gmVariable var = a_table->Get(a_machine,"SkipInUse");
		if(var.IsInt())
			SkipInUse(var.GetInt()!=0);
	}
	{
		gmVariable var = a_table->Get(a_machine,"NoFilters");
		if(var.IsInt() && var.GetInt()!=0)
			NoFilters();
	}
}

bool GoalManager::Query::CheckForMatch(MapGoalPtr & mg)
{
	if(mg->GetDeleteMe() || mg->GetDisabled())
		return false;

	if(m_SkipInUse && mg->GetInUse())
		return false;

	if(m_GoalType && m_GoalType!=mg->GetGoalTypeHash())
		return false;

	if(m_Client && mg->GetPriorityForClient(m_Client)==0.f)
		return false;

	// only skippage is when the mapgoal has a role
	if(mg->GetRoleMask().AnyFlagSet())
	{
		if(!(mg->GetRoleMask() & m_RoleMask).AnyFlagSet())
			return false;
	}

	// make sure the client has one of the weapons needed to use the goal.
	if(m_Client && mg->GetLimitWeapons().Get().AnyFlagSet())
	{
		if(!mg->GetLimitWeapons().IsAllowed(m_Client))
			return false;
	}

	bool bOwned = false;
	if(m_Client && mg->GetOwner() == m_Client->GetGameEntity())
		bOwned = true;

	if(!bOwned && (m_Team && !mg->IsAvailable(m_Team)))
		return false;

	if(m_TagName && mg->GetTagName() != m_TagName)
		return false;

	if(m_Entity.IsValid() && mg->GetEntity() != m_Entity)
		return false;

	if(m_SkipNoInProgressSlots && mg->GetSlotsOpen(MapGoal::TRACK_INPROGRESS) < 1)
		return false;

	if(m_SkipNoInUseSlots && mg->GetSlotsOpen(MapGoal::TRACK_INUSE) < 1)
		return false;

	if(m_CheckInRadius)
	{
		if((m_Position - mg->GetPosition()).SquaredLength() > Mathf::Sqr(m_Radius))
			return false;
	}

	if(!m_NameExp.empty() && !boost::regex_match(mg->GetName(), m_NameExp))
		return false;

	if(!m_GroupExp.empty() && !boost::regex_match(mg->GetGroupName(), m_GroupExp))
		return false;

	if(m_SkipDelayed && m_Client)
	{
		if(m_Client->GetBB().GetNumBBRecords(bbk_DelayGoal, mg->GetSerialNum()) > 0)
			return false;
	}

	return true;
}

void GoalManager::Query::OnQueryStart()
{
	m_Error = QueryOk;
	m_List.resize(0);
}

void GoalManager::Query::OnQueryFinish()
{
	if(!m_List.empty())
	{
		switch(m_SortType)
		{
		case SORT_BIAS:
			{
				if(m_List.size()>1)
				{
					std::sort(m_List.begin(),m_List.end(), PriorityGreaterThan(m_Client));

					typedef std::pair<MapGoalList::iterator, MapGoalList::iterator> MapGoalRange;

					// Randomize the ranges that have the same bias.
					MapGoalList::iterator itCurrent = m_List.begin();
					while(itCurrent != m_List.end())
					{
						MapGoalRange range = std::equal_range(itCurrent, 
							m_List.end(), 
							(*itCurrent), 
							PriorityGreaterThan(m_Client));

						std::random_shuffle(range.first, range.second);
						if(range.second != m_List.end())
							itCurrent = range.second+1;
						else
							break;
					}
				}				
				break;
			}
		case SORT_RANDOM_FULL:
			{
				if(m_List.size()>1)
				{
					std::random_shuffle(m_List.begin(), m_List.end());
				}
				break;
			}
		default:
			break;
		}
	}
}

void GoalManager::Query::OnMatch(MapGoalPtr & mg)
{
	m_List.push_back(mg);
}

const char *GoalManager::Query::QueryErrorString()
{
	switch(m_Error)
	{
	case QueryBadNameExpression:
		return "Bad Name Expression.";
	case QueryBadGroupExpression:
		return "Bad Group Expression.";
	case QueryOk:
	default:
		break;
	}
	return "";
}
//////////////////////////////////////////////////////////////////////////

GoalManager *GoalManager::m_Instance = 0;

GoalManager::GoalManager()
: m_EditMode(EditNone)
{
	m_Instance = this;
}

GoalManager::~GoalManager()
{
	Shutdown();
}

GoalManager *GoalManager::GetInstance()
{
	if(!m_Instance)
		m_Instance = new GoalManager;
	return m_Instance;
}

void GoalManager::DeleteInstance()
{
	OB_DELETE(m_Instance);
}

void GoalManager::Init()
{
	InitCommands();

	//////////////////////////////////////////////////////////////////////////
	//Options::GetValue("Debug Render","DrawGoals",m_DrawGoals);
	//Options::GetValue("Debug Render","DrawRoutes",m_DrawRoutes);
	//////////////////////////////////////////////////////////////////////////
}

void GoalManager::InitGameGoals()
{
	g_EngineFuncs->GetGoals();
}

void GoalManager::Reset()
{
	m_MapGoalList.clear();
}

void GoalManager::InitCommands()
{
	SetEx("show_goals", "prints out the names of each goal", 
		this, &GoalManager::cmdGoalShow);
	SetEx("show_goalroutes", "prints route info for matching map goals", 
		this, &GoalManager::cmdGoalShowRoutes);
	SetEx("draw_goals", "draws debug information for all mapgoals", 
		this, &GoalManager::cmdGoalDraw);
	SetEx("draw_goalroutes", "draws debug routes", 
		this, &GoalManager::cmdGoalDrawRoutes);

	SetEx("goal_edit", "Begins to edit a goal.", 
		this, &GoalManager::cmdGoalEdit);
	SetEx("goal_editx", "Begins to edit a goal.", 
		this, &GoalManager::cmdGoalEditx);
	SetEx("goal_help", "Lists all help text to the console.", 
		this, &GoalManager::cmdGoalHelp);
	SetEx("goal_save", "Saves the goals to their own file.", 
		this, &GoalManager::cmdGoalSave);
	SetEx("goal_load", "Loads the goals from their own file.", 
		this, &GoalManager::cmdGoalLoad);
	SetEx("goal_create", "Creates a map goal of a provided type.", 
		this, &GoalManager::cmdGoalCreate);
	SetEx("goal_delete", "Deletes the currently selected goal.", 
		this, &GoalManager::cmdGoalDelete);
	SetEx("goal_finish", "Completes edits on the selected goal.", 
		this, &GoalManager::cmdGoalFinish);
	SetEx("goal_setproperty", "Sets the property of the goal.", 
		this, &GoalManager::cmdGoalSetProperty);
	SetEx("goal_removeall", "Removes all bot defined goals.", 
		this, &GoalManager::cmdGoalRemoveAll);
	SetEx("goal_move", "Toggle. Begins or ends moving a goal based on aim position.", 
		this, &GoalManager::cmdGoalMove);
}

bool _GoalNameLT(const MapGoalPtr _pt1, const MapGoalPtr _pt2)
{
	return _pt1->GetName() < _pt2->GetName();
}

void GoalManager::cmdGoalShow(const StringVector &_args)
{
	File f;

	String strFile, strExpression = ".*";
	bool bShowSubPriorities = false;
	if(_args.size() > 3)
	{
		strFile = _args[3];		
	}
	if(_args.size() > 1)
	{
		strExpression = _args[1];		
	}
	if(_args.size() > 2 && _args[2]=="p")
	{
		bShowSubPriorities = true;
	}

	// Open the file if a filename was provided.
	if(!strFile.empty())
	{
		char strBuffer[1024] = {};
		sprintf(strBuffer, "user/%s", strFile.c_str());
		f.OpenForWrite(strBuffer, File::Text);
	}

	GoalManager::Query qry;
	qry.NoFilters();
	qry.Expression(strExpression.c_str());
	GetGoals(qry);
	
	std::sort(qry.m_List.begin(), qry.m_List.end(), _GoalNameLT);

	EngineFuncs::ConsoleMessage("- Goal List -");
	int iCount = 1;
	String txt;
	MapGoalList::iterator it = qry.m_List.begin();
	for( ; it != qry.m_List.end(); ++it)
	{
		txt = va("%d: ", iCount++);
		txt += (*it)->GetName();

		txt += " ->";

		if(!(*it)->GetGroupName().empty())
		{
			txt += " group ";
			txt += (*it)->GetGroupName();
		}

		String role = Utils::BuildRoleName((*it)->GetRoleMask().GetRawFlags());
		if(!role.empty())
		{
			txt += " role ";
			txt += role;
		}
		
		txt += " ";

		for(int team = 1; team <= 4; ++team)
		{
			txt += (*it)->IsAvailable(team) ? "1" : "0";
		}
		txt += " serial ";
		txt += va("%d", (*it)->GetSerialNum());
		txt += " priority ";
		txt += va("%.2f", (*it)->GetDefaultPriority());
		
		EngineFuncs::ConsoleMessage(txt.c_str());
		if(bShowSubPriorities)
		{
			(*it)->GetClassPriorities().GetPriorityText(txt);
		}

		if(f.IsOpen())
		{
			f.WriteString(txt.c_str());
			f.WriteNewLine();
		}
	}
	EngineFuncs::ConsoleMessage("- End Goal List -");
}

void GoalManager::cmdGoalShowRoutes(const StringVector &_args)
{
	File f;

	String strFile, strExpression = ".*";
	if(_args.size() > 2)
	{
		strFile = _args[2];		
	}
	if(_args.size() > 1)
	{
		strExpression = _args[1];		
	}

	// Open the file if a filename was provided.
	if(!strFile.empty())
	{
		char strBuffer[1024] = {};
		sprintf(strBuffer, "user/%s", strFile.c_str());
		f.OpenForWrite(strBuffer, File::Text);
	}

	GoalManager::Query qry;
	qry.Expression(strExpression.c_str());
	GetGoals(qry);

	std::sort(qry.m_List.begin(), qry.m_List.end(), _GoalNameLT);

	EngineFuncs::ConsoleMessage("- Route List -");
	int iCount = 1;
	String txt;
	MapGoalList::iterator it = qry.m_List.begin();
	for( ; it != qry.m_List.end(); ++it)
	{
		if((*it)->GetRoutes().empty())
			continue;

		const char *pMsg = va("%d: %s", iCount++, (*it)->GetName().c_str());
		EngineFuncs::ConsoleMessage(pMsg);
		if(f.IsOpen())
		{
			f.WriteString(pMsg);
			f.WriteNewLine();
		}
		
		int iNumRoutes = 1;
		MapGoal::Routes::const_iterator rIt = (*it)->GetRoutes().begin(), rItEnd = (*it)->GetRoutes().end();
		for(; rIt != rItEnd; ++rIt)
		{
			const char *pRoute = va("    %d: %s -> %s", iNumRoutes++, 
				rIt->m_Start->GetName().c_str(), rIt->m_End->GetName().c_str());

			EngineFuncs::ConsoleMessage(pRoute);

			if(f.IsOpen())
			{
				f.WriteString(pRoute);
				f.WriteNewLine();
			}
		}
	}
	EngineFuncs::ConsoleMessage("- End Route List -");
}

int NextDrawTime = 0;

void GoalManager::cmdGoalDraw(const StringVector &_args)
{
	const char *pExpression = 0;
	bool DrawGoals = false;
	switch(_args.size())
	{
	case 3:
		pExpression = _args[2].c_str();
	case 2:
		if(Utils::StringToTrue(_args[1]))
		{
			NextDrawTime = 0; // reset draw throttle timer

			DrawGoals = true;
			break;
		}	
		else if(Utils::StringToFalse(_args[1]))
		{
			DrawGoals = false;
			break;
		}
	case 1:
		EngineFuncs::ConsoleMessage("syntax: draw_goals on/off <optional goal name expression>");
		return;
	}
	
	//////////////////////////////////////////////////////////////////////////
	boost::regex NameExp;
	try
	{
		NameExp = boost::regex(pExpression ? pExpression : ".*", REGEX_OPTIONS);
	}
	catch(const std::exception&e)
	{
		EngineFuncs::ConsoleError(va("Bad expression: %s",e.what()));
		return;
	}

	int NumSet = 0;
	for(MapGoalList::iterator it = m_MapGoalList.begin();
		it != m_MapGoalList.end();
		++it)
	{
		if(pExpression && !boost::regex_match((*it)->GetName(), NameExp))
			continue;

		(*it)->SetRenderGoal(DrawGoals);
		++NumSet;
	}
	//////////////////////////////////////////////////////////////////////////
	EngineFuncs::ConsoleMessage(va("Goals Rendering, %d %s",NumSet,DrawGoals?"Enabled":"Disabled"));
}

void GoalManager::cmdGoalDrawRoutes(const StringVector &_args)
{
	const char *pExpression = 0;
	bool DrawRoutes = false;
	switch(_args.size())
	{
	case 3:
		pExpression = _args[2].c_str();
	case 2:
		if(Utils::StringToTrue(_args[1]))
		{
			DrawRoutes = true;
			break;
		}	
		else if(Utils::StringToFalse(_args[1]))
		{
			DrawRoutes = false;
			break;
		}
	case 1:
		EngineFuncs::ConsoleMessage("syntax: draw_goals on/off <optional goal name expression>");
		return;
	}
	
	//////////////////////////////////////////////////////////////////////////
	boost::regex NameExp;
	try
	{
		NameExp = boost::regex(pExpression ? pExpression : ".*", REGEX_OPTIONS);
	}
	catch(const std::exception&e)
	{
		EngineFuncs::ConsoleError(va("Bad expression: %s",e.what()));
		return;
	}

	int NumSet = 0;
	for(MapGoalList::iterator it = m_MapGoalList.begin();
		it != m_MapGoalList.end();
		++it)
	{
		if(pExpression && !boost::regex_match((*it)->GetName(), NameExp))
			continue;

		(*it)->SetRenderRoutes(DrawRoutes);
		++NumSet;
	}
	//////////////////////////////////////////////////////////////////////////
	EngineFuncs::ConsoleMessage(va("Goal Routes Rendering, %d %s",NumSet,DrawRoutes?"Enabled":"Disabled"));
}

const char *MapGoalTable = "_MG";

bool GoalManager::Save(const String &_map, ErrorObj &_err)
{
	const String filePath	= String("nav/") + String(g_EngineFuncs->GetMapName()) + "_goals.gm";

	gmMachine *pMachine = ScriptManager::GetInstance()->GetMachine();
	
	if(!m_LoadedMapGoals)
		m_LoadedMapGoals.Set(pMachine->AllocTableObject(),pMachine);

	m_LoadedMapGoals->Set(pMachine,"Version",gmVariable(MapGoalVersion));

	int GoalsSaved = 0;
	int GoalsSavedFailed = 0;
	int GoalsSavedSkipped = 0;

	for(MapGoalList::iterator it = m_MapGoalList.begin(); 
		it != m_MapGoalList.end(); 
		++it)
	{
		if(!(*it)->GetDontSave())
		{
			if((*it)->SaveToTable(pMachine,m_LoadedMapGoals,_err))
				GoalsSaved++;
			else
				GoalsSavedFailed++;
		}
		else
		{
			GoalsSavedSkipped++;
		}
	}

	//////////////////////////////////////////////////////////////////////////

	_err.AddInfo("%d Goals Saved Successfully, %d Goals could not save, %d skipped.",
		GoalsSaved,
		GoalsSavedFailed,
		GoalsSavedSkipped);

	File outFile;
	outFile.OpenForWrite(filePath.c_str(), File::Text);
	return gmUtility::DumpTable(pMachine,outFile,MapGoalTable,m_LoadedMapGoals,gmUtility::DUMP_ALL);
}

bool GoalManager::Load(const String &_map, ErrorObj &_err)
{
	gmMachine *pMachine = ScriptManager::GetInstance()->GetMachine();

	int GoalsLoaded = 0;
	int GoalsDeferred = 0;
	int GoalsLoadedFailed = 0;

	bool LoadedOk = true;

	filePath script( "nav/%s_goals.gm", _map.c_str() );

	int ThreadId = GM_INVALID_THREAD;
	pMachine->GetGlobals()->Set(pMachine,MapGoalTable,gmVariable::s_null);
	if(ScriptManager::GetInstance()->ExecuteFile(script, ThreadId))
	{
		//////////////////////////////////////////////////////////////////////////
		// remove old goals.
		MapGoalList::iterator it = m_MapGoalList.begin();
		for( ; it != m_MapGoalList.end(); )
		{
			if(!(*it)->GetInterfaceGoal())
				it = m_MapGoalList.erase(it);
			else
				++it;
		}
		//////////////////////////////////////////////////////////////////////////

		gmVariable var = pMachine->GetGlobals()->Get(pMachine,MapGoalTable);
		gmTableObject *tbl = var.GetTableObjectSafe();
		if(tbl)
		{
			m_LoadedMapGoals.Set(tbl,pMachine);

			int Version = -1;
			gmVariable gmVersion = tbl->Get(pMachine,"Version");
			if(gmVersion.GetInt(Version,-1) && Version == MapGoalVersion)
			{
				tbl->Set(pMachine,"Version",gmVariable::s_null);

				gmTableIterator tIt;
				gmTableNode *pNode = tbl->GetFirst(tIt);
				while(pNode)
				{
					gmTableObject *mgTbl = pNode->m_value.GetTableObjectSafe();
					if(mgTbl)
					{
						const String goalName = mgTbl->Get(pMachine,"Name").GetCStringSafe("");
						const String goalType = mgTbl->Get(pMachine,"GoalType").GetCStringSafe("");
						if(!goalType.empty() && !goalName.empty())
						{
							// if the goal exists already
							gmGCRoot<gmTableObject> goalTbl(mgTbl,pMachine);
							MapGoalPtr existingGoal = GetGoal(goalName);
							if(existingGoal)
							{
								if(existingGoal->LoadFromTable(pMachine,goalTbl,_err))
								{
									GoalsLoaded++;
								}
								else
								{
									_err.AddError("Problem reloading goal : %s",goalName.c_str());
									GoalsLoadedFailed++;
								}
							}
							else
							{
								MapGoalPtr mgPtr = g_MapGoalDatabase.GetNewMapGoal(goalType);
								if(mgPtr)
								{
									if(mgPtr->LoadFromTable(pMachine,goalTbl,_err))
									{
										if(mgPtr->GetCreateOnLoad())
										{
											mgPtr->InternalInitEntityState();
											AddGoal(mgPtr);
											GoalsLoaded++;
										} 
										else
										{
											GoalsDeferred++;
										}
										
									}
									else
									{
										GoalsLoadedFailed++;
									}
								}
								else
								{
									GoalsLoadedFailed++;
									_err.AddError("Unknown Goal Type : %s",goalType.c_str());
								}
							}
						}
					}
					pNode = tbl->GetNext(tIt);
				}
			}
			else
			{
				// TODO: attempt to run versioning scripts?
				if(!gmVersion.IsInt())
					EngineFuncs::ConsoleError(va("Map Goals Script couldn't find Version field, expecting: Version = %d",MapGoalVersion));
				else
					EngineFuncs::ConsoleError(va("Map Goals Script got version %d, expecting: Version = %d",Version,MapGoalVersion));
				LoadedOk = false;

				// release the reference to the table
				m_LoadedMapGoals = NULL;
			}
		}
	}
	else // GetInstance()->ExecuteFile failed
	{
		LoadedOk = false;
	}

	pMachine->GetGlobals()->Set(pMachine,MapGoalTable,gmVariable::s_null);
	pMachine->CollectGarbage(true);

	_err.AddInfo("%d Goals Loaded, %d Goals Deferred, %d Goals could not load.",
		GoalsLoaded,GoalsDeferred,GoalsLoadedFailed);

	return LoadedOk;
}

void GoalManager::cmdGoalSave(const StringVector &_args)
{
	ErrorObj err;
	Save(g_EngineFuncs->GetMapName(),err);
	err.PrintToConsole();
}

void GoalManager::cmdGoalLoad(const StringVector &_args)
{
	ErrorObj err;
	Load(g_EngineFuncs->GetMapName(),err);
	err.PrintToConsole();
}

MapGoalPtr GoalManager::_GetGoalInRange(const Vector3f &_pos, float _radius, bool _onlydrawenabled)
{
	float NearestDistance = Utils::FloatMax;

	MapGoalPtr nearest;

	for(MapGoalList::iterator it = m_MapGoalList.begin(); 
		it != m_MapGoalList.end(); 
		++it)
	{
		if(_onlydrawenabled && !(*it)->GetRenderGoal())
			continue;

		const float Dist = Length((*it)->GetPosition(),_pos);
		if(Dist < _radius && (!nearest || Dist < NearestDistance))
		{
			nearest = (*it);
			NearestDistance = Dist;
		}
	}
	return nearest;
}

void GoalManager::_SetActiveGoal(MapGoalPtr _mg)
{
	m_EditMode = EditNone;

	if(m_ActiveGoal)
		EngineFuncs::ConsoleMessage(va("Unselected Goal :%s", m_ActiveGoal->GetName().c_str()));

	m_ActiveGoal = _mg;

	if(m_ActiveGoal)
		EngineFuncs::ConsoleMessage(va("Selected Goal :%s", m_ActiveGoal->GetName().c_str()));
	else
		EngineFuncs::ConsoleMessage("No Goal Selected");
}
void GoalManager::_UpdateEditModes()
{
	if(m_EditMode == EditMove && m_ActiveGoal)
	{
		Vector3f AimPt;
		if(Utils::GetLocalAimPoint(AimPt))
			m_ActiveGoal->SetPosition(AimPt);
	}
}

void GoalManager::cmdGoalEdit(const StringVector &_args)
{	
	MapGoalPtr nearest;

	Vector3f vPos;
	if(Utils::GetLocalPosition(vPos))
	{
		nearest = _GetGoalInRange(vPos,150.f,true);
	}

	_SetActiveGoal(nearest);
	if(!m_ActiveGoal)
	{
		EngineFuncs::ConsoleError("No Goal In Range!");
	}
}

void GoalManager::cmdGoalEditx(const StringVector &_args)
{	
	_SetActiveGoal(m_HighlightedGoal);
	if(!m_ActiveGoal)
	{
		EngineFuncs::ConsoleError("No Goal In Range!");
	}
}

void GoalManager::cmdGoalHelp(const StringVector &_args)
{	
	if(m_ActiveGoal)
		m_ActiveGoal->ShowHelp();
}
void GoalManager::cmdGoalFinish(const StringVector &_args)
{
	_SetActiveGoal(MapGoalPtr());
}
void GoalManager::cmdGoalCreate(const StringVector &_args)
{
	const char *strUsage[] = 
	{ 
		"goal_create goaltype[string]",
		"> goaltype: type name of goal to create",
	};

	CHECK_NUM_PARAMS(_args, 2, strUsage);
	OPTIONAL_STRING_PARAM(goalname, 2, "");

	AABB aabb;
	Vector3f vFace;
	if(!Utils::GetLocalAABB(aabb) || !Utils::GetLocalFacing(vFace))
	{
		EngineFuncs::ConsoleError("Can't get local aabb or facing!");
		return;
	}

	Vector3f vPos;
	aabb.CenterBottom(vPos);

	MapGoalDef def;
	def.Props.SetString("Type",_args[1].c_str());
	def.Props.SetVector("Position",vPos);
	def.Props.SetVector("Facing",vFace);
	def.Props.SetFloat("MinRadius",32.f);
	def.Props.SetString("TagName",goalname.c_str());
	def.Props.SetInt("NewGoal",1);

	MapGoalPtr mg = AddGoal(def);
	_SetActiveGoal(mg);
	if(mg)
		mg->ShowHelp();
	if(!m_ActiveGoal)
	{
		EngineFuncs::ConsoleError(va("Unknown goal type: %s",_args[0].c_str()));
	}
}
void GoalManager::cmdGoalDelete(const StringVector &_args)
{
	if(!m_ActiveGoal)
	{
		EngineFuncs::ConsoleError("Select a goal for edit by adding a new one or using goal_edit");
		return;
	}
	m_ActiveGoal->SetDeleteMe(true);
	_SetActiveGoal(MapGoalPtr());
}
void GoalManager::cmdGoalSetProperty(const StringVector &_args)
{
	if(!m_ActiveGoal)
	{
		EngineFuncs::ConsoleError(va("Select a goal for edit by adding a new one or using goal_edit"));
		return;
	}
	const char *strUsage[] = 
	{ 
		"goal_setproperty propertyname[string] propertyvalue[string]",
		"> propertyname: name of property to set",
		"> propertyvalue: optional value of property to set, also accepts keywords",
		"        <facing>, <position>, <aimpoint>, <aimnormal>",
	};

	CHECK_NUM_PARAMS(_args, 2, strUsage);
	const String &propname = _args[1];
	String propval = _args.size()>2 ? _args[2] : "";
	
	obUserData val(propval.c_str());

	Vector3f v,v2;
	if(propval=="<facing>" && Utils::GetLocalFacing(v))
	{
		Utils::ConvertString(v,propval);
		val = obUserData(v);
	}
	if(propval=="<position>" && Utils::GetLocalPosition(v))
	{
		Utils::ConvertString(v,propval);
		val = obUserData(v);
	}
	if(propval=="<aimpoint>" && Utils::GetLocalAimPoint(v))
	{
		Utils::ConvertString(v,propval);
		val = obUserData(v);
	}
	if(propval=="<aimnormal>" && Utils::GetLocalAimPoint(v,&v2))
	{
		Utils::ConvertString(v2,propval);
		val = obUserData(v);
	}

	String oldName = m_ActiveGoal->GetName();
	m_ActiveGoal->SetProperty(propname,val);

	if ( m_LoadedMapGoals && oldName != m_ActiveGoal->GetName() ) {
		gmMachine *pMachine = ScriptManager::GetInstance()->GetMachine();
		m_LoadedMapGoals->Set( pMachine, oldName.c_str(), gmVariable::s_null );
	}
}

void GoalManager::cmdGoalRemoveAll(const StringVector &_args)
{
	int Removed = 0;
	MapGoalList::iterator it = m_MapGoalList.begin();
	for( ; it != m_MapGoalList.end(); )
	{
		if(!(*it)->GetInterfaceGoal())
		{
			++Removed;
			it = m_MapGoalList.erase(it);
		}
		else
			++it;
	}

	EngineFuncs::ConsoleMessage(va("Removed %d goals.",Removed));
}

void GoalManager::cmdGoalMove(const StringVector &_args)
{
	OPTIONAL_STRING_PARAM(toPlayer, 1, "");
	OPTIONAL_BOOL_PARAM(ground, 2, true);

	if(m_EditMode != EditMove)
	{
		if(m_ActiveGoal)
		{
			if(Utils::StringToLower(toPlayer) == "toplayer")
			{
				Vector3f vLocalPos;

				if(ground == true)
				{
					Vector3f vGroundPos;
					Utils::GetLocalGroundPosition(vGroundPos,TR_MASK_SHOT);
					m_ActiveGoal->SetPosition(vGroundPos);
				}
				else if(Utils::GetLocalPosition(vLocalPos))
					m_ActiveGoal->SetPosition(vLocalPos);
			}
			else
			{
				m_EditMode = EditMove;
				EngineFuncs::ConsoleMessage(va("Moving %s.",m_ActiveGoal->GetName().c_str()));
			}
		}
		else
		{
			EngineFuncs::ConsoleMessage(va("Select a goal for edit first! (goal_edit/goal_editx)"));
		}
	}
	else
	{
		m_EditMode = EditNone;
		EngineFuncs::ConsoleMessage(va("Moving %s stopped.",m_ActiveGoal->GetName().c_str()));
	}
}

void GoalManager::Shutdown()
{
	m_LoadedMapGoals = NULL;
	m_MapGoalList.clear();
}

void GoalManager::Update()
{
	Prof(GoalManagerUpdate);

	//////////////////////////////////////////////////////////////////////////
	// see which goal we should highlight, if any
	MapGoalPtr HighlightedGoal;
	Vector3f AimPt;
	if(Utils::GetLocalAimPoint(AimPt))
	{
		HighlightedGoal = _GetGoalInRange(AimPt,256.f,true);
	}
	
	if(m_HighlightedGoal != HighlightedGoal)
	{
		if(HighlightedGoal)
		{
			Utils::DrawRadius(
				HighlightedGoal->GetPosition()+Vector3f(0,0,32.f),
				HighlightedGoal->GetRadius(),
				COLOR::YELLOW,
				IGame::GetDeltaTimeSecs()*2.f);
		}		
	}
	m_HighlightedGoal = HighlightedGoal;
#ifdef ENABLE_DEBUG_WINDOW
	DW.Hud.SetActiveMapGoal(m_HighlightedGoal);
#endif
	//////////////////////////////////////////////////////////////////////////
	MapGoalList::iterator it = m_MapGoalList.begin();
	while(it != m_MapGoalList.end())
	{
		if((*it)->GetDeleteMe())
		{
			OnGoalDelete((*it));

			(*it)->SetAvailabilityTeams(0);
#ifdef _DEBUG
			LOG("Goal Deleted: " << (*it)->GetGoalType() << ", " << (*it)->GetName().c_str());
#endif
			it = m_MapGoalList.erase(it);
		}
		else
		{
			(*it)->RenderDebug(m_ActiveGoal==(*it),m_HighlightedGoal==(*it));
			(*it)->Update();
			++it;
		}
	}
	//////////////////////////////////////////////////////////////////////////

	if(IGame::GetTime() > NextDrawTime)
	{
		NextDrawTime = IGame::GetTime() + 2000;
	}

	_UpdateEditModes();
}

void GoalManager::AddGoal(MapGoalPtr newGoal)
{
	m_MapGoalList.push_back(newGoal);

	//////////////////////////////////////////////////////////////////////////
	gmMachine *pMachine = ScriptManager::GetInstance()->GetMachine();
	if(pMachine)
	{
		gmGCRoot<gmUserObject> pUser = newGoal->GetScriptObject(pMachine);

		gmCall call;
		if(call.BeginGlobalFunction(pMachine, "OnGoalAdded", gmVariable::s_null, true))
		{
			if(pUser)
				call.AddParamUser(pUser);
			call.End();
		}
	}
	//////////////////////////////////////////////////////////////////////////
}

MapGoalPtr GoalManager::AddGoal(const MapGoalDef &_goaldef)
{
	MapGoalDef goaldef = _goaldef;

	const char *GoalType = 0;
	goaldef.Props.GetString("Type",GoalType);
	if(!GoalType)
		return MapGoalPtr();

	MapGoalPtr newGoal = g_MapGoalDatabase.GetNewMapGoal(GoalType);
	//////////////////////////////////////////////////////////////////////////
	// If no goals of this type, the script can give us an alias
	// useful for deprecating old goals but still being able to load/convert them
	// to a new and improved goal
	if(!newGoal)
	{
		gmMachine *pMachine = ScriptManager::GetInstance()->GetMachine();
		gmCall call;
		if(call.BeginGlobalFunction(pMachine,"GetMapGoalUpgradeType"))
		{
			const String strLower = Utils::StringToLower(GoalType);
			call.AddParamString(strLower.c_str());
			call.End();

			const char *NewName = 0;
			call.GetReturnedString(NewName);
			if(NewName)
			{
				goaldef.Props.SetString("OldType",GoalType);
				GoalType = NewName;
				newGoal = g_MapGoalDatabase.GetNewMapGoal(NewName);
			}
		}
	}
	//////////////////////////////////////////////////////////////////////////
	if(newGoal)
	{
		// new
		int Teams = 0;
		if(goaldef.Props.GetInt("Team",Teams) && Teams)
		{
			newGoal->SetAvailabilityTeamsInit(Teams);
			newGoal->SetAvailabilityTeams(Teams);
		}
		else
		{
			for(int i = 1; i <= 4; ++i)
			{
				newGoal->SetAvailable(i, true); // everyone by default
				newGoal->SetAvailableInitial(i, true);
			}
		}
		const char *TagName = 0;
		goaldef.Props.GetString("TagName",TagName);
		if(TagName && TagName[0])
			newGoal->SetTagName(TagName);

		GameEntity Entity;
		if(goaldef.Props.GetEntity("Entity",Entity))
			newGoal->SetEntity(Entity);

		Vector3f Position;
		if(goaldef.Props.GetVector("Position",Position))
			newGoal->SetPosition(Position);

		Vector3f Facing;
		if(goaldef.Props.GetVector("Facing",Facing))
			newGoal->SetFacing(Facing);

		float MinRadius;
		if(goaldef.Props.GetFloat("MinRadius",MinRadius))
			newGoal->SetMinRadius(MinRadius);

		float Radius;
		if(goaldef.Props.GetFloat("Radius",Radius))
			newGoal->SetRadius(Radius);

		int AutoGen = 0;
		if(goaldef.Props.GetInt("InterfaceGoal",AutoGen))
			newGoal->SetInterfaceGoal(AutoGen!=0);

		int New = 0;
		if(goaldef.Props.GetInt("NewGoal",New) && New!=0)
		{
			newGoal->SetDisabled(true);
			newGoal->SetRenderGoal(true);
		}

		// If no version defined, assume 0
		int Version = 0;
		if(!goaldef.Props.GetInt("Version",Version))
			goaldef.Props.SetInt("Version",0);

		newGoal->InternalInitEntityState();

		newGoal->GenerateName();

		// convert the keyvals to a script table
		gmMachine *pMachine = ScriptManager::GetInstance()->GetMachine();
		gmGCRoot<gmTableObject> Props(pMachine->AllocTableObject(),pMachine);
		Utils::KeyValsToTable(goaldef.Props,Props,pMachine);

		// find the table from the save file and pass it in as well to
		// load script authoritative data
		gmGCRoot<gmTableObject> LoadFileProps;
		if(m_LoadedMapGoals)
		{
			gmVariable var = m_LoadedMapGoals->Get(pMachine,newGoal->GetName().c_str());
			gmTableObject * varTbl = var.GetTableObjectSafe();
			if(varTbl)
			{
				LoadFileProps.Set(varTbl,pMachine);
			}
		}

		// merge the safe file properties with the property list before we call init
		bool LoadFromTable = false;
		if(LoadFileProps)
		{
			gmTableIterator tIt;
			gmTableNode * pSaveNode = LoadFileProps->GetFirst(tIt);
			while(pSaveNode)
			{
				Props->Set(pMachine,pSaveNode->m_key,pSaveNode->m_value);
				pSaveNode = LoadFileProps->GetNext(tIt);
			}
			LoadFromTable = true;
		}

		if(LoadFromTable)
		{
			ErrorObj err;
			if(newGoal->LoadFromTable(pMachine,Props,err))
			{
#ifdef _DEBUG
				LOG("Goal Created: " << newGoal->GetName() << ", partially loaded from script");
#endif
				AddGoal(newGoal);
			}
			else
			{
				err.PrintToConsole();
			}
		}
		else if(newGoal->InternalInit(Props, New!=0))
		{
#ifdef _DEBUG
			LOG("Goal Created: " << newGoal->GetName());
#endif
			AddGoal(newGoal);
		}
		else
		{
			LOG("Goal Unable to Init: " << 
				newGoal->GetGoalType() << ", tag: " << 
				newGoal->GetTagName());
		}
	}
	else
	{
		const char *TagName = 0;
		goaldef.Props.GetString("TagName",TagName);
		TagName = TagName ? TagName : "noname";

		if(!GoalType)
		{
			LOG("Goal Type Not Provided: " << TagName);
		}
		else
		{
			LOG("Unknown Goal Type: " << GoalType << ", tag: " << TagName);
		}
	}
	return newGoal;
}

void GoalManager::RemoveGoalByName(const char *_goalname)
{
	const String goalnm = _goalname;
	MapGoalList::iterator it = m_MapGoalList.begin();
	while(it != m_MapGoalList.end())
	{
		const String temp = (*it)->GetName();
		if((*it)->GetName() == _goalname)
		{
			OnGoalDelete((*it));
#ifdef _DEBUG
			LOG("Goal Deleted: " << (*it)->GetGoalType() << ", " << (*it)->GetName().c_str());
#endif
			it = m_MapGoalList.erase(it);
		}
		else
			++it;
	}
}

void GoalManager::RemoveGoalsByType(const char *_goaltype)
{
	MapGoalList::iterator it = m_MapGoalList.begin();
	while(it != m_MapGoalList.end())
	{
		if((*it)->GetGoalType() == _goaltype)
		{
			OnGoalDelete((*it));
			(*it)->SetAvailabilityTeams(0);
#ifdef _DEBUG
			LOG("Goal Deleted: " << (*it)->GetGoalType() << ", " << (*it)->GetName().c_str());
#endif
			it = m_MapGoalList.erase(it);
		}
		else
			++it;
	}
}

void GoalManager::RemoveGoalByEntity(GameEntity _ent)
{
	MapGoalList::iterator it = m_MapGoalList.begin();
	while(it != m_MapGoalList.end())
	{
		if((*it)->GetEntity().GetIndex() == _ent.GetIndex())
			Utils::OutputDebug(kInfo,"SameIndex");

		if((*it)->GetEntity() == _ent)
		{
			OnGoalDelete((*it));
			(*it)->SetDeleteMe(true);
			(*it)->SetAvailabilityTeams(0);
#ifdef _DEBUG
			LOG("Goal Deleted: " << (*it)->GetGoalType() << ", " << (*it)->GetName().c_str());
#endif
			it = m_MapGoalList.erase(it);
		}
		else
			++it;
	}
}

void GoalManager::OnGoalDelete(const MapGoalPtr &_goal)
{
	gmMachine *pMachine = ScriptManager::GetInstance()->GetMachine();
	if(m_LoadedMapGoals)
		m_LoadedMapGoals->Set(pMachine,_goal->GetName().c_str(),gmVariable::s_null);
}

MapGoalPtr GoalManager::GetGoal(const String &_goalname)
{
	MapGoalPtr ptr;

	MapGoalList::iterator it = m_MapGoalList.begin(),
		itEnd = m_MapGoalList.end();
	while(it != itEnd)
	{
		if((*it)->GetName() == _goalname)
		{
			ptr = (*it);
			break;
		}
		++it;
	}

	return ptr;
}

MapGoalPtr  GoalManager::GetGoal(int _serialNum)
{
	MapGoalPtr ptr;

	MapGoalList::iterator it = m_MapGoalList.begin(),
		itEnd = m_MapGoalList.end();
	while(it != itEnd)
	{
		if((*it)->GetSerialNum() == _serialNum)
		{
			ptr = (*it);
			break;
		}
		++it;
	}

	return ptr;
}

void GoalManager::RegisterWaypointGoals(Waypoint *_wp, MapGoalDef *_def, int _num)
{
	//////////////////////////////////////////////////////////////////////////
	// Fill in the rest.
	for(int i = 0; i < _num; ++i)
	{
		if ( !_wp->GetName().empty() )  
		{ 
			_def[i].Props.SetString("TagName",_wp->GetName().c_str()); 
		}
		else 
		{
			_def[i].Props.SetString("TagName",va("%d",_wp->GetUID()));
		}

		if(_wp->IsFlagOn(F_NAV_TEAMONLY))
		{
			int Team = 0;
			if(_wp->IsFlagOn(F_NAV_TEAM1)) Team |= (1<<1);
			if(_wp->IsFlagOn(F_NAV_TEAM2)) Team |= (1<<2);
			if(_wp->IsFlagOn(F_NAV_TEAM3)) Team |= (1<<3);
			if(_wp->IsFlagOn(F_NAV_TEAM4)) Team |= (1<<4);
			_def[i].Props.SetInt("Team",Team);
		}

		_def[i].Props.SetInt("Version",0);
		_def[i].Props.SetVector("Position",_wp->GetPosition());
		_def[i].Props.SetVector("Facing",_wp->GetFacing());
		_def[i].Props.SetFloat("Radius",_wp->GetRadius());
		_def[i].Props.SetInt("NavigationUID",_wp->GetUID());

		_wp->GetPropertyMap().GetAsKeyVal(_def[i].Props);

		AddGoal(_def[i]);
	}
}

void GoalManager::CheckWaypointForGoal(Waypoint *_wp, BitFlag64 _used)
{
	enum { MaxGoals=8 };

	MapGoalDef Definition[MaxGoals];
	int NumDefs = 0;
	
	//////////////////////////////////////////////////////////////////////////
	if(_wp->IsFlagOn(F_NAV_HEALTH) && !(_used & BitFlag64(F_NAV_HEALTH)).AnyFlagSet())
	{
		/*MapGoalPtr goal(new HealthGoal);
		newGoals.push_back(goal);*/

		Definition[NumDefs++].Props.SetString("Type","health");
	}
	if(_wp->IsFlagOn(F_NAV_ARMOR) && !(_used & BitFlag64(F_NAV_ARMOR)).AnyFlagSet())
	{
		/*MapGoalPtr goal(new ArmorGoal);
		newGoals.push_back(goal);*/

		Definition[NumDefs++].Props.SetString("Type","armor");
	}
	if(_wp->IsFlagOn(F_NAV_AMMO) && !(_used & BitFlag64(F_NAV_AMMO)).AnyFlagSet())
	{
		/*MapGoalPtr goal(new AmmoGoal);
		newGoals.push_back(goal);*/

		Definition[NumDefs++].Props.SetString("Type","ammo");
	}
	if(_wp->IsFlagOn(F_NAV_ATTACK) && !(_used & BitFlag64(F_NAV_ATTACK)).AnyFlagSet())
	{
		/*MapGoalPtr goal(new AttackGoal);
		newGoals.push_back(goal);*/

		Definition[NumDefs++].Props.SetString("Type","attack");
	}
	if(_wp->IsFlagOn(F_NAV_DEFEND) && !(_used & BitFlag64(F_NAV_DEFEND)).AnyFlagSet())
	{
		/*MapGoalPtr goal(new DefendGoal);
		newGoals.push_back(goal);*/

		Definition[NumDefs++].Props.SetString("Type","defend");
	}
	if(_wp->IsFlagOn(F_NAV_SCRIPT) && !(_used & BitFlag64(F_NAV_SCRIPT)).AnyFlagSet())
	{
		//MapGoalPtr goal(new ScriptMapGoal);
		//newGoals.push_back(goal);
	}
	if(_wp->IsFlagOn(F_NAV_SNIPE) && !(_used & BitFlag64(F_NAV_SNIPE)).AnyFlagSet())
	{
		/*MapGoalPtr goal(new SnipeGoal);
		newGoals.push_back(goal);*/

		Definition[NumDefs++].Props.SetString("Type","snipe");
	}
	if(_wp->IsFlagOn(F_NAV_ROUTEPT) && !(_used & BitFlag64(F_NAV_ROUTEPT)).AnyFlagSet())
	{
		/*MapGoalPtr goal(new RouteGoal);
		newGoals.push_back(goal);*/

		Definition[NumDefs++].Props.SetString("Type","route");
	}

	RegisterWaypointGoals(_wp,Definition,NumDefs);
}

void GoalManager::UpdateGoalEntity( GameEntity oldent, GameEntity newent )
{
	MapGoalList::iterator it = m_MapGoalList.begin();
	while(it != m_MapGoalList.end())
	{
		if((*it)->GetEntity() == oldent)
		{
			(*it)->SetEntity( newent );
			LOG("Goal Update: " << (*it)->GetName());
		}
		else
			++it;
	}
}

void GoalManager::GetGoals(Query &_qry)
{
	//Prof(GetGoals);

	//////////////////////////////////////////////////////////////////////////

	String strTagName = _qry.m_TagName ? _qry.m_TagName : "";

	_qry.OnQueryStart();
	MapGoalList::iterator it = m_MapGoalList.begin(),
		itEnd = m_MapGoalList.end();
	for( ; it != itEnd; ++it)
	{
		if ( _qry.CheckForMatch((*it)) ) {
			_qry.OnMatch((*it));
		}
	}
	_qry.OnQueryFinish();
}
