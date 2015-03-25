////////////////////////////////////////////////////////////////////////////////
//
// $LastChangedBy$
// $LastChangedDate$
// $LastChangedRevision$
//
////////////////////////////////////////////////////////////////////////////////

#pragma once

#ifndef __CRITERIA_H__
#define __CRITERIA_H__

#include "common.h"

//////////////////////////////////////////////////////////////////////////

#define HANDLE_OPERATORS(VAR,OPERAND0,OPERAND1) \
	switch(crit.mOperator) \
{ \
	case OP_NONE: \
{ \
	break; \
} \
	case OP_LESSTHAN: \
{ \
	VAR = OPERAND0 < OPERAND1; \
	break; \
} \
	case OP_GREATERTHAN: \
{ \
	VAR = OPERAND0 > OPERAND1; \
	break; \
} \
	case OP_EQUALS: \
{ \
	VAR = OPERAND0 == OPERAND1; \
	break; \
} \
}

//////////////////////////////////////////////////////////////////////////

namespace Criteria
{
	enum eCriteria
	{
		NONE,
		ON_ENTITY_DELETED,
		ON_ENTITY_FLAG,
		ON_ENTITY_HEALTH,
		ON_ENTITY_WEAPON,
		ON_ENTITY_VELOCITY,
		ON_ENTITY_WEAPONCHARGED,
		ON_ENTITY_WEAPONHASAMMO,
		ON_MAPGOAL_AVAILABLE,

		FIRST_USER_CRITERIA,
	};

	enum Operator
	{
		OP_NONE,
		OP_LESSTHAN,
		OP_GREATERTHAN,
		OP_EQUALS,
	};
};

class CheckCriteria;
typedef bool( *pfnCheckCriteria )( CheckCriteria &crit, Client * bot );

class CheckCriteria
{
public:
	enum Token
	{
		TOK_CRITERIA,
		TOK_OPERATOR,
		TOK_OPERAND,
		TOK_DONE,
	};

	enum
	{
		MaxOperands = 2
	};

	Criteria::eCriteria	 mCriteria;
	obUserData			 mSubject;
	obUserData			 mOperand[ MaxOperands ];
	Criteria::Operator	 mOperator;

	bool ParseOperator( uint32_t sHash );

	void Clear()
	{
		mCriteria = Criteria::NONE;
	}

	bool Check( Client * bot );

	bool	 mPersistent : 1;
	bool	 mNegated : 1;

	static pfnCheckCriteria mpfnCheckCriteria;

	CheckCriteria()
		: mCriteria( Criteria::NONE )
		, mOperator( Criteria::OP_NONE )
		, mPersistent( false )
		, mNegated( false )
	{
	}
};

bool Base_CheckCriteria( CheckCriteria &crit, Client * bot );

#endif
