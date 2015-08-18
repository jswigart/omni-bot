#include "GOAP.h"

class GOAP_MoveTo : public GOAPAction
{
	virtual const Fact* Preconditions() const
	{

	}
	virtual const size_t NumPreconditions() const
	{

	}

	virtual const Fact* Postconditions() const
	{

	}
	virtual const size_t NumPostconditions() const
	{

	}

	virtual float CalculateCost()
	{
		return 0.0f;
	}
};
