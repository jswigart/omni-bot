////////////////////////////////////////////////////////////////////////////////
// 
// $LastChangedBy: drevil $
// $LastChangedDate: 2010-03-20 04:32:02 -0500 (Sat, 20 Mar 2010) $
// $LastChangedRevision: 4751 $
//
////////////////////////////////////////////////////////////////////////////////

#include "PrecompRTCW.h"
#include "RTCW_BaseStates.h"
#include "RTCW_FilterClosest.h"
#include "WeaponDatabase.h"

namespace AiState
{
	PlantExplosive::PlantExplosive()
		: StateChild("PlantExplosive")
		, FollowPathUser("PlantExplosive")
		, m_IgnoreTargets(false)
	{
		LimitToWeapon().SetFlag(RTCW_WP_DYNAMITE);
		SetAlwaysRecieveEvents(true);
	}

	void PlantExplosive::GetDebugString(StringStr &out)
	{
		switch(m_GoalState)
		{
		case LAY_EXPLOSIVE:
			out << "Lay Explosive ";
			break;
		case ARM_EXPLOSIVE:
			out << "Arm Explosive ";
			break;
		default:
			break;
		}

		out << std::endl << (m_MapGoal ? m_MapGoal->GetName() : "");
	}

	void PlantExplosive::RenderDebug()
	{
		if(IsActive())
		{
			Utils::OutlineOBB(m_MapGoal->GetWorldBounds(), COLOR::ORANGE, 5.f);
			Utils::DrawLine(GetClient()->GetEyePosition(),m_MapGoal->GetPosition(),COLOR::GREEN,5.f);
		}
	}

	// FollowPathUser functions.
	bool PlantExplosive::GetNextDestination(DestinationVector &_desination, bool &_final, bool &_skiplastpt)
	{
		if(m_MapGoal && m_MapGoal->RouteTo(GetClient(), _desination, 64.f))
			_final = false;
		else 
			_final = true;
		return true;
	}

	// AimerUser functions.
	bool PlantExplosive::GetAimPosition(Vector3f &_aimpos)
	{
		switch(m_GoalState)
		{
		case LAY_EXPLOSIVE:
			_aimpos = m_TargetPosition;
			break;
		case ARM_EXPLOSIVE:
			_aimpos = m_ExplosivePosition;
			break;
		default:
			OBASSERT(0, "Invalid Aim State");
			return false;
		}
		return true;
	}

	void PlantExplosive::OnTarget()
	{
		FINDSTATE(ws, WeaponSystem, GetRootState());
		if(ws)
		{
			if(m_GoalState == LAY_EXPLOSIVE)
			{
				if(ws->CurrentWeaponIs(RTCW_WP_DYNAMITE))
					ws->FireWeapon();
			}
			else if(m_GoalState == ARM_EXPLOSIVE)
			{
				if(ws->CurrentWeaponIs(RTCW_WP_PLIERS))
					ws->FireWeapon();
			}
		}
	}

	obReal PlantExplosive::GetPriority()
	{
		if(IsActive())
			return GetLastPriority();

		ExplosiveTargetType		targetType = XPLO_TYPE_DYNAMITE;
		RTCW_Weapon				weaponType = RTCW_WP_DYNAMITE;
		switch(GetClient()->GetClass())
		{
		case RTCW_CLASS_ENGINEER:
			{
				weaponType = RTCW_WP_DYNAMITE;
				targetType = XPLO_TYPE_DYNAMITE;
				break;
			}
		default:
			OBASSERT(0, "Wrong Class with Evaluator_PlantExplosive");
			return 0.0;
		}

		m_MapGoal.reset();

		if(InterfaceFuncs::IsWeaponCharged(GetClient(), weaponType, Primary)) 
		{
			{
				GoalManager::Query qry(0xbbcae592 /* PLANT */, GetClient());
				GoalManager::GetInstance()->GetGoals(qry);
				for(obuint32 i = 0; i < qry.m_List.size(); ++i)
				{
					if(BlackboardIsDelayed(qry.m_List[i]->GetSerialNum()))
						continue;

					if(!(qry.m_List[i]->GetGoalState() & targetType))
						continue;

					if(qry.m_List[i]->GetSlotsOpen(MapGoal::TRACK_INPROGRESS) < 1)
						continue;

					//CS: all this does is check if it's water proof?
					/*if(!_IsGoalAchievable(qry.m_List[i], weaponType))
						continue;*/

					m_MapGoal = qry.m_List[i];
					break;
				}
			}

			if(!m_MapGoal)
			{
				GoalManager::Query qry(0xa411a092 /* MOVER */, GetClient());
				GoalManager::GetInstance()->GetGoals(qry);
				for(obuint32 i = 0; i < qry.m_List.size(); ++i)
				{
					if(BlackboardIsDelayed(qry.m_List[i]->GetSerialNum()))
						continue;

					if(qry.m_List[i]->GetSlotsOpen(MapGoal::TRACK_INPROGRESS) < 1)
						continue;
					
					if(!(qry.m_List[i]->GetGoalState() & targetType))
						continue;

					m_MapGoal = qry.m_List[i];
					break;
				}
			}
		}
		return m_MapGoal ? m_MapGoal->GetPriorityForClient(GetClient()) : 0.f;
	}

	void PlantExplosive::Enter()
	{
		m_MapGoal->GetProperty("IgnoreTargets",m_IgnoreTargets);

		// set position to base of construction
		const Box3f obb = m_MapGoal->GetWorldBounds();
		m_ExplosivePosition = obb.GetCenterBottom();
		m_TargetPosition = m_ExplosivePosition;
		m_AdjustedPosition = false;
		m_GoalState = LAY_EXPLOSIVE;
		Tracker.InProgress = m_MapGoal;
		FINDSTATEIF(FollowPath, GetRootState(), Goto(this, Run, true));
	}

	void PlantExplosive::Exit()
	{
		FINDSTATEIF(FollowPath, GetRootState(), Stop(true));

		m_ExplosiveEntity.Reset();

		m_MapGoal.reset();
		FINDSTATEIF(Aimer,GetRootState(),ReleaseAimRequest(GetNameHash()));
		FINDSTATEIF(WeaponSystem, GetRootState(), ReleaseWeaponRequest(GetNameHash()));

		Tracker.Reset();
	}

	State::StateStatus PlantExplosive::Update(float fDt)
	{
		if(!m_MapGoal->IsAvailable(GetClient()->GetTeam()))
			return State_Finished;

		// If it's not destroyable, consider it a success.
		if(!InterfaceFuncs::IsDestroyable(GetClient(), m_MapGoal->GetEntity()))
			return State_Finished;

		if(m_ExplosiveEntity.IsValid() && !IGame::IsEntityValid(m_ExplosiveEntity))
			return State_Finished;

		if(DidPathSucceed())
		{
			switch(GetClient()->GetClass())
			{
			case RTCW_CLASS_ENGINEER:
				return _UpdateDynamite();
			default:
				OBASSERT(0, "Wrong Class in PlantExplosive");
			}
		}

		if(DidPathFail())
		{
			BlackboardDelay(10.f, m_MapGoal->GetSerialNum());
			return State_Finished;
		}
		return State_Busy;
	}

	State::StateStatus PlantExplosive::_UpdateDynamite()
	{
		Priority::ePriority pri = m_IgnoreTargets ? Priority::High : Priority::Medium;

		switch(m_GoalState)
		{
		case LAY_EXPLOSIVE:
			{
				if(!InterfaceFuncs::IsWeaponCharged(GetClient(), RTCW_WP_DYNAMITE, Primary))
					return State_Finished;

				/*if(m_Client->IsDebugEnabled(BOT_DEBUG_GOALS))
				{
					Utils::OutlineOBB(m_MapGoal->GetWorldBounds(), COLOR::ORANGE, 1.f);

					Vector3f vCenter;
					m_MapGoal->GetWorldBounds().CenterPoint(vCenter);
					Utils::DrawLine(m_Client->GetPosition(), vCenter, COLOR::GREEN, 1.0f);
					Utils::DrawLine(m_Client->GetPosition(), m_MapGoal->GetPosition(), COLOR::RED, 1.0f);
				}*/

				/*const float fDistanceToTarget = SquaredLength2d(m_TargetPosition, GetClient()->GetPosition());
				if (fDistanceToTarget > Mathf::Sqr(100.0f))
				{
					// check for badly waypointed maps
					if (!m_AdjustedPosition)
					{
						m_AdjustedPosition = true;

						// use our z value because some trigger entities may be below the ground
						Vector3f vCheckPos(m_TargetPosition.x, m_TargetPosition.y, GetClient()->GetEyePosition().z);
						//if(m_Client->IsDebugEnabled(BOT_DEBUG_GOALS))
						//{
						//	Utils::DrawLine(GetClient()->GetEyePosition(), vCheckPos, COLOR::GREEN, 2.0f);
						//}

						obTraceResult tr;
						EngineFuncs::TraceLine(tr, GetClient()->GetEyePosition(), vCheckPos, 
							NULL, (TR_MASK_SOLID | TR_MASK_PLAYERCLIP), GetClient()->GetGameID(), True);
						if (tr.m_Fraction != 1.0f && !tr.m_HitEntity.IsValid())
						{
							//m_TargetEntity->SetDeleteMe(true);
							return State_Finished;
						}

						// do a trace to adjust position
						EngineFuncs::TraceLine(tr, GetClient()->GetEyePosition(), m_TargetPosition, 
							NULL, (TR_MASK_SOLID | TR_MASK_PLAYERCLIP), -1, False);
						if (tr.m_Fraction != 1.0f)
						{
							m_TargetPosition = Vector3f(tr.m_Endpos);
						}
					}

					// Move toward it.
					GetClient()->GetSteeringSystem()->SetTarget(m_TargetPosition);
				}
				else
				{*/
					// We're within range, so let's start laying.
					GetClient()->ResetStuckTime();
					FINDSTATEIF(Aimer,GetRootState(),AddAimRequest(Priority::Medium,this,GetNameHash()));
					FINDSTATEIF(WeaponSystem, GetRootState(), AddWeaponRequest(pri, GetNameHash(), RTCW_WP_DYNAMITE));
				//}
				break;
			}
		case ARM_EXPLOSIVE:
			{
				GetClient()->PressButton(BOT_BUTTON_CROUCH);

				if(InterfaceFuncs::GetExplosiveState(GetClient(), m_ExplosiveEntity) == XPLO_ARMED)
				{
					BlackboardDelay(30.f, m_MapGoal->GetSerialNum());
					return State_Finished;
				}

				// Disable avoidance for this frame.
				GetClient()->GetSteeringSystem()->SetNoAvoidTime(IGame::GetTime());

				// update dynamite position
				EngineFuncs::EntityPosition(m_ExplosiveEntity, m_ExplosivePosition);

				// move a little bit close if dyno too far away
				if ((m_ExplosivePosition - GetClient()->GetPosition()).SquaredLength() > 2500.0f)
				{
					GetClient()->GetSteeringSystem()->SetTarget(m_ExplosivePosition);
				}
				else
				{
					GetClient()->ResetStuckTime();
					FINDSTATEIF(Aimer,GetRootState(),AddAimRequest(Priority::Medium,this,GetNameHash()));
					FINDSTATEIF(WeaponSystem, GetRootState(), AddWeaponRequest(Priority::Medium, GetNameHash(), RTCW_WP_PLIERS));					
				}
				break;
			}
		default:
			// keep processing
			break;
		}
		return State_Busy;
	}

	void PlantExplosive::ProcessEvent(const MessageHelper &_message, CallbackParameters &_cb)
	{
		Priority::ePriority pri = m_IgnoreTargets ? Priority::High : Priority::Medium;
		switch(_message.GetMessageId())
		{
			HANDLER(ACTION_WEAPON_FIRE)
			{
				const Event_WeaponFire *m = _message.Get<Event_WeaponFire>();
				if(m->m_WeaponId == RTCW_WP_DYNAMITE && m->m_Projectile.IsValid())
				{
					SetLastPriority(2.f); // up the priority so we aren't interrupted(to arm/det)
					m_ExplosiveEntity = m->m_Projectile;
					m_GoalState = ARM_EXPLOSIVE;
					SetLastPriority(2.f); // up the priority so we aren't interrupted(to arm)

					//cs: we don't want this activated outside this state, else it's never released
					if(IsActive())
						FINDSTATEIF(WeaponSystem, GetRootState(), AddWeaponRequest(pri, GetNameHash(), RTCW_WP_PLIERS));
				}
				break;
			}			
		}
	}

	bool PlantExplosive::_IsGoalAchievable(MapGoalPtr _g, int _weaponId)
	{
		static bool bDynaUnderWater = false;
		static bool bDynaCached = false;

		if(!bDynaCached)
		{
			WeaponPtr w = g_WeaponDatabase.GetWeapon(RTCW_WP_DYNAMITE);
			if(w)
			{
				bDynaUnderWater = w->GetFireMode(Primary).CheckFlag(Weapon::Waterproof);
				bDynaCached = true;
			}
		}

		if ( _weaponId==RTCW_WP_DYNAMITE && !bDynaUnderWater )
			return true;

		return false;
	}

	//////////////////////////////////////////////////////////////////////////

	MountMg42::MountMg42()
		: StateChild("MountMg42")
		, FollowPathUser("MountMg42")
		, m_MinCampTime(6000)
		, m_MaxCampTime(10000)
		, m_ExpireTime(0)
		, m_ScanDirection(SCAN_DEFAULT)
		, m_NextScanTime(0)
		, m_IgnoreTargets(false)
	{
	}

	void MountMg42::GetDebugString(StringStr &out)
	{
		if(IsActive())
		{
			if(!GetClient()->HasEntityFlag(RTCW_ENT_FLAG_MNT_MG42))
				out << "Mounting ";
			else
			{
				switch(m_ScanDirection)
				{
				case SCAN_DEFAULT:
					out << "Scan Facing ";
					break;
				case SCAN_MIDDLE:
					out << "Scan Middle ";
					break;
				case SCAN_LEFT:
					out << "Scan Left ";
					break;
				case SCAN_RIGHT:
					out << "Scan Right ";
					break;
				}
			}

			if(m_MapGoal)
				out << m_MapGoal->GetName();
		}
	}

	void MountMg42::RenderDebug()
	{
		if(IsActive())
		{
			Utils::OutlineOBB(m_MapGoal->GetWorldBounds(), COLOR::ORANGE, 5.f);
			Utils::DrawLine(GetClient()->GetEyePosition(),m_MapGoal->GetPosition(),COLOR::GREEN,5.f);
			m_TargetZone.RenderDebug();
		}
	}

	// FollowPathUser functions.
	bool MountMg42::GetNextDestination(DestinationVector &_desination, bool &_final, bool &_skiplastpt)
	{
		if(m_MapGoal && m_MapGoal->RouteTo(GetClient(), _desination, 64.f))
			_final = false;
		else 
			_final = true;
		return true;
	}

	// AimerUser functions.
	bool MountMg42::GetAimPosition(Vector3f &_aimpos)
	{
		_aimpos = m_AimPoint;
		return true;
	}

	void MountMg42::OnTarget()
	{
		if(!GetClient()->HasEntityFlag(RTCW_ENT_FLAG_MNT_MG42) && (IGame::GetFrameNumber()&1))
			GetClient()->PressButton(BOT_BUTTON_USE);
	}

	obReal MountMg42::GetPriority()
	{
		if(IsActive()) 
			return GetLastPriority();
		
		BitFlag64 entFlags;

		GoalManager::Query qry(0xe1a2b09c /* MOUNTMG42 */, GetClient());
		GoalManager::GetInstance()->GetGoals(qry);
		for(obuint32 i = 0; i < qry.m_List.size(); ++i)
		{
			if(BlackboardIsDelayed(qry.m_List[i]->GetSerialNum()))
				continue;

			if(qry.m_List[i]->GetSlotsOpen(MapGoal::TRACK_INPROGRESS) < 1)
				continue;

			GameEntity gunOwner = InterfaceFuncs::GetMountedPlayerOnMG42(GetClient(), qry.m_List[i]->GetEntity());
			int gunHealth = InterfaceFuncs::GetGunHealth(GetClient(), qry.m_List[i]->GetEntity());
			bool bBroken = InterfaceFuncs::IsMountableGunRepairable(GetClient(), qry.m_List[i]->GetEntity());

			if(bBroken)
				continue;

			if(!InterfaceFuncs::GetEntityFlags(qry.m_List[i]->GetEntity(), entFlags) ||
				!entFlags.CheckFlag(RTCW_ENT_FLAG_ISMOUNTABLE))
				continue;

			// Make sure nobody has it mounted.
			if((!gunOwner.IsValid() || !GetClient()->IsAllied(gunOwner)) && (gunHealth > 0))
			{
				m_MapGoal = qry.m_List[i];
				break;
			}
		}
		return m_MapGoal ? m_MapGoal->GetPriorityForClient(GetClient()) : 0.f;
	}

	void MountMg42::Enter()
	{
		m_MapGoal->GetProperty("MinCampTime",m_MinCampTime);
		m_MapGoal->GetProperty("MaxCampTime",m_MaxCampTime);
		m_MapGoal->GetProperty("IgnoreTargets",m_IgnoreTargets);

		m_ExpireTime = 0;

		m_ScanDirection = SCAN_MIDDLE;
		m_NextScanTime = IGame::GetTime() + (int)Mathf::IntervalRandom(2000.0f, 7000.0f);
		m_AimPoint = m_MapGoal->GetPosition();
		m_MG42Position = m_AimPoint;
		m_ScanLeft = Vector3f::ZERO;
		m_ScanRight = Vector3f::ZERO;
		m_GotGunProperties = false;
		Tracker.InProgress = m_MapGoal;
		m_TargetZone.Restart(256.f);
		FINDSTATEIF(FollowPath, GetRootState(), Goto(this, Run, true));
	}

	void MountMg42::Exit()
	{
		FINDSTATEIF(FollowPath, GetRootState(), Stop(true));

		m_MapGoal.reset();
		FINDSTATEIF(Aimer,GetRootState(),ReleaseAimRequest(GetNameHash()));

		if(GetClient()->HasEntityFlag(RTCW_ENT_FLAG_MNT_MG42))
			GetClient()->PressButton(BOT_BUTTON_USE);

		Tracker.Reset();
	}

	State::StateStatus MountMg42::Update(float fDt)
	{
		if(DidPathFail())
		{
			BlackboardDelay(10.f, m_MapGoal->GetSerialNum());
			return State_Finished;
		}

		if(!m_MapGoal->IsAvailable(GetClient()->GetTeam()))
			return State_Finished;

		//////////////////////////////////////////////////////////////////////////
		// Only fail if a friendly player is on this gun or gun has been destroyed in the meantime
		//int gunHealth = InterfaceFuncs::GetGunHealth(m_Client, m_MG42Goal->GetEntity());
		GameEntity mounter = InterfaceFuncs::GetMountedPlayerOnMG42(GetClient(), m_MapGoal->GetEntity());
		if(InterfaceFuncs::IsMountableGunRepairable(GetClient(), m_MapGoal->GetEntity()) ||
			(mounter.IsValid() && (mounter != GetClient()->GetGameEntity()) && GetClient()->IsAllied(mounter)))
		{
			return State_Finished;
		}
		//////////////////////////////////////////////////////////////////////////

		if(DidPathSucceed())
		{
			GetClient()->GetSteeringSystem()->SetTarget(m_MapGoal->GetPosition());

			const bool bMounted = GetClient()->HasEntityFlag(RTCW_ENT_FLAG_MNT_MG42);

			Priority::ePriority pri = m_IgnoreTargets && !bMounted ? Priority::Medium : Priority::Low;
			FINDSTATEIF(Aimer,GetRootState(),AddAimRequest(pri,this,GetNameHash()));

			if(bMounted)
			{
				// Only hang around here for a certain amount of time
				if(m_ExpireTime==0)
				{
					m_ExpireTime = IGame::GetTime()+Mathf::IntervalRandomInt(m_MinCampTime,m_MaxCampTime);
					Tracker.InUse = m_MapGoal;
				}
				else if(IGame::GetTime() > m_ExpireTime)
				{
					// Delay it from being used for a while.
					BlackboardDelay(10.f, m_MapGoal->GetSerialNum());
					return State_Finished;
				}

				m_TargetZone.Update(GetClient());

				if(!m_GotGunProperties)
				{
					m_GotGunProperties = true;
					_GetMG42Properties();
					m_AimPoint = m_MapGoal->GetPosition() + m_GunCenterArc * 512.f;
				}

				if(m_NextScanTime < IGame::GetTime())
				{
					m_NextScanTime = IGame::GetTime() + (int)Mathf::IntervalRandom(2000.0f, 7000.0f);
					m_ScanDirection = (int)Mathf::IntervalRandom(0.0f, (float)NUM_SCAN_TYPES);

					// we're mounted, so lets look around mid view.
					m_TargetZone.UpdateAimPosition();
				}

				if(m_TargetZone.HasAim())
					m_ScanDirection = SCAN_ZONES;

				switch(m_ScanDirection)
				{
				case SCAN_DEFAULT:
					if(m_MapGoal->GetFacing() != Vector3f::ZERO)
					{
						m_AimPoint = m_MG42Position + m_MapGoal->GetFacing() * 1024.f;
						break;
					}
				case SCAN_MIDDLE:
					{
						m_AimPoint = m_MG42Position + m_GunCenterArc * 1024.f;
						break;
					}
				case SCAN_LEFT:
					if(m_ScanLeft != Vector3f::ZERO)
					{
						m_AimPoint = m_MG42Position + m_ScanLeft * 1024.f;
						break;
					}						
				case SCAN_RIGHT:
					if(m_ScanRight != Vector3f::ZERO)
					{
						m_AimPoint = m_MG42Position + m_ScanRight * 1024.f;
						break;
					}
				case SCAN_ZONES:
					{
						m_AimPoint = m_TargetZone.GetAimPosition();
						break;
					}
				default:
					break;
				}
			}
		}
		return State_Busy;
	}

	bool MountMg42::_GetMG42Properties()
	{
		RTCW_MG42Info data;
		if(!InterfaceFuncs::GetMg42Properties(GetClient(), data))
			return false;

		m_GunCenterArc = Vector3f(data.m_CenterFacing);

		m_MinHorizontalArc = data.m_MinHorizontalArc;
		m_MaxHorizontalArc = data.m_MaxHorizontalArc;
		m_MinVerticalArc = data.m_MinVerticalArc;
		m_MaxVerticalArc = data.m_MaxVerticalArc;

		// Calculate the planes for the MG42

		/*Matrix3f planeMatrices[4];
		planeMatrices[0].FromEulerAnglesXYZ(m_MinHorizontalArc, 0.0f, 0.0f);
		planeMatrices[1].FromEulerAnglesXYZ(m_MaxHorizontalArc, 0.0f, 0.0f);
		planeMatrices[2].FromEulerAnglesXYZ(0.0f, m_MinHorizontalArc, 0.0f);
		planeMatrices[3].FromEulerAnglesXYZ(0.0f, m_MaxHorizontalArc, 0.0f);

		m_GunArcPlanes[0] = Plane3f(m_GunCenterArc * planeMatrices[0], m_MG42Position);
		m_GunArcPlanes[1] = Plane3f(m_GunCenterArc * planeMatrices[1], m_MG42Position);
		m_GunArcPlanes[2] = Plane3f(m_GunCenterArc * planeMatrices[2], m_MG42Position);
		m_GunArcPlanes[3] = Plane3f(m_GunCenterArc * planeMatrices[3], m_MG42Position);*/

		const float fScanPc = 0.4f;

		Quaternionf ql;
		ql.FromAxisAngle(Vector3f::UNIT_Z, Mathf::DegToRad(m_MinHorizontalArc * fScanPc));
		m_ScanLeft = ql.Rotate(m_GunCenterArc);

		Quaternionf qr;
		qr.FromAxisAngle(Vector3f::UNIT_Z, Mathf::DegToRad(m_MaxHorizontalArc * fScanPc));
		m_ScanRight = qr.Rotate(m_GunCenterArc);

		return true;
	}

	//////////////////////////////////////////////////////////////////////////

	RepairMg42::RepairMg42()
		: StateChild("RepairMg42")
		, FollowPathUser("RepairMg42")
		, m_IgnoreTargets(false)
	{
		LimitToWeapon().SetFlag(RTCW_WP_PLIERS);
	}

	void RepairMg42::GetDebugString(StringStr &out)
	{
		if(IsActive())
		{
			//if(!GetClient()->HasEntityFlag(ET_ENT_FLAG_MOUNTED))
			out << "Repairing " + (m_MapGoal?m_MapGoal->GetName():"");
		}
	}

	void RepairMg42::RenderDebug()
	{
		if(IsActive())
		{
			Utils::OutlineOBB(m_MapGoal->GetWorldBounds(), COLOR::ORANGE, 5.f);
			Utils::DrawLine(GetClient()->GetEyePosition(),m_MapGoal->GetPosition(),COLOR::GREEN,5.f);
		}
	}

	// FollowPathUser functions.
	bool RepairMg42::GetNextDestination(DestinationVector &_desination, bool &_final, bool &_skiplastpt)
	{
		if(m_MapGoal && m_MapGoal->RouteTo(GetClient(), _desination, 64.f))
			_final = false;
		else 
			_final = true;
		return true;
	}

	// AimerUser functions.
	bool RepairMg42::GetAimPosition(Vector3f &_aimpos)
	{
		_aimpos = m_MG42Position;
		return true;
	}

	void RepairMg42::OnTarget()
	{
		FINDSTATE(ws,WeaponSystem,GetRootState());
		if(ws && ws->GetCurrentRequestOwner() == GetNameHash())
			ws->FireWeapon();
	}

	obReal RepairMg42::GetPriority()
	{
		if(IsActive())
			return GetLastPriority();

		BitFlag64 entFlags;

		GoalManager::Query qry(0x17929136 /* REPAIRMG42 */, GetClient());
		GoalManager::GetInstance()->GetGoals(qry);
		for(obuint32 i = 0; i < qry.m_List.size(); ++i)
		{
			if(BlackboardIsDelayed(qry.m_List[i]->GetSerialNum()))
				continue;

			bool bBroken = InterfaceFuncs::IsMountableGunRepairable(GetClient(), qry.m_List[i]->GetEntity());
			if(!bBroken)
				continue;

			m_MapGoal = qry.m_List[i];
			break;
		}
		return m_MapGoal ? m_MapGoal->GetPriorityForClient(GetClient()) : 0.f;
	}

	void RepairMg42::Enter()
	{
		m_MapGoal->GetProperty("IgnoreTargets",m_IgnoreTargets);

		Tracker.InProgress = m_MapGoal;
		m_MG42Position = m_MapGoal->GetWorldBounds().Center;
		//m_MapGoal->GetWorldBounds().CenterBottom(m_MG42Position);
		FINDSTATEIF(FollowPath, GetRootState(), Goto(this, Run, true));
	}

	void RepairMg42::Exit()
	{
		FINDSTATEIF(FollowPath, GetRootState(), Stop(true));

		m_MapGoal.reset();
		Tracker.Reset();
		FINDSTATEIF(Aimer,GetRootState(),ReleaseAimRequest(GetNameHash()));
		FINDSTATEIF(WeaponSystem,GetRootState(),ReleaseWeaponRequest(GetNameHash()));
	}

	State::StateStatus RepairMg42::Update(float fDt)
	{
		if(DidPathFail())
		{
			BlackboardDelay(10.f, m_MapGoal->GetSerialNum());
			return State_Finished;
		}

		bool bBroken = InterfaceFuncs::IsMountableGunRepairable(GetClient(), m_MapGoal->GetEntity());
		if(!bBroken || !m_MapGoal->IsAvailable(GetClient()->GetTeam()))
			return State_Finished;

		if(DidPathSucceed())
		{
			GetClient()->PressButton(BOT_BUTTON_CROUCH);
			GetClient()->GetSteeringSystem()->SetTarget(m_MapGoal->GetPosition());

			Priority::ePriority pri = m_IgnoreTargets ? Priority::High : Priority::Medium;
			FINDSTATEIF(Aimer,GetRootState(),AddAimRequest(pri,this,GetNameHash()));
			FINDSTATEIF(WeaponSystem, GetRootState(), AddWeaponRequest(Priority::Medium, GetNameHash(), RTCW_WP_PLIERS));
		}
		return State_Busy;
	}

	//////////////////////////////////////////////////////////////////////////

	TakeCheckPoint::TakeCheckPoint()
		: StateChild("TakeCheckPoint")
		, FollowPathUser("TakeCheckPoint")
	{
	}

	void TakeCheckPoint::GetDebugString(StringStr &out)
	{
		out << (m_MapGoal ? m_MapGoal->GetName() : "");
	}

	void TakeCheckPoint::RenderDebug()
	{
		if(IsActive())
		{
			Utils::OutlineOBB(m_MapGoal->GetWorldBounds(), COLOR::ORANGE, 5.f);
			Utils::DrawLine(GetClient()->GetEyePosition(),m_MapGoal->GetPosition(),COLOR::GREEN,5.f);
		}
	}

	// FollowPathUser functions.
	bool TakeCheckPoint::GetNextDestination(DestinationVector &_desination, bool &_final, bool &_skiplastpt)
	{
		if(m_MapGoal && m_MapGoal->RouteTo(GetClient(), _desination, 64.f))
			_final = false;
		else 
			_final = true;
		return true;
	}

	obReal TakeCheckPoint::GetPriority()
	{
		if(IsActive())
			return GetLastPriority();

		m_MapGoal.reset();

		GoalManager::Query qry(0xf7e4a57f /* CHECKPOINT */, GetClient());
		GoalManager::GetInstance()->GetGoals(qry);
		qry.GetBest(m_MapGoal);

		return m_MapGoal ? m_MapGoal->GetPriorityForClient(GetClient()) : 0.f;
	}

	void TakeCheckPoint::Enter()
	{
		m_TargetPosition = m_MapGoal->GetWorldBounds().Center;
		Tracker.InProgress = m_MapGoal;
		FINDSTATEIF(FollowPath, GetRootState(), Goto(this, Run, true));
	}

	void TakeCheckPoint::Exit()
	{
		FINDSTATEIF(FollowPath, GetRootState(), Stop(true));

		m_MapGoal.reset();
		FINDSTATEIF(Aimer,GetRootState(),ReleaseAimRequest(GetNameHash()));
		FINDSTATEIF(WeaponSystem, GetRootState(), ReleaseWeaponRequest(GetNameHash()));

		Tracker.Reset();
	}

	State::StateStatus TakeCheckPoint::Update(float fDt)
	{
		if(DidPathFail())
		{
			BlackboardDelay(10.f, m_MapGoal->GetSerialNum());
			return State_Finished;
		}

		if(!m_MapGoal->IsAvailable(GetClient()->GetTeam()))
			return State_Finished;

		if(DidPathSucceed())
		{
			m_TargetPosition.z = GetClient()->GetPosition().z;
			GetClient()->GetSteeringSystem()->SetTarget(m_TargetPosition, 32.f);			
		}
		return State_Busy;
	}

	//////////////////////////////////////////////////////////////////////////

	ReviveTeammate::ReviveTeammate()
		: StateChild("ReviveTeammate")
		, FollowPathUser("ReviveTeammate")
	{
		LimitToWeapon().SetFlag(RTCW_WP_SYRINGE);
	}

	void ReviveTeammate::GetDebugString(StringStr &out)
	{
		switch(m_GoalState)
		{
		case REVIVING:
			out << "Reviving ";
			break;
		case HEALING:
			out << "Healing ";
			break;
		}

		if(m_MapGoal && m_MapGoal->GetEntity().IsValid())
			out << std::endl << EngineFuncs::EntityName(m_MapGoal->GetEntity(), "<noname>");
	}

	void ReviveTeammate::RenderDebug()
	{
		if(IsActive())
		{
			Utils::OutlineOBB(m_MapGoal->GetWorldBounds(), COLOR::ORANGE, 5.f);
			Utils::DrawLine(GetClient()->GetPosition(),m_MapGoal->GetPosition(),COLOR::GREEN,5.f);
			Utils::DrawLine(GetClient()->GetEyePosition(),m_MapGoal->GetPosition(),COLOR::MAGENTA,5.f);
		}
	}

	// FollowPathUser functions.
	bool ReviveTeammate::GetNextDestination(DestinationVector &_desination, bool &_final, bool &_skiplastpt)
	{
		if(m_MapGoal && m_MapGoal->RouteTo(GetClient(), _desination, 64.f))
			_final = false;
		else 
			_final = true;
		return true;
	}

	// AimerUser functions.
	bool ReviveTeammate::GetAimPosition(Vector3f &_aimpos)
	{
		const Box3f obb = m_MapGoal->GetWorldBounds();
		if ( m_GoalState == REVIVING )
			_aimpos = obb.GetCenterBottom();
		else
			_aimpos = obb.Center;
		return true;
	}

	void ReviveTeammate::OnTarget()
	{
		FINDSTATE(ws, WeaponSystem, GetRootState());
		if(ws)
		{
			if(InterfaceFuncs::IsAlive(m_MapGoal->GetEntity()))
			{
				if(ws->CurrentWeaponIs(RTCW_WP_MEDKIT))
					ws->FireWeapon();
			}
			else
			{
				if(ws->CurrentWeaponIs(RTCW_WP_SYRINGE))
					ws->FireWeapon();
			}
		}
	}

	obReal ReviveTeammate::GetPriority()
	{
		if(IsActive())
			return GetLastPriority();

		m_MapGoal.reset();

		GoalManager::Query qry(0x2086cdf0 /* REVIVE */, GetClient());
		GoalManager::GetInstance()->GetGoals(qry);
		//qry.GetBest(m_MapGoal);
		
		m_List = qry.m_List;
		m_MapGoal.reset();
		// TODO: check weapon capability vs target underwater?

		return !m_List.empty() ? m_List.front()->GetPriorityForClient(GetClient()) : 0.f;
	}

	void ReviveTeammate::Enter()
	{
		m_GoalState = REVIVING;

		m_CheckReviveTimer.Delay(2.f);

		FINDSTATE(fp, FollowPath, GetRootState());
		if(fp->Goto(this, m_List, Run))
		{
			m_MapGoal = m_List[GetDestinationIndex()];
			Tracker.InProgress = m_MapGoal;
		}
		else
		{
			m_MapGoal.reset();
			Tracker.Reset();
			for(obuint32 i = 0; i < m_List.size(); ++i)
			{
				BlackboardDelay(10.f, m_List[i]->GetSerialNum());
			}
		}
	}

	void ReviveTeammate::Exit()
	{
		FINDSTATEIF(FollowPath, GetRootState(), Stop(true));

		m_MapGoal.reset();

		FINDSTATEIF(Aimer,GetRootState(),ReleaseAimRequest(GetNameHash()));
		FINDSTATEIF(WeaponSystem, GetRootState(),ReleaseWeaponRequest(GetNameHash()));

		Tracker.Reset();
	}

	State::StateStatus ReviveTeammate::Update(float fDt)
	{
		if(!m_MapGoal)
			return State_Finished;

		if(m_CheckReviveTimer.IsExpired())
		{
			// finish if there's new goals, so we can activate again and go to a new goal
			if(AreThereNewReviveGoals())
				return State_Finished;
			m_CheckReviveTimer.Delay(2.f);
		}

		if(DidPathFail())
		{
			BlackboardDelay(10.f, m_MapGoal->GetSerialNum());
			return State_Finished;
		}

		if(!m_MapGoal->IsAvailable(GetClient()->GetTeam()))
			return State_Finished;

		GameEntity reviveEnt = m_MapGoal->GetEntity();

		//////////////////////////////////////////////////////////////////////////
		Msg_HealthArmor ha;
		if(InterfaceFuncs::GetHealthAndArmor(reviveEnt, ha) && ha.m_CurrentHealth >= ha.m_MaxHealth)
			return State_Finished;

		BitFlag64 ef;
		if(InterfaceFuncs::GetEntityFlags(reviveEnt,ef))
		{
			if(ef.CheckFlag(RTCW_ENT_FLAG_INLIMBO))
				return State_Finished;
		}
		//////////////////////////////////////////////////////////////////////////

		if(DidPathSucceed())
		{
			if(GetClient()->GetStuckTime() > 2000)
				return State_Finished;

			FINDSTATEIF(Aimer,GetRootState(),AddAimRequest(Priority::Medium,this,GetNameHash()));
			GetClient()->GetSteeringSystem()->SetTarget(m_MapGoal->GetPosition());

			switch(m_GoalState)
			{
			case REVIVING:
				{
					if(InterfaceFuncs::IsAlive(reviveEnt))
						m_GoalState = HEALING;

					GetClient()->PressButton(BOT_BUTTON_CROUCH);

					GetClient()->GetSteeringSystem()->SetNoAvoidTime(IGame::GetTime() + 1000);
					FINDSTATEIF(WeaponSystem, GetRootState(), AddWeaponRequest(Priority::Medium, GetNameHash(), RTCW_WP_SYRINGE));					
					break;
				}				
			case HEALING:
				{
					if(GetClient()->GetTargetingSystem()->HasTarget())
						return State_Finished;

					if(!InterfaceFuncs::IsWeaponCharged(GetClient(), RTCW_WP_MEDKIT, Primary))
						return State_Finished;

					//CS: TODO: need a better solution for this
					/*const float fDistanceToTarget = SquaredLength2d(m_MapGoal->GetPosition(), GetClient()->GetPosition());

					//step back a little so revivee can stand
					if (fDistanceToTarget < Mathf::Sqr(36.0f))
						GetClient()->PressButton(BOT_BUTTON_BACK);
					else if (fDistanceToTarget > Mathf::Sqr(144.0f))
						return State_Finished;*/ 

					GetClient()->GetSteeringSystem()->SetNoAvoidTime(IGame::GetTime() + 1000);
					FINDSTATEIF(WeaponSystem, GetRootState(), AddWeaponRequest(Priority::Medium, GetNameHash(), RTCW_WP_MEDKIT));
					break;
				}
			}
		}
		return State_Busy;
	}

	bool ReviveTeammate::AreThereNewReviveGoals()
	{
		GoalManager::Query qry(0x2086cdf0 /* REVIVE */, GetClient());
		GoalManager::GetInstance()->GetGoals(qry);

		for(obuint32 i = 0; i < qry.m_List.size(); ++i)
		{
			if(std::find(m_List.begin(),m_List.end(),qry.m_List[i]) == m_List.end())
				return true;
		}

		return false;
	}

	//////////////////////////////////////////////////////////////////////////

	DefuseDynamite::DefuseDynamite()
		: StateChild("DefuseDynamite")
		, FollowPathUser("DefuseDynamite")
	{
		LimitToWeapon().SetFlag(RTCW_WP_PLIERS);
	}

	void DefuseDynamite::GetDebugString(StringStr &out)
	{
		out << (m_MapGoal ? m_MapGoal->GetName() : "");
	}

	void DefuseDynamite::RenderDebug()
	{
		if(IsActive())
		{
			Utils::OutlineOBB(m_MapGoal->GetWorldBounds(), COLOR::ORANGE, 5.f);
			Utils::DrawLine(GetClient()->GetEyePosition(),m_MapGoal->GetPosition(),COLOR::GREEN,5.f);
		}
	}

	// FollowPathUser functions.
	bool DefuseDynamite::GetNextDestination(DestinationVector &_desination, bool &_final, bool &_skiplastpt)
	{
		if(m_MapGoal && m_MapGoal->RouteTo(GetClient(), _desination, 64.f))
			_final = false;
		else 
			_final = true;
		return true;
	}

	// AimerUser functions.
	bool DefuseDynamite::GetAimPosition(Vector3f &_aimpos)
	{
		_aimpos = m_MapGoal->GetPosition();
		return true;
	}

	void DefuseDynamite::OnTarget()
	{
		FINDSTATE(ws, WeaponSystem, GetRootState());
		if(ws && ws->CurrentWeaponIs(RTCW_WP_PLIERS))
			ws->FireWeapon();
	}

	obReal DefuseDynamite::GetPriority()
	{
		if(IsActive())
			return GetLastPriority();

		m_MapGoal.reset();

		GoalManager::Query qry(0x1899efc7 /* DEFUSE */, GetClient());
		GoalManager::GetInstance()->GetGoals(qry);
		for(obuint32 i = 0; i < qry.m_List.size(); ++i)
		{
			if(BlackboardIsDelayed(qry.m_List[i]->GetSerialNum()))
				continue;

			if(qry.m_List[i]->GetSlotsOpen(MapGoal::TRACK_INPROGRESS) < 1)
				continue;

			if(InterfaceFuncs::GetExplosiveState(GetClient(), qry.m_List[i]->GetEntity()) == XPLO_ARMED)
			{
				m_MapGoal = qry.m_List[i];
				break;
			}
			else
			{
				qry.m_List[i]->SetDeleteMe(true);
			}
		}
		return m_MapGoal ? m_MapGoal->GetPriorityForClient(GetClient()) : 0.f;
	}

	void DefuseDynamite::Enter()
	{
		m_TargetPosition = m_MapGoal->GetWorldBounds().Center;
		Tracker.InProgress = m_MapGoal;
		FINDSTATEIF(FollowPath, GetRootState(), Goto(this, Run));
	}

	void DefuseDynamite::Exit()
	{
		FINDSTATEIF(FollowPath, GetRootState(), Stop(true));

		m_MapGoal.reset();

		FINDSTATEIF(Aimer,GetRootState(),ReleaseAimRequest(GetNameHash()));
		FINDSTATEIF(WeaponSystem, GetRootState(),ReleaseWeaponRequest(GetNameHash()));

		Tracker.Reset();
	}

	State::StateStatus DefuseDynamite::Update(float fDt)
	{
		if(DidPathFail())
		{
			BlackboardDelay(10.f, m_MapGoal->GetSerialNum());
			return State_Finished;
		}

		if(!m_MapGoal->IsAvailable(GetClient()->GetTeam()))
			return State_Finished;

		if(DidPathSucceed())
		{
			ExplosiveState eState = InterfaceFuncs::GetExplosiveState(GetClient(), m_MapGoal->GetEntity());
			switch(eState)
			{
			case XPLO_INVALID:
			case XPLO_UNARMED:
				return State_Finished;
			default:
				break; // keep processing
			}

			m_TargetPosition = m_MapGoal->GetWorldBounds().Center;

			const float fDistanceToDynamite = SquaredLength2d(m_TargetPosition, GetClient()->GetPosition());
			if(fDistanceToDynamite > 2500.0f)
			{
				GetClient()->GetSteeringSystem()->SetTarget(m_TargetPosition);
			}
			else
			{
				GetClient()->PressButton(BOT_BUTTON_CROUCH);

				FINDSTATEIF(Aimer,GetRootState(),AddAimRequest(Priority::Medium,this,GetNameHash()));
				FINDSTATEIF(WeaponSystem, GetRootState(), AddWeaponRequest(Priority::Medium, GetNameHash(), RTCW_WP_PLIERS));
			}
		}
		return State_Busy;
	}

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
		/*RTCW_FilterClosest filter(GetClient(), AiState::SensoryMemory::EntEnemy);
		filter.AddCategory(ENT_CAT_SHOOTABLE);
		FINDSTATE(sensory,SensoryMemory,GetRootState());
		if(sensory)
		{
			sensory->QueryMemory(filter);
			m_TargetEntity = filter.GetBestEntity();
			if(m_TargetEntity.IsValid())
				return GetLastPriority();
		}*/
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

				if(!Client::HasLineOfSightTo(vSource,qry.m_List[i]->GetPosition()))
					continue;

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

				if(!Client::HasLineOfSightTo(vSource,qry.m_List[i]->GetPosition()))
					continue;

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
				m_ExpireTime = IGame::GetTime()+Mathf::IntervalRandomInt(m_MinCampTime,m_MaxCampTime);
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

			if (m_Stance==StanceProne)
				GetClient()->PressButton(BOT_BUTTON_PRONE);
			else if (m_Stance==StanceCrouch)
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

	Panzer::Panzer() 
		: StateChild("Panzer")
		, FollowPathUser("Panzer")
		, m_MinCampTime(2000)
		, m_MaxCampTime(8000)
		, m_ExpireTime(0)
		, m_Stance(StanceStand)
	{
		LimitToWeapon().SetFlag(RTCW_WP_PANZERFAUST);
	}

	void Panzer::GetDebugString(StringStr &out)
	{
		out << (m_MapGoal ? m_MapGoal->GetName() : "");
	}

	void Panzer::RenderDebug()
	{
		if(IsActive())
		{
			m_TargetZone.RenderDebug();
		}
	}

	bool Panzer::GetNextDestination(DestinationVector &_desination, bool &_final, bool &_skiplastpt)
	{
		_final = !m_MapGoal->RouteTo(GetClient(), _desination);
		return true;
	}

	bool Panzer::GetAimPosition(Vector3f &_aimpos)
	{
		_aimpos = m_AimPosition;
		return true;
	}

	void Panzer::OnTarget()
	{
		FINDSTATE(wp,WeaponSystem,GetRootState());

		if (wp)
		{
			if(!wp->CurrentWeaponIs(RTCW_WP_PANZERFAUST))
			{
				wp->AddWeaponRequest(Priority::Medium, GetNameHash(), RTCW_WP_PANZERFAUST);
			}
		}
	}

	obReal Panzer::GetPriority()
	{
		int curAmmo = 0, maxAmmo = 0;
		g_EngineFuncs->GetCurrentAmmo(GetClient()->GetGameEntity(),RTCW_WP_PANZERFAUST,Primary,curAmmo,maxAmmo);
		if(curAmmo <= 0)
			return 0.f;

		if(!InterfaceFuncs::IsWeaponCharged(GetClient(), RTCW_WP_PANZERFAUST, Primary))
			return 0.f;

		if(IsActive())
			return GetLastPriority();

		m_MapGoal.reset();

		GoalManager::Query qry(0x731f6315 /* PANZER */, GetClient());
		GoalManager::GetInstance()->GetGoals(qry);
		qry.GetBest(m_MapGoal);

		return m_MapGoal ? m_MapGoal->GetPriorityForClient(GetClient()) : 0.f;
	}

	void Panzer::Enter()
	{
		m_AimPosition = m_MapGoal->GetPosition() + m_MapGoal->GetFacing() * 1024.f;

		Tracker.InProgress = m_MapGoal;

		m_TargetZone.Restart(256.f);
		m_ExpireTime = 0;
		
		m_MapGoal->GetProperty("Stance",m_Stance);
		m_MapGoal->GetProperty("MinCampTime",m_MinCampTime);
		m_MapGoal->GetProperty("MaxCampTime",m_MaxCampTime);

		FINDSTATEIF(FollowPath, GetRootState(), Goto(this, Run));
	}

	void Panzer::Exit()
	{
		FINDSTATEIF(FollowPath, GetRootState(), Stop(true));

		m_MapGoal.reset();
		FINDSTATEIF(Aimer,GetRootState(),ReleaseAimRequest(GetNameHash()));
		FINDSTATEIF(WeaponSystem,GetRootState(),ReleaseWeaponRequest(GetNameHash()));
		Tracker.Reset();
	}

	State::StateStatus Panzer::Update(float fDt)
	{
		if(!m_MapGoal->IsAvailable(GetClient()->GetTeam()))
			return State_Finished;

		//////////////////////////////////////////////////////////////////////////
		if(DidPathFail())
		{
			// Delay it from being used for a while.
			BlackboardDelay(10.f, m_MapGoal->GetSerialNum());
			return State_Finished;
		}
		//////////////////////////////////////////////////////////////////////////

		if(!Tracker.InUse && m_MapGoal->GetSlotsOpen(MapGoal::TRACK_INUSE) < 1)
			return State_Finished;

		if(DidPathSucceed())
		{
			// Only hang around here for a certain amount of time
			if(m_ExpireTime==0)
			{
				m_ExpireTime = IGame::GetTime()+Mathf::IntervalRandomInt(m_MinCampTime,m_MaxCampTime);
				Tracker.InUse = m_MapGoal;
			}
			else if(IGame::GetTime() > m_ExpireTime)
			{
				// Delay it from being used for a while.
				BlackboardDelay(10.f, m_MapGoal->GetSerialNum());
				return State_Finished;
			}

			FINDSTATEIF(Aimer,GetRootState(),AddAimRequest(Priority::Low,this,GetNameHash()));

			m_AimPosition = m_MapGoal->GetPosition() + m_MapGoal->GetFacing() * 1024.f;

			m_TargetZone.Update(GetClient());

			if(m_TargetZone.HasAim())
				m_AimPosition = m_TargetZone.GetAimPosition();

			GetClient()->GetSteeringSystem()->SetTarget(m_MapGoal->GetPosition(), m_MapGoal->GetRadius());

			NavFlags nodeFlags = m_MapGoal->GetFlags();
			if (nodeFlags & F_NAV_PRONE || m_Stance==StanceProne)
				GetClient()->PressButton(BOT_BUTTON_PRONE);
			else if (nodeFlags & F_NAV_CROUCH || m_Stance==StanceCrouch)
				GetClient()->PressButton(BOT_BUTTON_CROUCH);
		}
		return State_Busy;
	}

	//////////////////////////////////////////////////////////////////////////

	Venom::Venom() 
		: StateChild("Venom")
		, FollowPathUser("Venom")
		, m_MinCampTime(2000)
		, m_MaxCampTime(8000)
		, m_ExpireTime(0)
		, m_Stance(StanceStand)
	{
		LimitToWeapon().SetFlag(RTCW_WP_VENOM);
	}

	void Venom::GetDebugString(StringStr &out)
	{
		out << (m_MapGoal ? m_MapGoal->GetName() : "");
	}

	void Venom::RenderDebug()
	{
		if(IsActive())
		{
			m_TargetZone.RenderDebug();
		}
	}

	bool Venom::GetNextDestination(DestinationVector &_desination, bool &_final, bool &_skiplastpt)
	{
		_final = !m_MapGoal->RouteTo(GetClient(), _desination);
		return true;
	}

	bool Venom::GetAimPosition(Vector3f &_aimpos)
	{
		_aimpos = m_AimPosition;
		return true;
	}

	void Venom::OnTarget()
	{
		FINDSTATE(wp,WeaponSystem,GetRootState());

		if (wp)
		{
			if(!wp->CurrentWeaponIs(RTCW_WP_VENOM))
			{
				wp->AddWeaponRequest(Priority::Medium, GetNameHash(), RTCW_WP_VENOM);
			}
		}
	}

	obReal Venom::GetPriority()
	{
		int curAmmo = 0, maxAmmo = 0;
		g_EngineFuncs->GetCurrentAmmo(GetClient()->GetGameEntity(),RTCW_WP_VENOM,Primary,curAmmo,maxAmmo);
		if(curAmmo <= 0)
			return 0.f;

		if(IsActive())
			return GetLastPriority();

		m_MapGoal.reset();

		GoalManager::Query qry(0x9fa9940a /* VENOM */, GetClient());
		GoalManager::GetInstance()->GetGoals(qry);
		qry.GetBest(m_MapGoal);

		return m_MapGoal ? m_MapGoal->GetPriorityForClient(GetClient()) : 0.f;
	}

	void Venom::Enter()
	{
		m_AimPosition = m_MapGoal->GetPosition() + m_MapGoal->GetFacing() * 1024.f;

		Tracker.InProgress = m_MapGoal;

		m_TargetZone.Restart(256.f);
		m_ExpireTime = 0;

		m_MapGoal->GetProperty("Stance",m_Stance);
		m_MapGoal->GetProperty("MinCampTime",m_MinCampTime);
		m_MapGoal->GetProperty("MaxCampTime",m_MaxCampTime);

		FINDSTATEIF(FollowPath, GetRootState(), Goto(this, Run));
	}

	void Venom::Exit()
	{
		FINDSTATEIF(FollowPath, GetRootState(), Stop(true));

		m_MapGoal.reset();
		FINDSTATEIF(Aimer,GetRootState(),ReleaseAimRequest(GetNameHash()));
		FINDSTATEIF(WeaponSystem,GetRootState(),ReleaseWeaponRequest(GetNameHash()));
		Tracker.Reset();
	}

	State::StateStatus Venom::Update(float fDt)
	{
		if(!m_MapGoal->IsAvailable(GetClient()->GetTeam()))
			return State_Finished;

		//////////////////////////////////////////////////////////////////////////
		if(DidPathFail())
		{
			// Delay it from being used for a while.
			BlackboardDelay(10.f, m_MapGoal->GetSerialNum());
			return State_Finished;
		}
		//////////////////////////////////////////////////////////////////////////

		if(!Tracker.InUse && m_MapGoal->GetSlotsOpen(MapGoal::TRACK_INUSE) < 1)
			return State_Finished;

		if(DidPathSucceed())
		{
			// Only hang around here for a certain amount of time
			if(m_ExpireTime==0)
			{
				m_ExpireTime = IGame::GetTime()+Mathf::IntervalRandomInt(m_MinCampTime,m_MaxCampTime);
				Tracker.InUse = m_MapGoal;
			}
			else if(IGame::GetTime() > m_ExpireTime)
			{
				// Delay it from being used for a while.
				BlackboardDelay(10.f, m_MapGoal->GetSerialNum());
				return State_Finished;
			}

			FINDSTATEIF(Aimer,GetRootState(),AddAimRequest(Priority::Low,this,GetNameHash()));

			m_AimPosition = m_MapGoal->GetPosition() + m_MapGoal->GetFacing() * 1024.f;

			m_TargetZone.Update(GetClient());

			if(m_TargetZone.HasAim())
				m_AimPosition = m_TargetZone.GetAimPosition();

			GetClient()->GetSteeringSystem()->SetTarget(m_MapGoal->GetPosition(), m_MapGoal->GetRadius());

			NavFlags nodeFlags = m_MapGoal->GetFlags();
			if (nodeFlags & F_NAV_PRONE || m_Stance==StanceProne)
				GetClient()->PressButton(BOT_BUTTON_PRONE);
			else if (nodeFlags & F_NAV_CROUCH || m_Stance==StanceCrouch)
				GetClient()->PressButton(BOT_BUTTON_CROUCH);
		}
		return State_Busy;
	}

	//////////////////////////////////////////////////////////////////////////

	Flamethrower::Flamethrower() 
		: StateChild("Flamethrower")
		, FollowPathUser("Flamethrower")
		, m_MinCampTime(2000)
		, m_MaxCampTime(8000)
		, m_ExpireTime(0)
		, m_Stance(StanceStand)
	{
		LimitToWeapon().SetFlag(RTCW_WP_FLAMETHROWER);
	}

	void Flamethrower::GetDebugString(StringStr &out)
	{
		out << (m_MapGoal ? m_MapGoal->GetName() : "");
	}

	void Flamethrower::RenderDebug()
	{
		if(IsActive())
		{
			m_TargetZone.RenderDebug();
		}
	}

	bool Flamethrower::GetNextDestination(DestinationVector &_desination, bool &_final, bool &_skiplastpt)
	{
		_final = !m_MapGoal->RouteTo(GetClient(), _desination);
		return true;
	}

	bool Flamethrower::GetAimPosition(Vector3f &_aimpos)
	{
		_aimpos = m_AimPosition;
		return true;
	}

	void Flamethrower::OnTarget()
	{
		FINDSTATE(wp,WeaponSystem,GetRootState());

		if (wp)
		{
			if(!wp->CurrentWeaponIs(RTCW_WP_FLAMETHROWER))
			{
				wp->AddWeaponRequest(Priority::Medium, GetNameHash(), RTCW_WP_FLAMETHROWER);
			}
		}
	}

	obReal Flamethrower::GetPriority()
	{
		int curAmmo = 0, maxAmmo = 0;
		g_EngineFuncs->GetCurrentAmmo(GetClient()->GetGameEntity(),RTCW_WP_FLAMETHROWER,Primary,curAmmo,maxAmmo);
		if(curAmmo <= 0)
			return 0.f;

		if(IsActive())
			return GetLastPriority();

		m_MapGoal.reset();

		GoalManager::Query qry(0x86584d00 /* FLAME */, GetClient());
		GoalManager::GetInstance()->GetGoals(qry);
		qry.GetBest(m_MapGoal);

		return m_MapGoal ? m_MapGoal->GetPriorityForClient(GetClient()) : 0.f;
	}

	void Flamethrower::Enter()
	{
		m_AimPosition = m_MapGoal->GetPosition() + m_MapGoal->GetFacing() * 1024.f;

		Tracker.InProgress = m_MapGoal;

		m_TargetZone.Restart(256.f);
		m_ExpireTime = 0;

		m_MapGoal->GetProperty("Stance",m_Stance);
		m_MapGoal->GetProperty("MinCampTime",m_MinCampTime);
		m_MapGoal->GetProperty("MaxCampTime",m_MaxCampTime);

		FINDSTATEIF(FollowPath, GetRootState(), Goto(this, Run));
	}

	void Flamethrower::Exit()
	{
		FINDSTATEIF(FollowPath, GetRootState(), Stop(true));

		m_MapGoal.reset();
		FINDSTATEIF(Aimer,GetRootState(),ReleaseAimRequest(GetNameHash()));
		FINDSTATEIF(WeaponSystem,GetRootState(),ReleaseWeaponRequest(GetNameHash()));
		Tracker.Reset();
	}

	State::StateStatus Flamethrower::Update(float fDt)
	{
		if(!m_MapGoal->IsAvailable(GetClient()->GetTeam()))
			return State_Finished;

		//////////////////////////////////////////////////////////////////////////
		if(DidPathFail())
		{
			// Delay it from being used for a while.
			BlackboardDelay(10.f, m_MapGoal->GetSerialNum());
			return State_Finished;
		}
		//////////////////////////////////////////////////////////////////////////

		if(!Tracker.InUse && m_MapGoal->GetSlotsOpen(MapGoal::TRACK_INUSE) < 1)
			return State_Finished;

		if(DidPathSucceed())
		{
			// Only hang around here for a certain amount of time
			if(m_ExpireTime==0)
			{
				m_ExpireTime = IGame::GetTime()+Mathf::IntervalRandomInt(m_MinCampTime,m_MaxCampTime);
				Tracker.InUse = m_MapGoal;
			}
			else if(IGame::GetTime() > m_ExpireTime)
			{
				// Delay it from being used for a while.
				BlackboardDelay(10.f, m_MapGoal->GetSerialNum());
				return State_Finished;
			}

			FINDSTATEIF(Aimer,GetRootState(),AddAimRequest(Priority::Low,this,GetNameHash()));

			m_AimPosition = m_MapGoal->GetPosition() + m_MapGoal->GetFacing() * 1024.f;

			m_TargetZone.Update(GetClient());

			if(m_TargetZone.HasAim())
				m_AimPosition = m_TargetZone.GetAimPosition();

			GetClient()->GetSteeringSystem()->SetTarget(m_MapGoal->GetPosition(), m_MapGoal->GetRadius());

			NavFlags nodeFlags = m_MapGoal->GetFlags();
			if (nodeFlags & F_NAV_PRONE || m_Stance==StanceProne)
				GetClient()->PressButton(BOT_BUTTON_PRONE);
			else if (nodeFlags & F_NAV_CROUCH || m_Stance==StanceCrouch)
				GetClient()->PressButton(BOT_BUTTON_CROUCH);
		}
		return State_Busy;
	}
};
