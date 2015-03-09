////////////////////////////////////////////////////////////////////////////////
//
// $LastChangedBy$
// $LastChangedDate$
// $LastChangedRevision$
//
////////////////////////////////////////////////////////////////////////////////

#pragma once

#ifndef __TF_GAME_H__
#define __TF_GAME_H__

class gmMachine;
class gmTableObject;

#include "IGame.h"
#include "TF_Config.h"

// class: TF_Game
//		Game Type for Fortress Forever
class TF_Game : public IGame
{
public:
	bool Init( System & system );
	void Shutdown();
		
	const char *FindClassName(obint32 _classId);

	void GetWeaponEnumeration(const IntEnum *&_ptr, int &num);

	void ProcessEvent(const MessageHelper &_message, CallbackParameters &_cb);

	static obReal _GetDesirabilityFromTargetClass(int _grentype, int _class);
	
	TF_Game() {};
	virtual ~TF_Game() {};
protected:
	// Script support.
	void InitScriptBinds(gmMachine *_machine);
	void InitScriptCategories(gmMachine *_machine, gmTableObject *_table);
	void InitScriptClasses(gmMachine *_machine, gmTableObject *_table);
	void InitScriptEvents(gmMachine *_machine, gmTableObject *_table);
	void InitScriptEntityFlags(gmMachine *_machine, gmTableObject *_table);
	void InitScriptPowerups(gmMachine *_machine, gmTableObject *_table);
	void InitScriptBotButtons(gmMachine *_machine, gmTableObject *_table);

	static const float TF_GetEntityClassTraceOffset(const int _class, const BitFlag64 &_entflags);
	static const float TF_GetEntityClassAimOffset(const int _class, const BitFlag64 &_entflags);
private:
};

#endif
