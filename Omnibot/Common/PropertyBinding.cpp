////////////////////////////////////////////////////////////////////////////////
//
// $LastChangedBy$
// $LastChangedDate$
// $LastChangedRevision$
//
////////////////////////////////////////////////////////////////////////////////

#include "PropertyBinding.h"

#include <type_traits>

//////////////////////////////////////////////////////////////////////////

enum PropType
{
	PropBool,
	PropCString,
	PropString,
	PropVector,
	PropInt,
	PropIntBitflag32,
	PropFloat,
	PropEntity,
	PropAABB,
	PropMat,
	PropFunc,
	PropNum,
};

static const char *dbgNames [] =
{
	"boolean",
	"std::string",
	"std::string",
	"vector",
	"int",
	"bitflag",
	"float",
	"entity",
	"aabb",
	"matrix",
	"func",
};
static_assert( sizeof( dbgNames ) / sizeof( dbgNames[ 0 ] ) == PropNum, "Mismatched Property Names" );
//////////////////////////////////////////////////////////////////////////
class Property
{
public:
	virtual PropType GetPropertyType() const = 0;
	std::string GetDebugTypeName()
	{
		return dbgNames[ GetPropertyType() ];
	}

	std::string GetName() const
	{
		return mName;
	}
	bool Check( uint32_t _f ) const
	{
		return ( mFlags&_f ) != 0;
	}

	virtual bool FromString( const std::string &_str ) = 0;
	virtual bool FromGMVar( gmMachine *_m, const gmVariable &_str ) = 0;

	//void SetBinding(PropertyBinding *_bind) { mBinding = _bind; }
	//PropertyBinding *GetBinding() const { return mBinding; }
	Property( const std::string &_name, uint32_t _flags )
		: mFlags( _flags )
		, mName( _name )
	{
	}
	virtual ~Property()
	{
	}

protected:
	uint32_t								mFlags;
private:
	std::string								mName;
	//PropertyBinding						*mBinding;
public:
};

//////////////////////////////////////////////////////////////////////////
class PropertyBool : public Property
{
public:
	virtual PropType GetPropertyType() const
	{
		return PropBool;
	}
	bool GetValue() const
	{
		return mValue;
	}
	bool FromString( const std::string &_str )
	{
		if ( Utils::StringToFalse( _str ) )
			mValue = false;
		else if ( Utils::StringToTrue( _str ) )
			mValue = true;
		else
			return false;
		return true;
	}
	bool FromGMVar( gmMachine *_m, const gmVariable &_v )
	{
		if ( _v.IsInt() )
		{
			mValue = _v.GetInt() != 0;
			return true;
		}
		return false;
	}
#ifdef ENABLE_DEBUG_WINDOW
	void action( const gcn::ActionEvent& actionEvent )
	{
		if ( actionEvent.getSource() == mCheckBox )
		{
			mValue = mCheckBox->isSelected();
		}
	}
	void AddToGui( gcn::contrib::PropertySheet *_propsheet )
	{
		mCheckBox = new gcn::CheckBox;
		if ( Check( Prop::PF_READONLY ) )
			mCheckBox->setEnabled( false );
		_propsheet->addProperty( GetName(), mCheckBox );
	}
	void DeAllocGui()
	{
		OB_DELETE( mCheckBox );
	}
	void UpdateToGui()
	{
		if ( mCheckBox )
		{
			mCheckBox->setCaption( mValue ? "true" : "false" );
			mCheckBox->setSelected( mValue );
			mCheckBox->adjustSize();
		}
	}
#endif
	PropertyBool( const std::string &_name, bool &v, uint32_t _flags = 0 )
		: Property( _name, _flags )
		, mValue( v )
#ifdef ENABLE_DEBUG_WINDOW
		, mCheckBox( 0 )
#endif
	{
	}
	~PropertyBool()
	{
	}
private:
	bool &			mValue;
#ifdef ENABLE_DEBUG_WINDOW
	gcn::CheckBox	*mCheckBox;
#endif
};
//////////////////////////////////////////////////////////////////////////
class PropertyCString : public Property
{
public:
	virtual PropType GetPropertyType() const
	{
		return PropCString;
	}

	const char *GetValue() const
	{
		return mValue;
	}

	bool FromString( const std::string &_str )
	{
		return false;
	}
	bool FromGMVar( gmMachine *_m, const gmVariable &_v )
	{
		return false;
	}
#ifdef ENABLE_DEBUG_WINDOW
	void action( const gcn::ActionEvent& actionEvent )
	{
	}
	void AddToGui( gcn::contrib::PropertySheet *_propsheet )
	{
		mTextField = new gcn::TextField;
		mTextField->setEnabled( false );
		mTextField->setReadOnly( true );
		_propsheet->addProperty( GetName(), mTextField );
	}
	void DeAllocGui()
	{
		OB_DELETE( mTextField );
	}
	void UpdateToGui()
	{
		if ( mTextField )
		{
			mTextField->setText( mValue ? mValue : "" );
			mTextField->adjustSize();
		}
	}
#endif
	PropertyCString( const std::string &_name, const char *&v, uint32_t _flags = 0 )
		: Property( _name, _flags )
		, mValue( v )
#ifdef ENABLE_DEBUG_WINDOW
		, mTextField( 0 )
#endif
	{
	}
	~PropertyCString()
	{
	}
private:
	const char *&	mValue;
#ifdef ENABLE_DEBUG_WINDOW
	gcn::TextField *mTextField;
#endif
};
//////////////////////////////////////////////////////////////////////////
class PropertyString : public Property
{
public:
	virtual PropType GetPropertyType() const
	{
		return PropString;
	}

	std::string GetValue() const
	{
		return mValue;
	}

	bool FromString( const std::string &_str )
	{
		mValue = _str;
		return true;
	}
	bool FromGMVar( gmMachine *_m, const gmVariable &_v )
	{
		const char *s = _v.GetCStringSafe( 0 );
		if ( s )
		{
			mValue = s;
			return true;
		}
		return false;
	}
#ifdef ENABLE_DEBUG_WINDOW
	void action( const gcn::ActionEvent& actionEvent )
	{
		if ( actionEvent.getSource() == mTextField )
		{
			mValue = mTextField->getText();
		}
	}
	void AddToGui( gcn::contrib::PropertySheet *_propsheet )
	{
		mTextField = new gcn::TextField;
		if ( Check( Prop::PF_READONLY ) )
			mTextField->setEnabled( false );
		_propsheet->addProperty( GetName(), mTextField );
	}
	void DeAllocGui()
	{
		OB_DELETE( mTextField );
	}
	void UpdateToGui()
	{
		if ( mTextField )
		{
			mTextField->setText( mValue );
			mTextField->adjustSize();
		}
	}
#endif
	PropertyString( const std::string &_name, std::string &v, uint32_t _flags = 0 )
		: Property( _name, _flags )
		, mValue( v )
#ifdef ENABLE_DEBUG_WINDOW
		, mTextField( 0 )
#endif
	{
	}
	~PropertyString()
	{
	}
private:
	std::string &		mValue;
#ifdef ENABLE_DEBUG_WINDOW
	gcn::TextField *mTextField;
#endif
};
//////////////////////////////////////////////////////////////////////////
class PropertyVector : public Property
{
public:
	virtual PropType GetPropertyType() const
	{
		return PropVector;
	}
	Vector3f GetValue() const
	{
		return mValue;
	}
	bool FromString( const std::string &_str )
	{
		Vector3f v;
		if ( Utils::ConvertString( _str, v ) )
		{
			mValue = v;
			return true;
		}
		return false;
	}
	bool FromGMVar( gmMachine *_m, const gmVariable &_v )
	{
		if ( _v.IsVector() )
		{
			_v.GetVector( mValue );
			return true;
		}
		return false;
	}
#ifdef ENABLE_DEBUG_WINDOW
	void action( const gcn::ActionEvent &evt )
	{
		if ( evt.getSource() == mBtnSetPos )
		{
			Utils::GetLocalFacing( mValue );
		}
		else if ( evt.getSource() == mBtnSetFacing )
		{
			Utils::GetLocalPosition( mValue );
		}
		else if ( evt.getSource() == mBtnSetAimPos )
		{
			Utils::GetLocalAimPoint( mValue );
		}
	}
	void AddToGui( gcn::contrib::PropertySheet *_propsheet )
	{
		mContainer = new gcn::contrib::AdjustingContainer;

		mTextField = new gcn::TextField;
		if ( Check( Prop::PF_READONLY ) )
			mTextField->setEnabled( false );
		mContainer->add( mTextField );

		mBtnSetPos = new gcn::Button( "Set Position" );
		mBtnSetPos->addActionListener( this );
		mContainer->add( mBtnSetPos );

		mBtnSetFacing = new gcn::Button( "Set Facing" );
		mBtnSetFacing->addActionListener( this );
		mContainer->add( mBtnSetFacing );

		mBtnSetAimPos = new gcn::Button( "Set Aim Position" );
		mBtnSetAimPos->addActionListener( this );
		mContainer->add( mBtnSetAimPos );

		_propsheet->addProperty( GetName(), mContainer );
	}
	void UpdateToGui()
	{
		if ( mTextField )
		{
			mTextField->setText( Utils::FormatVectorString( mValue ) );
			mTextField->adjustSize();
		}
	}
#endif
	PropertyVector( const std::string &_name, Vector3f &v, uint32_t _flags = 0 )
		: Property( _name, _flags )
		, mValue( v )
#ifdef ENABLE_DEBUG_WINDOW
		, mContainer( 0 )
		, mTextField( 0 )
		, mBtnSetPos( 0 )
		, mBtnSetFacing( 0 )
		, mBtnSetAimPos( 0 )
#endif
	{
	}
	~PropertyVector()
	{
	}
private:
	Vector3f &							mValue;
#ifdef ENABLE_DEBUG_WINDOW
	gcn::contrib::AdjustingContainer	*mContainer;
	gcn::TextField						*mTextField;
	gcn::Button							*mBtnSetPos;
	gcn::Button							*mBtnSetFacing;
	gcn::Button							*mBtnSetAimPos;
#endif
};
//////////////////////////////////////////////////////////////////////////
class PropertyMatrix : public Property
{
public:
	virtual PropType GetPropertyType() const
	{
		return PropMat;
	}
	Matrix3f GetValue() const
	{
		return mValue;
	}
	bool FromString( const std::string &_str )
	{
		/*Matrix3f v;
		if(Utils::ConvertString(_str,v))
		{
		mValue = v;
		return true;
		}*/
		return false;
	}
	bool FromGMVar( gmMachine *_m, const gmVariable &_v )
	{
		/*if(_v.IsVector())
		{
		_v.GetVector(mValue);
		return true;
		}	*/
		return false;
	}
#ifdef ENABLE_DEBUG_WINDOW
	void action( const gcn::ActionEvent& actionEvent )
	{
	}
	void AddToGui( gcn::contrib::PropertySheet *_propsheet )
	{
		mTextField = new gcn::TextField;
		if ( Check( Prop::PF_READONLY ) )
			mTextField->setEnabled( false );
		_propsheet->addProperty( GetName(), mTextField );
	}
	void DeAllocGui()
	{
		OB_DELETE( mButton1 );
		OB_DELETE( mButton2 );
		OB_DELETE( mButton3 );
		OB_DELETE( mTextField );
	}
	void UpdateToGui()
	{
		if ( mTextField )
		{
			mTextField->setText( Utils::FormatMatrixString( mValue ) );
			mTextField->adjustSize();
		}
	}
#endif
	PropertyMatrix( const std::string &_name, Matrix3f &v, uint32_t _flags = 0 )
		: Property( _name, _flags )
		, mValue( v )
#ifdef ENABLE_DEBUG_WINDOW
		, mTextField( 0 )
		, mButton1( 0 )
		, mButton2( 0 )
		, mButton3( 0 )
#endif
	{
	}
	~PropertyMatrix()
	{
	}
private:
	Matrix3f		&mValue;
#ifdef ENABLE_DEBUG_WINDOW
	gcn::TextField	*mTextField;
	gcn::Button		*mButton1;
	gcn::Button		*mButton2;
	gcn::Button		*mButton3;
#endif
};
//////////////////////////////////////////////////////////////////////////
#ifdef ENABLE_DEBUG_WINDOW
class PropertyInt : public Property, public gcn::ListModel
#else
class PropertyInt : public Property
#endif
{
public:
	virtual PropType GetPropertyType() const
	{
		return PropInt;
	}
	int GetValue() const
	{
		return mValue;
	}
	bool FromString( const std::string &_str )
	{
		int v;
		if ( Utils::ConvertString( _str, v ) )
		{
			if ( Check( Prop::PF_MS_TO_SECONDS ) )
				mValue = v * 1000;
			else
				mValue = v;
			return true;
		}
		else if ( mEnum && mEnumNum )
		{
			//const uint32_t uiKey = Utils::Hash32(_str);
			for ( int i = 0; i < mEnumNum; ++i )
			{
				if ( !Utils::StringCompareNoCase( mEnum[ i ].mKey, _str.c_str() ) )
				{
					mValue = mEnum[ i ].mValue;
					return true;
				}
			}
		}
		return false;
	}
	bool FromGMVar( gmMachine *_m, const gmVariable &_v )
	{
		if ( _v.IsInt() )
		{
			if ( Check( Prop::PF_MS_TO_SECONDS ) )
				mValue = _v.GetInt() * 1000;
			else
				mValue = _v.GetInt();
			return true;
		}
		else if ( mEnum && mEnumNum )
		{
			const char *key = _v.GetCStringSafe( 0 );
			if ( key )
			{
				//const uint32_t uiKey = Utils::Hash32(key);
				for ( int i = 0; i < mEnumNum; ++i )
				{
					if ( !Utils::StringCompareNoCase( mEnum[ i ].mKey, key ) )
					{
						mValue = mEnum[ i ].mValue;
						return true;
					}
				}
			}
		}
		return false;
	}
#ifdef ENABLE_DEBUG_WINDOW
	void action( const gcn::ActionEvent& actionEvent )
	{
		if ( actionEvent.getSource() == mDropDown )
		{
			mValue = mDropDown->getSelected();
		}
		if ( actionEvent.getSource() == mTextField )
		{
			float val = (float)atof( mTextField->getText().c_str() );
			if ( Check( Prop::PF_MS_TO_SECONDS ) )
				val *= 1000.f;

			mValue = (int)val;
		}
	}
	void AddToGui( gcn::contrib::PropertySheet *_propsheet )
	{
		if ( !mTextField )
		{
			if ( mEnum )
			{
				mDropDown = new gcn::DropDown( this );
				mDropDown->setSelected( mValue );
				mDropDown->adjustHeight();
				if ( Check( Prop::PF_READONLY ) )
					mDropDown->setEnabled( false );

				// resize to fit the biggest value.
				int width = mDropDown->getWidth();
				for ( int i = 0; i < mEnumNum; ++i )
				{
					int w = mDropDown->getFont()->getWidth( mEnum[ i ].mKey );
					if ( w > width )
						width = w;
				}
				mDropDown->setWidth( width );
				mDropDown->setSelected( mValue );
				_propsheet->addProperty( GetName(), mDropDown );
			}
			else
			{
				mTextField = new gcn::TextField();
				if ( Check( Prop::PF_READONLY ) )
					mTextField->setEnabled( false );
				_propsheet->addProperty( GetName(), mTextField );
			}
		}
	}
	void UpdateToGui()
	{
		if ( mEnum )
		{
			mValue = mDropDown->getSelected();
		}
		else if ( mTextField )
		{
			if ( Check( Prop::PF_MS_TO_SECONDS ) )
				mTextField->setText( ( std::string )va( "%g", (float)mValue / 1000.f ) );
			else
				mTextField->setText( ( std::string )va( "%d", mValue ) );
			//mTextField->adjustSize();
		}
	}
#endif
	int getNumberOfElements()
	{
		return mEnumNum;
	}
	std::string getElementAt( int i, int column )
	{
		return mEnum[ i%mEnumNum ].mKey;
	}
	PropertyInt( const std::string &_name, int &v, uint32_t _flags = 0, const IntEnum *_enum = 0, int _numenum = 0 )
		: Property( _name, _flags )
		, mValue( v )
		, mEnum( _enum )
		, mEnumNum( _numenum )
#ifdef ENABLE_DEBUG_WINDOW
		, mTextField( 0 )
		, mDropDown( 0 )
#endif
	{
	}
	~PropertyInt()
	{
	}
private:
	int								&mValue;
	const IntEnum	*mEnum;
	int								mEnumNum;
#ifdef ENABLE_DEBUG_WINDOW
	gcn::TextField	*mTextField;
	gcn::DropDown	*mDropDown;
#endif
};
//////////////////////////////////////////////////////////////////////////
#ifdef ENABLE_DEBUG_WINDOW
class PropertyBitflag32 : public Property, public gcn::ListModel
#else
class PropertyBitflag32 : public Property
#endif
{
public:
	virtual PropType GetPropertyType() const
	{
		return PropIntBitflag32;
	}
	BitFlag32 GetValue() const
	{
		return mValue;
	}
	bool FromString( const std::string &_str )
	{
		if ( mEnum && mEnumNum )
		{
			StringVector sv;
			Utils::Tokenize( _str, "|,:", sv );

			// for each std::string in the token
			BitFlag32 bf;
			for ( uint32_t s = 0; s < sv.size(); ++s )
			{
				bool bRecognized = false;

				for ( int e = 0; e < mEnumNum; ++e )
				{
					if ( mEnum[ e ].mKey )
					{
						if ( !Utils::StringCompareNoCase( mEnum[ e ].mKey, sv[ s ].c_str() ) )
						{
							bRecognized = true;
							bf.SetFlag( mEnum[ e ].mValue, true );
							break;
						}
					}
				}

				if ( !bRecognized )
				{
					return false;
				}
			}
			mValue = bf;
		}
		return false;
	}
	bool FromGMVar( gmMachine *_m, const gmVariable &_v )
	{
		if ( _v.IsInt() )
		{
			mValue = BitFlag32( _v.GetInt() );
			return true;
		}
		else if ( mEnum && mEnumNum )
		{
			const char *key = _v.GetCStringSafe( 0 );
			if ( key )
			{
				StringVector sv;
				Utils::Tokenize( key, "|,:", sv );

				// for each std::string in the token
				BitFlag32 bf;
				for ( uint32_t s = 0; s < sv.size(); ++s )
				{
					bool bRecognized = false;

					for ( int e = 0; e < mEnumNum; ++e )
					{
						if ( mEnum[ e ].mKey )
						{
							if ( !Utils::StringCompareNoCase( mEnum[ e ].mKey, sv[ s ].c_str() ) )
							{
								bRecognized = true;
								bf.SetFlag( mEnum[ e ].mValue, true );
								break;
							}
						}
					}

					if ( !bRecognized )
					{
						return false;
					}
				}

				mValue = bf;
				return true;
			}
		}
		return false;
	}
#ifdef ENABLE_DEBUG_WINDOW
	void action( const gcn::ActionEvent& actionEvent )
	{
		gcn::CheckBox *cb = static_cast<gcn::CheckBox*>( .mContainer->findWidgetById( actionEvent.getId() ) );
		const int bit = atoi( actionEvent.getId().c_str() );
		mValue.SetFlag( bit, cb->isSelected() );
	}
	void AddToGui( gcn::contrib::PropertySheet *_propsheet )
	{
		mContainer = new gcn::contrib::AdjustingContainer;
		mContainer->setNumberOfColumns( 2 );
		for ( int i = 0; i < mEnumNum; ++i )
		{
			gcn::CheckBox *cb = new gcn::CheckBox( .mEnum[ i ].mKey, .mValue.CheckFlag( i ) );
			cb->setId( ( std::string )va( "%d", i ) );
			mContainer->add( cb );
		}
		_propsheet->addProperty( GetName(), .mContainer );
	}
	void UpdateToGui()
	{
	}
#endif
	int getNumberOfElements()
	{
		return mEnumNum;
	}
	std::string getElementAt( int i, int column )
	{
		return mEnum[ i%mEnumNum ].mKey;
	}
	PropertyBitflag32( const std::string &_name, BitFlag32 &v, uint32_t _flags = 0, const IntEnum *_enum = 0, int _numenum = 0 )
		: Property( _name, _flags )
		, mValue( v )
		, mEnum( _enum )
		, mEnumNum( _numenum )
#ifdef ENABLE_DEBUG_WINDOW
		, mContainer( 0 )
#endif
	{
	}
	~PropertyBitflag32()
	{
	}
private:
	BitFlag32						& mValue;
	const IntEnum	* mEnum;
	int								 mEnumNum;
#ifdef ENABLE_DEBUG_WINDOW
	gcn::contrib::AdjustingContainer	*.mContainer;
#endif
};
//////////////////////////////////////////////////////////////////////////
class PropertyFloat : public Property
{
public:
	virtual PropType GetPropertyType() const
	{
		return PropFloat;
	}
	float GetValue() const
	{
		return mValue;
	}
	bool FromString( const std::string &_str )
	{
		float v;
		if ( Utils::ConvertString( _str, v ) )
		{
			mValue = v;
			return true;
		}
		return false;
	}
	bool FromGMVar( gmMachine *_m, const gmVariable &_v )
	{
		if ( _v.IsNumber() )
		{
			mValue = _v.GetFloatSafe();
			return true;
		}
		return false;
	}
#ifdef ENABLE_DEBUG_WINDOW
	void action( const gcn::ActionEvent& actionEvent )
	{
		if ( actionEvent.getSource() == mTextField )
		{
			mValue = (float)atof( .mTextField->getText().c_str() );
		}
	}
	void AddToGui( gcn::contrib::PropertySheet *_propsheet )
	{
		mTextField = new gcn::TextField;
		if ( Check( Prop::PF_READONLY ) )
			mTextField->setEnabled( false );
		_propsheet->addProperty( GetName(), .mTextField );
	}
	void DeAllocGui()
	{
		OB_DELETE( .mTextField );
	}
	void UpdateToGui()
	{
		if ( .mTextField )
		{
			mTextField->setText( ( std::string )va( "%g", .mValue ) );
		}
	}
#endif
	PropertyFloat( const std::string &_name, float &v, uint32_t _flags = 0 )
		: Property( _name, _flags )
		, mValue( v )
#ifdef ENABLE_DEBUG_WINDOW
		, mTextField( 0 )
#endif
	{
	}
	~PropertyFloat()
	{
	}
private:
	float &		 mValue;
#ifdef ENABLE_DEBUG_WINDOW
	gcn::TextField *.mTextField;
#endif
};
//////////////////////////////////////////////////////////////////////////
class PropertyEntity : public Property
{
public:
	virtual PropType GetPropertyType() const
	{
		return PropEntity;
	}
	GameEntity GetValue() const
	{
		return mValue;
	}
	bool FromString( const std::string &_str )
	{
		int v;
		if ( Utils::ConvertString( _str, v ) )
		{
			mValue = gEngineFuncs->EntityFromID( v );
			return true;
		}
		return false;
	}
	bool FromGMVar( gmMachine *_m, const gmVariable &_v )
	{
		if ( _v.IsEntity() )
		{
			mValue.FromInt( _v.GetEntity() );
			return true;
		}
		return false;
	}
#ifdef ENABLE_DEBUG_WINDOW
	void action( const gcn::ActionEvent& actionEvent )
	{
		if ( actionEvent.getSource() == mTextField )
		{
		}
	}
	void AddToGui( gcn::contrib::PropertySheet *_propsheet )
	{
		mTextField = new gcn::TextField;
		if ( Check( Prop::PF_READONLY ) )
			mTextField->setEnabled( false );
		_propsheet->addProperty( GetName(), .mTextField );
	}
	void DeAllocGui()
	{
		OB_DELETE( .mTextField );
	}
	void UpdateToGui()
	{
		if ( .mTextField )
		{
			mTextField->setText( Utils::FormatEntityString( .mValue ) );
			mTextField->adjustSize();
		}
	}
#endif
	PropertyEntity( const std::string &_name, GameEntity &v, uint32_t _flags = 0 )
		: Property( _name, _flags )
		, mValue( v )
#ifdef ENABLE_DEBUG_WINDOW
		, mTextField( 0 )
#endif
	{
	}
	~PropertyEntity()
	{
	}
private:
	GameEntity	& mValue;
#ifdef ENABLE_DEBUG_WINDOW
	gcn::TextField *.mTextField;
#endif
};
//////////////////////////////////////////////////////////////////////////
class PropertyAABB : public Property
{
public:
	virtual PropType GetPropertyType() const
	{
		return PropAABB;
	}
	AABB GetValue() const
	{
		return mValue;
	}
	bool FromString( const std::string &_str )
	{
		/*AABB v;
		if(Utils::ConvertString(_str,v))
		{
		mValue = v;
		return true;
		}*/
		return false;
	}
	bool FromGMVar( gmMachine *_m, const gmVariable &_v )
	{
		/*if(_v.IsNumber())
		{
		mValue = _v.GetFloatSafe();
		return true;
		}*/
		return false;
	}
#ifdef ENABLE_DEBUG_WINDOW
	void action( const gcn::ActionEvent& actionEvent )
	{
		if ( actionEvent.getSource() == mTextField_Mins )
		{
		}
		if ( actionEvent.getSource() == mTextField_Maxs )
		{
		}
	}
	void AddToGui( gcn::contrib::PropertySheet *_propsheet )
	{
		/*.mTextField = new gcn::TextField;
		if(Check(Prop::PF_READONLY))
		mTextField->setEnabled(false);
		_propsheet->addProperty(GetName(),.mTextField);*/
	}
	void DeAllocGui()
	{
		OB_DELETE( .mTextField_Mins );
		OB_DELETE( .mTextField_Maxs );
	}
	void UpdateToGui()
	{
		if ( .mTextField_Mins )
		{
			mTextField_Mins->setText( Utils::FormatVectorString( .mValue.mMins ) );
			mTextField_Mins->adjustSize();
		}
		if ( .mTextField_Maxs )
		{
			mTextField_Maxs->setText( Utils::FormatVectorString( .mValue.mMaxs ) );
			mTextField_Maxs->adjustSize();
		}
	}
#endif
	PropertyAABB( const std::string &_name, AABB &v, uint32_t _flags = 0 )
		: Property( _name, _flags )
		, mValue( v )
#ifdef ENABLE_DEBUG_WINDOW
		, mTextField_Mins( 0 )
		, mTextField_Maxs( 0 )
#endif
	{
	}
	~PropertyAABB()
	{
	}
private:
	AABB	&	 mValue;
#ifdef ENABLE_DEBUG_WINDOW
	gcn::TextField *.mTextField_Mins;
	gcn::TextField *.mTextField_Maxs;
#endif
};
//////////////////////////////////////////////////////////////////////////
class PropertyFunction : public Property
{
public:
	virtual PropType GetPropertyType() const
	{
		return PropFunc;
	}
	bool FromString( const std::string &_str )
	{
		return false;
	}
	bool FromGMVar( gmMachine *_m, const gmVariable &_v )
	{
		return false;
	}
#ifdef ENABLE_DEBUG_WINDOW
	void action( const gcn::ActionEvent &evt )
	{
		if ( evt.getSource() == .mButton )
		{
			StringVector params;
			( *.mFunction )( params );
		}
	}
	void AddToGui( gcn::contrib::PropertySheet *_propsheet )
	{
		mButton = new gcn::Button;
		if ( Check( Prop::PF_READONLY ) )
			mButton->setEnabled( false );
		mButton->addActionListener( this );
		_propsheet->addProperty( GetName(), .mButton );
	}
	void DeAllocGui()
	{
		OB_DELETE( .mButton );
	}
	void UpdateToGui()
	{
	}
#endif
	PropertyFunction( const std::string &_name, CommandFunctorPtr _ptr, uint32_t _flags = 0 )
		: Property( _name, _flags )
		, mFunction( _ptr )
#ifdef ENABLE_DEBUG_WINDOW
		, mButton( 0 )
#endif
	{
	}
	~PropertyFunction()
	{
	}
private:
	CommandFunctorPtr mFunction;
#ifdef ENABLE_DEBUG_WINDOW
	gcn::Button		* mButton;
#endif
};
//////////////////////////////////////////////////////////////////////////
PropertyBinding::PropertyBinding()
{
}

bool PropertyBinding::FromPropertyMap( const PropertyMap &_propmap, std::stringstream &errorOut )
{
	bool bGood = true;
	bool bHandelledProp = false;
	PropertyList::iterator it = mPropertyList.begin();
	for ( ; it != mPropertyList.end(); ++it )
	{
		std::string n = ( *it )->GetName();

		PropertyMap::ValueMap::const_iterator pIt = _propmap.GetProperties().begin();
		for ( ; pIt != _propmap.GetProperties().end(); ++pIt )
		{
			if ( !Utils::StringCompareNoCase( pIt->first.c_str(), n.c_str() ) )
			{
				const bool bSuccess = ( *it )->FromString( pIt->second );
				bHandelledProp |= bSuccess;

				if ( !bSuccess && ( *it )->Check( Prop::PF_REQUIRED ) )
				{
					std::string t = ( *it )->GetDebugTypeName();
					errorOut << "Required Property " << n.c_str() << " as " << t.c_str() << std::endl;
					bGood = false;
				}
			}
		}
	}

	if ( !bHandelledProp )
		bGood = false;

	return bGood;
}

bool PropertyBinding::FromScriptTable( gmMachine *_machine, gmTableObject *_tbl, std::stringstream &errorOut )
{
	bool bGood = true;

	gmTableIterator tIt;
	gmTableNode *pNode = _tbl->GetFirst( tIt );
	while ( pNode )
	{
		const char *PropName = pNode->m_key.GetCStringSafe( 0 );
		if ( PropName )
		{
			FromScriptVar( _machine, PropName, pNode->m_value, errorOut );
		}
		pNode = _tbl->GetNext( tIt );
	}
	return bGood;
}

bool PropertyBinding::FromScriptVar( gmMachine *_machine, const char *_key, gmVariable &_var, std::stringstream &errorOut )
{
	bool bGood = false;
	PropertyList::iterator it = mPropertyList.begin();
	for ( ; it != mPropertyList.end(); ++it )
	{
		std::string n = ( *it )->GetName();
		if ( !Utils::StringCompareNoCase( n, _key ) )
		{
			const bool bSuccess = ( *it )->FromGMVar( _machine, _var );
			if ( bSuccess )
			{
				bGood = true;
			}
			else
			{
				std::string t = ( *it )->GetDebugTypeName();
				errorOut << "Expected Property " << n.c_str() << " as " << t.c_str() << std::endl;
			}
		}
	}
	return bGood;
}

void PropertyBinding::BindProperty( const std::string &_name, bool &_val, uint32_t _flags )
{
	mPropertyList.push_back( PropertyPtr( new PropertyBool( _name, _val ) ) );
	//.mPropertyList.back()->SetBinding(this);
}
void PropertyBinding::BindProperty( const std::string &_name, const char *&_val, uint32_t _flags )
{
	mPropertyList.push_back( PropertyPtr( new PropertyCString( _name, _val ) ) );
	//.mPropertyList.back()->SetBinding(this);
}

void PropertyBinding::BindProperty( const std::string &_name, std::string &_val, uint32_t _flags )
{
	mPropertyList.push_back( PropertyPtr( new PropertyString( _name, _val, _flags ) ) );
	//.mPropertyList.back()->SetBinding(this);
}

void PropertyBinding::BindProperty( const std::string &_name, Vector3f &_val, uint32_t _flags )
{
	mPropertyList.push_back( PropertyPtr( new PropertyVector( _name, _val, _flags ) ) );
	//.mPropertyList.back()->SetBinding(this);
}
void PropertyBinding::BindProperty( const std::string &_name, Matrix3f &_val, uint32_t _flags )
{
	mPropertyList.push_back( PropertyPtr( new PropertyMatrix( _name, _val, _flags ) ) );
	//.mPropertyList.back()->SetBinding(this);
}
void PropertyBinding::BindProperty( const std::string &_name, int &_val, uint32_t _flags, const IntEnum *_enum, int _numenum )
{
	mPropertyList.push_back( PropertyPtr( new PropertyInt( _name, _val, _flags, _enum, _numenum ) ) );
	//.mPropertyList.back()->SetBinding(this);
}
void PropertyBinding::BindProperty( const std::string &_name, BitFlag32 &_val, uint32_t _flags, const IntEnum *_enum, int _numenum )
{
	mPropertyList.push_back( PropertyPtr( new PropertyBitflag32( _name, _val, _flags, _enum, _numenum ) ) );
	//.mPropertyList.back()->SetBinding(this);
}
void PropertyBinding::BindProperty( const std::string &_name, float &_val, uint32_t _flags )
{
	mPropertyList.push_back( PropertyPtr( new PropertyFloat( _name, _val, _flags ) ) );
	//.mPropertyList.back()->SetBinding(this);
}
void PropertyBinding::BindProperty( const std::string &_name, GameEntity &_val, uint32_t _flags )
{
	mPropertyList.push_back( PropertyPtr( new PropertyEntity( _name, _val ) ) );
	//.mPropertyList.back()->SetBinding(this);
}
void PropertyBinding::BindProperty( const std::string &_name, AABB &_val, uint32_t _flags )
{
	mPropertyList.push_back( PropertyPtr( new PropertyAABB( _name, _val, _flags ) ) );
	//.mPropertyList.back()->SetBinding(this);
}
void PropertyBinding::BindFunction( const std::string _name, CommandFunctorPtr _functor )
{
	mPropertyList.push_back( PropertyPtr( new PropertyFunction( _name, _functor, 0 ) ) );
	//.mPropertyList.back()->SetBinding(this);
}
//////////////////////////////////////////////////////////////////////////

PropertyBinding::PropertyPtr PropertyBinding::Get( const std::string &_name )
{
	PropertyList::iterator it = mPropertyList.begin();
	for ( ; it != mPropertyList.end(); ++it )
	{
		if ( ( *it )->GetName() == _name )
			return *it;
	}
	return PropertyPtr();
}

bool PropertyBinding::GetProperty( const std::string &_name, bool &_val )
{
	PropertyPtr pp = Get( _name );
	if ( pp && pp->GetPropertyType() == PropBool )
	{
		PropertyBool *p = static_cast<PropertyBool*>( pp.get() );
		_val = p->GetValue();
		return true;
	}
	return false;
}

bool PropertyBinding::GetProperty( const std::string &_name, const char *&_val )
{
	PropertyPtr pp = Get( _name );
	if ( pp && pp->GetPropertyType() == PropCString )
	{
		PropertyCString *p = static_cast<PropertyCString*>( pp.get() );
		_val = p->GetValue();
		return true;
	}
	return false;
}

bool PropertyBinding::GetProperty( const std::string &_name, std::string &_val )
{
	PropertyPtr pp = Get( _name );
	if ( pp && pp->GetPropertyType() == PropString )
	{
		PropertyString *p = static_cast<PropertyString*>( pp.get() );
		_val = p->GetValue();
		return true;
	}
	return false;
}

bool PropertyBinding::GetProperty( const std::string &_name, Vector3f &_val )
{
	PropertyPtr pp = Get( _name );
	if ( pp && pp->GetPropertyType() == PropVector )
	{
		PropertyVector *p = static_cast<PropertyVector*>( pp.get() );
		_val = p->GetValue();
		return true;
	}
	return false;
}

bool PropertyBinding::GetProperty( const std::string &_name, int &_val )
{
	PropertyPtr pp = Get( _name );
	if ( pp && pp->GetPropertyType() == PropInt )
	{
		PropertyInt *p = static_cast<PropertyInt*>( pp.get() );
		_val = p->GetValue();
		return true;
	}
	return false;
}

bool PropertyBinding::GetProperty( const std::string &_name, float &_val )
{
	PropertyPtr pp = Get( _name );
	if ( pp && pp->GetPropertyType() == PropFloat )
	{
		PropertyFloat *p = static_cast<PropertyFloat*>( pp.get() );
		_val = p->GetValue();
		return true;
	}
	return false;
}

bool PropertyBinding::GetProperty( const std::string &_name, GameEntity &_val )
{
	PropertyPtr pp = Get( _name );
	if ( pp && pp->GetPropertyType() == PropEntity )
	{
		PropertyEntity *p = static_cast<PropertyEntity*>( pp.get() );
		_val = p->GetValue();
		return true;
	}
	return false;
}

bool PropertyBinding::GetProperty( const std::string &_name, AABB &_val )
{
	PropertyPtr pp = Get( _name );
	if ( pp && pp->GetPropertyType() == PropAABB )
	{
		PropertyAABB *p = static_cast<PropertyAABB*>( pp.get() );
		_val = p->GetValue();
		return true;
	}
	return false;
}

//////////////////////////////////////////////////////////////////////////
#ifdef ENABLE_DEBUG_WINDOW
void PropertyBinding::GatherProperties( gcn::contrib::PropertySheet *_container )
{	
	for ( PropertyList::iterator it = mPropertyList.begin(); it != mPropertyList.end(); ++it )
	{
		( *it )->AddToGui( _container );
		( *it )->UpdateToGui();
	}
}
void PropertyBinding::DeAllocGui()
{	
	for ( PropertyList::iterator it = mPropertyList.begin(); it != mPropertyList.end(); ++it )
	{
		( *it )->DeAllocGui();
	}
}
void PropertyBinding::UpdatePropertiesToGui()
{	
	for ( PropertyList::iterator it = mPropertyList.begin(); it != mPropertyList.end(); ++it )
	{
		( *it )->UpdateToGui();
	}
}
#endif

//void PropertyBinding::action(const gcn::ActionEvent& actionEvent)
//{
//	PropertyPtr pptr = Get(actionEvent.getId());
//	if(pptr)
//	{
//
//	}
//}