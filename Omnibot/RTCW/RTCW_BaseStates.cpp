////////////////////////////////////////////////////////////////////////////////
// 
// $LastChangedBy$
// $LastChangedDate$
// $LastChangedRevision$
//
////////////////////////////////////////////////////////////////////////////////

#include "PrecompRTCW.h"
#include "RTCW_BaseStates.h"
#include "RTCW_FilterClosest.h"
#include "WeaponDatabase.h"

namespace AiState
{
	//////////////////////////////////////////////////////////////////////////

	//RepairMg42::RepairMg42()
	//	: StateChild("RepairMg42")
	//	, FollowPathUser("RepairMg42")
	//	, m_IgnoreTargets(false)
	//{
	//	LimitToWeapon().SetFlag(RTCW_WP_PLIERS);
	//}

	//void RepairMg42::GetDebugString(StringStr &out)
	//{
	//	if(IsActive())
	//	{
	//		//if(!GetClient()->HasEntityFlag(ET_ENT_FLAG_MOUNTED))
	//		out << "Repairing " + (m_MapGoal?m_MapGoal->GetName():"");
	//	}
	//}

	//void RepairMg42::RenderDebug()
	//{
	//	if(IsActive())
	//	{
	//		Utils::OutlineOBB(m_MapGoal->GetWorldBounds(), COLOR::ORANGE, 5.f);
	//		Utils::DrawLine(GetClient()->GetEyePosition(),m_MapGoal->GetPosition(),COLOR::GREEN,5.f);
	//	}
	//}

	//// FollowPathUser functions.
	//bool RepairMg42::GetNextDestination(DestinationVector &_desination, bool &_final, bool &_skiplastpt)
	//{
	//	if(m_MapGoal && m_MapGoal->RouteTo(GetClient(), _desination, 64.f))
	//		_final = false;
	//	else 
	//		_final = true;
	//	return true;
	//}

	//// AimerUser functions.
	//bool RepairMg42::GetAimPosition(Vector3f &_aimpos)
	//{
	//	_aimpos = m_MG42Position;
	//	return true;
	//}

	//void RepairMg42::OnTarget()
	//{
	//	FINDSTATE(ws,WeaponSystem,GetRootState());
	//	if(ws && ws->GetCurrentRequestOwner() == GetNameHash())
	//		ws->FireWeapon();
	//}

	//obReal RepairMg42::GetPriority()
	//{
	//	if(IsActive())
	//		return GetLastPriority();

	//	BitFlag64 entFlags;

	//	GoalManager::Query qry(0x17929136 /* REPAIRMG42 */, GetClient());
	//	GoalManager::GetInstance()->GetGoals(qry);
	//	for(obuint32 i = 0; i < qry.m_List.size(); ++i)
	//	{
	//		if(BlackboardIsDelayed(qry.m_List[i]->GetSerialNum()))
	//			continue;

	//		bool bBroken = InterfaceFuncs::IsMountableGunRepairable(GetClient(), qry.m_List[i]->GetEntity());
	//		if(!bBroken)
	//			continue;
	//		else
	//		{
	//			float fDistToRepair = SquaredLength2d(qry.m_List[i]->GetPosition(), GetClient()->GetPosition());
	//			if ( fDistToRepair > 4000000 ) { // 2000 squared
	//				BlackboardDelay(10.f, qry.m_List[i]->GetSerialNum());
	//				continue;
	//			}
	//		}

	//		m_MapGoal = qry.m_List[i];
	//		break;
	//	}
	//	return m_MapGoal ? m_MapGoal->GetPriorityForClient(GetClient()) : 0.f;
	//}

	//void RepairMg42::Enter()
	//{
	//	m_MapGoal->GetProperty("IgnoreTargets",m_IgnoreTargets);

	//	Tracker.InProgress = m_MapGoal;
	//	m_MG42Position = m_MapGoal->GetWorldBounds().Center;
	//	//m_MapGoal->GetWorldBounds().CenterBottom(m_MG42Position);
	//	FINDSTATEIF(FollowPath, GetRootState(), Goto(this, Run, true));
	//}

	//void RepairMg42::Exit()
	//{
	//	FINDSTATEIF(FollowPath, GetRootState(), Stop(true));

	//	m_MapGoal.reset();
	//	Tracker.Reset();
	//	FINDSTATEIF(Aimer,GetRootState(),ReleaseAimRequest(GetNameHash()));
	//	FINDSTATEIF(WeaponSystem,GetRootState(),ReleaseWeaponRequest(GetNameHash()));
	//}

	//State::StateStatus RepairMg42::Update(float fDt)
	//{
	//	if(DidPathFail())
	//	{
	//		BlackboardDelay(10.f, m_MapGoal->GetSerialNum());
	//		return State_Finished;
	//	}

	//	bool bBroken = InterfaceFuncs::IsMountableGunRepairable(GetClient(), m_MapGoal->GetEntity());
	//	if(!bBroken || !m_MapGoal->IsAvailable(GetClient()->GetTeam()))
	//		return State_Finished;

	//	if(DidPathSucceed())
	//	{
	//		GetClient()->PressButton(BOT_BUTTON_CROUCH);
	//		GetClient()->GetSteeringSystem()->SetTarget(m_MapGoal->GetPosition());

	//		Priority::ePriority pri = m_IgnoreTargets ? Priority::High : Priority::Medium;
	//		FINDSTATEIF(Aimer,GetRootState(),AddAimRequest(pri,this,GetNameHash()));
	//		FINDSTATEIF(WeaponSystem, GetRootState(), AddWeaponRequest(Priority::Medium, GetNameHash(), RTCW_WP_PLIERS));
	//	}
	//	return State_Busy;
	//}

	//////////////////////////////////////////////////////////////////////////

	//TakeCheckPoint::TakeCheckPoint()
	//	: StateChild("TakeCheckPoint")
	//	, FollowPathUser("TakeCheckPoint")
	//{
	//}

	//void TakeCheckPoint::GetDebugString(StringStr &out)
	//{
	//	out << (m_MapGoal ? m_MapGoal->GetName() : "");
	//}

	//void TakeCheckPoint::RenderDebug()
	//{
	//	if(IsActive())
	//	{
	//		Utils::OutlineOBB(m_MapGoal->GetWorldBounds(), COLOR::ORANGE, 5.f);
	//		Utils::DrawLine(GetClient()->GetEyePosition(),m_MapGoal->GetPosition(),COLOR::GREEN,5.f);
	//	}
	//}

	// FollowPathUser functions.
	//bool TakeCheckPoint::GetNextDestination(DestinationVector &_desination, bool &_final, bool &_skiplastpt)
	//{
	//	if(m_MapGoal && m_MapGoal->RouteTo(GetClient(), _desination, 64.f))
	//		_final = false;
	//	else 
	//		_final = true;
	//	return true;
	//}

	//obReal TakeCheckPoint::GetPriority()
	//{
	//	if(IsActive())
	//		return GetLastPriority();

	//	m_MapGoal.reset();

	//	GoalManager::Query qry(0xf7e4a57f /* CHECKPOINT */, GetClient());
	//	GoalManager::GetInstance()->GetGoals(qry);
	//	qry.GetBest(m_MapGoal);

	//	return m_MapGoal ? m_MapGoal->GetPriorityForClient(GetClient()) : 0.f;
	//}

	//void TakeCheckPoint::Enter()
	//{
	//	m_TargetPosition = m_MapGoal->GetWorldBounds().Center;
	//	Tracker.InProgress = m_MapGoal;
	//	FINDSTATEIF(FollowPath, GetRootState(), Goto(this, Run, true));
	//}

	//void TakeCheckPoint::Exit()
	//{
	//	FINDSTATEIF(FollowPath, GetRootState(), Stop(true));

	//	m_MapGoal.reset();
	//	FINDSTATEIF(Aimer,GetRootState(),ReleaseAimRequest(GetNameHash()));
	//	FINDSTATEIF(WeaponSystem, GetRootState(), ReleaseWeaponRequest(GetNameHash()));

	//	Tracker.Reset();
	//}

	//State::StateStatus TakeCheckPoint::Update(float fDt)
	//{
	//	if(DidPathFail())
	//	{
	//		BlackboardDelay(10.f, m_MapGoal->GetSerialNum());
	//		return State_Finished;
	//	}

	//	if(!m_MapGoal->IsAvailable(GetClient()->GetTeam()))
	//		return State_Finished;

	//	if(DidPathSucceed())
	//	{
	//		m_TargetPosition.z = GetClient()->GetPosition().z;
	//		GetClient()->GetSteeringSystem()->SetTarget(m_TargetPosition, 32.f);			
	//	}
	//	return State_Busy;
	//}

	//////////////////////////////////////////////////////////////////////////

	//ReviveTeammate::ReviveTeammate()
	//	: StateChild("ReviveTeammate")
	//	, FollowPathUser("ReviveTeammate")
	//{
	//	LimitToWeapon().SetFlag(RTCW_WP_SYRINGE);
	//}

	//void ReviveTeammate::GetDebugString(StringStr &out)
	//{
	//	switch(m_GoalState)
	//	{
	//	case REVIVING:
	//		out << "Reviving ";
	//		break;
	//	case HEALING:
	//		out << "Healing ";
	//		break;
	//	}

	//	if(m_MapGoal && m_MapGoal->GetEntity().IsValid())
	//		out << std::endl << EngineFuncs::EntityName(m_MapGoal->GetEntity(), "<noname>");
	//}

	//void ReviveTeammate::RenderDebug()
	//{
	//	if(IsActive())
	//	{
	//		Utils::OutlineOBB(m_MapGoal->GetWorldBounds(), COLOR::ORANGE, 5.f);
	//		Utils::DrawLine(GetClient()->GetPosition(),m_MapGoal->GetPosition(),COLOR::GREEN,5.f);
	//		Utils::DrawLine(GetClient()->GetEyePosition(),m_MapGoal->GetPosition(),COLOR::MAGENTA,5.f);
	//	}
	//}

	//// FollowPathUser functions.
	//bool ReviveTeammate::GetNextDestination(DestinationVector &_desination, bool &_final, bool &_skiplastpt)
	//{
	//	if(m_MapGoal && m_MapGoal->RouteTo(GetClient(), _desination, 64.f))
	//		_final = false;
	//	else 
	//		_final = true;
	//	return true;
	//}

	//// AimerUser functions.
	//bool ReviveTeammate::GetAimPosition(Vector3f &_aimpos)
	//{
	//	const Box3f obb = m_MapGoal->GetWorldBounds();
	//	if ( m_GoalState == REVIVING )
	//		_aimpos = obb.GetCenterBottom();
	//	else
	//		_aimpos = obb.Center;
	//	return true;
	//}

	//void ReviveTeammate::OnTarget()
	//{
	//	FINDSTATE(ws, WeaponSystem, GetRootState());
	//	if(ws)
	//	{
	//		if(InterfaceFuncs::IsAlive(m_MapGoal->GetEntity()))
	//		{
	//			if(ws->CurrentWeaponIs(RTCW_WP_MEDKIT))
	//				ws->FireWeapon();
	//		}
	//		else
	//		{
	//			if(ws->CurrentWeaponIs(RTCW_WP_SYRINGE))
	//				ws->FireWeapon();
	//		}
	//	}
	//}

	//obReal ReviveTeammate::GetPriority()
	//{
	//	if(IsActive())
	//		return GetLastPriority();

	//	m_MapGoal.reset();

	//	GoalManager::Query qry(0x2086cdf0 /* REVIVE */, GetClient());
	//	GoalManager::GetInstance()->GetGoals(qry);
	//	//qry.GetBest(m_MapGoal);

	//	//qry.FilterList();

	//	float fDistToRevive;
	//	float fClosestRevive = 0.f;
	//	MapGoalList::iterator mIt = qry.m_List.begin();
	//	for(; mIt != qry.m_List.end(); )
	//	{
	//		fDistToRevive = SquaredLength2d((*mIt)->GetPosition(), GetClient()->GetPosition());
	//		if ( fDistToRevive > 4000000 ) // 2000 squared
	//		{
	//			BlackboardDelay(5.f, (*mIt)->GetSerialNum()); // ignore it for a while so dist calcs aren't done every frame
	//			mIt = qry.m_List.erase(mIt);
	//			continue;
	//		}

	//		// use the closest one or the first one found within 200 units
	//		if ( fClosestRevive == 0.f || (fDistToRevive < fClosestRevive && fDistToRevive > 40000.f) )
	//		{
	//			fClosestRevive = fDistToRevive;
	//			m_MapGoal = (*mIt);
	//		}

	//		++mIt;
	//	}

	//	m_List = qry.m_List;
	//	m_MapGoal.reset();
	//	// TODO: check weapon capability vs target underwater?

	//	return !m_List.empty() ? m_List.front()->GetPriorityForClient(GetClient()) : 0.f;
	//}

	//void ReviveTeammate::Enter()
	//{
	//	m_GoalState = REVIVING;

	//	m_CheckReviveTimer.Delay(2.f);

	//	FINDSTATE(fp, FollowPath, GetRootState());
	//	if(fp->Goto(this, m_List, Run))
	//	{
	//		m_MapGoal = m_List[GetDestinationIndex()];
	//		Tracker.InProgress = m_MapGoal;
	//	}
	//	else
	//	{
	//		m_MapGoal.reset();
	//		Tracker.Reset();
	//		for(obuint32 i = 0; i < m_List.size(); ++i)
	//		{
	//			BlackboardDelay(10.f, m_List[i]->GetSerialNum());
	//		}
	//	}
	//}

	//void ReviveTeammate::Exit()
	//{
	//	FINDSTATEIF(FollowPath, GetRootState(), Stop(true));

	//	m_MapGoal.reset();

	//	FINDSTATEIF(Aimer,GetRootState(),ReleaseAimRequest(GetNameHash()));
	//	FINDSTATEIF(WeaponSystem, GetRootState(),ReleaseWeaponRequest(GetNameHash()));

	//	Tracker.Reset();
	//}

	//State::StateStatus ReviveTeammate::Update(float fDt)
	//{
	//	if(!m_MapGoal)
	//		return State_Finished;

	//	if(m_CheckReviveTimer.IsExpired())
	//	{
	//		// finish if there's new goals, so we can activate again and go to a new goal
	//		if(AreThereNewReviveGoals())
	//			return State_Finished;
	//		m_CheckReviveTimer.Delay(2.f);
	//	}

	//	if(DidPathFail())
	//	{
	//		BlackboardDelay(10.f, m_MapGoal->GetSerialNum());
	//		return State_Finished;
	//	}

	//	if(!m_MapGoal->IsAvailable(GetClient()->GetTeam()))
	//		return State_Finished;

	//	GameEntity reviveEnt = m_MapGoal->GetEntity();

	//	//////////////////////////////////////////////////////////////////////////
	//	Msg_HealthArmor ha;
	//	if(InterfaceFuncs::GetHealthAndArmor(reviveEnt, ha) && ha.m_CurrentHealth >= ha.m_MaxHealth)
	//		return State_Finished;

	//	BitFlag64 ef;
	//	if(InterfaceFuncs::GetEntityFlags(reviveEnt,ef))
	//	{
	//		if(ef.CheckFlag(RTCW_ENT_FLAG_INLIMBO))
	//			return State_Finished;
	//	}
	//	//////////////////////////////////////////////////////////////////////////

	//	if(DidPathSucceed())
	//	{
	//		if(GetClient()->GetStuckTime() > 2000)
	//			return State_Finished;

	//		FINDSTATEIF(Aimer,GetRootState(),AddAimRequest(Priority::Medium,this,GetNameHash()));
	//		GetClient()->GetSteeringSystem()->SetTarget(m_MapGoal->GetPosition());

	//		switch(m_GoalState)
	//		{
	//		case REVIVING:
	//			{
	//				if(InterfaceFuncs::IsAlive(reviveEnt))
	//				{
	//					m_GoalState = HEALING;
	//				}
	//				else
	//				{
	//					GetClient()->GetSteeringSystem()->SetNoAvoidTime(IGame::GetTime() + 1000);
	//					FINDSTATEIF(WeaponSystem, GetRootState(), AddWeaponRequest(Priority::High, GetNameHash(), RTCW_WP_SYRINGE));
	//				}

	//				// if the height differences are significant, jump/crouch
	//				const Vector3f eyePos = GetClient()->GetEyePosition();
	//				Vector3f aimPos;
	//				GetAimPosition( aimPos );
	//				const float heightDiff = aimPos.z - eyePos.z;
	//				if ( heightDiff > 20.f ) {
	//					if(GetClient()->GetEntityFlags().CheckFlag(ENT_FLAG_ONGROUND)) {
	//						GetClient()->PressButton(BOT_BUTTON_JUMP);
	//					}
	//				} else /*if ( heightDiff > 20.f )*/ {
	//					BitFlag64 btns;
	//					btns.SetFlag(BOT_BUTTON_CROUCH);
	//					GetClient()->HoldButton(btns,IGame::GetDeltaTime()*2);
	//				}

	//				break;
	//			}				
	//		case HEALING:
	//			{
	//				// cs: bb delay any time the goal needs interrupted here
	//				// otherwise it's a loop of goto and then finish which causes them
	//				// to just 'stick' to the target without doing anything useful 

	//				if(GetClient()->GetTargetingSystem()->HasTarget())
	//				{
	//					BlackboardDelay(5.f, m_MapGoal->GetSerialNum());
	//					return State_Finished;
	//				}

	//				if(!InterfaceFuncs::IsWeaponCharged(GetClient(), RTCW_WP_MEDKIT, Primary))
	//				{
	//					BlackboardDelay(5.f, m_MapGoal->GetSerialNum());
	//					return State_Finished;
	//				}

	//				GetClient()->GetSteeringSystem()->SetNoAvoidTime(IGame::GetTime() + 1000);
	//				FINDSTATEIF(WeaponSystem, GetRootState(), AddWeaponRequest(Priority::High, GetNameHash(), RTCW_WP_MEDKIT));
	//				break;
	//			}
	//		}
	//	}
	//	return State_Busy;
	//}

	//bool ReviveTeammate::AreThereNewReviveGoals()
	//{
	//	GoalManager::Query qry(0x2086cdf0 /* REVIVE */, GetClient());
	//	GoalManager::GetInstance()->GetGoals(qry);

	//	for(obuint32 i = 0; i < qry.m_List.size(); ++i)
	//	{
	//		if(std::find(m_List.begin(),m_List.end(),qry.m_List[i]) == m_List.end())
	//			return true;
	//	}

	//	return false;
	//}

	//////////////////////////////////////////////////////////////////////////

	//DefuseDynamite::DefuseDynamite()
	//	: StateChild("DefuseDynamite")
	//	, FollowPathUser("DefuseDynamite")
	//{
	//	LimitToWeapon().SetFlag(RTCW_WP_PLIERS);
	//}

	//void DefuseDynamite::GetDebugString(StringStr &out)
	//{
	//	out << (m_MapGoal ? m_MapGoal->GetName() : "");
	//}

	//void DefuseDynamite::RenderDebug()
	//{
	//	if(IsActive())
	//	{
	//		Utils::OutlineOBB(m_MapGoal->GetWorldBounds(), COLOR::ORANGE, 5.f);
	//		Utils::DrawLine(GetClient()->GetEyePosition(),m_MapGoal->GetPosition(),COLOR::GREEN,5.f);
	//	}
	//}

	//// FollowPathUser functions.
	//bool DefuseDynamite::GetNextDestination(DestinationVector &_desination, bool &_final, bool &_skiplastpt)
	//{
	//	if(m_MapGoal && m_MapGoal->RouteTo(GetClient(), _desination, 64.f))
	//		_final = false;
	//	else 
	//		_final = true;
	//	return true;
	//}

	//// AimerUser functions.
	//bool DefuseDynamite::GetAimPosition(Vector3f &_aimpos)
	//{
	//	_aimpos = m_MapGoal->GetWorldBounds().Center;
	//	return true;
	//}

	//void DefuseDynamite::OnTarget()
	//{
	//	FINDSTATE(ws, WeaponSystem, GetRootState());
	//	if(ws && ws->CurrentWeaponIs(RTCW_WP_PLIERS))
	//		ws->FireWeapon();
	//}

	//obReal DefuseDynamite::GetPriority()
	//{
	//	if(IsActive())
	//		return GetLastPriority();

	//	m_MapGoal.reset();

	//	GoalManager::Query qry(0x1899efc7 /* DEFUSE */, GetClient());
	//	GoalManager::GetInstance()->GetGoals(qry);
	//	for(obuint32 i = 0; i < qry.m_List.size(); ++i)
	//	{
	//		if(BlackboardIsDelayed(qry.m_List[i]->GetSerialNum()))
	//			continue;

	//		if(qry.m_List[i]->GetSlotsOpen(MapGoal::TRACK_INPROGRESS) < 1)
	//			continue;

	//		if(InterfaceFuncs::GetExplosiveState(GetClient(), qry.m_List[i]->GetEntity()) == XPLO_ARMED)
	//		{
	//			m_MapGoal = qry.m_List[i];
	//			break;
	//		}
	//		else
	//		{
	//			qry.m_List[i]->SetDeleteMe(true);
	//		}
	//	}
	//	return m_MapGoal ? m_MapGoal->GetPriorityForClient(GetClient()) : 0.f;
	//}

	//void DefuseDynamite::Enter()
	//{
	//	m_TargetPosition = m_MapGoal->GetWorldBounds().Center;
	//	Tracker.InProgress = m_MapGoal;
	//	FINDSTATEIF(FollowPath, GetRootState(), Goto(this, Run));
	//}

	//void DefuseDynamite::Exit()
	//{
	//	FINDSTATEIF(FollowPath, GetRootState(), Stop(true));

	//	m_MapGoal.reset();

	//	FINDSTATEIF(Aimer,GetRootState(),ReleaseAimRequest(GetNameHash()));
	//	FINDSTATEIF(WeaponSystem, GetRootState(),ReleaseWeaponRequest(GetNameHash()));

	//	Tracker.Reset();
	//}

	//State::StateStatus DefuseDynamite::Update(float fDt)
	//{
	//	if(DidPathFail())
	//	{
	//		BlackboardDelay(10.f, m_MapGoal->GetSerialNum());
	//		return State_Finished;
	//	}

	//	if(!m_MapGoal->IsAvailable(GetClient()->GetTeam()))
	//		return State_Finished;

	//	if(DidPathSucceed())
	//	{
	//		ExplosiveState eState = InterfaceFuncs::GetExplosiveState(GetClient(), m_MapGoal->GetEntity());
	//		switch(eState)
	//		{
	//		case XPLO_INVALID:
	//		case XPLO_UNARMED:
	//			return State_Finished;
	//		default:
	//			break; // keep processing
	//		}

	//		m_TargetPosition = m_MapGoal->GetWorldBounds().Center;

	//		const float fDistanceToDynamite = SquaredLength2d(m_TargetPosition, GetClient()->GetPosition());
	//		if(fDistanceToDynamite > 2500.0f)
	//		{
	//			GetClient()->GetSteeringSystem()->SetTarget(m_TargetPosition);
	//		}
	//		else
	//		{
	//			GetClient()->PressButton(BOT_BUTTON_CROUCH);

	//			FINDSTATEIF(Aimer,GetRootState(),AddAimRequest(Priority::Medium,this,GetNameHash()));
	//			FINDSTATEIF(WeaponSystem, GetRootState(), AddWeaponRequest(Priority::Medium, GetNameHash(), RTCW_WP_PLIERS));
	//		}
	//	}
	//	return State_Busy;
	//}

	//////////////////////////////////////////////////////////////////////////

	CallArtillery::CallArtillery()
		: StateChild("CallArtillery")
		, FollowPathUser("CallArtillery")
		, m_MinCampTime(1000)
		, m_MaxCampTime(2000)
		, m_ExpireTime(0)
		, m_Stance(StanceStand)
	{
		LimitToWeapon().SetFlag(RTCW_WP_BINOCULARS);
	}

	void CallArtillery::GetDebugString(StringStr &out)
	{
		out << (m_MapGoal ? m_MapGoal->GetName() : "");
	}

	void CallArtillery::RenderDebug()
	{
		if(IsActive())
		{
			if(m_MapGoal)
			{
				Utils::OutlineOBB(m_MapGoal->GetWorldBounds(),COLOR::ORANGE, 5.f);
				Utils::DrawLine(GetClient()->GetEyePosition(),m_MapGoal->GetPosition(),COLOR::GREEN,5.f);
			}
		}
	}

	// FollowPathUser functions.
	bool CallArtillery::GetNextDestination(DestinationVector &_desination, bool &_final, bool &_skiplastpt)
	{
		if(m_MapGoal && m_MapGoal->RouteTo(GetClient(), _desination, 64.f))
			_final = false;
		else 
			_final = true;
		return true;
	}

	// AimerUser functions.
	bool CallArtillery::GetAimPosition(Vector3f &_aimpos)
	{
		if(m_MapGoalTarget)
		{
			_aimpos = m_MapGoalTarget->GetPosition();
		}
		else if(m_TargetEntity.IsValid())
		{
			const float LOOKAHEAD_TIME = 5.0f;
			const MemoryRecord *mr = GetClient()->GetSensoryMemory()->GetMemoryRecord(m_TargetEntity);
			if(mr)
			{
				const Vector3f vVehicleOffset = Vector3f(0.0f, 0.0f, 32.0f);
				_aimpos = vVehicleOffset + mr->m_TargetInfo.m_LastPosition + 
					mr->m_TargetInfo.m_LastVelocity * LOOKAHEAD_TIME;
				m_FireTime = IGame::GetTime() + 1000;
			}
		}
		return true;
	}

	void CallArtillery::OnTarget()
	{
		FINDSTATE(ws, WeaponSystem, GetRootState());
		if(ws && ws->CurrentWeaponIs(RTCW_WP_BINOCULARS))
		{
			GetClient()->PressButton(BOT_BUTTON_AIM);
			if(m_FireTime < IGame::GetTime())
			{
				if(GetClient()->HasEntityFlag(ENT_FLAG_ZOOMING))
				{
					ws->FireWeapon();
				}
			}
		}
	}

	obReal CallArtillery::GetPriority()
	{
		if(!InterfaceFuncs::IsWeaponCharged(GetClient(), RTCW_WP_BINOCULARS, Primary))
			return 0.f;

		if(IsActive())
			return GetLastPriority();

		m_MapGoal.reset();
		m_MapGoalTarget.reset();

		//////////////////////////////////////////////////////////////////////////
		RTCW_FilterClosest filter(GetClient(), AiState::SensoryMemory::EntEnemy);
		filter.AddCategory(ENT_CAT_SHOOTABLE);
		FINDSTATE(sensory,SensoryMemory,GetRootState());
		if(sensory)
		{
			sensory->QueryMemory(filter);
			m_TargetEntity = filter.GetBestEntity();
			if(m_TargetEntity.IsValid())
				return GetLastPriority();
		}
		//////////////////////////////////////////////////////////////////////////
		{
			GoalManager::Query qry(0x312ad48d /* CALLARTILLERY */, GetClient());
			GoalManager::GetInstance()->GetGoals(qry);
			for(obuint32 i = 0; i < qry.m_List.size(); ++i)
			{
				if(BlackboardIsDelayed(qry.m_List[i]->GetSerialNum()))
					continue;

				if(qry.m_List[i]->GetSlotsOpen(MapGoal::TRACK_INPROGRESS) < 1)
					continue;

				m_MapGoal = qry.m_List[i];
				break;
			}
		}
		if(!m_MapGoal)
			return 0.f;
		//////////////////////////////////////////////////////////////////////////
		Vector3f vSource = m_MapGoal->GetPosition();
		//vSource = vSource + Vector3(0,0,60);
		vSource.z = vSource.z + 60;
		//////////////////////////////////////////////////////////////////////////
		if(!m_MapGoalTarget)
		{
			GoalManager::Query qry(0xb708821b /* ARTILLERY_S */, GetClient());
			GoalManager::GetInstance()->GetGoals(qry);
			for(obuint32 i = 0; i < qry.m_List.size(); ++i)
			{
				if(BlackboardIsDelayed(qry.m_List[i]->GetSerialNum()))
					continue;

				if(qry.m_List[i]->GetSlotsOpen(MapGoal::TRACK_INPROGRESS) < 1)
					continue;

				{
					Vector3f vTarget = qry.m_List[i]->GetPosition();
					vTarget.z = vTarget.z + 20;
					if(!Client::HasLineOfSightTo(vSource,vTarget))
						continue;
				}

				m_MapGoalTarget = qry.m_List[i];
				break;
			}
		}
		//////////////////////////////////////////////////////////////////////////
		if(!m_MapGoalTarget)
		{
			GoalManager::Query qry(0xac0870ca /* ARTILLERY_D */, GetClient());
			GoalManager::GetInstance()->GetGoals(qry);
			for(obuint32 i = 0; i < qry.m_List.size(); ++i)
			{
				if(BlackboardIsDelayed(qry.m_List[i]->GetSerialNum()))
					continue;

				if(qry.m_List[i]->GetSlotsOpen(MapGoal::TRACK_INPROGRESS) < 1)
					continue;

				{
					Vector3f vTarget = qry.m_List[i]->GetPosition();
					vTarget.z = vTarget.z + 20;
					if(!Client::HasLineOfSightTo(vSource,vTarget))
						continue;
				}

				m_MapGoalTarget = qry.m_List[i];
				break;
			}
		}
		return m_MapGoalTarget ? m_MapGoalTarget->GetPriorityForClient(GetClient()) : 0.f;
	}

	void CallArtillery::Enter()
	{
		if(m_MapGoalTarget && m_MapGoalTarget->GetGoalType()=="ARTILLERY_D")
			m_FireTime = std::numeric_limits<int>::max();
		else
			m_FireTime = 0;			

		m_Fired = false;
		m_ExpireTime = 0;
		
		if(m_MapGoal)
		{
			m_MapGoal->GetProperty("Stance",m_Stance);
			m_MapGoal->GetProperty("MinCampTime",m_MinCampTime);
			m_MapGoal->GetProperty("MaxCampTime",m_MaxCampTime);
		}

		if(m_MapGoalTarget)
		{
			if(!m_WatchFilter)
			{
				m_WatchFilter.reset(new RTCW_FilterClosest(GetClient(), AiState::SensoryMemory::EntEnemy));
			}
			
			m_WatchFilter->AddClass(FilterSensory::ANYPLAYERCLASS);
			m_WatchFilter->AddPosition(m_MapGoalTarget->GetPosition());
			m_WatchFilter->SetMaxDistance(100.f);
			FINDSTATEIF(ProximityWatcher, GetRootState(), AddWatch(GetNameHash(),m_WatchFilter,false));
		}
		Tracker.InProgress = m_MapGoal;
		FINDSTATEIF(FollowPath, GetRootState(), Goto(this, Run));
	}

	void CallArtillery::Exit()
	{
		FINDSTATEIF(FollowPath, GetRootState(), Stop(true));

		m_MapGoal.reset();

		FINDSTATEIF(Aimer,GetRootState(),ReleaseAimRequest(GetNameHash()));
		FINDSTATEIF(WeaponSystem, GetRootState(),ReleaseWeaponRequest(GetNameHash()));
		FINDSTATEIF(ProximityWatcher, GetRootState(), RemoveWatch(GetNameHash()));

		Tracker.Reset();
	}

	State::StateStatus CallArtillery::Update(float fDt)
	{
		if(DidPathFail())
		{
			if(m_MapGoal)
				BlackboardDelay(10.f, m_MapGoal->GetSerialNum());
			return State_Finished;
		}

		if(m_MapGoal && !m_MapGoal->IsAvailable(GetClient()->GetTeam()))
			return State_Finished;
		if(m_MapGoalTarget && !m_MapGoalTarget->IsAvailable(GetClient()->GetTeam()))
			return State_Finished;

		if(DidPathSucceed())
		{
			if(m_Fired)
				return State_Finished;

			if(m_ExpireTime==0)
			{
				m_ExpireTime = IGame::GetTime()+Mathf::IntervalRandomInt(m_MinCampTime.GetMs(),m_MaxCampTime.GetMs());
				Tracker.InUse = m_MapGoal;
			}
			else if(IGame::GetTime() > m_ExpireTime)
			{
				// Delay it from being used for a while.
				if(m_MapGoal)
					BlackboardDelay(30.f, m_MapGoal->GetSerialNum());
				return State_Finished;
			}

			FINDSTATEIF(Aimer,GetRootState(),AddAimRequest(Priority::Medium,this,GetNameHash()));
			FINDSTATEIF(WeaponSystem, GetRootState(), AddWeaponRequest(Priority::Medium, GetNameHash(), RTCW_WP_BINOCULARS));

			if (m_Stance==StanceCrouch)
				GetClient()->PressButton(BOT_BUTTON_CROUCH);
		}
		return State_Busy;
	}

	void CallArtillery::ProcessEvent(const MessageHelper &_message, CallbackParameters &_cb)
	{
		switch(_message.GetMessageId())
		{
			HANDLER(MESSAGE_PROXIMITY_TRIGGER)
			{
				const AiState::Event_ProximityTrigger *m = _message.Get<AiState::Event_ProximityTrigger>();
				if(m->m_OwnerState == GetNameHash())
				{
					m_FireTime = IGame::GetTime();// + 1000;
				}
				break;
			}
			HANDLER(ACTION_WEAPON_FIRE)
			{
				const Event_WeaponFire *m = _message.Get<Event_WeaponFire>();
				if(m && m->m_WeaponId == RTCW_WP_BINOCULARS)
				{
					m_Fired = true;
				}
				break;
			}
		}
	}
	
	//////////////////////////////////////////////////////////////////////////
};
