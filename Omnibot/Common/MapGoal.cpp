////////////////////////////////////////////////////////////////////////////////
// 
// $LastChangedBy$
// $LastChangedDate$
// $LastChangedRevision$
//
////////////////////////////////////////////////////////////////////////////////

#include "PrecompCommon.h"
#include "MapGoal.h"
#include "gmbinder2_class.h"
#include "ScriptManager.h"
#include "NavigationManager.h"
#include "gmSchemaLib.h"

#ifdef ENABLE_DEBUG_WINDOW
#include <guichan.hpp>
#endif

static obint32 sNextSerialNum = 0;

//////////////////////////////////////////////////////////////////////////

Prof_Define(MapGoal);

//////////////////////////////////////////////////////////////////////////
#define GETMACHINE() ScriptManager::GetInstance()->GetMachine()
//////////////////////////////////////////////////////////////////////////

#ifdef ENABLE_DEBUG_WINDOW
class MgListModel : public gcn::ListModel, public gcn::DeathListener
{
public:
	//gcn::DeathListener *getDeathListener() { return this; }
	void death(const gcn::Event& event)
	{
		delete this;
	}
	int getNumberOfElements()
	{
		return mTable ? mTable->Count() : 0;
	}
	bool getColumnTitle(int column, std::string &title, int &columnwidth) 
	{
		/*if(mColumns)
		{
		gmTableObject *columnTable = mColumns->Get(gmVariable(column)).GetTableObjectSafe();
		if(columnTable)
		{
		const char *columnName = columnTable->Get(GETMACHINE(),"Name").GetCStringSafe();
		if(columnName)
		title = columnName;
		return columnName || columnTable->Get(GETMACHINE(),"Width").GetIntSafe(columnwidth);
		}
		}*/
		return false; 
	}
	std::string getElementAt(int i, int column)
	{
		if(mTable)
		{
			char buffer[256] = {};
			gmVariable v = mTable->Get(gmVariable(i));
			gmTableObject *subtbl = v.GetTableObjectSafe();
			if(subtbl)
			{
				gmVariable v2 = subtbl->Get(gmVariable(column));
				return v2.AsString(GETMACHINE(), buffer, 256);
			}
			else
			{
				return v.AsString(GETMACHINE(), buffer, 256);
			}
		}
		return "";
	}
	MgListModel(const gmTableObject *a_tbl)
	{
		mTable.Set(const_cast<gmTableObject*>(a_tbl),GETMACHINE());
	}
	//////////////////////////////////////////////////////////////////////////
	gmGCRoot<gmTableObject>	mTable;
};
#endif

//////////////////////////////////////////////////////////////////////////

obint32 GetMapGoalSerial()
{
	return ++sNextSerialNum;
}

MapGoal::MapGoal(const char *_goaltype) 
	: m_GoalType			(_goaltype?_goaltype:"")
	, m_GoalTypeHash		(_goaltype?Utils::MakeHash32(_goaltype):0)
{
	_Init();
}

void MapGoal::CopyFrom(MapGoal *_other)
{
	_Init();

	m_GoalType = _other->m_GoalType;
	m_Radius = _other->m_Radius;
	m_MinRadius = _other->m_MinRadius;
	m_DefaultPriority = _other->m_DefaultPriority;
	m_RolePriorityBonus = _other->m_RolePriorityBonus;

	m_AvailableTeams = _other->m_AvailableTeams;

	m_RoleMask = _other->m_RoleMask;

	for(int i = 0; i < NUM_TRACK_CATS; ++i)
		m_MaxUsers[i] = _other->m_MaxUsers[i];

	// Copy goal priorities
	for(int t = 0; t < ClassPriority::MaxTeams; ++t)
	{
		for(int c = 0; c < ClassPriority::MaxClasses; ++c)
		{
			m_ClassPriority.Priorities[t][c] = _other->m_ClassPriority.Priorities[t][c];
		}
	}

	m_GoalStateFunction = _other->m_GoalStateFunction;

	m_Version = _other->m_Version;
	m_UpgradeFunc = _other->m_UpgradeFunc;
	m_RenderFunc = _other->m_RenderFunc;
	m_SerializeFunc = _other->m_SerializeFunc;
	m_InitNewFunc = _other->m_InitNewFunc;
	m_SetPropertyFunc = _other->m_SetPropertyFunc;
	m_HelpFunc = _other->m_HelpFunc;
	m_UpdateFunc = _other->m_UpdateFunc;
	m_HudDisplay = _other->m_HudDisplay;

	gmMachine *pMachine = ScriptManager::GetInstance()->GetMachine();
	gmBind2::Class<MapGoal>::CloneTable(pMachine,_other->GetScriptObject(pMachine),GetScriptObject(pMachine));

#ifdef Prof_ENABLED
	m_ProfZone = _other->m_ProfZone;
#endif

	// copy select flags.
	PROPERTY_PROPOGATE(DefaultDrawFlags);
	PROPERTY_PROPOGATE(RenderHeight);
	PROPERTY_PROPOGATE(DefaultRenderRadius);
	PROPERTY_PROPOGATE(DisableWithEntityFlag);
	PROPERTY_PROPOGATE(DeleteWithEntityFlag);
	PROPERTY_PROPOGATE(DisableForControllingTeam);
	PROPERTY_PROPOGATE(DynamicPosition);
	PROPERTY_PROPOGATE(DynamicOrientation);
	PROPERTY_PROPOGATE(RemoveWithEntity);
	PROPERTY_PROPOGATE(DontSave);
	PROPERTY_PROPOGATE(CreateOnLoad);	
}

void MapGoal::_Init()
{
	m_AvailableTeams.ClearAll();

	m_Position = Vector3f::ZERO;
	m_InterfacePosition = Vector3f::ZERO;
	m_Orientation = Matrix3f::IDENTITY;
	m_LocalBounds = AABB(Vector3f::ZERO, Vector3f::ZERO);
	m_Radius = 0.f;
	m_MinRadius = 0.f;
	m_NavFlags = 0;
	m_SerialNum = 0;
	m_DefaultPriority = 1.f;
	m_RolePriorityBonus = 0.f;

	m_SerialNum = GetMapGoalSerial();

	// By default, max out the counters.
	for(int i = 0; i < NUM_TRACK_CATS; ++i)
		m_MaxUsers[i] = 10000;

	ResetGoalPriorities();

	m_RoleMask = BitFlag32(0);

	m_GoalStateFunction = GoalStateNone;
	m_GoalState = 0;

	m_Version = 0;

	m_ControllingTeam = 0;

#ifdef Prof_ENABLED
	m_ProfZone = 0;
#endif

	//////////////////////////////////////////////////////////////////////////
	// init flags
	PROPERTY_INIT(DefaultDrawFlags,DrawAll);
	PROPERTY_INIT(DefaultRenderRadius,2048.f);
	PROPERTY_INIT(RenderHeight,IGame::GetGameVars().mPlayerHeight*0.5f);
	PROPERTY_INIT(DeleteMe,false);
	PROPERTY_INIT(DynamicPosition,false);
	PROPERTY_INIT(DynamicOrientation,false);
	PROPERTY_INIT(PropertiesBound,false);
	PROPERTY_INIT(RemoveWithEntity,true);
	PROPERTY_INIT(InterfaceGoal,false);
	PROPERTY_INIT(Disabled,false);
	PROPERTY_INIT(InUse,false);
	PROPERTY_INIT(DisableForControllingTeam,false);
	PROPERTY_INIT(DontSave,false);
	PROPERTY_INIT(RenderGoal,false);
	PROPERTY_INIT(RenderRoutes,false);
	PROPERTY_INIT(CreateOnLoad,true);	
}

MapGoal::~MapGoal() 
{
	gmBind2::Class<MapGoal>::NullifyUserObject(m_ScriptObject);
}

gmGCRoot<gmUserObject> MapGoal::GetScriptObject(gmMachine *_machine) const
{
	if(!m_ScriptObject)
		m_ScriptObject = gmBind2::Class<MapGoal>::WrapObject(_machine,const_cast<MapGoal*>(this),true);
	return m_ScriptObject;
}

void MapGoal::SetProfilerZone(const String &_name)
{
#ifdef Prof_ENABLED
	m_ProfZone = gDynamicZones.FindZone(_name.c_str());
	OBASSERT(m_ProfZone,"No Profiler Zone Available!");
#endif
}

MapGoalPtr MapGoal::GetSmartPtr()
{
	MapGoalPtr ptr = m_WeakPtr.lock();
	return ptr;
}

void MapGoal::SetSmartPtr(MapGoalPtr ptr)
{
	m_WeakPtr = ptr;
}

bool MapGoal::LoadFromFile(const fs::path &_file)
{
	gmGCRoot<gmUserObject> mgref = GetScriptObject(ScriptManager::GetInstance()->GetMachine());
	gmVariable varThis(mgref);

	int iThreadId;
	if(ScriptManager::GetInstance()->ExecuteFile(_file, iThreadId, &varThis))
	{
		return true;
	}
	else
	{
		OBASSERT(0, "Error Running MapGoal Script");
	}

	return false;
}

void MapGoal::GenerateName(int _instance, bool _skipdupecheck)
{
	obint32 iNavId = g_EngineFuncs->IDFromEntity(GetEntity());
	if(m_TagName.empty())
	{
		String sNavName;		
		PathPlannerBase *pBase = NavigationManager::GetInstance()->GetCurrentPathPlanner();
		pBase->GetNavInfo(GetPosition(),iNavId,m_TagName);
	}
	
	String gtype = GetGoalType();
	std::transform(gtype.begin(),gtype.end(),gtype.begin(),toUpper());

	if(!m_TagName.empty())
		m_Name = Utils::VA("%s_%s",gtype.c_str(),m_TagName.c_str());
	else
	{
		m_Name = Utils::VA("%s_%d",gtype.c_str(),iNavId); 
	}
	boost::replace_all(m_Name, " ", "_");

	//////////////////////////////////////////////////////////////////////////
	// Dupe name handling, append an instance number
	if(_instance > 0)
	{ 
		m_Name += Utils::VA("_%d",_instance);
	}

	// see if it already exists
	if(!_skipdupecheck)
	{
		MapGoalPtr exists = GoalManager::GetInstance()->GetGoal(m_Name);
		if(exists && exists.get() != this)
		{
			GenerateName(_instance+1);
		}
	}
	CheckForPersistentPriority();
}

bool MapGoal::IsAvailable(int _team) const
{
	if(GetDisabled())
		return false;
	if(GetDisableForControllingTeam() && m_ControllingTeam==_team)
		return false;
	return m_AvailableTeams.CheckFlag(_team) && !GetDeleteMe();
}

void MapGoal::SetAvailable(int _team, bool _available)
{
	m_AvailableTeams.SetFlag(_team,_available);
}

bool MapGoal::IsAvailableInitial(int _team) const
{
	return m_AvailableTeamsInit.CheckFlag(_team);
}

void MapGoal::SetAvailableInitial(int _team, bool _available)
{
	m_AvailableTeamsInit.SetFlag(_team,_available);
}

void MapGoal::Update()
{
	Prof_Scope(MapGoal);

#ifdef Prof_ENABLED
	Prof_Scope_Var Scope(*m_ProfZone, Prof_cache);
#endif

	{
		Prof(Update);

		//////////////////////////////////////////////////////////////////////////
		if(GetEntity().IsValid())
		{
			if(GetRemoveWithEntity())
			{
				if(!IGame::IsEntityValid(GetEntity()))
				{
					SetDeleteMe(true);
					return;
				}
			}
			if(GetDeleteWithEntityFlag().AnyFlagSet())
			{
				BitFlag64 bf;
				InterfaceFuncs::GetEntityFlags(GetEntity(),bf);
				if((bf & GetDeleteWithEntityFlag()).AnyFlagSet())
				{
					SetDeleteMe(true);
					return;
				}
			}
			if(GetDisableWithEntityFlag().AnyFlagSet())
			{
				BitFlag64 bf;
				InterfaceFuncs::GetEntityFlags(GetEntity(),bf);
				if((bf & GetDisableWithEntityFlag()).AnyFlagSet())
				{
					SetDisabled(true);
				}
				else
				{
					SetDisabled(false);
				}
			}
		}
		//////////////////////////////////////////////////////////////////////////
		_CheckControllingTeam();
		//////////////////////////////////////////////////////////////////////////
		switch(m_GoalStateFunction)
		{
		case GoalStateFlagState:
			_UpdateFlagState();
			break;
			/*case GoalStateFlagHoldState:
			_UpdateFlagHoldState();
			break;*/
		case GoalStateNone:
		default:
			break;
		}
		//////////////////////////////////////////////////////////////////////////
		if(m_UpdateFunc)
		{
			if(!m_ActiveThread[ON_UPDATE].IsActive())
			{
				gmMachine *pMachine = ScriptManager::GetInstance()->GetMachine();
				gmCall call;
				gmGCRoot<gmUserObject> mgref = GetScriptObject(pMachine);
				if(call.BeginFunction(pMachine, m_UpdateFunc, gmVariable(mgref)))
				{
					if(call.End() == gmThread::EXCEPTION)
					{
					}

					m_ActiveThread[ON_UPDATE] = call.GetThreadId();
					if(call.DidReturnVariable())
						m_ActiveThread[ON_UPDATE] = 0;
				}
			}
		}
	}
}

void MapGoal::_UpdateFlagState()
{
	GameEntity owner;
	FlagState newFlagState;
	if(InterfaceFuncs::GetFlagState(GetEntity(), newFlagState, owner))
	{
		SetOwner(owner);

		if(newFlagState != m_GoalState)
		{
			const char *pFlagState = 0;
			switch(newFlagState)
			{
			case S_FLAG_NOT_A_FLAG:
				pFlagState = "deleted";
				break;
			case S_FLAG_AT_BASE:
				pFlagState = "returned";
				break;
			case S_FLAG_DROPPED:
				pFlagState = "dropped";
				break;
			case S_FLAG_CARRIED:
				pFlagState = "pickedup";
				break;
			case S_FLAG_UNAVAILABLE:
				pFlagState = "unavailable";
				break;
			case S_FLAG_UNKNOWN:
				pFlagState = "unknown";
				break;
			}

			if(pFlagState)
			{
				TriggerInfo ti;
				ti.m_Entity = GetEntity();
				ti.m_Activator = owner;
				Utils::VarArgs(ti.m_TagName, TriggerBufferSize, "Flag %s %s", GetTagName().c_str(), pFlagState);
				Utils::StringCopy(ti.m_Action, pFlagState, TriggerBufferSize);
				TriggerManager::GetInstance()->HandleTrigger(ti);
			}
			else
			{
				OBASSERT(0, "Invalid Flag State");
			}
			m_GoalState = newFlagState;
		}
	}
}

//void MapGoal::_UpdateFlagHoldState()
//{
//	if(GetEntity().IsValid())
//	{
//		const int newControllingTeam = InterfaceFuncs::GetControllingTeam(GetEntity());
//		if(newControllingTeam != m_GoalState)
//		{
//			TriggerInfo ti;
//			ti.m_Entity = GetEntity();
//			ti.m_Activator = GameEntity();
//			Utils::VarArgs(ti.m_TagName, 
//				TriggerBufferSize, 
//				"%s to team %d", 
//				GetName().c_str(), 
//				newControllingTeam);
//			Utils::StringCopy(ti.m_Action, "controlling team", TriggerBufferSize);
//			TriggerManager::GetInstance()->HandleTrigger(ti);
//
//			m_GoalState = newControllingTeam;
//		}
//	}
//}

void MapGoal::_CheckControllingTeam()
{
	if(GetEntity().IsValid())
	{
		const int newControllingTeam = InterfaceFuncs::GetControllingTeam(GetEntity());
		if(newControllingTeam != m_ControllingTeam)
		{
			m_ControllingTeam = newControllingTeam;

			TriggerInfo ti;
			ti.m_Entity = GetEntity();
			ti.m_Activator = GameEntity();
			Utils::VarArgs(ti.m_TagName, 
				TriggerBufferSize, 
				"%s to team %d", 
				GetName().c_str(), 
				newControllingTeam);
			Utils::StringCopy(ti.m_Action, "controlling team", TriggerBufferSize);
			TriggerManager::GetInstance()->HandleTrigger(ti);
		}
	}
}

void MapGoal::InternalInitEntityState()
{
	// cache the values.
	if(GetEntity().IsValid())
	{
		AABB worldbounds;
		bool b1 = EngineFuncs::EntityWorldAABB(GetEntity(), worldbounds);
		bool b2 = EngineFuncs::EntityPosition(GetEntity(), m_Position);

		// cache the auto detected position
		if(b2) m_InterfacePosition = m_Position;

		worldbounds.UnTranslate(m_Position);
		m_LocalBounds = worldbounds;

		Vector3f vFwd, vRight, vUp;
		bool b3 = EngineFuncs::EntityOrientation(GetEntity(), vFwd, vRight, vUp);
		m_Orientation = Matrix3f(vRight, vFwd, vUp, true);

		OBASSERT(b1&&b2&&b3,"Lost Entity!");
	}

	if(m_LocalBounds.IsZero())
	{
		m_LocalBounds.Expand(5.f);
	}
}

bool MapGoal::InternalInit(gmGCRoot<gmTableObject> &_propmap, bool _newgoal)
{
	CheckPropertiesBound();

	//////////////////////////////////////////////////////////////////////////
	gmMachine *pMachine = ScriptManager::GetInstance()->GetMachine();
	
	//////////////////////////////////////////////////////////////////////////
	if(_newgoal)
	{
		if(m_InitNewFunc)
		{
			gmCall call;

			gmGCRoot<gmUserObject> mgref = GetScriptObject(pMachine);
			if(call.BeginFunction(pMachine, m_InitNewFunc, gmVariable(mgref)))
			{
				call.AddParamTable(_propmap);
				call.End();
			}
		}
	}
	else 
	{
		if(m_UpgradeFunc)
		{
			while(true)
			{			
				gmCall call;

				gmGCRoot<gmUserObject> mgref = GetScriptObject(pMachine);
				if(call.BeginFunction(pMachine, m_UpgradeFunc, gmVariable(mgref)))
				{
					gmVariable gmVersionBefore = _propmap->Get(pMachine,"Version");
					call.AddParamTable(_propmap);
					const int ThreadState = call.End();
					gmVariable gmVersionAfter = _propmap->Get(pMachine,"Version");

					//meh
					_propmap->Set(pMachine,"OldType",gmVariable::s_null);

					if(!gmVersionBefore.IsInt() || 
						!gmVersionAfter.IsInt() ||
						ThreadState==gmThread::EXCEPTION)
					{
						EngineFuncs::ConsoleMessagef("%s goal could not upgrade properly, disabling",
							GetName().c_str());
						SetDisabled(true);
						return false;
					}
					else if(gmVersionBefore.GetInt() != gmVersionAfter.GetInt())
					{
						//EngineFuncs::ConsoleMessagef("%s goal updated, version %d to %d",
						//	GetName().c_str(),gmVersionBefore.GetInt(),gmVersionAfter.GetInt());
					}
					else if(gmVersionAfter.GetInt() == m_Version)
					{
						//EngineFuncs::ConsoleMessagef("%s goal up to date, version %d",
						//	GetName().c_str(),gmVersionAfter.GetInt());
						break;
					}
					else
					{
						EngineFuncs::ConsoleMessagef("%s goal could not upgrade properly, disabling",
							GetName().c_str());
						SetDisabled(true);
						return false;
					}
				}
			}
		}
	}
	return true;
}

void MapGoal::SetPosition(const Vector3f &_pos)
{
	m_Position = _pos;
}

const Vector3f &MapGoal::GetPosition()
{
	if(GetDynamicPosition())
	{
		bool b = EngineFuncs::EntityPosition(GetEntity(), m_Position);
		SOFTASSERTALWAYS(b,"Lost Entity for MapGoal %s!", GetName().c_str());
	}
	return m_Position;
}

void MapGoal::SetPosition_Script(const Vec3 &_pos)
{
	m_Position = Vector3f(_pos.x,_pos.y,_pos.z);
}

Vec3 MapGoal::GetPosition_Script()
{
	return Vec3(GetPosition());
}

void MapGoal::SetFacing(const Vector3f &_facing)
{
	m_Orientation = Matrix3f(_facing.Cross(Vector3f::UNIT_Z), _facing, Vector3f::UNIT_Z, true);
}

void MapGoal::SetFacing_Script(const Vec3 &_facing)
{
	Vector3f facing(_facing.x,_facing.y,_facing.z);
	m_Orientation = Matrix3f(
		facing.Cross(Vector3f::UNIT_Z), 
		facing, 
		Vector3f::UNIT_Z, true);
}


Vector3f MapGoal::GetFacing()
{
	return GetMatrix().GetColumn(1);
}

Vec3 MapGoal::GetFacing_Script()
{
	return Vec3(GetFacing());
}

void MapGoal::SetMatrix(const Matrix3f &_mat)
{
	m_Orientation = _mat;
}

Matrix3f MapGoal::GetMatrix()
{
	if(GetDynamicOrientation())
	{
		Vector3f vFwd, vRight, vUp;
		bool b = EngineFuncs::EntityOrientation(GetEntity(), vFwd, vRight, vUp);
		OBASSERT(b,"Lost Entity!");
		m_Orientation = Matrix3f(vRight, vFwd, vUp, false);
	}
	return m_Orientation;
}

void MapGoal::SetGoalBounds(const AABB &_bounds)
{
	m_LocalBounds = _bounds;
}

void MapGoal::SetBounds_Script(const Vec3 &_mins, const Vec3 &_maxs)
{
	for(int i = 0; i < 3; ++i)
	{
		m_LocalBounds.m_Mins[i] = _mins[i];
		m_LocalBounds.m_Maxs[i] = _maxs[i];
	}
}

Vec3 MapGoal::GetBoundsCenter_Script()
{
	Box3f box = GetWorldBounds();
	return Vec3(box.Center);
}

Box3f MapGoal::GetWorldBounds()
{
	Box3f obb;
	obb.Clear();
	obb.Center = GetPosition();
	obb.Extent[0] = obb.Extent[1] = obb.Extent[2] = 8.f;
	EngineFuncs::EntityOrientation(GetEntity(), obb.Axis[0], obb.Axis[1], obb.Axis[2]);
	EngineFuncs::EntityWorldOBB( GetEntity(), obb );
	return obb;
}

const AABB &MapGoal::GetLocalBounds() const
{
	return m_LocalBounds;
}

void MapGoal::AddUsePoint(const Vector3f &_pos, bool _relative)
{
	m_LocalUsePoints.resize(m_LocalUsePoints.size()+1);
	m_LocalUsePoints[m_LocalUsePoints.size()-1] = _pos;

	m_RelativeUsePoints.resize(m_LocalUsePoints.size());
	m_RelativeUsePoints.set(m_RelativeUsePoints.size()-1, _relative);
}

Vector3f MapGoal::GetWorldUsePoint(obint32 _index)
{
	if(!m_LocalUsePoints.empty())
	{
		if(_index == -1 || _index < 0 || _index >= (obint32)m_LocalUsePoints.size())
		{
			int iRand = Mathf::IntervalRandomInt(0, (int)m_LocalUsePoints.size());

			Vector3f vUsePt = m_LocalUsePoints[iRand];

			if(m_RelativeUsePoints.test(iRand))
			{
				vUsePt = GetPosition() + GetMatrix() * vUsePt;
			}
			return vUsePt;
		}

		Vector3f vUsePt = m_LocalUsePoints[_index];

		if(m_RelativeUsePoints.test(_index))
		{
			vUsePt = GetPosition() + GetMatrix() * vUsePt;
		}
		return vUsePt;
	}
	return GetPosition();
}

void MapGoal::GetAllUsePoints(Vector3List &_pv)
{
	_pv.reserve(GetNumUsePoints());
	for(int i = 0; i < GetNumUsePoints(); ++i)
	{
		_pv.push_back(GetWorldUsePoint(i));
	}
}

bool MapGoal::AddRoute_Script(const std::string &_start, const std::string &_end, float _weight)
{
	MapGoalPtr mgStart = GoalManager::GetInstance()->GetGoal(_start);
	MapGoalPtr mgEnd = GoalManager::GetInstance()->GetGoal(_end);
	return AddRoute(mgStart,mgEnd,_weight);
}

bool MapGoal::AddRoute(const MapGoalPtr &_routeStart, const MapGoalPtr &_midpt, float _weight)
{
	if(_routeStart && _midpt)
	{
		Route r;
		r.m_Start = _routeStart;
		r.m_End = _midpt;
		r.m_Weight = _weight;

		m_Routes.reserve(m_Routes.size()+1);
		m_Routes.push_back(r);
		return true;
	}	
	return false;
}

bool MapGoal::RouteTo(Client *_bot, DestinationVector &_dest, float _minradius)
{
	Routes routes;

	float fTotalWeight = 0.f;
	Routes::const_iterator cIt = m_Routes.begin(), cItEnd = m_Routes.end();
	for(; cIt != cItEnd; ++cIt)
	{
		const Route &r = (*cIt);
		float fDistSq = SquaredLength(_bot->GetPosition(), r.m_Start->GetPosition());
		if(r.m_Start->IsAvailable(_bot->GetTeam()) &&
			r.m_End->IsAvailable(_bot->GetTeam()) &&
			fDistSq < Mathf::Sqr(r.m_Start->GetRadius()))
		{
			routes.push_back(r);
			fTotalWeight += r.m_Weight;
		}
	}

	if(!routes.empty())
	{
		int iIndex = routes.size()-1;

		float fWght = Mathf::IntervalRandom(0.f, fTotalWeight);
		for(obuint32 i = 0; i < routes.size(); ++i)
		{
			fWght -= routes[i].m_Weight;
			if(fWght <= 0.f)
			{
				iIndex = i;
				break;
			}
		}

		Destination d;
		d.m_Position = routes[iIndex].m_End->GetPosition();
		d.m_Radius = Mathf::Max(routes[iIndex].m_End->GetRadius(), _minradius);
		_dest.push_back(d);

		return true;
	}

	if(GetNumUsePoints() > 0)
	{
		for(obint32 i = 0; i < GetNumUsePoints(); ++i)
		{
			Destination d;
			d.m_Position = GetWorldUsePoint(i);
			d.m_Radius = Mathf::Max(GetRadius(), _minradius);
			_dest.push_back(d);
		}
	}
	else
	{
		Destination d;
		d.m_Position = GetWorldUsePoint();
		d.m_Radius = Mathf::Max(GetRadius(), _minradius);
		_dest.push_back(d);
	}
	return false;
}

void MapGoal::SetProperty(const String &_propname, const obUserData &_val)
{
	gmMachine *pMachine = ScriptManager::GetInstance()->GetMachine();
	
	DisableGCInScope gcEn(pMachine);
	
	gmVariable var = Utils::UserDataToGmVar(pMachine,_val);

	bool Processed = false;

	StringStr err;

	if(!Processed)
	{
		if(_val.IsString())
		{
			PropertyMap pm;
			pm.AddProperty(_propname,_val.GetString());
			Processed = FromPropertyMap(pm,err);
		}
	}

	if(!Processed)
	{
		Processed = FromScriptVar(pMachine,_propname.c_str(),var,err);
	}
	
	if(!Processed)
	{
		if(m_SetPropertyFunc)
		{
			gmGCRoot<gmUserObject> mgref = GetScriptObject(pMachine);

			gmCall call;
			if(call.BeginFunction(pMachine, m_SetPropertyFunc, gmVariable(mgref)))
			{
				call.AddParamString(_propname.c_str());
				call.AddParam(var);
				call.End();
			}
		}
	}
	
	// rebuild the name
	GenerateName(0);

	if(!Processed && !err.str().empty())
		EngineFuncs::ConsoleErrorf("%s",err.str().c_str());
}

extern int NextDrawTime;

void MapGoal::RenderDebug(bool _editing, bool _highlighted)
{
	Prof_Scope(MapGoal);

#ifdef Prof_ENABLED
	Prof_Scope_Var Scope(*m_ProfZone, Prof_cache);
#endif

	{
		Prof(Render);

		if(GetRenderGoal())
		{
			if(m_RenderFunc)
			{
				if(!m_ActiveThread[ON_RENDER].IsActive())
				{
					gmMachine *pMachine = ScriptManager::GetInstance()->GetMachine();

					gmGCRoot<gmUserObject> mgref = GetScriptObject(pMachine);

					gmCall call;
					if(call.BeginFunction(pMachine, m_RenderFunc, gmVariable(mgref)))
					{
						call.AddParamInt(_editing?1:0);
						call.AddParamInt(_highlighted?1:0);
						if(call.End() == gmThread::EXCEPTION)
						{
							/*SetEnable(false, Utils::VA("Error in Update Callback in Goal: %s", GetName().c_str()));
							return State_Finished;*/
						}/**/

						m_ActiveThread[ON_RENDER] = call.GetThreadId();
						if(call.DidReturnVariable())
							m_ActiveThread[ON_RENDER] = 0;
					}
				}
			}
			else
			{
				if(IGame::GetTime() > NextDrawTime)
				{
					RenderDefault();
				}
			}
		}
		else
		{
			m_ActiveThread[ON_RENDER].Reset();
		}

		//////////////////////////////////////////////////////////////////////////
		if(IGame::GetTime() > NextDrawTime)
		{
			if(GetRenderRoutes())
			{
				DrawRoute(COLOR::GREEN,2.f);
			}
		}
	}
}

void MapGoal::RenderDefault()
{
	const BitFlag32 bf(GetDefaultDrawFlags());

	Vector3f vRenderPos = GetPosition();
	obColor vRenderColor = COLOR::GREEN; // customize?

	if(bf.CheckFlag(DrawCenterBounds))
	{
		vRenderPos = GetWorldBounds().Center;
	}

	vRenderPos.z += GetRenderHeight();

	Vector3f vLocalPos, vLocalFace;
	if(!Utils::GetLocalFacing(vLocalFace) || !Utils::GetLocalPosition(vLocalPos))
		return;

	//////////////////////////////////////////////////////////////////////////
	if(GetDefaultRenderRadius() < Utils::FloatMax)
	{		
		if(Length(vLocalPos,vRenderPos) > GetDefaultRenderRadius())
		{
			return;
		}
	}
	//////////////////////////////////////////////////////////////////////////

	String txtOut;
	
	if(bf.CheckFlag(DrawName))
	{
		txtOut += GetName();
		txtOut += "\n";
	}
	if(bf.CheckFlag(DrawGroup))
	{
		if(!GetGroupName().empty())
		{
			txtOut += " Group: ";
			txtOut += GetGroupName();
			txtOut += "\n";
		}
	}
	if(bf.CheckFlag(DrawRole))
	{
		const String strRole = Utils::BuildRoleName(GetRoleMask().GetRawFlags());
		if(strRole != "None")
		{
			txtOut += "Roles: ";
			txtOut += strRole;
			txtOut += "\n";
		}
	}
	if(bf.CheckFlag(DrawInitialAvail))
	{
		txtOut += "Initial: ";
		txtOut += Utils::GetTeamString(m_AvailableTeamsInit.GetRawFlags());
		txtOut += "\n";
	}
	if(bf.CheckFlag(DrawCurrentAvail))
	{
		txtOut += "Active: ";
		txtOut += Utils::GetTeamString(m_AvailableTeams.GetRawFlags());
		txtOut += "\n";
	}

	// bounds
	if(bf.CheckFlag(DrawBounds))
	{
		/*if(m_bUseOrientedBox)
		Utils::OutlineOBB(GetPosition(),GetMatrix(),m_OrientedBounds,COLOR::MAGENTA,2.f);
		else*/
		Utils::OutlineOBB(GetWorldBounds(), COLOR::ORANGE, 2.f);
	}
	
	// radius
	if(bf.CheckFlag(DrawRadius))
	{
		if(GetRadius() != 0.f)
			Utils::DrawRadius(GetPosition(), GetRadius(), COLOR::ORANGE, 2.f);
		else
			Utils::DrawLine(GetPosition(), GetPosition() + Vector3f::UNIT_Z * 32.f, COLOR::ORANGE, 2.f);
	}

	// use pts
	for(int i = 0; i < GetNumUsePoints(); ++i)
	{
		Vector3f vUsePt = GetWorldUsePoint(i);
		Utils::DrawLine(vUsePt, vUsePt + Vector3f::UNIT_Z * 32.f, COLOR::GREEN, 2.f);
	}

	if(bf.CheckFlag(DrawDisabled))
	{
		if(GetDisabled())
		{
			txtOut += "DISABLED";
			txtOut += "\n";
			vRenderColor = COLOR::RED;
		}
	}
	
	if(m_ExtraDebugText)
	{
		gmStringObject *so = m_ExtraDebugText;
		const char *str = so->GetString();
		if(str)
		{
			txtOut += str;
			txtOut += "\n";
		}
	}

	Utils::PrintText(
		vRenderPos,
		vRenderColor,
		2.f,
		"%s",
		txtOut.c_str());
}

Vector3f MapGoal::CalculateFarthestFacing()
{
	const float fRayLength = 5000.0f;

	float fBestVectorLength = 0.0f;
	Vector3f vBestVector = Vector3f::UNIT_Z;

	obTraceResult tr;

	// Calculate the farthest looking vector from this point, at 5 degree intervals.
	static float fInterval = 5.0f;
	for(float fAng = fInterval; fAng <= 360.0f; fAng += fInterval)
	{
		Quaternionf quat(Vector3f::UNIT_Z, Mathf::DegToRad(fAng));
		Vector3f vVec = quat.Rotate(Vector3f::UNIT_Y*fRayLength);
		EngineFuncs::TraceLine(tr, GetPosition(), GetPosition()+vVec, NULL, TR_MASK_SHOT, -1, False);
		float fSightLength = tr.m_Fraction * fRayLength;
		if(fSightLength > fBestVectorLength)
		{
			fBestVectorLength = fSightLength;
			vBestVector = vVec;
		}
	}
	vBestVector.Normalize();
	return vBestVector;
}

void MapGoal::CheckPropertiesBound()
{
	if(!GetPropertiesBound())
	{
		BindProperties();
		SetPropertiesBound(true);
	}
}

void MapGoal::BindProperties()
{
	BindProperty("Name",m_Name,Prop::PF_READONLY);
	BindProperty("TagName",m_TagName);
	BindProperty("Group",m_GroupName);
	BindProperty("Type",m_GoalType);
	BindProperty("Entity",m_Entity);
	BindProperty("SerialNum",m_SerialNum);
	BindProperty("Priority",m_DefaultPriority);
	BindProperty("Radius",m_Radius);
	
	{
		int EnumSize = 0;
		const IntEnum *Enum = 0;
		IGameManager::GetInstance()->GetGame()->GetTeamEnumeration(Enum,EnumSize);
		BindProperty("Available",m_AvailableTeamsInit,0,Enum,EnumSize);
	}

	{
		int EnumSize = 0;
		const IntEnum *Enum = 0;
		IGameManager::GetInstance()->GetGame()->GetRoleEnumeration(Enum,EnumSize);
		BindProperty("Roles",m_RoleMask,0,Enum,EnumSize);
	}

	BindProperty("Position",m_Position,Prop::PF_POSITION);
	//BindProperty("Facing",m_Facing,Prop::PF_FACING);
	//BindFunction("DebugRender",this,&MapGoal::ToggleRender);
}

void MapGoal::ResetGoalPriorities()
{
	for(int t = 0; t < ClassPriority::MaxTeams; ++t)
	{
		for(int c = 0; c < ClassPriority::MaxClasses; ++c)
		{
			m_ClassPriority.Priorities[t][c] = -1.f;
		}
	}
}

void MapGoal::SetPriorityForClass(int _teamid, int _classId, obReal _priority)
{
	//////////////////////////////////////////////////////////////////////////
	if(_teamid)
		_teamid=(1<<_teamid);
	else
		_teamid = -1;

	if(_classId)
		_classId=(1<<_classId);
	else
		_classId = -1;

	//////////////////////////////////////////////////////////////////////////
	for(int t = 1; t < ClassPriority::MaxTeams; ++t)
	{
		for(int c = 1; c < ClassPriority::MaxClasses; ++c)
		{
			if((1<<t)&_teamid && (1<<c)&_classId)
			{
				m_ClassPriority.Priorities[t-1][c-1] = _priority;
			}			
		}
	}
}

obReal MapGoal::GetPriorityForClient(Client *_client)
{
	float prio = GetPriorityForClass(_client->GetTeam(),_client->GetClass());
	if(prio > 0.f && GetRoleMask().AnyFlagSet())
	{
		if((GetRoleMask() & _client->GetRoleMask()).AnyFlagSet())
		{
			prio += m_RolePriorityBonus;
		}
	}
	return prio;
}

obReal MapGoal::GetPriorityForClass(int _teamid, int _classId)
{
	if(_teamid>0 && _teamid<ClassPriority::MaxTeams &&
		_classId>0 && _classId<ClassPriority::MaxClasses)
	{
		float classPrio = m_ClassPriority.Priorities[_teamid-1][_classId-1];
		if(classPrio != -1.f)
		{
			return classPrio;
		}
	}
	return GetDefaultPriority();
}

//////////////////////////////////////////////////////////////////////////

struct PersistentPriority
{
	String	m_Expression;
	int		m_Team;
	int		m_Class;
	float	m_Priority;
};

typedef std::vector<PersistentPriority> PersPriorityList;
PersPriorityList gPriorityList;

void MapGoal::ClassPriority::GetPriorityText(std::string &_txtout) const
{
	//////////////////////////////////////////////////////////////////////////
	int CurrentIndex = 0;
	enum { MaxClassPriority = 32 };
	struct PrioritySummary
	{
		obint32		m_TeamId;
		obint32		m_ClassId;
		obReal		m_Priority;
	};
	PrioritySummary Summary[MaxClassPriority];

	// Search all entries to build a list of summarized priority information
	for(int t = 1; t <= ClassPriority::MaxTeams && CurrentIndex<MaxClassPriority-1; ++t)
	{
		if(!Utils::TeamExists(t))
			continue;

		for(int c = 1; c <= ClassPriority::MaxClasses && CurrentIndex<MaxClassPriority-1; ++c)
		{
			if(!Utils::ClassExists(c))
				continue;

			// class and team ids start at 1
			const obReal CurrentPriority = Priorities[t-1][c-1];

			if(CurrentPriority == -1.f)
				continue;

			bool bFound = false;

			// search for matching entry.
			for(int i = 0; i < CurrentIndex; ++i)
			{
				if(Summary[i].m_Priority == CurrentPriority && Summary[i].m_TeamId & (1<<t))
				{
					Summary[i].m_TeamId |= (1<<t);
					Summary[i].m_ClassId |= (1<<c);
					bFound = true;
				}
			}

			if(!bFound)
			{
				Summary[CurrentIndex].m_Priority = CurrentPriority;
				Summary[CurrentIndex].m_TeamId = (1<<t);
				Summary[CurrentIndex].m_ClassId = (1<<c);
				++CurrentIndex;
			}
		}
	}

	//////////////////////////////////////////////////////////////////////////
	// print the output
	for(int i = 0; i < CurrentIndex; ++i)
	{		
		if(Summary[i].m_ClassId && Summary[i].m_TeamId)
		{
			_txtout = "    "; // indent
			_txtout += Utils::GetTeamString(Summary[i].m_TeamId);
			_txtout += " ";
			_txtout += Utils::GetClassString(Summary[i].m_ClassId);
			_txtout += " ";
			_txtout += Utils::VA(" %.2f",Summary[i].m_Priority);
			EngineFuncs::ConsoleMessage(_txtout.c_str());
		}
	}
}

void MapGoal::SetPersistentPriorityForClass(const String &_exp, int _team, int _class, float _priority)
{
	for(obuint32 i = 0; i < gPriorityList.size(); ++i)
	{
		if(_exp == gPriorityList[i].m_Expression)
		{
			gPriorityList[i].m_Team = _team;
			gPriorityList[i].m_Class = _class;
			gPriorityList[i].m_Priority = _priority;
			return;
		}
	}

	PersistentPriority pp;
	pp.m_Expression = _exp;
	pp.m_Team = _team;
	pp.m_Class = _class;
	pp.m_Priority = _priority;
	gPriorityList.push_back(pp);
}

void MapGoal::CheckForPersistentPriority()
{
	for(obuint32 i = 0; i < gPriorityList.size(); ++i)
	{
		boost::regex expression(gPriorityList[i].m_Expression, REGEX_OPTIONS);

		if(boost::regex_match(GetName(), expression))
		{
			SetPriorityForClass(
				gPriorityList[i].m_Team,
				gPriorityList[i].m_Class,
				gPriorityList[i].m_Priority);
			return;
		}
	}
}

//void MapGoal::DrawBounds(int _color, float _duration)
//{
//	Utils::OutlineAABB(GetWorldBounds(), _color, _duration);
//}
//
//void MapGoal::DrawRadius(int _color, float _duration)
//{
//	if(GetRadius() != 0.f)
//		Utils::DrawRadius(GetPosition(), GetRadius(), _color, _duration);
//	else
//		Utils::DrawLine(GetPosition(), GetPosition() + Vector3f::UNIT_Z * 32.f, _color, _duration);
//}
//
//void MapGoal::DrawUsePoints(int _color, float _duration)
//{
//	for(int i = 0; i < GetNumUsePoints(); ++i)
//	{
//		Vector3f vUsePt = GetWorldUsePoint(i);
//		Utils::DrawLine(vUsePt, vUsePt + Vector3f::UNIT_Z * 32.f, _color, _duration);
//	}
//}
void MapGoal::DrawRoute(int _color, float _duration)
{
	PathPlannerBase *planner = NavigationManager::GetInstance()->GetCurrentPathPlanner();

	Routes::const_iterator cIt = m_Routes.begin(), cItEnd = m_Routes.end();
	for(; cIt != cItEnd; ++cIt)
	{
		const Route &r = (*cIt);
		planner->PlanPathToGoal(NULL,r.m_Start->GetPosition(),r.m_End->GetPosition(),0);

		Path p;
		planner->GetPath(p);
		p.DebugRender(_color,_duration);
	}
}

void MapGoal::FromScriptTable(gmMachine *_machine, gmTableObject *_tbl, bool _caseSense)
{
	gmTableObject *mytbl = gmBind2::Class<MapGoal>::GetTable(GetScriptObject(_machine));
	if(mytbl)
	{
		gmTableIterator tIt;
		gmTableNode *pNode = _tbl->GetFirst(tIt);
		while(pNode)
		{
			gmTableNode * existingNode = !_caseSense ? mytbl->GetTableNode(_machine,pNode->m_key,false) : 0;
			if(existingNode)
			{
				existingNode->m_value = pNode->m_value;
			}
			else
			{
				mytbl->Set(_machine,pNode->m_key,pNode->m_value);
			}
			pNode = _tbl->GetNext(tIt);
		}
	}
}

gmVariable MapGoal::GetProperty(const char *_name)
{
	gmMachine *pMachine = ScriptManager::GetInstance()->GetMachine();

	gmTableObject *tbl = gmBind2::Class<MapGoal>::GetTable(GetScriptObject(pMachine));
	if(tbl)
		return pMachine->Lookup(_name,tbl);
	return gmVariable::s_null;
}

bool MapGoal::GetProperty(const char *_name, Vector3f &_var)
{
	gmVariable gmVar = GetProperty(_name);
	return gmVar.GetVector(_var);
}

bool MapGoal::GetProperty(const char *_name, float &_var)
{
	gmVariable gmVar = GetProperty(_name);
	return gmVar.GetFloatSafe(_var,0.f);
}

bool MapGoal::GetProperty(const char *_name, bool &_var)
{
	gmVariable gmVar = GetProperty(_name);
	int intvar = 0;
	if(gmVar.GetIntSafe(intvar,0))
	{
		_var = intvar != 0;
		return true;
	}
	return false;
}
bool MapGoal::GetProperty(const char *_name, int &_var)
{
	gmVariable gmVar = GetProperty(_name);
	return gmVar.GetIntSafe(_var,0);
}

bool MapGoal::GetProperty(const char *_name, String &_var)
{
	gmVariable gmVar = GetProperty(_name);
	const char *cstr = gmVar.GetCStringSafe();
	if(cstr)
		_var = cstr;

	return cstr!=0;
}
bool MapGoal::GetProperty(const char *_name, Seconds &_var)
{
	gmVariable gmVar = GetProperty(_name);
	float Secs = 0.f;
	if(gmVar.GetFloatSafe(Secs,0.f))
	{
		_var = Seconds(Secs);
		return true;
	}
	return false;
}

bool MapGoal::SaveToTable(gmMachine *_machine, gmGCRoot<gmTableObject> &_savetable, ErrorObj &_err)
{
	gmGCRoot<gmTableObject> GoalTable(_machine->AllocTableObject(),_machine);

	if(m_SerializeFunc)
	{
		gmCall call;
		gmGCRoot<gmUserObject> mgref = GetScriptObject(_machine);
		if(call.BeginFunction(_machine, m_SerializeFunc, gmVariable(mgref)))
		{				
			call.AddParamTable(GoalTable);
			const int ThreadState = call.End();
			if(ThreadState!=gmThread::KILLED)
			{
				_err.AddError("Error Calling Script Serialize function!");
				return false;
			}
		}
	}

	//////////////////////////////////////////////////////////////////////////
	// Standard info.
	GoalTable->Set(_machine,"Version",gmVariable(m_Version));
	GoalTable->Set(_machine,"GoalType",GetGoalType().c_str());
	GoalTable->Set(_machine,"Name",GetName().c_str());
	GoalTable->Set(_machine,"TagName",GetTagName().c_str());	
	GoalTable->Set(_machine,"GroupName",GetGroupName().c_str());
	GoalTable->Set(_machine,"Position",gmVariable(m_InterfacePosition.IsZero() ? m_Position : m_InterfacePosition));
	GoalTable->Set(_machine,"Radius",gmVariable(m_Radius));
	GoalTable->Set(_machine,"MinRadius",gmVariable(m_MinRadius));
	GoalTable->Set(_machine,"SerialNum",gmVariable(m_SerialNum));
	GoalTable->Set(_machine,"CreateOnLoad",gmVariable(m_CreateOnLoad));

	/*gmGCRoot<gmUserObject> userBounds = gmBind2::Class<BoundingBox>::WrapObject(_machine,&m_Bounds,true);
	GoalTable->Set(_machine,"Bounds",gmVariable(userBounds));*/

	Vector3f Euler(0,0,0);
	m_Orientation.ToEulerAnglesZXY(Euler.x,Euler.y,Euler.z);
	GoalTable->Set(_machine,"Orientation",gmVariable(Euler));
	//////////////////////////////////////////////////////////////////////////

	GoalTable->Set(_machine,"TeamAvailability",gmVariable(m_AvailableTeamsInit.GetRawFlags()));

	GoalTable->Set(_machine,"Roles",gmVariable::s_null);
	if(m_RoleMask.AnyFlagSet())
	{
		gmTableObject * roleTable = _machine->AllocTableObject();

		int NumElements = 0;
		const IntEnum *Enum = 0;
		IGameManager::GetInstance()->GetGame()->GetRoleEnumeration(Enum,NumElements);
		for(int i = 0; i < 32; ++i)
		{
			if(m_RoleMask.CheckFlag(i))
			{
				for(int e = 0; e < NumElements; ++e)
				{
					if(Enum[e].m_Value == i)
					{
						roleTable->Set(_machine,roleTable->Count(),Enum[e].m_Key);
						break;
					}
				}
			}
		}
		GoalTable->Set(_machine,"Roles",gmVariable(roleTable));
	}
	

	//////////////////////////////////////////////////////////////////////////
	_savetable->Set(_machine,GetName().c_str(),gmVariable(GoalTable));
	return true;
}

bool MapGoal::LoadFromTable(gmMachine *_machine, gmGCRoot<gmTableObject> &_loadtable, ErrorObj &_err)
{
	gmGCRoot<gmTableObject> proptable;
	proptable.Set(_machine->AllocTableObject(),_machine);

	_loadtable->CopyTo(_machine,proptable);
	
	if(const char *TagName = proptable->Get(_machine,"TagName").GetCStringSafe(0))
		m_TagName = TagName;
	else
	{
		_err.AddError("Goal.TagName Field Missing!");
		return false;
	}
	if(const char *Name = proptable->Get(_machine,"Name").GetCStringSafe(0))
		m_Name = Name;
	else
	{
		_err.AddError("Goal.Name Field Missing!");
		return false;
	}
	if(const char *GroupName = proptable->Get(_machine,"GroupName").GetCStringSafe(0))
		m_GroupName = GroupName;
	else
	{
		_err.AddError("Goal.GroupName Field Missing!");
		return false;
	}
	if(!proptable->Get(_machine,"Version").GetInt(m_Version,0))
	{
		_err.AddError("Goal.Version Field Missing!");
		return false;
	}
	if(!proptable->Get(_machine,"Position").GetVector(m_Position))
	{
		_err.AddError("Goal.Position Field Missing!");
		return false;
	}
	if(!proptable->Get(_machine,"Radius").GetFloatSafe(m_Radius))
	{
		_err.AddError("Goal.Radius Field Missing!");
		return false;
	}
	if(!proptable->Get(_machine,"MinRadius").GetFloatSafe(m_MinRadius))
	{
		_err.AddError("Goal.MinRadius Field Missing!");
		return false;
	}
	if(!proptable->Get(_machine,"SerialNum").GetIntSafe(m_SerialNum))
	{
		_err.AddError("Goal.SerialNum Field Missing!");
		return false;
	}

	Vector3f Euler(0,0,0);
	if(!proptable->Get(_machine,"Orientation").GetVector(Euler))
	{
		_err.AddError("Goal.Orientation Field Missing!");
		return false;
	}
	m_Orientation.FromEulerAnglesZXY(Euler.x,Euler.y,Euler.z);

	int InitialTeams = 0;
	if(!proptable->Get(_machine,"TeamAvailability").GetIntSafe(InitialTeams,0))
	{
		//_err.AddError("Goal.TeamAvailability Field Missing!");
		//return false;
	}
	m_AvailableTeamsInit = BitFlag32(InitialTeams);
	m_AvailableTeams = m_AvailableTeamsInit;
	
	m_RoleMask.ClearAll();
	if(gmTableObject * roleTable = proptable->Get(_machine,"Roles").GetTableObjectSafe())
	{
		int NumElements = 0;
		const IntEnum *Enum = 0;
		IGameManager::GetInstance()->GetGame()->GetRoleEnumeration(Enum,NumElements);

		gmTableIterator tIt;
		gmTableNode * pNodeRole = roleTable->GetFirst(tIt);
		while(pNodeRole)
		{
			const char * roleName = pNodeRole->m_value.GetCStringSafe(0);
			if(roleName)
			{
				for(int e = 0; e < NumElements; ++e)
				{
					if(!Utils::StringCompareNoCase(roleName,Enum[e].m_Key))
					{
						m_RoleMask.SetFlag(Enum[e].m_Value);
					}
				}
			}
			pNodeRole = roleTable->GetNext(tIt);
		}
	}

	int CreateOnLoad = GetCreateOnLoad();
	if(!proptable->Get(_machine,"CreateOnLoad").GetIntSafe(CreateOnLoad,CreateOnLoad))
	{
		//_err.AddError("Goal.TeamAvailability Field Missing!");
		//return false;
	}
	SetCreateOnLoad(CreateOnLoad!=0);

	// clear out the properties we don't want to pass along.
	//proptable->Set(_machine,"Version",gmVariable::s_null);
	proptable->Set(_machine,"Name",gmVariable::s_null);
	proptable->Set(_machine,"TagName",gmVariable::s_null);
	proptable->Set(_machine,"GroupName",gmVariable::s_null);
	proptable->Set(_machine,"Position",gmVariable::s_null);
	proptable->Set(_machine,"Radius",gmVariable::s_null);
	proptable->Set(_machine,"MinRadius",gmVariable::s_null);
	proptable->Set(_machine,"SerialNum",gmVariable::s_null);
	proptable->Set(_machine,"GoalType",gmVariable::s_null);
	proptable->Set(_machine,"Orientation",gmVariable::s_null);
	proptable->Set(_machine,"TeamAvailability",gmVariable::s_null);
	proptable->Set(_machine,"Roles",gmVariable::s_null);
	
	return InternalInit(proptable,false);
}

void MapGoal::ShowHelp()
{
	if(m_HelpFunc)
	{
		gmMachine *pMachine = ScriptManager::GetInstance()->GetMachine();

		gmGCRoot<gmUserObject> mgref = GetScriptObject(pMachine);

		gmCall call;
		if(call.BeginFunction(pMachine, m_HelpFunc, gmVariable(mgref)))
		{
			call.End();
		}
	}
}

void MapGoal::HudDisplay()
{
#ifdef ENABLE_DEBUG_WINDOW
	if(DW.Hud.mPropertySheet)
	{
		GatherProperties(DW.Hud.mPropertySheet);

		if(m_HudDisplay)
		{
			gmMachine *pMachine = ScriptManager::GetInstance()->GetMachine();

			gmGCRoot<gmUserObject> mgref = GetScriptObject(pMachine);

			gmCall call;
			if(call.BeginFunction(pMachine, m_HudDisplay, gmVariable(mgref)))
			{
				call.AddParamUser(DW.Hud.mUserObject);
				call.End();
			}
		}
	}
#endif
}

void MapGoal::CreateGuiFromSchema(gmMachine *a_machine, gmTableObject *a_schema)
{
#ifdef ENABLE_DEBUG_WINDOW
	gmTableIterator it;
	gmTableNode *pNode = a_schema->GetFirst(it);
	while(pNode)
	{
		const char *PropName = pNode->m_key.GetCStringSafe();
		if(PropName)
		{
			gmVariable thisObj(GetScriptObject(a_machine));

			gmVariable current(gmVariable::s_null);
			const gmSchema::ElementType et = gmSchema::GetElementType(a_machine,pNode->m_value);
			switch (et)
			{
				case gmSchema::EL_TABLEOF:
					{
						break;
					}
				case gmSchema::EL_ENUM:
					{
						const gmTableObject *options = gmSchema::GetEnumOptions(a_machine,pNode->m_value,thisObj,current);
						if(options)
						{
							MgListModel *lm = new MgListModel(options);
							gcn::DropDown *widget = new gcn::DropDown(lm);
							widget->getScrollArea()->setHeight(widget->getFont()->getHeight() * 4);
							widget->setActionEventId(PropName);
							widget->addActionListener(this);
							widget->addDeathListener(lm);

							DW.Hud.mPropertySheet->addProperty(PropName,widget);
						}
						break;
					}
				case gmSchema::EL_FLOATRANGE:
					{
						break;
					}
				case gmSchema::EL_INTRANGE:
					{
						break;
					}
				case gmSchema::EL_NUMRANGE:
					{
						float minrange, maxrange;
						if(gmSchema::GetNumRange(a_machine,pNode->m_value,thisObj,current,minrange,maxrange))
						{
							gcn::Slider *widget = new gcn::Slider(minrange, maxrange);
							widget->setActionEventId(PropName);
							widget->addActionListener(this);
							widget->setValue(/*current*/0.f);
							DW.Hud.mPropertySheet->addProperty(PropName,widget);
						}
						break;
					}
				case gmSchema::EL_VARTYPE:
					{
						break;
					}
				case gmSchema::EL_NONE:
				default:
					{
						break;
					}
			}
		}
		pNode = a_schema->GetNext(it);
	}
#endif
}

#ifdef ENABLE_DEBUG_WINDOW
void MapGoal::action(const gcn::ActionEvent& actionEvent)
{

}
#endif

//////////////////////////////////////////////////////////////////////////

static int MG_HandleMaxUsers(gmThread *a_thread, MapGoal::TrackingCat _cat)
{
	obint32 iMaxUsers = 0;

	MapGoal *NativePtr = 0;
	if(!gmBind2::Class<MapGoal>::FromThis(a_thread,NativePtr) || !NativePtr)
	{
		GM_EXCEPTION_MSG("Script Function on NULL MapGoal"); 
		return GM_EXCEPTION;
	}

	switch(a_thread->GetNumParams())
	{
	case 1:
		{
			GM_CHECK_INT_PARAM(newval, 0);
			iMaxUsers = NativePtr->GetMaxUsers(_cat);
			NativePtr->SetMaxUsers(_cat, newval);
		}
	case 0:
		{
			iMaxUsers = NativePtr->GetMaxUsers(_cat);
			break;
		}
	default:
		GM_EXCEPTION_MSG("Expected 0 or 1 param."); 
		return GM_EXCEPTION;
	}	

	a_thread->PushInt(iMaxUsers);
	return GM_OK;
}

static int gmfMaxUsers_InProgress(gmThread *a_thread)
{
	return MG_HandleMaxUsers(a_thread, MapGoal::TRACK_INPROGRESS);	
}

static int gmfMaxUsers_InUse(gmThread *a_thread)
{
	return MG_HandleMaxUsers(a_thread, MapGoal::TRACK_INUSE);	
}

static int gmfGetEntity(gmThread *a_thread)
{
	MapGoal *NativePtr = 0;
	if(!gmBind2::Class<MapGoal>::FromThis(a_thread,NativePtr) || !NativePtr)
	{
		GM_EXCEPTION_MSG("Script Function on NULL MapGoal"); 
		return GM_EXCEPTION;
	}

	if(NativePtr->GetEntity().IsValid())
		a_thread->PushEntity(NativePtr->GetEntity().AsInt());
	else
		a_thread->PushNull();
	return GM_OK;
}

static int gmfGetOwner(gmThread *a_thread)
{
	MapGoal *NativePtr = 0;
	if(!gmBind2::Class<MapGoal>::FromThis(a_thread,NativePtr) || !NativePtr)
	{
		GM_EXCEPTION_MSG("Script Function on NULL MapGoal"); 
		return GM_EXCEPTION;
	}

	if(NativePtr->GetOwner().IsValid())
		a_thread->PushEntity(NativePtr->GetOwner().AsInt());
	else
		a_thread->PushNull();
	return GM_OK;
}

static int gmfAddUsePoint(gmThread *a_thread)
{
	MapGoal *NativePtr = 0;
	if(!gmBind2::Class<MapGoal>::FromThis(a_thread,NativePtr) || !NativePtr)
	{
		GM_EXCEPTION_MSG("Script Function on NULL MapGoal"); 
		return GM_EXCEPTION;
	}

	GM_CHECK_NUM_PARAMS(1);
	GM_CHECK_VECTOR_PARAM(v,0);
	GM_INT_PARAM(rel, 1, 0);

	NativePtr->AddUsePoint(Vector3f(v.x,v.y,v.z), rel != 0);
	return GM_OK;
}

static int gmfLimitToWeapon(gmThread *a_thread)
{
	MapGoal *NativePtr = 0;
	if(!gmBind2::Class<MapGoal>::FromThis(a_thread,NativePtr) || !NativePtr)
	{
		GM_EXCEPTION_MSG("Script Function on NULL MapGoal"); 
		return GM_EXCEPTION;
	}

	return NativePtr->GetLimitWeapons().FromScript(a_thread);
}

static int gmfDisableWithEntityFlags(gmThread *a_thread)
{
	MapGoal *NativePtr = 0;
	if(!gmBind2::Class<MapGoal>::FromThis(a_thread,NativePtr) || !NativePtr)
	{
		GM_EXCEPTION_MSG("Script Function on NULL MapGoal"); 
		return GM_EXCEPTION;
	}

	BitFlag64 bf;
	for(int p = 0; p < a_thread->GetNumParams(); ++p)
	{
		GM_CHECK_INT_PARAM(fl,p);
		bf.SetFlag(fl,true);
		
	}
	NativePtr->SetDisableWithEntityFlag(bf);
	return GM_OK;
}

static int gmfDeleteWithEntityFlags(gmThread *a_thread)
{
	MapGoal *NativePtr = 0;
	if(!gmBind2::Class<MapGoal>::FromThis(a_thread,NativePtr) || !NativePtr)
	{
		GM_EXCEPTION_MSG("Script Function on NULL MapGoal"); 
		return GM_EXCEPTION;
	}

	BitFlag64 bf;
	for(int p = 0; p < a_thread->GetNumParams(); ++p)
	{
		GM_CHECK_INT_PARAM(fl,p);
		bf.SetFlag(fl,true);

	}
	NativePtr->SetDeleteWithEntityFlag(bf);
	return GM_OK;
}

static int gmfGetPriorityForClient(gmThread *a_thread)
{
	MapGoal *NativePtr = 0;
	if(!gmBind2::Class<MapGoal>::FromThis(a_thread,NativePtr) || !NativePtr)
	{
		GM_EXCEPTION_MSG("Script Function on NULL MapGoal"); 
		return GM_EXCEPTION;
	}

	if(a_thread->GetNumParams()==1)
	{
		GM_CHECK_NUM_PARAMS(1);
		GM_CHECK_GMBIND_PARAM(Client*, gmBot, bot, 0);
		a_thread->PushFloat(NativePtr->GetPriorityForClient(bot));
	}
	else if(a_thread->GetNumParams()==2)
	{
		GM_CHECK_INT_PARAM(teamId,0);
		GM_CHECK_INT_PARAM(classId,1);
		a_thread->PushFloat(NativePtr->GetPriorityForClass(teamId,classId));
	}
	else
	{
		GM_EXCEPTION_MSG("expected (int,int), or (bot)");
		return GM_EXCEPTION;
	}
	return GM_OK;
}

static int gmfSetRoles(gmThread *a_thread)
{
	MapGoal *NativePtr = 0;
	if(!gmBind2::Class<MapGoal>::FromThis(a_thread,NativePtr) || !NativePtr)
	{
		GM_EXCEPTION_MSG("Script Function on NULL MapGoal"); 
		return GM_EXCEPTION;
	}

	GM_CHECK_NUM_PARAMS(1);

	BitFlag32 rolemask;
	for(int p = 0; p < a_thread->GetNumParams(); ++p)
	{
		GM_CHECK_INT_PARAM(r,p);
		rolemask.SetFlag(r,true);
	}
	NativePtr->SetRoleMask(rolemask);
	return GM_OK;
}

static int gmfClearRoles(gmThread *a_thread)
{
	MapGoal *NativePtr = 0;
	if(!gmBind2::Class<MapGoal>::FromThis(a_thread,NativePtr) || !NativePtr)
	{
		GM_EXCEPTION_MSG("Script Function on NULL MapGoal"); 
		return GM_EXCEPTION;
	}

	GM_CHECK_NUM_PARAMS(1);

	BitFlag32 rolemask;
	for(int p = 0; p < a_thread->GetNumParams(); ++p)
	{
		GM_CHECK_INT_PARAM(r,p);
		rolemask.SetFlag(r,false);
	}
	NativePtr->SetRoleMask(rolemask);
	return GM_OK;
}

static int gmfHasRole(gmThread *a_thread)
{
	MapGoal *NativePtr = 0;
	if(!gmBind2::Class<MapGoal>::FromThis(a_thread,NativePtr) || !NativePtr)
	{
		GM_EXCEPTION_MSG("Script Function on NULL MapGoal"); 
		return GM_EXCEPTION;
	}

	GM_CHECK_NUM_PARAMS(1);
	for(int p = 0; p < a_thread->GetNumParams(); ++p)
	{
		GM_CHECK_INT_PARAM(r,p);
		if(NativePtr->GetRoleMask().CheckFlag(r))
		{
			a_thread->PushInt(1);
			return GM_OK;
		}
	}
	a_thread->PushInt(0);
	return GM_OK;
}

static int gmfSetBaseGoalType(gmThread *a_thread)
{
	MapGoal *NativePtr = 0;
	if(!gmBind2::Class<MapGoal>::FromThis(a_thread,NativePtr) || !NativePtr)
	{
		GM_EXCEPTION_MSG("Script Function on NULL MapGoal"); 
		return GM_EXCEPTION;
	}

	GM_CHECK_NUM_PARAMS(1);
	GM_CHECK_STRING_PARAM(basetype,0);

	bool Good = false;
	try
	{
		fs::path goalPath("scripts/mapgoals",fs::native);
		goalPath /= basetype;
		if(NativePtr->LoadFromFile(goalPath))
		{
			Good = true;
		}
	}
	catch(const std::exception &)
	{
	}

	if(!Good)
	{
		GM_EXCEPTION_MSG("Unable to set base goal type: %s",basetype); 
		return GM_EXCEPTION;
	}

	return GM_OK;
}

static int gmfCreateGuiFromSchema(gmThread *a_thread)
{
	GM_CHECK_USER_PARAM_TYPE(gmSchema::GM_SCHEMA,0);

	MapGoal *NativePtr = 0;
	if(!gmBind2::Class<MapGoal>::FromThis(a_thread,NativePtr) || !NativePtr)
	{
		GM_EXCEPTION_MSG("Script Function on NULL MapGoal"); 
		return GM_EXCEPTION;
	}

	gmTableObject *Schema = static_cast<gmTableObject*>(a_thread->Param(0).GetUserSafe(gmSchema::GM_SCHEMA));
	NativePtr->CreateGuiFromSchema(a_thread->GetMachine(),Schema);
	return GM_OK;
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

void MapGoal_AsString(MapGoal *a_var, char * a_buffer, int a_bufferSize)
{
	_gmsnprintf(a_buffer, a_bufferSize, 
		"MapGoal(%s::%s)",a_var->GetGoalType().c_str(),a_var->GetName().c_str());
}

//////////////////////////////////////////////////////////////////////////

//bool MapGoal::pfnSetDotEx(gmThread * a_thread, MapGoal * a_goal, const char *a_key, gmVariable * a_operands)
//{
//	MapGoal *Mg = 0;
//	if(gmBind2::Class<MapGoal>::FromVar(a_thread,a_operands[1],Mg) && Mg)
//	{
//		MapGoalPtr mg = Mg->GetSmartPtr();
//		
//		return true;
//	}
//
//	return false;
//}
//bool MapGoal::pfnGetDotEx(gmThread * a_thread, MapGoal * a_goal, const char *a_key, gmVariable * a_operands)
//{
//	return false;
//}

void MapGoal::Bind(gmMachine *_m)
{
	gmBind2::Global(_m,"InternalGoalState")
		.var((int)GoalStateFlagState,"FlagState")
		//.var((int)GoalStateFlagHoldState,"FlagHoldState")
		;

	gmBind2::Class<MapGoal>("MapGoal",_m)
		//.constructor()
		.asString(MapGoal_AsString)

		.func(&MapGoal::IsAvailable,	"IsAvailable","Is goal available for a given team.")
		.func(&MapGoal::SetAvailable,	"SetAvailable","Set goal available for a given team.")

		.func(&MapGoal::IsAvailableInitial,	"IsAvailableInitial","Is goal available for team initially.")
		.func(&MapGoal::SetAvailableInitial,"SetAvailableInitial","Set goal available for team initially.")

		.func(&MapGoal::GetGoalState,	"GetGoalState")
		.func(&MapGoal::GetGoalType,	"GetGoalType")
		.func(&MapGoal::GetTagName,		"GetTagName")
		.func(&MapGoal::GetName,		"GetName","Get the full name of the goal.")

		.func(&MapGoal::GetGroupName,	"GetGroupName","Get the current group this goal is assigned to.")
		.func(&MapGoal::SetGroupName,	"SetGroupName","Set the current group this goal is assigned to.")

		.func(gmfSetBaseGoalType,			"SetBaseGoalType","Clones a base goal type. Should be done first thing in goal script.")		

		.func(&MapGoal::GetPosition_Script,	"GetPosition","Get Goal Position")
		.func(&MapGoal::SetPosition_Script,	"SetPosition","Set Goal Position")
		.func(&MapGoal::GetFacing_Script,	"GetFacing","Get Goal Facing")
		.func(&MapGoal::SetFacing_Script,	"SetFacing","Set Goal Position")

		.func(&MapGoal::GetDisabled,		"IsDisabled","Get whether the goal is currently disabled.")
		.func(&MapGoal::SetDisabled,		"DisableGoal","Set whether the goal is currently disabled.")

		//.func(&MapGoal::GetWorldBounds,	"GetBounds")
		//.func(&MapGoal::GetLocalBounds,	"GetLocalBounds")
		//.func(&MapGoal::SetGoalBounds,	"SetBounds")
		.func(&MapGoal::SetBounds_Script,	"SetBounds","Set the object space bounding box of the goal.")
		.func(&MapGoal::GetBoundsCenter_Script,	"GetCenterBounds","Get the center of the bounding box.")
		//.func(&MapGoal::GetMatrix,		"GetMatrix")
		//.func(&MapGoal::SetMatrix,		"SetMatrix")

		.func(&MapGoal::GetRadius,			"GetRadius","Get the Goal Radius")
		.func(&MapGoal::SetRadius,			"SetRadius","Set the Goal Radius")

		//.func(&MapGoal::SetProperties,	"SetProperties")

		.func(gmfAddUsePoint,				"AddUsePoint","Adds a 'use' point to the goal.")
		.func(&MapGoal::GetWorldUsePoint,	"GetUsePoint","Gets a use point in world space, by index.")
		.func(&MapGoal::GetNumUsePoints,	"GetNumUsePoint","Gets the number of use points currently defined.")

		.func(gmfSetRoles,					"SetRoles","Sets the roles that are allowed to use this goal.")
		.func(gmfClearRoles,				"ClearRoles","Removes the given roles from this goal.")
		.func(gmfHasRole,					"HasRole","Return true if the goal has any of the roles provided as params.")

		.func(gmfDisableWithEntityFlags,	"DisableIfEntityFlag","Sets one or more entity flags that will cause the goal to be disabled.")
		.func(gmfDeleteWithEntityFlags,		"DeleteIfEntityFlag","Sets one or more entity flags that will cause the goal to be deleted.")
		
		/*.func(&MapGoal::DrawBounds,			"DrawBounds")
		.func(&MapGoal::DrawRadius,			"DrawRadius")
		.func(&MapGoal::DrawUsePoints,		"DrawUsePoints")
		.func(&MapGoal::DrawRoute,			"DrawRoutes")*/
		
		//.func(&MapGoal::SetEnableDraw,	"SetEnableDraw")

		//.func(&MapGoal::AddRoute,			"AddRoute")

		.func(&MapGoal::SetPriorityForClass,	"SetGoalPriority","Sets the priority for a given class/team.")
		.func(gmfGetPriorityForClient,			"GetGoalPriority","Gets the priority for a given class/team.")
		.func(&MapGoal::ResetGoalPriorities,	"ResetGoalPriorities","Clears all the current priorities.")		

		.func(&MapGoal::SetDeleteMe,			"SetRemoveFlag","Mark the goal for deletion.")

		.func(&MapGoal::RenderDefault,			"RenderDefault","Render the default debug options.")
		.var(&MapGoal::m_DefaultRenderRadius,	"DefaultRenderRadius","Radius in which debug options will be displayed.")
		.var(&MapGoal::m_RenderHeight,			"DefaultRenderHeight","Goal height offset where rendering will take place.")
		.var_bitfield(&MapGoal::m_DefaultDrawFlags,DrawName,"RenderDefaultName","Draw the name of the goal.")
		.var_bitfield(&MapGoal::m_DefaultDrawFlags,DrawGroup,"RenderDefaultGroup","Draw the group of the goal.")
		.var_bitfield(&MapGoal::m_DefaultDrawFlags,DrawRole,"RenderDefaultRole","Draw the roles for the goal.")
		.var_bitfield(&MapGoal::m_DefaultDrawFlags,DrawBounds,"RenderDefaultBounds","Draw the bounds of the goal.")
		.var_bitfield(&MapGoal::m_DefaultDrawFlags,DrawRadius,"RenderDefaultRadius","Draw the radius of the goal.")
		.var_bitfield(&MapGoal::m_DefaultDrawFlags,DrawInitialAvail,"RenderDefaultInitialAvailability","Draw the initial availability of the goal.")
		.var_bitfield(&MapGoal::m_DefaultDrawFlags,DrawCurrentAvail,"RenderDefaultCurrentAvailability","Draw the current availability of the goal.")
		.var_bitfield(&MapGoal::m_DefaultDrawFlags,DrawCenterBounds,"RenderDefaultAtCenterBounds","Draw debug options using the center of the bounding box.")
		
		.func(gmfMaxUsers_InProgress,			"MaxUsers_InProgress","Set the max number of 'inprogress' users that can use the goal")
		.func(gmfMaxUsers_InUse,				"MaxUsers_InUse","Set the max number of 'inuse' users that can use the goal")

		.func(gmfGetEntity,						"GetEntity","Get the entity of the goal, if any.")
		.func(gmfGetOwner,						"GetOwner","Gets the entity owner of the goal, if any.")

		.func(gmfCreateGuiFromSchema,						"CreateGuiFromSchema", "Create Gui elements for schema properties.")

		.func(&MapGoal::AddRoute_Script,		"AddRoute","Adds a route for this goal.")

		.func(gmfLimitToWeapon,					"LimitToWeapon","Adds a list of weapons that are required from any user of the goal.")

		.var(&MapGoal::m_Version,				"Version","int","Gets the goal version.")

		.var(&MapGoal::m_GoalState,				"GoalState","int","Gets the goal state")
		.var(&MapGoal::m_GoalStateFunction,		"GoalStateFunction","enum InternalGoalState")
				
		.var(&MapGoal::m_InitNewFunc,			"InitNewGoal","Callback","Called on goal creation to initialize any internal variables.")
		.var(&MapGoal::m_UpgradeFunc,			"UpgradeVersion","Callback","Called to upgrade the goal to the latest used version.")
		.var(&MapGoal::m_RenderFunc,			"Render","Callback","Called when draw_goals is enabled for this goal. Used to render itself.")
		.var(&MapGoal::m_UpdateFunc,			"Update","Callback","Called every frame to update the state of the goal if needed.")
		.var(&MapGoal::m_SerializeFunc,			"SaveToTable","Callback","Called when the goals are saved to a file. Allows the goal to serialize persistent information.")
		.var(&MapGoal::m_SetPropertyFunc,		"SetProperty","Callback","Called on bot goal_setproperty x y, where x is a property name and y is a value or keyword.")
		.var(&MapGoal::m_HelpFunc,				"Help","Callback","Called on bot goal_help to print requirements and available properties for the goal.")
		.var(&MapGoal::m_HudDisplay,			"HudDisplay","Callback","Called when goal is highlighted to create gui elements for debug visualization.")
		
		.var(&MapGoal::m_ExtraDebugText,		"ExtraDebugText", "string","Additional debug text to render in RenderDefault function.")

		.var(&MapGoal::m_Radius,				"Radius","float","Radius of the goal.")
		.var(&MapGoal::m_MinRadius,				"MinRadius","float","Minimum allowed radius of the goal.")
		//.var(&MapGoal::m_Bounds,				"Bounds")

		.var(&MapGoal::m_GoalType,				"GoalType","string","Type of goal.")
		
		.var(&MapGoal::m_DefaultPriority,		"DefaultPriority","float","Default priority of the goal, if no class/team specific priorities.")
		.var(&MapGoal::m_RolePriorityBonus,		"RolePriorityBonus","float","Role priority bonus of the goal, for users matching role.")
		
		.var_readonly(&MapGoal::m_SerialNum,	"SerialNum","int readonly","Auto generated unique serial number of the goal.")

		// flags		
		.var(&MapGoal::m_DeleteMe,				"MarkForRemoval","bool","Mark the goal for deletion.")
		.var(&MapGoal::m_DynamicPosition,		"DynamicPosition","bool","Goal should update its position from its entity.")
		.var(&MapGoal::m_DynamicOrientation,	"DynamicOrientation","bool","Goal should update its orientation from its entity.")
		.var(&MapGoal::m_RemoveWithEntity,		"RemoveWithEntity","bool","Goal should be removed if its entity is removed.")
		.var(&MapGoal::m_InUse,					"InUse","bool","Goal is inuse and should not be chosen.")
		.var(&MapGoal::m_DisableForControllingTeam,"DisableForControllingTeam","bool","Goal will be made unavailable for the team which controls it(GetOwner)")
		.var(&MapGoal::m_DontSave,				"DontSave","bool","Dont save this goal into the map goal script.")
		.var(&MapGoal::m_RenderGoal,			"RenderGoal","bool","Enable rendering for this goal.")
		.var(&MapGoal::m_RenderRoutes,			"RenderRoutes","bool","Enable rendering of the routes for this goal.")
		.var(&MapGoal::m_CreateOnLoad,			"CreateOnLoad","bool","False to not create the goal at load time, but keep the data around for when created by the interface.")
				
		/*.setDotEx(pfnSetDotEx)
		.getDotEx(pfnGetDotEx)*/
		;
}

