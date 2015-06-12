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

				if(qry.m_List[i]->GetSlotsOpen(MapGoal::TRACK_INPROGRESS, GetClient()->GetTeam()) < 1)
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

				if(qry.m_List[i]->GetSlotsOpen(MapGoal::TRACK_INPROGRESS, GetClient()->GetTeam()) < 1)
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

				if(qry.m_List[i]->GetSlotsOpen(MapGoal::TRACK_INPROGRESS, GetClient()->GetTeam()) < 1)
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
		Tracker.InProgress.Set(m_MapGoal, GetClient()->GetTeam());
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
				Tracker.InUse.Set(m_MapGoal, GetClient()->GetTeam());
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
