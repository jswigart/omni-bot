////////////////////////////////////////////////////////////////////////////////
// 
// $LastChangedBy: drevil $
// $LastChangedDate: 2008-07-22 22:13:08 -0700 (Tue, 22 Jul 2008) $
// $LastChangedRevision: 3109 $
//
////////////////////////////////////////////////////////////////////////////////

#ifndef __ET_CLIENT_H__
#define __ET_CLIENT_H__

#include "Client.h"
#include "ET_Config.h"

// class: ET_Client
//		Extended client class for Enemy-Territory.
class ET_Client : public Client
{
public:	
	friend void gmBindETBotLibrary(gmMachine *_machine);

	void Init(int _gameid);

	NavFlags GetTeamFlag();
	NavFlags GetTeamFlag(int _team);

	void SendVoiceMacro(int _macroId);

	void ProcessGotoNode(const Path &_path);

	float GetGameVar(GameVar _var) const;
	float GetAvoidRadius(int _class) const;

	bool DoesBotHaveFlag(MapGoalPtr _mapgoal);
	bool IsFlagGrabbable(MapGoalPtr _mapgoal);

	bool CanBotSnipe();
	bool GetSniperWeapon(int &nonscoped, int &scoped);

	bool GetSkills(gmMachine *machine, gmTableObject *tbl);

	float GetBreakableTargetDist() const { return m_BreakableTargetDistance; }

	float NavCallback(const NavFlags &_flag, Waypoint *from, Waypoint *to) ;

	void SetupBehaviorTree();

	ET_Client();
	virtual ~ET_Client();
protected:
	void ProcessEvent(const MessageHelper &_message, CallbackParameters &_cb);
	int HandleVoiceMacroEvent(const MessageHelper &_message);

	float		m_BreakableTargetDistance;
};

#endif
