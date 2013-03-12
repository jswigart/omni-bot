////////////////////////////////////////////////////////////////////////////////
//
// $LastChangedBy$
// $LastChangedDate$
// $LastChangedRevision$
//
////////////////////////////////////////////////////////////////////////////////

#include "IGame.h"
#include "IGameManager.h"
#include "KeyValueIni.h"
#include "TargetInfo.h"
#include "BotWeaponSystem.h"

#ifdef WIN32
#include "windows.h"
#endif

typedef std::map<obuint32, obuint32> HashIndexMap;
static HashIndexMap		g_HashIndexMap;
static std::string			g_StringRepository;

//////////////////////////////////////////////////////////////////////////
// turn radius
// turnradius = fWheelBase / (2.f * Math::Sin(Math::DegToRad(fWheelAngle/2.f)));
//////////////////////////////////////////////////////////////////////////

namespace Priority
{
	const char *AsString(int n)
	{
		static const char *str[] =
		{
			"Zero",
			"Min",
			"Idle",
			"VeryLow",
			"Low",
			"LowMed",
			"Medium",
			"High",
			"VeryHigh",
			"Override",
		};
		BOOST_STATIC_ASSERT(sizeof(str) / sizeof(str[0]) == Priority::NumPriority);
		if(n >= 0 && n < Priority::NumPriority)
			return str[n];
		return "";
	}
};

//////////////////////////////////////////////////////////////////////////

namespace Utils
{
	bool RegexMatch( const char * exp, const char * str ) {
		try
		{
			boost::regex expression( exp, REGEX_OPTIONS );
			return boost::regex_match( str, expression );
		}
		catch(const std::exception & ex)
		{
			ex;
			OBASSERT(0, ex.what());
		}
		return false;
	}

	float FloatMax = std::numeric_limits<float>::max();

	void StringCopy(char *_destination, const char *_source, int _buffersize)
	{
		strncpy(_destination, _source, _buffersize);
	}

	obint32 StringCompare(const char *_s1, const char *_s2)
	{
		return strcmp(_s1, _s2);
	}

	obint32 StringCompareNoCase(const char *_s1, const char *_s2)
	{
#ifdef __GNUC__
		return strcasecmp(_s1, _s2);
#else
		return stricmp(_s1, _s2);
#endif
	}

	obint32 StringCompare(const std::string &_s1, const std::string &_s2)
	{
		return StringCompare(_s1.c_str(), _s2.c_str());
	}
	obint32 StringCompareNoCase(const std::string &_s1, const std::string &_s2)
	{
		return StringCompareNoCase(_s1.c_str(), _s2.c_str());
	}

	std::string StringToLower(const std::string &_s1)
	{
		std::string s = _s1;
		std::transform(s.begin(), s.end(), s.begin(), toLower());
		return s;
	}

	obint32 MakeId32(obint16 a, obint16 b)
	{
		return (((a)<<16) | (b));
	}

	void SplitId32(obint32 id, obint16 &id1, obint16 &id2)
	{
		id1 = (obint16)(id>>16);
		id2 = (obint16)id & 0xFF;
	}

	obint32 MakeId32(obint8 a, obint8 b, obint8 c, obint8 d )
	{
		return (((d)<<24) | ((c)<<16) | ((b)<<8) | (a));
	}

	obint32 MakeId32(const char *_st)
	{
		return MakeId32(_st[0], _st[1], _st[2], _st[3]);
	}

	obuint32 MakeHash32(const std::string &_str, bool _log /*= true*/)
	{
		if(_str.empty())
			return 0;

		if(_log)
			AddHashedString(_str);
		return Hash32(_str.c_str());
	}

	std::string FormatEntityString(GameEntity _e)
	{
		return std::string(va("%d:%d", _e.GetIndex(), _e.GetSerial()));
	}
	std::string FormatVectorString(const Vector3f &v)
	{
		return std::string(va("(%.3f, %.3f, %.3f)",v.X(),v.Y(),v.Z()));
	}

	std::string FormatMatrixString(const Matrix3f &m)
	{
		return std::string(va("(%.3f, %.3f, %.3f) (%.3f, %.3f, %.3f) (%.3f, %.3f, %.3f)",
			m.GetColumn(0)[0],m.GetColumn(0)[1],m.GetColumn(0)[2],
			m.GetColumn(1)[0],m.GetColumn(1)[1],m.GetColumn(1)[2],
			m.GetColumn(2)[0],m.GetColumn(2)[1],m.GetColumn(2)[2]));
	}

	/*
	* FNV-1a hash 32 bit http://www.isthe.com/chongo/tech/comp/fnv/
	*/
	obuint32 Hash32(const char *_name)
	{
		const obuint32 FNV_32_PRIME = ((obuint32)0x01000193);
		const obuint32 FNV1A_32_INIT = ((obuint32)0x811c9dc5);

		const char *s = _name;
		obuint32 hval = FNV1A_32_INIT;
		while (*s)
		{
			char c = (char)tolower(*s++);
			hval ^= (obuint32)c;
			hval *= FNV_32_PRIME;
		}
		return hval;
	}
	/*
	* FNV-1a hash 64 bit http://www.isthe.com/chongo/tech/comp/fnv/
	*/
	obuint64 Hash64(const char *_name)
	{
		const obuint64 FNV_64_PRIME = ((obuint64)0x100000001b3ULL);
		const obuint64 FNV1A_64_INIT = ((obuint64)0x84222325 << 32 | (obuint64)0x1b3);

		const char *s = _name;
		obuint64 hval = FNV1A_64_INIT;
		while (*s)
		{
			char c = (char)tolower(*s++);
			hval ^= (obuint64)c;
			hval *= FNV_64_PRIME;
		}
		return hval;
	}

	void AddHashedString(const std::string &_str)
	{
		obuint32 hash = Hash32(_str.c_str());
		HashIndexMap::iterator it = g_HashIndexMap.find(hash);
		if(it == g_HashIndexMap.end())
		{
			obuint32 iOffset = (obuint32)g_StringRepository.size();
			g_StringRepository.append(_str.c_str(), (int)_str.size()+1);
			g_HashIndexMap.insert(std::make_pair(hash, iOffset));
		}
	}

	std::string HashToString(obuint32 _hash)
	{
		HashIndexMap::iterator it = g_HashIndexMap.find(_hash);
		if(it != g_HashIndexMap.end())
		{
			if(it->second < g_StringRepository.size())
			{
				return &g_StringRepository[it->second];
			}
		}
		return std::string(va("%x", _hash));
	}

	bool IsWhiteSpace(const char _ch)
	{
		const obuint8 cr = 0x0D;
		const obuint8 lf = 0x0A;
		return
			_ch == cr ||
			_ch == lf ||
			_ch == '\t' ||
			_ch == '\n' ||
			_ch == ' ';
	}

	void Tokenize(const std::string &_s, const std::string &_separators, StringVector &_tokens)
	{
		size_t pos_start, pos_end;

		pos_start = _s.find_first_not_of(_separators);
		if (pos_start == std::string::npos)
			return;

		pos_end = _s.find_first_of(_separators, pos_start);
		if (pos_end == std::string::npos)
		{
			_tokens.push_back(_s);
			return;
		}
		_tokens.push_back(_s.substr(pos_start, pos_end - pos_start));

		while ((pos_start = _s.find_first_not_of (_separators, pos_end)) != std::string::npos)
		{
			pos_end = _s.find_first_of(_separators, pos_start);
			if (pos_end == std::string::npos) {
				_tokens.push_back(_s.substr (pos_start));
				return;
			}
			_tokens.push_back(_s.substr(pos_start, pos_end - pos_start));
		}
	}

	const char *VarArgs(char *_outbuffer, int _buffsize, const char* _msg, ...)
	{
		va_list list;
		va_start(list, _msg);
#ifdef WIN32
		_vsnprintf(_outbuffer, _buffsize, _msg, list);
#else
		vsnprintf(_outbuffer, _buffsize, _msg, list);
#endif
		va_end(list);
		return _outbuffer;
	}

	void OutputDebugBasic(eMessageType _type, const char* _msg)
	{
#ifdef WIN32
		OutputDebugString(_msg);
		OutputDebugString("\n");
#endif
#ifdef ENABLE_DEBUG_WINDOW
		DW.Log.AddLine(_type,_msg);
#endif
	}
	void OutputDebug(eMessageType _type, const char* _msg, ...)
	{
		enum { BufferSize=2048 };
		char buffer[BufferSize] = {0};
		va_list list;
		va_start(list, _msg);
#ifdef WIN32
		_vsnprintf(buffer, BufferSize, _msg, list);
#else
		vsnprintf(buffer, BufferSize, _msg, list);
#endif
		va_end(list);
		OutputDebugBasic(_type,buffer);
	}

	int GetLocalGameId()
	{
		return g_EngineFuncs->IDFromEntity(GetLocalEntity());
	}

	GameEntity GetLocalEntity()
	{
		return g_EngineFuncs->GetLocalGameEntity();
	}

	bool GetLocalPosition(Vector3f &_pos)
	{
		return EngineFuncs::EntityPosition(GetLocalEntity(), _pos);
	}

	bool GetLocalGroundPosition(Vector3f &_pos, int _tracemask)
	{
		obTraceResult tr;
		Vector3f vPos;
		if(GetLocalEyePosition(vPos))
		{
			EngineFuncs::TraceLine(tr, vPos, vPos - Vector3f::UNIT_Z * 4096.f,
				NULL, _tracemask, GetLocalGameId(), False);

			if(tr.m_Fraction < 1.f)
			{
				_pos = Vector3f( tr.m_Endpos );
				return true;
			}
		}
		return false;
	}

	bool GetLocalGroundPosition(Vector3f &_pos, Vector3f *_normal, int _tracemask /*= TR_MASK_FLOODFILL*/)
	{
		obTraceResult tr;
		Vector3f vPos;
		if(GetLocalEyePosition(vPos))
		{
			EngineFuncs::TraceLine(tr, vPos, vPos - Vector3f::UNIT_Z * 4096.f,
				NULL, _tracemask, GetLocalGameId(), False);

			if(tr.m_Fraction < 1.f)
			{
				_pos = Vector3f( tr.m_Endpos );
				if(_normal)
					*_normal = Vector3f( tr.m_Normal );
				return true;
			}
		}
		return false;
	}

	bool GetLocalEyePosition(Vector3f &_pos)
	{
		return EngineFuncs::EntityEyePosition(GetLocalEntity(), _pos);
	}

	bool GetLocalFacing(Vector3f &_face)
	{
		return EngineFuncs::EntityOrientation(GetLocalEntity(), _face, NULL, NULL);
	}

	bool GetLocalAABB(AABB &_aabb)
	{
		return EngineFuncs::EntityWorldAABB(GetLocalEntity(), _aabb);
	}

	bool GetLocalAimPoint(Vector3f &_pos, Vector3f *_normal, int _tracemask /*= TR_MASK_FLOODFILL*/, int * _contents, int * _surface)
	{
		if(_contents)
			*_contents = 0;
		if(_surface)
			*_surface = 0;

		obTraceResult tr;
		Vector3f vNewStart, vPos, vFace;
		if(GetLocalEyePosition(vPos) &&
			GetLocalFacing(vFace) &&
			GetNearestNonSolid(vNewStart, vPos, vPos + vFace * 4096.f, _tracemask))
		{
			EngineFuncs::TraceLine(tr, vNewStart, vNewStart + vFace * 4096.f,
				NULL, _tracemask, GetLocalGameId(), False);

			if(tr.m_Fraction < 1.f)
			{
				_pos = Vector3f( tr.m_Endpos );
				if(_normal)
					*_normal = Vector3f( tr.m_Normal );
				if(_contents)
					*_contents = tr.m_Contents;
				if(_surface)
					*_surface = tr.m_Surface;
				return true;
			}
		}
		return false;
	}

	bool GetNearestNonSolid(Vector3f &_pos, const Vector3f &_start, const Vector3f &_end, int _tracemask /*= TR_MASK_FLOODFILL*/)
	{
		obTraceResult tr;
		Vector3f vStart = _start;
		Vector3f vEnd = _end;
		Vector3f vDir = vEnd - vStart;
		float fLength = vDir.Normalize();

		const float fStepSize = 32.f;
		while(fLength > 0.f)
		{
			EngineFuncs::TraceLine(tr, vStart, vEnd,
				NULL, _tracemask, GetLocalGameId(), False);

			if(!tr.m_StartSolid)
			{
				_pos = vStart;
				return true;
			}

			vStart += vDir * fStepSize;
			fLength -= fStepSize;
		}
		return false;
	}

	std::string FormatByteString(obuint64 _bytes)
	{
		const char * byteUnits[] =
		{
			" bytes",
			" KB",
			" MB",
			" GB",
			" TB"
		};

		obuint64 iNumUnits = sizeof(byteUnits) / sizeof(byteUnits[0]);
		int iUnit = 0;
		for(int i = 1; i < iNumUnits; ++i)
		{
			if(_bytes / pow(1024.0, i) >= 1)
				iUnit = i;
		}

		OBASSERT(iUnit >= 0 && iUnit < iNumUnits, "Out of Bounds!");

		std::stringstream str;
		str << (iUnit > 0 ? (_bytes / pow(1024.0, iUnit)) : _bytes) << byteUnits[iUnit];

		return str.str();
	}

	void GetAABBBoundary(const AABB &_aabb, Vector3List &_list)
	{
		Vector3f vVertex[8] = { Vector3f::ZERO };

		vVertex[0] = Vector3f(_aabb.m_Mins[0], _aabb.m_Mins[1], _aabb.m_Mins[2]);
		vVertex[1] = Vector3f(_aabb.m_Maxs[0], _aabb.m_Mins[1], _aabb.m_Mins[2]);
		vVertex[2] = Vector3f(_aabb.m_Maxs[0], _aabb.m_Maxs[1], _aabb.m_Mins[2]);
		vVertex[3] = Vector3f(_aabb.m_Mins[0], _aabb.m_Maxs[1], _aabb.m_Mins[2]);

		vVertex[4] = Vector3f(_aabb.m_Mins[0], _aabb.m_Mins[1], _aabb.m_Maxs[2]);
		vVertex[5] = Vector3f(_aabb.m_Maxs[0], _aabb.m_Mins[1], _aabb.m_Maxs[2]);
		vVertex[6] = Vector3f(_aabb.m_Maxs[0], _aabb.m_Maxs[1], _aabb.m_Maxs[2]);
		vVertex[7] = Vector3f(_aabb.m_Mins[0], _aabb.m_Maxs[1], _aabb.m_Maxs[2]);

		_list.push_back(vVertex[0]);
		_list.push_back(vVertex[1]);
		_list.push_back(vVertex[2]);
		_list.push_back(vVertex[3]);
	}

	const char *FindClassName(obint32 _classId)
	{
		return IGameManager::GetInstance()->GetGame()->FindClassName(_classId);
	}

	obint32 GetRoleMask(const std::string &_name)
	{
		gmMachine *pMachine = ScriptManager::GetInstance()->GetMachine();
		gmTableObject *pTbl = pMachine->GetGlobals()->Get(pMachine,"Role").GetTableObjectSafe();
		if(pTbl)
		{
			gmTableIterator it;
			gmTableNode *pNode = pTbl->GetFirst(it);
			while(pNode)
			{
				const char *pName = pNode->m_key.GetCStringSafe(0);
				if(pName && pNode->m_value.IsInt() && !StringCompareNoCase(_name,pName))
				{
					return pNode->m_value.GetInt();
				}
				pTbl->GetNext(it);
			}
		}
		return 0;
	}
	std::string BuildRoleName(obint32 _mask)
	{
		if(_mask==0)
			return "None";

		bool AllRoles = true;
		std::string ret;

		int EnumSize = 0;
		const IntEnum *Enum = 0;
		IGameManager::GetInstance()->GetGame()->GetRoleEnumeration(Enum,EnumSize);

		for(int r = 0; r < EnumSize; ++r)
		{
			if((1<<Enum[r].m_Value) & _mask)
			{
				ret += Enum[r].m_Key;
				ret += " ";
			}
			else
			{
				AllRoles = false;
			}
		}

		if(AllRoles)
			return "All Roles";
		return ret;
	}

	bool ToLocalSpace(GameEntity _ent, const Vector3f &_worldpos, Vector3f &_out)
	{
		Vector3f vPos, vForward, vRight, vUp;
		if(EngineFuncs::EntityPosition(_ent, vPos) &&
			EngineFuncs::EntityOrientation(_ent, vForward, vRight, vUp))
		{
			Matrix3f mTransform(vRight, vForward, vUp, true);
			mTransform.Inverse();
			_out = (_worldpos - vPos) * mTransform;
			return true;
		}
		return false;
	}

	bool ToWorldSpace(GameEntity _ent, const Vector3f &_localpos, Vector3f &_out)
	{
		Vector3f vPos, vForward, vRight, vUp;
		if(EngineFuncs::EntityPosition(_ent, vPos) &&
			EngineFuncs::EntityOrientation(_ent, vForward, vRight, vUp))
		{
			Matrix3f mTransform(vRight, vForward, vUp, true);
			//Vector3f vOffset = mTransform.Inverse() * _localpos;

			_out = vPos + (mTransform * _localpos);
			return true;
		}

		return false;
	}

	Vector3f PredictFuturePositionOfTarget(const Vector3f &_tgpos, const Vector3f &_tgvel, float _timeahead)
	{
		return _tgpos + (_tgvel * _timeahead);
	}

	Vector3f PredictFuturePositionOfTarget(
		const Vector3f &_mypos,
		float _projspeed,
		const Vector3f &_tgpos,
		const Vector3f &_tgvel)
	{
		//if the target is ahead and facing the agent shoot at its current pos
		Vector3f vToEnemy = _tgpos - _mypos;

		//the lookahead time is proportional to the distance between the enemy
		//and the pursuer; and is inversely proportional to the sum of the
		//agent's velocities
		float fLookAheadTime = vToEnemy.Length() / (_projspeed + _tgvel.Length());

		//return the predicted future position of the enemy
		return _tgpos + (_tgvel * fLookAheadTime);
	}

	Vector3f PredictFuturePositionOfTarget(
		const Vector3f &_mypos,
		float _projspeed,
		const TargetInfo &_tg,
		const Vector3f &_extravelocity,
		float _minleaderror,
		float _maxleaderror)
	{
		//if the target is ahead and facing the agent shoot at its current pos
		Vector3f vToEnemy = _tg.m_LastPosition - _mypos;

		//the lookahead time is proportional to the distance between the enemy
		//and the pursuer; and is inversely proportional to the sum of the
		//agent's velocities
		float fLookAheadTime = vToEnemy.Length() / (_projspeed + _tg.m_LastVelocity.Length());

		if(_minleaderror != _maxleaderror)
			fLookAheadTime += Mathf::IntervalRandom(_minleaderror,_maxleaderror);
		else
			fLookAheadTime += _minleaderror;

		//return the predicted future position of the enemy
		Vector3f vPredictedPos = _tg.m_LastPosition + ((_tg.m_LastVelocity-_extravelocity) * fLookAheadTime);

		// Compensate for airborne target?
		//if(!_tg.m_EntityFlags.CheckFlag(ENT_FLAG_ONGROUND))
		//{
		//	//vPredictedPos.Z() -= _tg.m_LastVelocity.Z() + IGame::GetGravity() * fLookAheadTime;
		//	vPredictedPos.Z() = Trajectory::HeightForTrajectory(
		//		vPredictedPos,
		//		_tg.m_LastVelocity.Z(),
		//		IGame::GetGravity(),
		//		fLookAheadTime);
		//}
		return vPredictedPos;
	}

	bool InFieldOfView(const Vector3f &_face1, const Vector3f &_face2, float _fovAngles)
	{
		const float fFovInRadians = Mathf::DEG_TO_RAD * _fovAngles;
		return (_face1.Dot(_face2) >= cosf(fFovInRadians/2.0f) * _face1.Length() * _face2.Length());
	}

	bool InFieldOfView2d(const Vector3f &_face1, const Vector3f &_face2, float _fovAngles)
	{
		const Vector2f v1( _face1.X(), _face1.Y() );
		const Vector2f v2( _face2.X(), _face2.Y() );

		float fFovInRadians = Mathf::DEG_TO_RAD * _fovAngles;
		return (v1.Dot(v2) >= cosf(fFovInRadians/2.0f) * v1.Length() * v2.Length());
	}

	float AngleBetween(const Vector3f &_v1, const Vector3f &_v2)
	{
		return Mathf::ACos(_v1.Dot(_v2));
	}

	Vector3f AveragePoint(const Vector3List &_list)
	{
		Vector3f vAvg = Vector3f::ZERO;

		if(!_list.empty())
		{
			for(obuint32 v = 0; v < _list.size(); ++v)
			{
				vAvg += _list[v];
			}

			vAvg /= (float)_list.size();
		}
		return vAvg;
	}

	float ClosestPointOfApproachTime(const Vector3f& aP1, const Vector3f& aV1, const Vector3f& aP2, const Vector3f& aV2)
	{
		Vector3f dv = aV1 - aV2;
		float dv2 = dv.Dot(dv);
		if(dv2 < Mathf::EPSILON)
			return 0.0;
		return -(aP1 - aP2).Dot(dv) / dv2;
	}

	void StringTrimCharacters(std::string &_out, const std::string &_trim)
	{
		obuint32 i;
		for(obuint32 t = 0; t < _trim.size(); ++t)
		{
			while((i = (obuint32)_out.find(_trim[t])) != _out.npos)
				_out.erase(i, 1);
		}
	}

	bool StringToTrue(const std::string &_str)
	{
		return (_str == "1" || _str == "on" || _str == "true");
	}

	bool StringToFalse(const std::string &_str)
	{
		return (_str == "0" || _str == "off" || _str == "false");
	}

	bool AssertFunction(bool _bexp, const char* _exp, const char* _file, int _line, const char *_msg, ...)
	{
		if(!_bexp)
		{
			enum { BufferSize=2048 };
			char buffer[BufferSize] = {0};
			va_list list;
			va_start(list, _msg);
#ifdef WIN32
			_vsnprintf(buffer, BufferSize, _msg, list);
#else
			vsnprintf(buffer, BufferSize, _msg, list);
#endif
			va_end(list);

#ifdef WIN32
			char strBigBuffer[BufferSize] = {};
			sprintf(strBigBuffer, "Assertion: %s\n%s\n%s : %d\nAbort to break\nRetry to continue\nIgnore to ignore this assert",
				_exp, buffer, _file, _line);
			int iRes = MessageBox(NULL, strBigBuffer, "Omni-bot: Assertion Failed", MB_ABORTRETRYIGNORE | MB_ICONWARNING);
			if(iRes == IDABORT)
				DebugBreak();
			return iRes != IDIGNORE;
#else
			vsnprintf(buffer, 2048, _msg, list);
			assert(_bexp /*&& buffer*/); //cs: gcc - the address of �buffer� will always evaluate as �true�
			return true;
#endif
		}
		return true;
	}

	bool SoftAssertFunction(AssertMode _mode, bool _bexp, const char* _exp, const char* _file, int _line, const char *_msg, ...)
	{
		if(!_bexp)
		{
			enum { BufferSize=2048 };
			char buffer[BufferSize] = {0};
			va_list list;
			va_start(list, _msg);
#ifdef WIN32
			_vsnprintf(buffer, BufferSize, _msg, list);
#else
			vsnprintf(buffer, BufferSize, _msg, list);
#endif
			va_end(list);

			char strBigBuffer[BufferSize] = {};
			sprintf(strBigBuffer, "--------------------\nAssertion: %s\n%s\n%s : %d\n--------------------\n",
				_exp, buffer, _file, _line);
			/*int iRes = MessageBox(NULL, strBigBuffer, "Omni-bot: Assertion Failed", MB_ABORTRETRYIGNORE | MB_ICONWARNING);
			if(iRes == IDABORT)
			DebugBreak();*/
#ifdef _WIN32
			OutputDebugString(strBigBuffer);
			return _mode != FireOnce;
#else
			std::cout << strBigBuffer;
			return false;
#endif
		}
		return true;
	}

	std::string FindOpenPlayerName()
	{
		// FIX THIS! CHECK WITH ENGINE
		static int nextIndex = 0;
		return std::string(va("OmniBot[%i]", nextIndex++));
	}

	bool TestSegmentForOcclusion(const Segment3f &seg)
	{
		AABB b(Vector3f(-32,-32,0), Vector3f(32,32,64));

		obTraceResult tr;
		EngineFuncs::TraceLine( tr, seg.Center, seg.Center, &b, TR_MASK_FLOODFILL, -1, False );
		if ( tr.m_Fraction < 0.f )
			return true;

		return false;
	}

	bool GetSegmentOverlap(const Segment3f &_seg1, const Segment3f &_seg2, Segment3f &out)
	{
		static float DotThreshold = -0.98f;
		//static float DistanceThreshold = 8.f;

		// TODO: move these out.
		static float MinOverlapWidth = 10.f;
		static float MaxStepHeight = 20.f;
		static float MaxDropHeight = 32.f;

		static float MaxHorizontalDist = 32.f;

		// make sure the direction is proper
		const float fDot = _seg1.Direction.Dot(_seg2.Direction);
		if(fDot > DotThreshold)
			return false;

		DistPoint3Segment3f dist( _seg1.Center, Segment3f( _seg2.P0,_seg2.P1 ) );
		dist.Get();

		Vector3f cp =
			dist.GetSegment().Center +
			dist.GetSegmentParameter() * dist.GetSegment().Direction;

		// make sure the lines 'overlap' on the same line
		const float f2dDist = Length2d(cp,_seg1.Center);

		const float fStepDist = (cp.Z() - _seg1.Center.Z());
		if(f2dDist > MaxHorizontalDist)
			return false;
		if(fStepDist > MaxStepHeight)
			return false;
		if(fStepDist < -MaxDropHeight)
			return false;

		// make sure they overlap in segment space
		const float Dist = Length( _seg1.Center, _seg2.Center );
		if(Dist > (_seg1.Extent + _seg2.Extent))
			return false;

		Vector3f vMin = _seg1.Center,vMax = _seg2.Center;

		float fT = 0.f;
		//////////////////////////////////////////////////////////////////////////

		fT = ClosestPtOnLine_Unclamped(_seg1.P0,_seg1.P1,_seg2.P0,cp);
		if(fT > 1.f)
		{
			vMax = _seg1.P1;
		}
		else if(fT >= 0.f)
		{
			vMax = _seg2.P0;
		}
		else
			OBASSERT(0,"Unexpected");

		fT = ClosestPtOnLine_Unclamped(_seg1.P0,_seg1.P1,_seg2.P1,cp);
		if(fT < 0.f)
		{
			vMin = _seg1.P0;
		}
		else if(fT <= 1.f)
		{
			vMin = _seg2.P1;
		}
		else
			OBASSERT(0,"Unexpected");
		//////////////////////////////////////////////////////////////////////////
		/*fT = ClosestPtOnLine_Unclamped(_seg2.P1,_seg2.P0,_seg1.P1,cp);
		if(fT > 1.f)
		{
		vMax = _seg1.P1;
		}
		else if(fT > 0.f)
		{
		vMax = _seg2.P0;
		}
		else
		OBASSERT(0,"Unexpected");

		fT = ClosestPtOnLine_Unclamped(_seg2.P1,_seg2.P0,_seg1.P1,cp);
		if(fT < 0.f)
		{
		vMin = _seg2.P0;
		}
		else if(fT < 1.f)
		{
		vMin = _seg1.P1;
		}
		else
		OBASSERT(0,"Unexpected");*/
		//////////////////////////////////////////////////////////////////////////

		out = Segment3f(vMin,vMax);

		if(out.Extent * 2.f < MinOverlapWidth)
			return false;

		/*Vector3f cp;

		const fT1p = Utils::ClosestPtOnLine(
		_seg2.P1,
		_seg2.P0,
		_seg1.P1,
		cp);
		const fT1e = Utils::ClosestPtOnLine(
		_seg2.P1,
		_seg2.P0,
		_seg1.P0,
		cp);
		const fT2p = Utils::ClosestPtOnLine(
		_seg1.P1,
		_seg1.P0,
		_seg2.P1,
		cp);
		const fT2e = Utils::ClosestPtOnLine(
		_seg1.P1,
		_seg1.P0,
		_seg2.P0,
		cp);*/

		return true;
	}

	float ClosestPtOnLine(const Vector3f& p1, const Vector3f& p2, const Vector3f& p, Vector3f &cp)
	{
		Vector3f norm = p2 - p1;
		float len = norm.Normalize();
		float t = norm.Dot(p - p1);
		if(t <= 0.f || len <= Mathf::EPSILON)
		{
			t = 0.f;
			cp = p1;
		}
		else if(t >= len)
		{
			t = 1.f;
			cp = p2;
		}
		else
		{
			cp = p1 + norm * t;
			t /= len;
		}
		return t;
	}

	float ClosestPtOnLine_Unclamped(const Vector3f& p1, const Vector3f& p2, const Vector3f& p, Vector3f &cp)
	{
		Vector3f norm = p2 - p1;
		float len = norm.Normalize();
		float t = norm.Dot(p - p1);

		cp = p1 + norm * t;
		t /= len;
		return t;
	}

	Vector3f ChangePitch(const Vector3f &fwd, float _pitchangle)
	{
		float fHeading = fwd.XYHeading();
		Vector3f vNewFacing;
		vNewFacing.FromSpherical(fHeading, Mathf::DegToRad(_pitchangle), 1.f);
		return vNewFacing;
	}

	bool ClosestPtOnLine(const Vector3List &list, const Vector3f &pos, Vector3f &cp, bool loop)
	{
		if(list.size() < 2)
			return false;

		Vector3f vClosestPt, vPtOnLine;
		float fClosestDist = Utils::FloatMax;
		for(obuint32 v = 0; v < list.size()-1; ++v)
		{
			Utils::ClosestPtOnLine(list[v], list[v+1],pos,vPtOnLine);

			float fDist = SquaredLength(pos,vPtOnLine);
			if(fDist < fClosestDist)
			{
				fClosestDist = fDist;
				vClosestPt = vPtOnLine;
			}
		}

		if(loop)
		{
			Utils::ClosestPtOnLine(list.front(), list.back(),pos,vPtOnLine);
			float fDist = SquaredLength(pos,vPtOnLine);
			if(fDist < fClosestDist)
			{
				fClosestDist = fDist;
				vClosestPt = vPtOnLine;
			}
		}

		cp =  vClosestPt;
		return true;
	}

	Vector3List CreatePolygon(const Vector3f &_pos, const Vector3f &_normal, float _size)
	{
		Vector3List vl;

		Plane3f p = Plane3f(_normal,_pos);

		Quaternionf q;
		q.FromAxisAngle(_normal, Mathf::DegToRad(90.f));

		Vector3f v = _normal.Perpendicular();
		v *= _size;

		vl.push_back(_pos+v);
		for(int i = 0; i < 3; ++i)
		{
			v = q.Rotate(v);
			vl.push_back(_pos+v);
		}
		return vl;
	}

	Vector3List ClipPolygonToPlanes( const Vector3List & poly, const Plane3f & plane )
	{
		Vector3List newlist = poly;

		if ( newlist.size() > 2 )
		{
			int numVerts = newlist.size();

			// reserve extra space
			newlist.resize( 64 );

			ClipConvexPolygonAgainstPlane(
				-plane.Normal,
				-plane.Constant,
				numVerts,
				&newlist[ 0 ] );

			newlist.resize( numVerts );
		}
		else
		{
			newlist.resize( 0 );
		}
		return newlist;
	}

	//float DistancePointToLineSqr(const Vector3f &_point,
	//	const Vector3f &_pt0,
	//	const Vector3f &_pt1,
	//	Vector3f *_linePt)
	//{
	//	Vector3f v = _point - _pt0;
	//	Vector3f s = _pt1 - _pt0;
	//	float lenSq = s.SquaredLength();
	//	float dot = v.Dot(s) / lenSq;
	//	Vector3f disp = s * dot;
	//	if (_linePt)
	//		*_linePt = _pt0 + disp;
	//	v -= disp;
	//	return v.SquaredLength();
	//}

	//float DistancePointToLine(const Vector3f &_point,
	//	const Vector3f &_pt0,
	//	const Vector3f &_pt1,
	//	Vector3f *_linePt)
	//{
	//	return Mathf::Sqrt( DistancePointToLineSqr(_point, _pt0, _pt1, _linePt) );
	//}

	//float DistancePointToTri(const Vector3f &_point,
	//	const Vector3f &_pt0,
	//	const Vector3f &_pt1,
	//	const Vector3f &_pt2,
	//	Vector3f *_pt)
	//{
	//	// Compute vectors
	//	const Vector3f v0 = _pt2 - _pt0;
	//	const Vector3f v1 = _pt1 - _pt0;
	//	const Vector3f v2 = _point - _pt0;

	//	// Compute dot products
	//	const float dot00 = v0.Dot(v0);
	//	const float dot01 = v0.Dot(v1);
	//	const float dot02 = v0.Dot(v2);
	//	const float dot11 = v1.Dot(v1);
	//	const float dot12 = v1.Dot(v2);

	//	// Compute barycentric coordinates
	//	const float invDenom = 1 / (dot00 * dot11 - dot01 * dot01);
	//	const float u = (dot11 * dot02 - dot01 * dot12) * invDenom;
	//	const float v = (dot00 * dot12 - dot01 * dot02) * invDenom;

	//	// Check if point is in triangle
	//	return (u >= 0) && (v >= 0) && (u + v < 1);
	//}

	//////////////////////////////////////////////////////////////////////////
	// inSegment(): determine if a point is inside a segment
	//    Input:  a point P, and a collinear segment S
	//    Return: 1 = P is inside S
	//            0 = P is not inside S
	int inSegment(Vector3f P, Segment3f _S)
	{
		if (_S.P0.X() != _S.P1.X()) {    // S is not vertical
			if (_S.P0.X() <= P.X() && P.X() <= _S.P1.X())
				return 1;
			if (_S.P0.X() >= P.X() && P.X() >= _S.P1.X())
				return 1;
		}
		else {    // S is vertical, so test y coordinate
			if (_S.P0.Y() <= P.Y() && P.Y() <= _S.P1.Y())
				return 1;
			if (_S.P0.Y() >= P.Y() && P.Y() >= _S.P1.Y())
				return 1;
		}
		return 0;
	}

	inline float dot(const Vector3f &u,const Vector3f &v)
	{
		return ((u).X() * (v).X() + (u).Y() * (v).Y() + (u).Z() * (v).Z());
	}
	inline float perp(const Vector3f &u,const Vector3f &v)
	{
		return ((u).X() * (v).Y() - (u).Y() * (v).X());
	}

	// intersect2D_2Segments(): the intersection of 2 finite 2D segments
	//    Input:  two finite segments S1 and S2
	//    Output: *I0 = intersect point (when it exists)
	//            *I1 = endpoint of intersect segment [I0,I1] (when it exists)
	//    Return: 0=disjoint (no intersect)
	//            1=intersect in unique point I0
	//            2=overlap in segment from I0 to I1
	int intersect2D_Segments(const Segment3f &S1,const Segment3f &S2, Vector3f* I0, Vector3f* I1 )
	{
		Vector3f    u = S1.P1 - S1.P0;
		Vector3f    v = S2.P1 - S2.P0;
		Vector3f    w = S1.P0 - S2.P0;
		float		D = perp(u,v);

		// test if they are parallel (includes either being a point)
		if (fabs(D) < Mathf::EPSILON)
		{          // S1 and S2 are parallel
			if (perp(u,w) != 0 || perp(v,w) != 0)
			{
				return 0;                   // they are NOT collinear
			}
			// they are collinear or degenerate
			// check if they are degenerate points
			float du = dot(u,u);
			float dv = dot(v,v);
			if (du==0 && dv==0)
			{           // both segments are points
				if (S1.P0 != S2.P0)         // they are distinct points
					return 0;
				*I0 = S1.P0;                // they are the same point
				return 1;
			}
			if (du==0)
			{                    // S1 is a single point
				if (inSegment(S1.P0, S2) == 0)  // but is not in S2
					return 0;
				*I0 = S1.P0;
				return 1;
			}
			if (dv==0)
			{                    // S2 a single point
				if (inSegment(S2.P0, S1) == 0)  // but is not in S1
					return 0;
				*I0 = S2.P0;
				return 1;
			}
			// they are collinear segments - get overlap (or not)
			float t0, t1;                   // endpoints of S1 in eqn for S2
			Vector3f w2 = S1.P1 - S2.P0;
			if (v.X() != 0)
			{
				t0 = w.X() / v.X();
				t1 = w2.X() / v.X();
			}
			else
			{
				t0 = w.Y() / v.Y();
				t1 = w2.Y() / v.Y();
			}
			if (t0 > t1)
			{                  // must have t0 smaller than t1
				float t=t0; t0=t1; t1=t;    // swap if not
			}
			if (t0 > 1 || t1 < 0)
			{
				return 0;     // NO overlap
			}
			t0 = t0<0? 0 : t0;              // clip to min 0
			t1 = t1>1? 1 : t1;              // clip to max 1
			if (t0 == t1)
			{                 // intersect is a point
				*I0 = S2.P0 + t0 * v;
				return 1;
			}

			// they overlap in a valid subsegment
			*I0 = S2.P0 + t0 * v;
			*I1 = S2.P0 + t1 * v;
			return 2;
		}

		// the segments are skew and may intersect in a point
		// get the intersect parameter for S1
		float     sI = perp(v,w) / D;

		// no intersect with S1
		if (sI < 0 || sI > 1)
			return 0;

		// get the intersect parameter for S2
		float     tI = perp(u,w) / D;

		// no intersect with S2
		if (tI < 0.f)
		{
			*I0 = S2.P0;
			return 1;
		}
		if (tI > 1.f)
		{
			*I0 = S2.P1;
			return 1;
		}

		*I0 = S1.P0 + sI * u;               // compute S1 intersect point
		return 1;
	}

	gmVariable UserDataToGmVar(gmMachine *_machine, const obUserData &bud)
	{
		DisableGCInScope gcEn(_machine);

		switch(bud.DataType)
		{
		case obUserData::dtNone:
			return gmVariable::s_null;
		case obUserData::dtVector:
			return gmVariable(bud.GetVector()[0], bud.GetVector()[1], bud.GetVector()[2]);
		case obUserData::dtString:
			return gmVariable(_machine->AllocStringObject(bud.GetString() ? bud.GetString() : ""));
		case obUserData::dtInt:
			return gmVariable(bud.GetInt());
			break;
		case obUserData::dtFloat:
			return gmVariable(bud.GetFloat());
			break;
		case obUserData::dtEntity:
			{
				gmVariable v;
				v.SetEntity(bud.GetEntity().AsInt());
				return v;
			}
		case obUserData::dt3_4byteFlags:
			{
				gmTableObject *pTbl = _machine->AllocTableObject();
				for(int i = 0; i < 3; ++i)
					pTbl->Set(_machine, i, gmVariable(bud.Get4ByteFlags()[i]));
				return gmVariable(pTbl);
			}
		case obUserData::dt3_Strings:
			{
				gmTableObject *pTbl = _machine->AllocTableObject();
				for(int i = 0; i < 3; ++i)
					if(bud.GetStrings(i))
						pTbl->Set(_machine, i, gmVariable(_machine->AllocStringObject(bud.GetStrings(i))));
				return gmVariable(pTbl);
			}
		case obUserData::dt6_2byteFlags:
			{
				gmTableObject *pTbl = _machine->AllocTableObject();
				for(int i = 0; i < 6; ++i)
					pTbl->Set(_machine, i, gmVariable(bud.Get2ByteFlags()[i]));
				return gmVariable(pTbl);
			}
		case obUserData::dt12_1byteFlags:
			{
				gmTableObject *pTbl = _machine->AllocTableObject();
				for(int i = 0; i < 12; ++i)
					pTbl->Set(_machine, i, gmVariable(bud.Get1ByteFlags()[i]));
				return gmVariable(pTbl);
			}
		};
		return gmVariable::s_null;
	}
};

//////////////////////////////////////////////////////////////////////////

va::va(const char* msg, ...)
{
	va_list list;
	va_start(list, msg);
#ifdef WIN32
	_vsnprintf(buffer, BufferSize, msg, list);
#else
	vsnprintf(buffer, BufferSize, msg, list);
#endif
	va_end(list);
}

//////////////////////////////////////////////////////////////////////////

filePath::filePath()
{
	buffer[ 0 ] = 0;
}

filePath::filePath(const char* msg, ...)
{
	va_list list;
	va_start(list, msg);
#ifdef WIN32
	_vsnprintf(buffer, BufferSize, msg, list);
#else
	vsnprintf(buffer, BufferSize, msg, list);
#endif
	va_end(list);
	FixPath();
}

std::string filePath::FileName() const
{
	const char * fileName = buffer;
	const char *pC = buffer;
	while(*pC != '\0')
	{
		if(*pC == '\\' || *pC == '/')
			fileName = pC+1;
		++pC;
	}
	return fileName;
}

void filePath::FixPath()
{
	// unixify the path slashes
	char *pC = buffer;
	while(*pC != '\0')
	{
		if(*pC == '\\')
			*pC = '/';
		++pC;
	}

	// trim any trailing slash
	while(*pC == '/' && pC > buffer)
	{
		*pC = '\0';
		--pC;
	}
}

std::ostream& operator <<(std::ostream& _o, const filePath& _filePath) {
	_o << _filePath.c_str();
	return _o;
}

//////////////////////////////////////////////////////////////////////////

bool PropertyMap::AddProperty(const std::string &_name, const std::string &_data)
{
	if(_name.empty())
	{
		LOGERR("Invalid Waypoint Property Name or Data");
		return false;
	}

	// remove the old, case insensitive version
	ValueMap::iterator iter = m_Properties.begin();
	for(; iter != m_Properties.end(); ++iter)
	{
		if(!Utils::StringCompareNoCase(iter->first,_name))
		{
			m_Properties.erase(iter);
			break;
		}
	}

	m_Properties.insert(std::make_pair(_name, _data));
	return true;
}

void PropertyMap::DelProperty(const std::string &_name)
{
	ValueMap::iterator iter = m_Properties.find(_name);
	if(iter != m_Properties.end())
		m_Properties.erase(iter);
}

std::string PropertyMap::GetProperty(const std::string &_name) const
{
	ValueMap::const_iterator iter = m_Properties.find(_name);
	return (iter != m_Properties.end()) ? iter->second : std::string();
}

void PropertyMap::GetAsKeyVal(KeyVals &kv)
{
	PropertyMap::ValueMap::const_iterator cIt = m_Properties.begin();
	for(;cIt != m_Properties.end(); ++cIt)
	{
		kv.SetString(cIt->first.c_str(),cIt->second.c_str());
	}
}

std::ostream& operator <<(std::ostream& _o, const obUserData_t& _bud)
{
	_o << "UserData(";
	switch(_bud.DataType)
	{
	case obUserData_t::dtNone:
		_o << "dtNone";
		break;
	case obUserData_t::dtVector:
		_o << "dtVector, " <<
			_bud.udata.m_Vector[0] <<  ", " <<
			_bud.udata.m_Vector[1] <<  ", " <<
			_bud.udata.m_Vector[2];
		break;
	case obUserData_t::dtString:
		if(_bud.udata.m_String) _o << "dtString, " << _bud.udata.m_String;
		break;
	case obUserData_t::dtInt:
		_o << "dtInt, " << _bud.udata.m_Int;
		break;
	case obUserData_t::dtFloat:
		_o << "dtFloat, " << _bud.udata.m_Float;
		break;
	case obUserData_t::dtEntity:
		_o << "dtEntity, " << _bud.udata.m_Entity;
		break;
	case obUserData_t::dt3_4byteFlags:
		_o << "dt3_4byteFlags, " <<
			_bud.udata.m_4ByteFlags[0] << ", " <<
			_bud.udata.m_4ByteFlags[1] << ", " <<
			_bud.udata.m_4ByteFlags[2];
		break;
	case obUserData_t::dt3_Strings:
		_o << "dt3_Strings";
		if(_bud.udata.m_CharPtrs[0]) _o << ", " << _bud.udata.m_CharPtrs[0];
		if(_bud.udata.m_CharPtrs[1]) _o << ", " <<  _bud.udata.m_CharPtrs[1];
		if(_bud.udata.m_CharPtrs[2]) _o << ", " << _bud.udata.m_CharPtrs[2];
		break;
	case obUserData_t::dt6_2byteFlags:
		_o << "dt6_2byteFlags, " <<
			_bud.udata.m_2ByteFlags[0] << ", " <<
			_bud.udata.m_2ByteFlags[1] << ", " <<
			_bud.udata.m_2ByteFlags[2] << ", " <<
			_bud.udata.m_2ByteFlags[3] << ", " <<
			_bud.udata.m_2ByteFlags[4] << ", " <<
			_bud.udata.m_2ByteFlags[5];
		break;
	case obUserData_t::dt12_1byteFlags:
		_o << "dt12_1byteFlags, " <<
			(int)_bud.udata.m_1ByteFlags[0] << ", " <<
			(int)_bud.udata.m_1ByteFlags[1] << ", " <<
			(int)_bud.udata.m_1ByteFlags[2] << ", " <<
			(int)_bud.udata.m_1ByteFlags[3] << ", " <<
			(int)_bud.udata.m_1ByteFlags[4] << ", " <<
			(int)_bud.udata.m_1ByteFlags[5] << ", " <<
			(int)_bud.udata.m_1ByteFlags[6] << ", " <<
			(int)_bud.udata.m_1ByteFlags[7] << ", " <<
			(int)_bud.udata.m_1ByteFlags[8] << ", " <<
			(int)_bud.udata.m_1ByteFlags[9] << ", " <<
			(int)_bud.udata.m_1ByteFlags[10] << ", " <<
			(int)_bud.udata.m_1ByteFlags[11];
		break;
	}
	_o << ")";
	return _o;
}

std::ostream& operator <<(std::ostream& _o, const TriggerInfo_t& _ti)
{
	_o << "Trigger:";
	if(_ti.m_TagName) _o << " TagName: " << _ti.m_TagName;
	if(_ti.m_Action) _o << " Action: " << _ti.m_Action;
	if(_ti.m_Entity.IsValid())
		_o << " Entity: (" << _ti.m_Entity.GetIndex() << ":" << _ti.m_Entity.GetSerial() << ")";
	else
		_o << " Entity: (null)";
	if(_ti.m_Entity.IsValid())
		_o << " Activator: (" << _ti.m_Activator.GetIndex() << ":" << _ti.m_Activator.GetSerial() << ")";
	else
		_o << " Activator: (null)";

	return _o;
}

//////////////////////////////////////////////////////////////////////////

KeyValueIni		*FileOptions = 0;
bool			OptionsChanged = false;

void Options::Init()
{
}

void Options::Shutdown()
{
	if(FileOptions)
	{
		releaseKeyValueIni(FileOptions);
		FileOptions = 0;
	}
}

bool Options::LoadConfigFile(const std::string &_file)
{
	obuint32 NumSections = 0;

	File f;
	if(f.OpenForRead(_file.c_str(),File::Text))
	{
		std::string contents;
		const obuint64 FileSize = f.ReadWholeFile(contents);
		if(FileSize)
		{
			if(FileOptions)
			{
				releaseKeyValueIni(FileOptions);
				FileOptions = 0;
			}

			FileOptions = loadKeyValueIni(contents.c_str(),(unsigned int)contents.size(),NumSections);
		}
		return true;
	}
	return false;
}

bool Options::SaveConfigFile(const std::string &_file)
{
	if(FileOptions)
	{
		File f;
		if(f.OpenForWrite(_file.c_str(),File::Text))
		{
			obuint32 FileLength = 0;
			void *FileData = saveKeyValueIniMem(FileOptions,FileLength);

			f.Write(FileData,FileLength);
			f.Close();

			releaseIniMem(FileData);
			return true;
		}
	}
	return false;
}

bool Options::SaveConfigFileIfChanged(const std::string &_file)
{
	if(OptionsChanged)
	{
		OptionsChanged = false;
		return SaveConfigFile(_file);
	}
	return false;
}

const char *Options::GetRawValue(const char *_section, const char *_key)
{
	if(FileOptions)
	{
		obuint32 KeyCount = 0, LineNo = 0;
		const KeyValueSection *Section = locateSection(FileOptions,_section,KeyCount,LineNo);
		if(Section)
		{
			return locateValue(Section,_key,LineNo);
		}
	}
	return 0;
}

bool Options::GetValue(const char *_section, const char *_key, bool &_out)
{
	const char *Value = GetRawValue(_section,_key);
	if(Value)
	{
		if(Utils::StringToTrue(Value))
			_out = true;
		else if(Utils::StringToFalse(Value))
			_out = false;
		else
			return false;
		return true;
	}
	return false;
}

bool Options::GetValue(const char *_section, const char *_key, int &_out)
{
	const char *Value = GetRawValue(_section,_key);
	if(Value && Utils::ConvertString(std::string(Value),_out))
	{
		return true;
	}
	return false;
}

bool Options::GetValue(const char *_section, const char *_key, float &_out)
{
	const char *Value = GetRawValue(_section,_key);
	if(Value && Utils::ConvertString(std::string(Value),_out))
	{
		return true;
	}
	return false;
}

bool Options::GetValue(const char *_section, const char *_key, std::string &_out)
{
	const char *Value = GetRawValue(_section,_key);
	if(Value)
	{
		_out = Value;
		return true;
	}
	return false;
}

bool Options::SetValue(const char *_section, const char *_key, bool _val, bool _overwrite)
{
	std::string sVal = _val?"true":"false";
	return SetValue(_section,_key,sVal,_overwrite);
}

bool Options::SetValue(const char *_section, const char *_key, int _val, bool _overwrite)
{
	std::string s;
	if(Utils::ConvertString(_val,s))
		return SetValue(_section,_key,s,_overwrite);
	return false;
}

bool Options::SetValue(const char *_section, const char *_key, float _val, bool _overwrite)
{
	std::string s;
	if(Utils::ConvertString(_val,s))
		return SetValue(_section,_key,s,_overwrite);
	return false;
}

bool Options::SetValue(const char *_section, const char *_key, const char *_val, bool _overwrite)
{
	return SetValue(_section,_key,std::string(_val),_overwrite);
}

bool Options::SetValue(const char *_section, const char *_key, const std::string &_val, bool _overwrite)
{
	if(!FileOptions)
		FileOptions = createKeyValueIni();

	if(FileOptions)
	{
		KeyValueSection *Section = createKeyValueSection(FileOptions,_section,false);

		obuint32 LineNo = 0;
		if(!_overwrite && locateValue(Section,_key,LineNo))
			return false;

		bool bGood = addKeyValue(Section,_key,_val.c_str());

		OptionsChanged = true;
		return bGood;
	}
	return false;
}

//////////////////////////////////////////////////////////////////////////

bool Utils::TeamExists(obint32 _team)
{
	gmMachine *pM = ScriptManager::GetInstance()->GetMachine();
	gmTableObject *pTeams = pM->GetGlobals()->Get(pM,"TEAM").GetTableObjectSafe();
	gmTableIterator tit;
	gmTableNode *pNode = pTeams->GetFirst(tit);
	while(pNode)
	{
		if(pNode->m_value.GetInt()==_team)
			return true;
		pNode = pTeams->GetNext(tit);
	}
	return false;
}

bool Utils::ClassExists(obint32 _class)
{
	if(_class > 0 && _class <= FilterSensory::ANYPLAYERCLASS)
		return true;
	return false;
}

std::string Utils::GetTeamString(obint32 _team)
{
	gmMachine *pM = ScriptManager::GetInstance()->GetMachine();
	gmTableObject *pTeams = pM->GetGlobals()->Get(pM,"TEAM").GetTableObjectSafe();

	std::string sOut;

	bool bAllTeams = true;
	bool bNoTeams = true;

	// append all effecting teams.
	gmTableIterator tit;
	gmTableNode *pNode = pTeams->GetFirst(tit);
	while(pNode)
	{
		if(pNode->m_value.GetInt()!=OB_TEAM_SPECTATOR)
		{
			if(_team & (1<<pNode->m_value.GetInt()))
			{
				bNoTeams = false;
				sOut += pNode->m_key.GetCStringSafe("!!!");
				sOut += " ";
			}
			else
				bAllTeams = false;
		}
		pNode = pTeams->GetNext(tit);
	}

	if(bAllTeams)
		sOut = "All Teams";
	if(bNoTeams)
		sOut = "None";

	return sOut;
}

std::string Utils::GetClassString(obint32 _class)
{
	IGame *pGame = IGameManager::GetInstance()->GetGame();

	std::string sOut;
	bool bAllClasses = true;

	// append all effecting classes
	for(int c = 1; c < FilterSensory::ANYPLAYERCLASS; ++c)
	{
		if(_class & (1<<c))
		{
			const char *classname = pGame->FindClassName(c);
			sOut += (classname?classname:"!!!");
			sOut += " ";
		}
		else
			bAllClasses = false;
	}

	if(bAllClasses)
		sOut = "All Classes";

	return sOut;
}

void Utils::KeyValsToTable(const KeyVals &_kv, gmGCRoot<gmTableObject> _tbl, gmMachine *_machine)
{
	for(int i = 0; i < KeyVals::MaxArgs; ++i)
	{
		const char *Key = 0;
		obUserData Val;
		_kv.GetKV(i,Key,Val);
		if(Key)
		{
			_tbl->Set(_machine,
				gmVariable(_machine->AllocPermanantStringObject(Key)),
				UserDataToGmVar(_machine,Val)
				);
		}
	}
}

//////////////////////////////////////////////////////////////////////////

int LimitChecker::FromScript(gmThread *a_thread)
{
	GM_CHECK_NUM_PARAMS(1);
	Get().ClearAll();
	for(int i = 0; i < a_thread->GetNumParams(); ++i)
	{
		gmTableObject *Tbl = a_thread->Param(i).GetTableObjectSafe();
		if(Tbl)
		{
			gmTableIterator tIt;
			gmTableNode *pNode = Tbl->GetFirst(tIt);
			while(pNode)
			{
				if(pNode->m_key.IsInt())
					Get().SetFlag(pNode->m_key.GetInt());
				pNode = Tbl->GetNext(tIt);
			}
		}
		else
		{
			GM_CHECK_INT_PARAM(id, i);
			Get().SetFlag(id);
		}
	}
	return GM_OK;
}

bool LimitWeapons::IsAllowed(Client *_client)
{
	if(mFlags.AnyFlagSet())
	{
		AiState::WeaponSystem *ws = _client->GetWeaponSystem();

		BitFlag128 hasWeapons = (mFlags & ws->GetWeaponMask());
		if(!hasWeapons.AnyFlagSet())
			return false;

		bool bOutOfAmmo = true;
		for(int i = 0; i < 128; ++i)
		{
			if(hasWeapons.CheckFlag(i))
			{
				WeaponPtr w = ws->GetWeapon(i);
				if(w)
				{
					w->UpdateAmmo();

					if(w->OutOfAmmo()==InvalidFireMode)
					{
						bOutOfAmmo = false;
						break;
					}
				}
			}
		}
		if(bOutOfAmmo)
			return false;
	}
	return true;
}

void ErrorObj::AddInfo(const char* _msg, ...)
{
	char buffer[8192] = {0};
	va_list list;
	va_start(list, _msg);
#ifdef WIN32
	_vsnprintf(buffer, 8192, _msg, list);
#else
	vsnprintf(buffer, 8192, _msg, list);
#endif
	va_end(list);

	mInfo.push_back(buffer);
}

void ErrorObj::AddError(const char* _msg, ...)
{
	char buffer[8192] = {0};
	va_list list;
	va_start(list, _msg);
#ifdef WIN32
	_vsnprintf(buffer, 8192, _msg, list);
#else
	vsnprintf(buffer, 8192, _msg, list);
#endif
	va_end(list);

	mErrors.push_back(buffer);
}

void ErrorObj::PrintToConsole()
{
	for(StringList::iterator it = mInfo.begin();
		it != mInfo.end();
		++it)
	{
		EngineFuncs::ConsoleMessage((*it).c_str());
	}

	for(StringList::iterator it = mErrors.begin();
		it != mErrors.end();
		++it)
	{
		EngineFuncs::ConsoleError((*it).c_str());
	}
}

StringBuffer::StringBuffer(obuint32 _maxStrings, obuint32 _bufferSize)
	: m_BufferSize(_bufferSize)
	, m_MaxStrings(_maxStrings)
{
	m_Buffer = new char[m_BufferSize];
	m_Strings = new char*[m_MaxStrings];

	memset(m_Strings,0,sizeof(char*)*m_MaxStrings);
	memset(m_Buffer,0,sizeof(char)*m_BufferSize);
	m_BufferOffset = 0;
}

StringBuffer::~StringBuffer()
{
	delete [] m_Strings;
	delete [] m_Buffer;
}

const char *StringBuffer::AddUniqueString(const std::string & _str)
{
	const char * exists = Find(_str);
	if(exists)
		return exists;

	if(m_BufferOffset + _str.length() + 1 >= m_BufferSize)
		return NULL;

	for(obuint32 s = 0; s < m_MaxStrings; ++s)
	{
		if(!m_Strings[s])
		{
			m_Strings[s] = &m_Buffer[m_BufferOffset];
			Utils::StringCopy(&m_Buffer[m_BufferOffset],_str.c_str(),(int)_str.length()+1);
			m_BufferOffset += (obuint32)_str.length()+1;
			return m_Strings[s];
		}
	}
	return NULL;
}

const char *StringBuffer::Find(const std::string & _str)
{
	for(obuint32 s = 0; s < m_MaxStrings; ++s)
	{
		if(m_Strings[s] && _str == m_Strings[s])
			return m_Strings[s];
	}
	return 0;
}

obColor GetCoolWarmColor( float scalar )
{
	const obColor mapping[] = {
		obColor( 59, 76, 192 ),
		obColor( 60, 78, 194 ),
		obColor( 61, 80, 195 ),
		obColor( 62, 81, 197 ),
		obColor( 63, 83, 198 ),
		obColor( 64, 85, 200 ),
		obColor( 66, 87, 201 ),
		obColor( 67, 88, 203 ),
		obColor( 68, 90, 204 ),
		obColor( 69, 92, 206 ),
		obColor( 70, 93, 207 ),
		obColor( 71, 95, 209 ),
		obColor( 73, 97, 210 ),
		obColor( 74, 99, 211 ),
		obColor( 75, 100, 213 ),
		obColor( 76, 102, 214 ),
		obColor( 77, 104, 215 ),
		obColor( 79, 105, 217 ),
		obColor( 80, 107, 218 ),
		obColor( 81, 109, 219 ),
		obColor( 82, 110, 221 ),
		obColor( 84, 112, 222 ),
		obColor( 85, 114, 223 ),
		obColor( 86, 115, 224 ),
		obColor( 87, 117, 225 ),
		obColor( 89, 119, 226 ),
		obColor( 90, 120, 228 ),
		obColor( 91, 122, 229 ),
		obColor( 93, 123, 230 ),
		obColor( 94, 125, 231 ),
		obColor( 95, 127, 232 ),
		obColor( 96, 128, 233 ),
		obColor( 98, 130, 234 ),
		obColor( 99, 131, 235 ),
		obColor( 100, 133, 236 ),
		obColor( 102, 135, 237 ),
		obColor( 103, 136, 238 ),
		obColor( 104, 138, 239 ),
		obColor( 106, 139, 239 ),
		obColor( 107, 141, 240 ),
		obColor( 108, 142, 241 ),
		obColor( 110, 144, 242 ),
		obColor( 111, 145, 243 ),
		obColor( 112, 147, 243 ),
		obColor( 114, 148, 244 ),
		obColor( 115, 150, 245 ),
		obColor( 116, 151, 246 ),
		obColor( 118, 153, 246 ),
		obColor( 119, 154, 247 ),
		obColor( 120, 156, 247 ),
		obColor( 122, 157, 248 ),
		obColor( 123, 158, 249 ),
		obColor( 124, 160, 249 ),
		obColor( 126, 161, 250 ),
		obColor( 127, 163, 250 ),
		obColor( 129, 164, 251 ),
		obColor( 130, 165, 251 ),
		obColor( 131, 167, 252 ),
		obColor( 133, 168, 252 ),
		obColor( 134, 169, 252 ),
		obColor( 135, 171, 253 ),
		obColor( 137, 172, 253 ),
		obColor( 138, 173, 253 ),
		obColor( 140, 174, 254 ),
		obColor( 141, 176, 254 ),
		obColor( 142, 177, 254 ),
		obColor( 144, 178, 254 ),
		obColor( 145, 179, 254 ),
		obColor( 147, 181, 255 ),
		obColor( 148, 182, 255 ),
		obColor( 149, 183, 255 ),
		obColor( 151, 184, 255 ),
		obColor( 152, 185, 255 ),
		obColor( 153, 186, 255 ),
		obColor( 155, 187, 255 ),
		obColor( 156, 188, 255 ),
		obColor( 158, 190, 255 ),
		obColor( 159, 191, 255 ),
		obColor( 160, 192, 255 ),
		obColor( 162, 193, 255 ),
		obColor( 163, 194, 255 ),
		obColor( 164, 195, 254 ),
		obColor( 166, 196, 254 ),
		obColor( 167, 197, 254 ),
		obColor( 168, 198, 254 ),
		obColor( 170, 199, 253 ),
		obColor( 171, 199, 253 ),
		obColor( 172, 200, 253 ),
		obColor( 174, 201, 253 ),
		obColor( 175, 202, 252 ),
		obColor( 176, 203, 252 ),
		obColor( 178, 204, 251 ),
		obColor( 179, 205, 251 ),
		obColor( 180, 205, 251 ),
		obColor( 182, 206, 250 ),
		obColor( 183, 207, 250 ),
		obColor( 184, 208, 249 ),
		obColor( 185, 208, 248 ),
		obColor( 187, 209, 248 ),
		obColor( 188, 210, 247 ),
		obColor( 189, 210, 247 ),
		obColor( 190, 211, 246 ),
		obColor( 192, 212, 245 ),
		obColor( 193, 212, 245 ),
		obColor( 194, 213, 244 ),
		obColor( 195, 213, 243 ),
		obColor( 197, 214, 243 ),
		obColor( 198, 214, 242 ),
		obColor( 199, 215, 241 ),
		obColor( 200, 215, 240 ),
		obColor( 201, 216, 239 ),
		obColor( 203, 216, 238 ),
		obColor( 204, 217, 238 ),
		obColor( 205, 217, 237 ),
		obColor( 206, 217, 236 ),
		obColor( 207, 218, 235 ),
		obColor( 208, 218, 234 ),
		obColor( 209, 219, 233 ),
		obColor( 210, 219, 232 ),
		obColor( 211, 219, 231 ),
		obColor( 213, 219, 230 ),
		obColor( 214, 220, 229 ),
		obColor( 215, 220, 228 ),
		obColor( 216, 220, 227 ),
		obColor( 217, 220, 225 ),
		obColor( 218, 220, 224 ),
		obColor( 219, 220, 223 ),
		obColor( 220, 221, 222 ),
		obColor( 221, 221, 221 ),
		obColor( 222, 220, 219 ),
		obColor( 223, 220, 218 ),
		obColor( 224, 219, 216 ),
		obColor( 225, 219, 215 ),
		obColor( 226, 218, 214 ),
		obColor( 227, 218, 212 ),
		obColor( 228, 217, 211 ),
		obColor( 229, 216, 209 ),
		obColor( 230, 216, 208 ),
		obColor( 231, 215, 206 ),
		obColor( 232, 215, 205 ),
		obColor( 232, 214, 203 ),
		obColor( 233, 213, 202 ),
		obColor( 234, 212, 200 ),
		obColor( 235, 212, 199 ),
		obColor( 236, 211, 197 ),
		obColor( 236, 210, 196 ),
		obColor( 237, 209, 194 ),
		obColor( 238, 209, 193 ),
		obColor( 238, 208, 191 ),
		obColor( 239, 207, 190 ),
		obColor( 240, 206, 188 ),
		obColor( 240, 205, 187 ),
		obColor( 241, 204, 185 ),
		obColor( 241, 203, 184 ),
		obColor( 242, 202, 182 ),
		obColor( 242, 201, 181 ),
		obColor( 243, 200, 179 ),
		obColor( 243, 199, 178 ),
		obColor( 244, 198, 176 ),
		obColor( 244, 197, 174 ),
		obColor( 245, 196, 173 ),
		obColor( 245, 195, 171 ),
		obColor( 245, 194, 170 ),
		obColor( 245, 193, 168 ),
		obColor( 246, 192, 167 ),
		obColor( 246, 191, 165 ),
		obColor( 246, 190, 163 ),
		obColor( 246, 188, 162 ),
		obColor( 247, 187, 160 ),
		obColor( 247, 186, 159 ),
		obColor( 247, 185, 157 ),
		obColor( 247, 184, 156 ),
		obColor( 247, 182, 154 ),
		obColor( 247, 181, 152 ),
		obColor( 247, 180, 151 ),
		obColor( 247, 178, 149 ),
		obColor( 247, 177, 148 ),
		obColor( 247, 176, 146 ),
		obColor( 247, 174, 145 ),
		obColor( 247, 173, 143 ),
		obColor( 247, 172, 141 ),
		obColor( 247, 170, 140 ),
		obColor( 247, 169, 138 ),
		obColor( 247, 167, 137 ),
		obColor( 247, 166, 135 ),
		obColor( 246, 164, 134 ),
		obColor( 246, 163, 132 ),
		obColor( 246, 161, 131 ),
		obColor( 246, 160, 129 ),
		obColor( 245, 158, 127 ),
		obColor( 245, 157, 126 ),
		obColor( 245, 155, 124 ),
		obColor( 244, 154, 123 ),
		obColor( 244, 152, 121 ),
		obColor( 244, 151, 120 ),
		obColor( 243, 149, 118 ),
		obColor( 243, 147, 117 ),
		obColor( 242, 146, 115 ),
		obColor( 242, 144, 114 ),
		obColor( 241, 142, 112 ),
		obColor( 241, 141, 111 ),
		obColor( 240, 139, 109 ),
		obColor( 240, 137, 108 ),
		obColor( 239, 136, 106 ),
		obColor( 238, 134, 105 ),
		obColor( 238, 132, 103 ),
		obColor( 237, 130, 102 ),
		obColor( 236, 129, 100 ),
		obColor( 236, 127, 99 ),
		obColor( 235, 125, 97 ),
		obColor( 234, 123, 96 ),
		obColor( 233, 121, 95 ),
		obColor( 233, 120, 93 ),
		obColor( 232, 118, 92 ),
		obColor( 231, 116, 90 ),
		obColor( 230, 114, 89 ),
		obColor( 229, 112, 88 ),
		obColor( 228, 110, 86 ),
		obColor( 227, 108, 85 ),
		obColor( 227, 106, 83 ),
		obColor( 226, 104, 82 ),
		obColor( 225, 102, 81 ),
		obColor( 224, 100, 79 ),
		obColor( 223, 98, 78 ),
		obColor( 222, 96, 77 ),
		obColor( 221, 94, 75 ),
		obColor( 220, 92, 74 ),
		obColor( 218, 90, 73 ),
		obColor( 217, 88, 71 ),
		obColor( 216, 86, 70 ),
		obColor( 215, 84, 69 ),
		obColor( 214, 82, 67 ),
		obColor( 213, 80, 66 ),
		obColor( 212, 78, 65 ),
		obColor( 210, 75, 64 ),
		obColor( 209, 73, 62 ),
		obColor( 208, 71, 61 ),
		obColor( 207, 69, 60 ),
		obColor( 205, 66, 59 ),
		obColor( 204, 64, 57 ),
		obColor( 203, 62, 56 ),
		obColor( 202, 59, 55 ),
		obColor( 200, 57, 54 ),
		obColor( 199, 54, 53 ),
		obColor( 198, 51, 52 ),
		obColor( 196, 49, 50 ),
		obColor( 195, 46, 49 ),
		obColor( 193, 43, 48 ),
		obColor( 192, 40, 47 ),
		obColor( 190, 37, 46 ),
		obColor( 189, 34, 45 ),
		obColor( 188, 30, 44 ),
		obColor( 186, 26, 43 ),
		obColor( 185, 22, 41 ),
		obColor( 183, 17, 40 ),
		obColor( 181, 11, 39 ),
		obColor( 180, 4, 38 ),
	};
	const int numMappings = sizeof(mapping)/sizeof(mapping[0]);
	return mapping[ ClampT<int>( scalar * numMappings, 0, numMappings ) ];
}