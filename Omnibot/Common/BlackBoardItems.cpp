////////////////////////////////////////////////////////////////////////////////
//
// $LastChangedBy$
// $LastChangedDate$
// $LastChangedRevision$
//
////////////////////////////////////////////////////////////////////////////////

#include "BlackBoardItems.h"
#include "IGame.h"

//////////////////////////////////////////////////////////////////////////

bbItem::bbItem(int _type)
	: mOwner(0)
	, mTarget(0)
	, mExpireTime(std::numeric_limits<int>::max())
	, mDeleteOnExpire(false)
	, mDeleteOnRefCount1(false)
	, mType(_type)
	, mInBB(false)
{
}

bool bbItem::FromScriptTable(gmMachine *_machine, gmTableObject *_from)
{
	gmVariable vOwner = _from->Get(_machine, "Owner");
	gmVariable vTarget = _from->Get(_machine, "Target");
	gmVariable vDuration = _from->Get(_machine, "Duration");
	gmVariable vDeleteOnExpire = _from->Get(_machine, "DeleteOnExpire");

	bool bDeleteOnExpire = 0;
	float fDuration = 0.f;

	vDuration.GetFloatSafe(fDuration, 0.f);
	vDuration.GetBoolSafe(bDeleteOnExpire, false);

	if(vOwner.IsInt() && vTarget.IsInt())
	{
		mOwner = vOwner.GetInt();
		mTarget = vTarget.GetInt();
		mExpireTime = IGame::GetTime() + Utils::SecondsToMilliseconds(fDuration);
		mDeleteOnExpire = vDeleteOnExpire.GetInt()!=0;
		return true;
	}
	return false;
}

bool bbItem::ToScriptTable(gmMachine *_machine, gmTableObject *&_to)
{
	_to->Set(_machine, "Owner", gmVariable(mOwner));
	_to->Set(_machine, "Target", gmVariable(mTarget));
	_to->Set(_machine, "ExpireTime", gmVariable(mExpireTime));
	_to->Set(_machine, "DeleteOnExpire", gmVariable(mDeleteOnExpire ? 1 : 0));
	return true;
}

//////////////////////////////////////////////////////////////////////////

//class bbRunAway : public bbItem
//{
//public:
//	bbRunAway() : bbItem(bbk_RunAway) {}
//};

//////////////////////////////////////////////////////////////////////////

bool bbScriptItem::FromScriptTable(gmMachine *_machine, gmTableObject *_from)
{
	mDataTable.Set(_from, _machine);
	return bbItem::FromScriptTable(_machine, _from);
}

bool bbScriptItem::ToScriptTable(gmMachine *_machine, gmTableObject *&_to)
{
	_to = mDataTable;
	return true;
}