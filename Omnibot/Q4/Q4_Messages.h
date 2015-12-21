////////////////////////////////////////////////////////////////////////////////
//
// $LastChangedBy$
// $LastChangedDate$
// $LastChangedRevision$
//
// Title: Q4 Message Structure Definitions
//
////////////////////////////////////////////////////////////////////////////////

#pragma once

#ifndef __Q4_MESSAGES_H__
#define __Q4_MESSAGES_H__

#include "Base_Messages.h"
#include "Q4_Config.h"

#pragma pack(push)
#pragma pack(4)

//////////////////////////////////////////////////////////////////////////

struct Q4_Location
{
	float mPosition[ 3 ];
	char mLocationName[ 64 ];
};

struct Q4_PlayerCash
{
	float mCash;
};

struct Q4_IsBuyingAllowed
{
	bool mBuyingAllowed;
};

struct Q4_ItemBuy
{
	int	 mItem;
	bool mSuccess;
};

struct Q4_CanPickUp
{
	GameEntity mEntity;
	bool	 mCanPickUp;
};
//////////////////////////////////////////////////////////////////////////

#pragma pack(pop)

#endif
