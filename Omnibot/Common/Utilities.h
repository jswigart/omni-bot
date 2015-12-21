////////////////////////////////////////////////////////////////////////////////
//
// $LastChangedBy$
// $LastChangedDate$
// $LastChangedRevision$
//
////////////////////////////////////////////////////////////////////////////////

#pragma once

#ifndef __UTILS_H__
#define __UTILS_H__

#include <stdio.h>
#include <stdarg.h>
#include <stdint.h>
#include <sstream>

#include "Trajectory.h"
#include "gmGCRoot.h"

#include <stdlib.h> //#include <malloc.h>
#define StackAlloc alloca

#include "Opcode.h"
#include "OPC_IceHook.h"
#include "modeldata.pb.h"


//////////////////////////////////////////////////////////////////////////

template < typename EnumType >
struct EnumerationValues
{
	const char *	mName;
	EnumType		mValue;

	static bool ValueForName( const char * name, EnumType& val )
	{
		for ( size_t i = 0; i < sKeyValCount; ++i )
		{
			if ( !stricmp( sKeyVal[ i ].mName, name ) )
			{
				val = sKeyVal[ i ].mValue;
				return true;
			}
		}
		return false;
	}

	static bool ParseDelimitedValue( const char * name, const char * delimiters, EnumType& val )
	{
		StringVector tokens;
		Tokenize( name, delimiters, tokens );

		size_t matchedTokens = 0;
		for ( size_t i = 0; i < tokens.size(); ++i )
		{
			for ( size_t i = 0; i < sKeyValCount; ++i )
			{
				if ( !stricmp( sKeyVal[ i ].mName, name ) )
				{
					val = (EnumType)( val | sKeyVal[ i ].mValue );
					++matchedTokens;
				}
			}
		}
		return matchedTokens == tokens.size();
	}

	static bool NameForValue( EnumType val, std::string& name )
	{
		for ( size_t i = 0; i < sKeyValCount; ++i )
		{
			if ( sKeyVal[ i ].mValue == val )
			{
				name = sKeyVal[ i ].mName;
				return true;
			}
		}
		return false;
	}

	static bool NameForValueBitfield( EnumType val, std::string& name )
	{
		std::string str;
		for ( size_t i = 0; i < sKeyValCount; ++i )
		{
			if ( ( sKeyVal[ i ].mValue & val ) == sKeyVal[ i ].mValue )
			{
				if ( !str.empty() )
					str += ", ";
				str += sKeyVal[ i ].mName;
			}
		}
		name += str;
		return !str.empty();
	}

	static std::string ValidNames()
	{
		std::string str;
		for ( size_t i = 0; i < sKeyValCount; ++i )
		{
			if ( !str.empty() )
				str += ", ";

			str += sKeyVal[ i ].mName;
		}
		return str;
	}

	static const EnumerationValues<EnumType>	sKeyVal[];
	static const size_t							sKeyValCount;
};

//////////////////////////////////////////////////////////////////////////

class File;
class Client;
class TargetInfo;

//////////////////////////////////////////////////////////////////////////
namespace Priority
{
	enum ePriority
	{
		Zero,
		Min,
		Idle,
		VeryLow,
		Low,
		LowMed,
		Medium,
		High,
		VeryHigh,
		Override,
		NumPriority
	};
	const char *AsString( int n );
};
//////////////////////////////////////////////////////////////////////////

typedef std::vector< std::string > StringVector;
typedef std::list< std::string > StringList;

// file: Utils
//		Contains various utilities that may be useful throughout the bot.
namespace Utils
{
	//////////////////////////////////////////////////////////////////////////
	extern float FloatMax;
	//////////////////////////////////////////////////////////////////////////
	bool RegexMatch( const char * exp, const char * str );

	void StringCopy( char *_destination, const char *_source, int _buffersize );

	int32_t StringCompare( const char *_s1, const char *_s2 );
	int32_t StringCompareNoCase( const char *_s1, const char *_s2 );

	int32_t StringCompare( const std::string &_s1, const std::string &_s2 );
	int32_t StringCompareNoCase( const std::string &_s1, const std::string &_s2 );

	std::string StringToLower( const std::string &_s1 );

	bool IsWhiteSpace( const char _ch );

	std::string BuildRoleName( int32_t _mask );

	// todo: depracate these for type safe time types
	inline float MillisecondsToSeconds( int _ms )
	{
		return (float)_ms / 1000.f;
	}
	inline int SecondsToMilliseconds( float _seconds )
	{
		return (int)( _seconds*1000.f );
	}
	inline int HzToMilliseconds( int _hz )
	{
		return (int)( 1000.f / (float)_hz );
	}
	inline float HzToSeconds( int _hz )
	{
		return MillisecondsToSeconds( HzToMilliseconds( _hz ) );
	}

	bool InFieldOfView( const Vector3f &_face1, const Vector3f &_face2, float _fovAngles );

	bool InFieldOfView2d( const Vector3f &_face1, const Vector3f &_face2, float _fovAngles );

	float AngleBetween( const Vector3f &_v1, const Vector3f &_v2 );

	Vector3f AveragePoint( const Vector3List &_lis );

	void WeldVertices( Vector3List & poly, float epsilon );

	float ClosestPointOfApproachTime( const Vector3f& aP1, const Vector3f& aV1, const Vector3f& aP2, const Vector3f& aV2 );

	// function: Tokenize
	//		Splits a std::string into tokens using provided seperators. Stores in <StringVector>
	void Tokenize( const std::string &_s, const std::string &_separators, StringVector &_tokens );

	const char *VarArgs( CHECK_VALID_BYTES( _buffsize ) char *_outbuffer, int _buffsize, CHECK_PRINTF_ARGS const char* _msg, ... );

	void OutputDebugBasic( MessageType _type, const char* _msg );
	void OutputDebug( MessageType _type, CHECK_PRINTF_ARGS const char* _msg, ... );

	GameEntity GetLocalEntity();
	int GetLocalGameId();

	bool GetLocalPosition( Vector3f &_pos );
	bool GetLocalGroundPosition( Vector3f &_pos, Vector3f *_normal, int _tracemask = TR_MASK_FLOODFILL );
	bool GetLocalGroundPosition( Vector3f &_pos, int _tracemask );
	bool GetLocalEyePosition( Vector3f &_pos );
	bool GetLocalFacing( Vector3f &_face );
	bool GetLocalAABB( AABB &_aabb );
	bool GetLocalWorldOBB( Box3f & obb );
	bool GetLocalAimPoint( Vector3f &_pos, Vector3f *_normal = 0, int _tracemask = TR_MASK_FLOODFILLENT, int * _contents = 0, int * _surface = 0 );
	bool GetNearestNonSolid( Vector3f &_pos, const Vector3f &_start, const Vector3f &_end, int _tracemask = TR_MASK_FLOODFILL );

	int32_t MakeId32( int8_t a, int8_t b, int8_t c, int8_t d );
	int32_t MakeId32( int16_t a, int16_t b );
	int32_t MakeId32( const char *_st );
	void SplitId32( int32_t id, int16_t &id1, int16_t &id2 );

	uint32_t MakeHash32( const std::string &_str, bool _log = true );
	uint32_t Hash32( const char *_name );
	uint64_t Hash64( const char *_name );

	void AddHashedString( const std::string &_str );
	std::string HashToString( uint32_t _hash );

	// function: FormatByteString
	//		Formats a byte value into a readable std::string
	//		with a byte unit postfix, kB, MB, etc...
	std::string FormatByteString( uint64_t _bytes );

	std::string FormatEntityString( GameEntity _e );
	std::string FormatVectorString( const Vector3f &v );
	std::string FormatMatrixString( const Matrix3f &m );

	void GetAABBBoundary( const AABB &_aabb, Vector3List &_list );

	bool ToLocalSpace( GameEntity _ent, const Vector3f &_worldpos, Vector3f &_out );
	bool ToWorldSpace( GameEntity _ent, const Vector3f &_localpos, Vector3f &_out );

	// function: PredictFuturePositionOfTarget
	//		Static helper function for calculating target leading.
	//
	// Parameters:
	//
	//		_tgpos - The <Vector3> position of the target.
	//		_tgvel - The <Vector3> velocity of the target.
	//		_timeahead - How many seconds ahead to calculate.
	//
	// Returns:
	//		Vector3 - A point along the targets current velocity to aim towards.
	Vector3f PredictFuturePositionOfTarget( const Vector3f &_tgpos, const Vector3f &_tgvel, float _timeahead );

	// function: PredictFuturePositionOfTarget
	//		Static helper function for calculating target leading.
	//
	// Parameters:
	//
	//		_mypos - The <Vector3> position of the shooter.
	//		_projectilespeed - The speed of the projectile being fired.
	//		_tgpos - The <Vector3> position of the target.
	//		_tgvel - The <Vector3> velocity of the target.
	//
	// Returns:
	//		Vector3 - A point along the targets current velocity to aim towards.
	Vector3f PredictFuturePositionOfTarget(
		const Vector3f &_mypos,
		float _projectilespeed,
		const Vector3f &_tgpos,
		const Vector3f &_tgvel );

	Vector3f PredictFuturePositionOfTarget(
		const Vector3f &_mypos,
		float _projspeed,
		const TargetInfo &_tg,
		const Vector3f &_extravelocity,
		float _minleaderror = 0.f,
		float _maxleaderror = 0.f );

	bool TestSegmentForOcclusion( const Segment3f &seg );
	bool GetSegmentOverlap( const Segment3f &_seg1, const Segment3f &_seg2, Segment3f &out );

	float ClosestPtOnLine( const Vector3f &p1, const Vector3f &p2, const Vector3f &p, Vector3f &cp );
	bool ClosestPtOnLine( const Vector3List &list, const Vector3f &pos, Vector3f &cp, bool loop );
	float ClosestPtOnLine_Unclamped( const Vector3f& p1, const Vector3f& p2, const Vector3f& p, Vector3f &cp );

	Vector3f ChangePitch( const Vector3f &fwd, float _pitchangle );

	bool StringToBool( const std::string &_str, bool& bOut );
	bool StringToTrue( const std::string &_str );
	bool StringToFalse( const std::string &_str );
	void StringTrimCharacters( std::string &_out, const std::string &_trim );

	std::string GetTeamString( int32_t _team );
	std::string GetClassString( int32_t _class );

	bool TeamExists( int32_t _team );
	bool ClassExists( int32_t _class );

	void KeyValsToTable( const KeyVals &_kv, gmGCRoot<gmTableObject> _tbl, gmMachine *_machine );

	gmVariable UserDataToGmVar( gmMachine *_machine, const obUserData &bud );

	template <typename T>
	bool ConvertString( const std::string &_str, T &_var )
	{
		std::stringstream st;
		st << _str;
		st >> _var;
		return !st.fail();
	}

	template <typename T>
	bool ConvertString( const T &_var, std::string &_str )
	{
		std::stringstream st;
		st << _var;
		_str = st.str();
		return !st.fail();
	}

	std::string FindOpenPlayerName();

	std::string IndentString( const int indentlevel );

	enum AssertMode
	{
		FireOnce,
		FireAlways,
	};
	bool AssertFunction( bool _bexp, const char* _exp, const char* _file, int _line, CHECK_PRINTF_ARGS const char *_msg, ... );
	bool SoftAssertFunction( AssertMode _mode, bool _bexp, const char* _exp, const char* _file, int _line, CHECK_PRINTF_ARGS const char *_msg, ... );

	Vector3List CreatePolygon( const Vector3f &_pos, const Vector3f &_normal, float _size = 512.f );
	Vector3List ClipPolygonToPlanes( const Vector3List & poly, const Plane3f & plane );

	void PushPointToPlane( Vector3f & pt, const Plane3f & plane, const Vector3f & pushdirection );
	/*float DistancePointToLineSqr(const Vector3f &_point,
	const Vector3f &_pt0,
	const Vector3f &_pt1,
	Vector3f *_linePt = NULL);

	float DistancePointToLine(const Vector3f &_point,
	const Vector3f &_pt0,
	const Vector3f &_pt1,
	Vector3f *_linePt = NULL);*/

	Vector3f CalculateCenter( const Vector3List & poly );
	Plane3f CalculatePlane( const Vector3List & poly );

	int intersect2D_Segments( const Segment3f &S1, const Segment3f &S2, Vector3f* I0 = 0, Vector3f* I1 = 0 );
};

//////////////////////////////////////////////////////////////////////////
class PropertyMap
{
public:
	typedef std::map<std::string, std::string> ValueMap;

	bool AddProperty( const std::string &_name, const std::string &_data );
	void DelProperty( const std::string &_name );
	std::string GetProperty( const std::string &_name ) const;

	uint32_t GetNumProperties() const
	{
		return (uint32_t)mProperties.size();
	}

	template <typename T>
	bool AddPropertyT( const std::string &_name, T &_value )
	{
		std::string p;
		return ( Utils::ConvertString( _value, p ) && AddProperty( _name, p ) );
	}

	template <typename T>
	bool GetProperty( const std::string &_name, T &_value ) const
	{
		std::string p = GetProperty( _name );
		return ( !p.empty() && Utils::ConvertString( p, _value ) );
	}
	/*template <>
	bool GetProperty(const std::string &_name, std::string &_value) const
	{
	_value = GetProperty(_name);
	return !_value.empty();
	}*/
	void GetAsKeyVal( KeyVals &kv );

	const ValueMap &GetProperties() const
	{
		return mProperties;
	}
private:
	ValueMap mProperties;
};

//////////////////////////////////////////////////////////////////////////

class va
{
public:
	const char * c_str() const
	{
		return buffer;
	}

	va( CHECK_PRINTF_ARGS const char* msg, ... );
protected:
	enum
	{
		BufferSize = 1024
	};
	char buffer[ BufferSize ];
};

//////////////////////////////////////////////////////////////////////////

class filePath
{
public:
	const char * c_str() const
	{
		return buffer;
	}
	operator const char *( ) const
	{
		return buffer;
	}

	std::string FileName() const;

	filePath();
	filePath( const char* msg, ... );
private:
	enum
	{
		BufferSize = 1024
	};
	char buffer[ BufferSize ];

	void FixPath();
};

std::ostream& operator <<( std::ostream& _o, const filePath& _filePath );

//////////////////////////////////////////////////////////////////////////

// Templated helper functions.
template<class Type>
static inline Type MinT( const Type &_val1, const Type &_val2 )
{
	return ( _val1 < _val2 ) ? _val1 : _val2;
}

template<class Type>
static inline Type MaxT( const Type &_val1, const Type &_val2 )
{
	return ( _val1 > _val2 ) ? _val1 : _val2;
}

template<class Type>
static inline Type ClampT( const Type &_val, const Type &_low, const Type &_high )
{
	return ( ( _val<_low ? _low : ( _val>_high ) ? _high : _val ) );
}

template<class Type>
static inline Type InRangeT( const Type &_val, const Type &_low, const Type &_high )
{
	return ( ( _val >= _low ) && ( _val <= _high ) );
}

template<class Type>
static inline bool CheckBitT( const Type &_val, const Type &_bit )
{
	return ( ( _val & _bit ) == _bit );
}

template<class Type>
static inline void SetBitT( Type &_val, const Type &_bit )
{
	( _val |= _bit );
}

template<class Type>
static inline void ClearBitT( Type &_val, const Type &_bit )
{
	( _val &= ~_bit );
}

template<class Type>
static inline void SwapT( Type &_1, Type &_2 )
{
	Type t = _1;
	_1 = _2;
	_2 = t;
}

template<class Type>
inline Type InterpolateT( const Type &_1, const Type &_2, float _t )
{
	return _1 + ( _2 - _1 ) * _t;
}

template<typename T>
inline Vector2<T> To2d( const Vector3<T> & v3 )
{
	return Vector2<T>( v3.X(), v3.Y() );
}

// ostream helpers
std::ostream& operator <<( std::ostream& _o, const obUserData& _bud );
std::ostream& operator <<( std::ostream& _o, const TriggerInfo& _ti );

//////////////////////////////////////////////////////////////////////////
class Options
{
public:
	static void Init();
	static void Shutdown();
	static bool LoadConfigFile( const std::string &_file );
	static bool SaveConfigFile( const std::string &_file );

	static bool SaveConfigFileIfChanged( const std::string &_file );

	static const char *GetRawValue( const char *_section, const char *_key );

	static bool GetValue( const char *_section, const char *_key, bool &_out );
	static bool GetValue( const char *_section, const char *_key, int &_out );
	static bool GetValue( const char *_section, const char *_key, float &_out );
	static bool GetValue( const char *_section, const char *_key, std::string &_out );

	static bool SetValue( const char *_section, const char *_key, bool _val, bool _overwrite = true );
	static bool SetValue( const char *_section, const char *_key, int _val, bool _overwrite = true );
	static bool SetValue( const char *_section, const char *_key, float _val, bool _overwrite = true );
	static bool SetValue( const char *_section, const char *_key, const char *_val, bool _overwrite = true );
	static bool SetValue( const char *_section, const char *_key, const std::string &_val, bool _overwrite = true );
};

//////////////////////////////////////////////////////////////////////////

class LimitChecker
{
public:
	virtual bool IsAllowed( Client *_client ) = 0;
	BitFlag128 &Get()
	{
		return mFlags;
	}
	int FromScript( gmThread *a_thread );
	LimitChecker()
	{
	}
protected:
	BitFlag128		mFlags;
};

class LimitWeapons : public LimitChecker
{
public:
	bool IsAllowed( Client *_client );
	LimitWeapons()
	{
	}
private:
};

//////////////////////////////////////////////////////////////////////////

class ErrorObj
{
public:
	void AddInfo( CHECK_PRINTF_ARGS const char* _msg, ... );
	void AddError( CHECK_PRINTF_ARGS const char* _msg, ... );

	void PrintToConsole();
private:
	StringList	mInfo;
	StringList	mErrors;
};

class StringBuffer
{
public:

	const char *AddUniqueString( const std::string & _str );
	const char *Find( const std::string & _str );

	StringBuffer( uint32_t _maxStrings = 64, uint32_t _bufferSize = 1024 );
	~StringBuffer();
private:
	uint32_t mBufferOffset;
	char **	 mStrings;
	char *	 mBuffer;

	const uint32_t mBufferSize;
	const uint32_t mMaxStrings;
};

obColor GetCoolWarmColor( float scalar );

struct sqlite3;
int CheckSqliteError( sqlite3 * db, int errcode );

AxisAlignedBox3f ComputeAABB( const Box3f & obb );

Vector3f Convert( const IceMaths::Point & pt );
Vector3f Convert( const modeldata::Vec3 & vec );
IceMaths::Matrix4x4 Convert( const modeldata::Node & node );

int ConvexhullXY( const Vector3f* pts, int npts, int* out, int maxOut, float& minheight, float& maxheight );

#endif
