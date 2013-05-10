////////////////////////////////////////////////////////////////////////////////
//
// $LastChangedBy: jswigart@gmail.com $
// $LastChangedDate: 2013-03-15 00:12:40 -0500 (Fri, 15 Mar 2013) $
// $LastChangedRevision: 861 $
//
////////////////////////////////////////////////////////////////////////////////

#include "PathPlannerNavMesh.h"
#include "System.h"
#include "ScriptManager.h"

// Title: PathPlannerNavMesh Script Commands
//static PathPlannerNavMesh *GetWpPlanner()
//{
//	PathPlannerBase * pPlanner = System::mInstance->mNavigation;
//	if(pPlanner->GetPlannerType() == NAVID_WP)
//		return static_cast<PathPlannerNavMesh*>(pPlanner);
//	return 0;
//}
//
////////////////////////////////////////////////////////////////////////////
//
//// function: AddWaypoint
////		Add a waypoint at a specified location.
////
//// Parameters:
////
////		<Vector3> - The <Vector3> position to add waypoint.
////		<Vector3> - The <Vector3> facing for the waypoint.
////
//// Returns:
////		int - Waypoint Id if successful
////		OR
////		nul if there was an error adding waypoint.
//static int GM_CDECL gmfAddWaypoint(gmThread *a_thread)
//{
//	GM_CHECK_NUM_PARAMS(2);
//	GM_CHECK_VECTOR_PARAM(v1,0);
//	GM_CHECK_VECTOR_PARAM(v2,1);
//
//	PathPlannerNavMesh *pWp = GetWpPlanner();
//	if(pWp)
//	{
//		Waypoint *pWaypoint = pWp->AddWaypoint(Vector3f(v1.x,v1.y,v1.z), Vector3f(v2.x,v2.y,v2.z));
//		if(pWaypoint)
//		{
//			a_thread->PushInt(pWaypoint->GetUID());
//			return GM_OK;
//		}
//	}
//	a_thread->PushNull();
//	return GM_OK;
//}
//
////////////////////////////////////////////////////////////////////////////
//
//// function: DeleteWaypoint
////		Delete a waypoint from a specified location.
////
//// Parameters:
////
////		<Vector3> - The <Vector3> position to delete waypoint from.
////		- OR -
////		<int> - Waypoint Guid to delete
////
//// Returns:
////		int - true if successful, false if not
//static int GM_CDECL gmfDeleteWaypoint(gmThread *a_thread)
//{
//	GM_CHECK_NUM_PARAMS(1);
//
//	bool bSuccess = false;
//	PathPlannerNavMesh *pWp = GetWpPlanner();
//	if(pWp)
//	{
//		if(a_thread->ParamType(0) == GM_INT)
//		{
//			GM_CHECK_INT_PARAM(wpid,0);
//			Waypoint *w = pWp->GetWaypointByGUID(wpid);
//			if(w)
//				bSuccess = pWp->DeleteWaypoint(w->GetPosition());
//		}
//		else if(a_thread->ParamType(GM_VEC3))
//		{
//			GM_CHECK_VECTOR_PARAM(v,0);
//			bSuccess = pWp->DeleteWaypoint(Vector3f(v.x,v.y,v.z));
//		}
//	}
//	a_thread->PushInt(bSuccess ? 1 : 0);
//	return GM_OK;
//}
//
////////////////////////////////////////////////////////////////////////////
//
//void SetWaypointDataInTable(gmMachine *_machine, gmTableObject *_table, const Waypoint *_waypoint)
//{
//	DisableGCInScope gcEn(_machine);
//
//	_table->Set(_machine, "position", gmVariable(_waypoint->GetPosition().X(), _waypoint->GetPosition().Y(), _waypoint->GetPosition().Z()));
//	_table->Set(_machine, "facing", gmVariable(_waypoint->GetFacing().X(), _waypoint->GetFacing().Y(), _waypoint->GetFacing().Z()));
//	_table->Set(_machine, "guid", gmVariable(_waypoint->GetUID()));
//	_table->Set(_machine, "radius", gmVariable(_waypoint->GetRadius()));
//
//	gmTableObject *pFlagTable = _machine->AllocTableObject();
//
//	_table->Set(_machine, "flags", gmVariable(pFlagTable));
//
//	// add all flags this waypoint has.
//	//////////////////////////////////////////////////////////////////////////
//	{
//		const PathPlannerNavMesh::FlagMap &flagMap =  GetWpPlanner()->GetNavFlagMap();
//		PathPlannerNavMesh::FlagMap::const_iterator it = flagMap.begin(),
//			itEnd = flagMap.end();
//		for(;it != itEnd; ++it)
//		{
//			if(_waypoint->IsFlagOn(it->second))
//				pFlagTable->Set(_machine, it->first.c_str(), gmVariable(1));
//		}
//	}
//	//////////////////////////////////////////////////////////////////////////
//	{
//		gmTableObject *pPropertyTable = _machine->AllocTableObject();
//		_table->Set(_machine, "property", gmVariable(pPropertyTable));
//		const PropertyMap &pm = _waypoint->GetPropertyMap();
//		for(PropertyMap::ValueMap::const_iterator it = pm.GetProperties().begin();
//			it != pm.GetProperties().end();
//			++it)
//		{
//			pPropertyTable->Set(_machine, it->first.c_str(), it->second.c_str());
//		}
//	}
//	//////////////////////////////////////////////////////////////////////////
//}
//
//// function: GetWaypointByName
////		Gets a waypoint information by its name.
////
//// Parameters:
////
////		<std::string> - The name of the waypoint to get the info of.
////		<table> - An empty table, the function will fill in position, facing, guid.
////
//// Returns:
////		int - true if successful, false if not
//static int GM_CDECL gmfGetWaypointByName(gmThread *a_thread)
//{
//	GM_CHECK_NUM_PARAMS(2);
//	GM_CHECK_STRING_PARAM(name, 0);
//	GM_CHECK_TABLE_PARAM(params, 1);
//
//	gmMachine *pMachine = a_thread->GetMachine();
//	DisableGCInScope gcEn(pMachine);
//
//	bool bSuccess = false;
//	PathPlannerNavMesh *pWp = GetWpPlanner();
//	if(pWp)
//	{
//		Waypoint *pWaypoint = pWp->GetWaypointByName(name);
//		if(pWaypoint)
//		{
//			SetWaypointDataInTable(pMachine, params, pWaypoint);
//			bSuccess = true;
//		}
//	}
//
//	a_thread->PushInt(bSuccess ? 1 : 0);
//	return GM_OK;
//}
//
////////////////////////////////////////////////////////////////////////////
//
//// function: GetWaypointByGUID
////		Gets a waypoint information by its guid.
////
//// Parameters:
////
////		<int> - Guid of a waypoint.
////		<table> - An empty table, the function will fill in position, facing, guid.
////
//// Returns:
////		int - true if successful, false if not
//static int GM_CDECL gmfGetWaypointByUID(gmThread *a_thread)
//{
//	GM_CHECK_NUM_PARAMS(2);
//	GM_CHECK_INT_PARAM(guid, 0);
//	GM_CHECK_TABLE_PARAM(params, 1);
//
//	gmMachine *pMachine = a_thread->GetMachine();
//	DisableGCInScope gcEn(pMachine);
//
//	bool bSuccess = false;
//	PathPlannerNavMesh *pWp = GetWpPlanner();
//	if(pWp)
//	{
//		Waypoint *pWaypoint = pWp->GetWaypointByGUID(guid);
//		if(pWaypoint)
//		{
//			SetWaypointDataInTable(pMachine, params, pWaypoint);
//			bSuccess = true;
//		}
//	}
//	a_thread->PushInt(bSuccess ? 1 : 0);
//	return GM_OK;
//}
//
////////////////////////////////////////////////////////////////////////////
//
//// function: ConnectWaypoints
////		Gets a waypoint information by its guid.
////
//// Parameters:
////
////		<int> - Guid of a waypoint.
////		<int> - Guid of another waypoint.
////		<table> - An empty table, the function will fill in position, facing, guid.
////
//// Returns:
////		int - true if successful, false if not
//static int GM_CDECL gmfConnectWaypoints(gmThread *a_thread)
//{
//	GM_CHECK_NUM_PARAMS(2);
//	GM_CHECK_INT_PARAM(guid1, 0);
//	GM_CHECK_INT_PARAM(guid2, 1);
//
//	bool bSuccess = false;
//	PathPlannerNavMesh *pWp = GetWpPlanner();
//	if(pWp)
//	{
//		Waypoint *pWaypoint1 = pWp->GetWaypointByGUID(guid1);
//		Waypoint *pWaypoint2 = pWp->GetWaypointByGUID(guid2);
//		if(pWaypoint1 != NULL && pWaypoint2 != NULL)
//		{
//			pWaypoint1->ConnectTo(pWaypoint2);
//			bSuccess = true;
//		}
//	}
//
//	a_thread->PushInt(bSuccess ? 1 : 0);
//	return GM_OK;
//}
//
////////////////////////////////////////////////////////////////////////////
//
//// function: SetRadius
////		Sets a wapoint radius by guid.
////
//// Parameters:
////
////		<int> - Guid of a waypoint.
////		<int> - Radius.
////
//// Returns:
////		int - true if successful, false if not
//static int GM_CDECL gmfSetRadius(gmThread *a_thread)
//{
//	GM_CHECK_NUM_PARAMS(2);
//	GM_CHECK_INT_PARAM(guid1, 0);
//	GM_CHECK_FLOAT_PARAM(radius, 1);
//
//	bool bSuccess = false;
//	PathPlannerNavMesh *pWp = GetWpPlanner();
//	if(pWp)
//	{
//		Waypoint *pWaypoint1 = pWp->GetWaypointByGUID(guid1);
//		if(pWaypoint1)
//		{
//			pWaypoint1->SetRadius(radius);
//			bSuccess = true;
//		}
//	}
//
//	a_thread->PushInt(bSuccess ? 1 : 0);
//	return GM_OK;
//}
//
////////////////////////////////////////////////////////////////////////////
//
//// function: SetWaypointFlag
////		Sets the flag on a waypoint by name.
////
//// Parameters:
////
////		<int> - Guid of a waypoint.
////		- OR -
////		<std::string> - Name of the waypoint.
////		<std::string> - The name of the flag to set.
////		<int> - True to set, false to clear.
////
//// Returns:
////		None
//static int GM_CDECL gmfSetWaypointFlag(gmThread *a_thread)
//{
//	GM_CHECK_NUM_PARAMS(3);
//
//	Waypoint *pWaypoint = 0;
//	PathPlannerNavMesh *pWp = GetWpPlanner();
//	if(pWp)
//	{
//		if(a_thread->ParamType(0) == GM_INT)
//		{
//			GM_CHECK_INT_PARAM(guid, 0);
//			pWaypoint = pWp->GetWaypointByGUID(guid);
//		}
//		else if(a_thread->ParamType(0) == GM_STRING)
//		{
//			GM_CHECK_STRING_PARAM(name, 0);
//			pWaypoint = pWp->GetWaypointByName(name);
//		}
//	}
//	else
//	{
//		GM_EXCEPTION_MSG("Wrong Path Planner");
//		return GM_EXCEPTION;
//	}
//
//	if(!pWaypoint)
//	{
//		GM_EXCEPTION_MSG("Invalid Waypoint specified in param 0");
//		return GM_EXCEPTION;
//	}
//
//	GM_CHECK_STRING_PARAM(flagname, 1);
//	GM_CHECK_INT_PARAM(enable, 2);
//
//	NavFlags flag;
//	if(!pWp->GetNavFlagByName(flagname, flag))
//	{
//		GM_EXCEPTION_MSG("Invalid Navigation Flag specified in param 1");
//		return GM_EXCEPTION;
//	}
//
//	if(enable)
//		pWaypoint->AddFlag(flag);
//	else
//		pWaypoint->RemoveFlag(flag);
//
//	if(!pWaypoint->IsAnyFlagOn(F_NAV_TEAM_ALL))
//	{
//		if(pWaypoint->IsFlagOn(F_NAV_TEAMONLY))
//		{
//			pWaypoint->RemoveFlag(F_NAV_TEAMONLY);
//		}
//	}
//	else
//	{
//		// At least one of them is on, so make sure the teamonly flag is set.
//		pWaypoint->AddFlag(F_NAV_TEAMONLY);
//	}
//
//	if(flag & pWp->GetBlockableMask())
//		pWp->BuildBlockableList();
//
//	pWp->BuildFlagMap();
//	return GM_OK;
//}
//
////////////////////////////////////////////////////////////////////////////
//
//// function: SetWaypointProperty
////		Sets the flag on a waypoint by name.
////
//// Parameters:
////
////		<int> - Guid of a waypoint.
////		- OR -
////		<std::string> - Name of the waypoint.
////		<std::string> - The name of the flag to set.
////		<std::string> - Property value to set.
////
//// Returns:
////		None
//static int GM_CDECL gmfSetWaypointProperty(gmThread *a_thread)
//{
//	GM_CHECK_NUM_PARAMS(3);
//
//	Waypoint *pWaypoint = 0;
//	PathPlannerNavMesh *pWp = GetWpPlanner();
//	if(pWp)
//	{
//		if(a_thread->ParamType(0) == GM_INT)
//		{
//			GM_CHECK_INT_PARAM(guid, 0);
//			pWaypoint = pWp->GetWaypointByGUID(guid);
//		}
//		else if(a_thread->ParamType(0) == GM_STRING)
//		{
//			GM_CHECK_STRING_PARAM(name, 0);
//			pWaypoint = pWp->GetWaypointByName(name);
//		}
//	}
//	else
//	{
//		GM_EXCEPTION_MSG("Wrong Path Planner");
//		return GM_EXCEPTION;
//	}
//
//	if(!pWaypoint)
//	{
//		GM_EXCEPTION_MSG("Invalid Waypoint specified in param 0");
//		return GM_EXCEPTION;
//	}
//
//	GM_CHECK_STRING_PARAM(propname, 1);
//	GM_CHECK_STRING_PARAM(propvalue, 2);
//
//	if(*propvalue != 0 && propvalue)
//		pWaypoint->GetPropertyMap().AddProperty(propname,propvalue);
//	else
//		pWaypoint->GetPropertyMap().DelProperty(propname);
//
//	return GM_OK;
//}
//
////////////////////////////////////////////////////////////////////////////
//
//extern float g_fTopWaypointOffset;
//extern float g_fBottomWaypointOffset;
//extern float g_fWaypointPathOffset;
//extern float g_fTopPathOffset;
//extern float g_fBottomPathOffset;
//extern float g_fBlockablePathOffset;
//extern float g_fFacingOffset;
//extern float g_fWaypointTextOffset;
//extern float g_fWaypointTextDuration;
//extern float g_fPathLevelOffset;
//
////////////////////////////////////////////////////////////////////////////
//
//// function: WaypointColor
////		Customize the color of various types of waypoints, paths, etc...
////		See waypoint_color command for help.
////
//// Parameters:
////
////		<std::string> - The name of the category to set a color for.
////		<int> - The color to use for this type.
////
//// Returns:
////		None
//static int GM_CDECL gmfWaypointColor(gmThread *a_thread)
//{
//	GM_CHECK_NUM_PARAMS(2);
//	GM_CHECK_STRING_PARAM(wptype, 0);
//	GM_CHECK_INT_PARAM(c, 1);
//
//	obColor color(c);
//
//	StringVector v;
//	v.push_back("waypoint_color");
//	v.push_back(wptype);
//	v.push_back((std::string)va("%d", color.r()));
//	v.push_back((std::string)va("%d", color.g()));
//	v.push_back((std::string)va("%d", color.b()));
//	v.push_back((std::string)va("%d", color.a()));
//	CommandReciever::DispatchCommand(v);
//	return GM_OK;
//}
//
//static int GM_CDECL gmfGetAllWaypoints(gmThread *a_thread)
//{
//	GM_CHECK_NUM_PARAMS(1);
//	GM_CHECK_TABLE_PARAM(tbl, 0);
//
//	PathPlannerNavMesh *pWp = GetWpPlanner();
//	if(pWp)
//	{
//		const PathPlannerNavMesh::WaypointList &wl = pWp->GetWaypointList();
//		PathPlannerNavMesh::WaypointList::const_iterator wIt = wl.begin(), wItEnd = wl.end();
//
//		obint32 iNum = 0;
//		for(; wIt != wItEnd; ++wIt)
//		{
//			const Waypoint *pWaypoint = (*wIt);
//			gmTableObject *pWpTable = a_thread->GetMachine()->AllocTableObject();
//			tbl->Set(a_thread->GetMachine(), iNum++, gmVariable(pWpTable));
//			SetWaypointDataInTable(a_thread->GetMachine(),pWpTable,pWaypoint);
//		}
//	}
//	return GM_OK;
//}
//
//static int GM_CDECL gmfGetAllSelectedWaypoints(gmThread *a_thread)
//{
//	GM_CHECK_NUM_PARAMS(1);
//	GM_CHECK_TABLE_PARAM(tbl, 0);
//
//	PathPlannerNavMesh *pWp = GetWpPlanner();
//	if(pWp)
//	{
//		const PathPlannerNavMesh::WaypointList &wl = pWp->GetSelectedWaypointList();
//		PathPlannerNavMesh::WaypointList::const_iterator wIt = wl.begin(), wItEnd = wl.end();
//
//		obint32 iNum = 0;
//		for(; wIt != wItEnd; ++wIt)
//		{
//			const Waypoint *pWaypoint = (*wIt);
//			gmTableObject *pWpTable = a_thread->GetMachine()->AllocTableObject();
//			tbl->Set(a_thread->GetMachine(), iNum++, gmVariable(pWpTable));
//			SetWaypointDataInTable(a_thread->GetMachine(),pWpTable,pWaypoint);
//		}
//	}
//	return GM_OK;
//}

static int GM_CDECL gmfNavSave(gmThread *a_thread)
{
	GM_CHECK_NUM_PARAMS(0);
	GM_STRING_PARAM(wpname,0,0);

	PathPlannerNavMesh *obj = NULL;
	if(gmBind2::GetThisGMType<PathPlannerNavMesh>(a_thread, obj)==GM_EXCEPTION)
		return GM_EXCEPTION;

	std::string strFile = wpname ? wpname : g_EngineFuncs->GetMapName();
	a_thread->PushInt(obj->Save(strFile)?1:0);
	return GM_OK;
}

static int GM_CDECL gmfNavLoad(gmThread *a_thread)
{
	GM_CHECK_NUM_PARAMS(0);
	GM_STRING_PARAM(wpname,0,0);

	PathPlannerNavMesh *obj = NULL;
	if(gmBind2::GetThisGMType<PathPlannerNavMesh>(a_thread, obj)==GM_EXCEPTION)
		return GM_EXCEPTION;

	std::string strFile = wpname ? wpname : g_EngineFuncs->GetMapName();
	a_thread->PushInt(obj->Load(strFile)?1:0);
	return GM_OK;
}

//////////////////////////////////////////////////////////////////////////

void PathPlannerNavMesh::RegisterScriptFunctions(gmMachine *a_machine)
{
	gmBind2::Class<PathPlannerNavMesh>("_NavMesh", a_machine)
		/*.func(gmfAddWaypoint, "AddWaypoint")
		.func(gmfDeleteWaypoint, "DeleteWaypoint")
		.func(gmfGetWaypointByName, "GetWaypointByName")
		.func(gmfGetWaypointByUID, "GetWaypointByGUID")
		.func(gmfConnectWaypoints, "Connect")
		.func(gmfSetRadius, "SetRadius")
		.func(gmfSetWaypointFlag, "SetWaypointFlag")
		.func(gmfSetWaypointProperty, "SetWaypointProperty")
		.func(gmfWaypointColor, "WaypointColor")
		.func(gmfGetAllWaypoints, "GetAllWaypoints")
		.func(gmfGetAllSelectedWaypoints, "GetAllSelectedWaypoints")*/
		.func(gmfNavSave,"Save")
		.func(gmfNavLoad,"Load")
		/*.func((bool (PathPlannerNavMesh::*)())&PathPlannerNavMesh::IsViewOn,"IsWaypointViewOn")
		.func((bool (PathPlannerNavMesh::*)(int, const std::string &))&PathPlannerNavMesh::SetWaypointName,"SetWaypointName")
		.var(&g_fTopWaypointOffset,"TopWaypointOffset")
		.var(&g_fBottomWaypointOffset,"BottomWaypointOffset")
		.var(&g_fTopPathOffset,"TopPathOffset")
		.var(&g_fBottomPathOffset,"BottomPathOffset")
		.var(&g_fBlockablePathOffset,"BlockablePathOffset")
		.var(&g_fFacingOffset,"FacingOffset")
		.var(&g_fWaypointTextOffset,"TextOffset")
		.var(&g_fWaypointTextDuration,"TextDuration")
		.var(&g_fPathLevelOffset,"PathLevelOffset")*/
		;

	mNavRef = gmBind2::Class<PathPlannerNavMesh>::WrapObject(a_machine,this);
	a_machine->GetGlobals()->Set(a_machine,"Nav",gmVariable(mNavRef));
}