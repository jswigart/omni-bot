////////////////////////////////////////////////////////////////////////////////
//
// $LastChangedBy$
// $LastChangedDate$
// $LastChangedRevision$
//
////////////////////////////////////////////////////////////////////////////////

#pragma once

#ifndef __RTCW_CLIENT_H__
#define __RTCW_CLIENT_H__

#include "Client.h"
#include "RTCW_Config.h"

// class: RTCW_Client
//		Extended client class for Return to Castle Wolfenstein.
class RTCW_Client : public Client
{
public:
	friend class RTCW_Game;

	void Init( int _gameid );

	NavFlags GetTeamFlag( int _team ) const;
	void GetNavFlags( NavFlags & includeFlags, NavFlags & excludeFlags );

	void SendVoiceMacro( int _macroId );

	void ProcessGotoNode( const PathInterface::PathCorner corners[ 2 ], const size_t numEdges );
	void ProcessGotoNode( const Path &_path );

	float GetGameVar( GameVar _var ) const;

	bool DoesBotHaveFlag( MapGoalPtr _mapgoal );
	bool IsFlagGrabbable( MapGoalPtr _mapgoal );

	bool CanBotSnipe();
	bool GetSniperWeapon( int &nonscoped, int &scoped );

	float GetBreakableTargetDist() const
	{
		return mBreakableTargetDistance;
	}
	float GetHealthEntityDist() const
	{
		return mHealthEntityDistance;
	}
	float GetAmmoEntityDist() const
	{
		return mAmmoEntityDistance;
	}
	float GetWeaponEntityDist() const
	{
		return mWeaponEntityDistance;
	}
	float GetProjectileEntityDist() const
	{
		return mProjectileEntityDistance;
	}

	void SetupBehaviorTree();

	RTCW_Client();
	virtual ~RTCW_Client();
protected:
	void ProcessEvent( const Message & message, CallbackParameters & cb );
	int HandleVoiceMacroEvent( const EvVoiceMacro* msg );

	float	 mBreakableTargetDistance;
	float	 mHealthEntityDistance;
	float	 mAmmoEntityDistance;
	float	 mWeaponEntityDistance;
	float	 mProjectileEntityDistance;
	int	 mStrafeJump;
};

#endif
