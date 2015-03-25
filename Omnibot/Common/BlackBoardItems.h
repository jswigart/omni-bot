////////////////////////////////////////////////////////////////////////////////
//
// $LastChangedBy$
// $LastChangedDate$
// $LastChangedRevision$
//
////////////////////////////////////////////////////////////////////////////////

#pragma once

#ifndef __BLACKBOARDITEMS_H__
#define __BLACKBOARDITEMS_H__

#include "common.h"
#include "Omni-Bot_Events.h"
#include "gmGCRoot.h"

class gmMachine;
class gmTableObject;

//////////////////////////////////////////////////////////////////////////

class bbItem
{
public:
	int		mOwner;
	int		mTarget;
	int		mExpireTime;
	bool	mDeleteOnExpire : 1;
	bool	mDeleteOnRefCount1 : 1;

	virtual bool FromScriptTable(gmMachine *_machine, gmTableObject *_from);
	virtual bool ToScriptTable(gmMachine *_machine, gmTableObject *&_to);

	inline int GetType() const { return mType; }
	bbItem(int _type);
	virtual ~bbItem() {}
private:
	int		mType;
	bool	mInBB;
};

//////////////////////////////////////////////////////////////////////////

class bbDelayGoal : public bbItem
{
public:
	bbDelayGoal() : bbItem(bbk_DelayGoal) {}
};

//////////////////////////////////////////////////////////////////////////

class bbIsTaken : public bbItem
{
public:
	bbIsTaken() : bbItem(bbk_IsTaken) {}
};

//////////////////////////////////////////////////////////////////////////

class bbRunAway : public bbItem
{
public:

	Vector3f	mPosition;
	float		mRadius;

	bbRunAway() : bbItem(bbk_RunAway), mPosition(0.f,0.f,0.f), mRadius(0.f) {}
};

//////////////////////////////////////////////////////////////////////////

class bbScriptItem : public bbItem
{
public:

	virtual bool FromScriptTable(gmMachine *_machine, gmTableObject *_from);
	virtual bool ToScriptTable(gmMachine *_machine, gmTableObject *&_to);

	bbScriptItem(int _type) : bbItem(_type) {}

	gmGCRoot<gmTableObject> mDataTable;
};

#endif
