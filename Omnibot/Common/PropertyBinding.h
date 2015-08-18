////////////////////////////////////////////////////////////////////////////////
//
// $LastChangedBy$
// $LastChangedDate$
// $LastChangedRevision$
//
////////////////////////////////////////////////////////////////////////////////

#pragma once

#ifndef __PROPERTYBINDING_H__
#define __PROPERTYBINDING_H__

#include "common.h"
#include "CommandFunctor.h"

class Property;

#ifdef ENABLE_DEBUG_WINDOW
#include <guichan.hpp>
#endif

namespace Prop
{
	enum BaseFlags
	{
		PF_READONLY = ( 1 << 0 ),
		PF_REQUIRED = ( 1 << 1 ),

		// must be last!
		PF_LAST = ( 1 << 2 ),
	};

	enum NumberFlags
	{
		PF_MS_TO_SECONDS = ( PF_LAST << 0 ),
	};

	enum Vector3Flags
	{
		PF_POSITION = ( PF_LAST << 0 ),
		PF_FACING = ( PF_LAST << 1 ),

		PF_VEC_LAST = ( PF_LAST << 2 ),
	};

	enum MatrixFlags
	{
		PF_FULLTRANS = ( PF_VEC_LAST << 0 ),
	};
};

//////////////////////////////////////////////////////////////////////////

struct IntEnum
{
	const char *	mKey;
	int				mValue;
	IntEnum( const char *_key = 0, int _val = 0 ) : mKey( _key ), mValue( _val )
	{
	}
};

class PropertyBinding
{
public:
	typedef std::shared_ptr<Property> PropertyPtr;
	typedef std::vector<PropertyPtr> PropertyList;

	void BindProperty( const std::string &_name, bool &_val, uint32_t _flags = 0 );
	void BindProperty( const std::string &_name, const char *&_val, uint32_t _flags = 0 );
	void BindProperty( const std::string &_name, std::string &_val, uint32_t _flags = 0 );
	void BindProperty( const std::string &_name, Vector3f &_val, uint32_t _flags = Prop::PF_POSITION );
	void BindProperty( const std::string &_name, Matrix3f &_val, uint32_t _flags = 0 );
	void BindProperty( const std::string &_name, int &_val, uint32_t _flags = 0, const IntEnum *_enum = 0, int _numenum = 0 );
	void BindProperty( const std::string &_name, BitFlag32 &_val, uint32_t _flags = 0, const IntEnum *_enum = 0, int _numenum = 0 );
	void BindProperty( const std::string &_name, float &_val, uint32_t _flags = 0 );
	void BindProperty( const std::string &_name, GameEntity &_val, uint32_t _flags = 0 );
	void BindProperty( const std::string &_name, AABB &_val, uint32_t _flags = 0 );

	template <typename T, typename Fn>
	void BindFunction( const std::string _name, T *_src, Fn _fn )
	{
		BindFunction( _name, CommandFunctorPtr( new Delegate0<T, Fn>( _src, _fn ) ) );
	}
	void BindFunction( const std::string _name, CommandFunctorPtr _functor );

	bool GetProperty( const std::string &_name, bool &_val );
	bool GetProperty( const std::string &_name, const char *&_val );
	bool GetProperty( const std::string &_name, std::string &_val );
	bool GetProperty( const std::string &_name, Vector3f &_val );
	bool GetProperty( const std::string &_name, int &_val );
	bool GetProperty( const std::string &_name, float &_val );
	bool GetProperty( const std::string &_name, GameEntity &_val );
	bool GetProperty( const std::string &_name, AABB &_val );

	bool FromPropertyMap( const PropertyMap &_propmap, std::stringstream &errorOut );
	bool FromScriptTable( gmMachine *_machine, gmTableObject *_tbl, std::stringstream &errorOut );
	bool FromScriptVar( gmMachine *_machine, const char *_key, gmVariable &_var, std::stringstream &errorOut );

#ifdef ENABLE_DEBUG_WINDOW
	void DeAllocGui();
	void GatherProperties(gcn::contrib::PropertySheet *_propsheet);
	void UpdatePropertiesToGui();
#endif

	//void action(const gcn::ActionEvent& actionEvent);

	PropertyBinding();
	virtual ~PropertyBinding()
	{
	}
private:
	PropertyList		mPropertyList;
	PropertyPtr Get( const std::string &_name );
};

//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////

#endif
