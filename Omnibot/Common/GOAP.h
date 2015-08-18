
enum OpCode
{
	OP_EQUALS,
	OP_NEQUALS,
	OP_LT,
	OP_LTE,
	OP_GT,
	OP_GTE,
	OP_WITHIN,
};

enum FactType
{
	HEALTH,
	POSITION,
	HAS_WEAPON,
	HAS_CLASS,
	HAS_TEAM,
};

struct Fact
{
	FactType	mType;
	OpCode		mOpcode;
	int			mValue1;
	int			mValue2;
};

class Goal
{
public:
	virtual ~Goal();

	//float	GetPriority() const;
};

class GOAPAction
{
	virtual const Fact* Preconditions() const = 0;
	virtual const size_t NumPreconditions() const = 0;

	virtual const Fact* Postconditions() const = 0;
	virtual const size_t NumPostconditions() const = 0;

	virtual float CalculateCost() const = 0;
protected:
	virtual ~GOAPAction() {}
};

class GOAPPlanner
{
public:

};

void Test()
{

}
