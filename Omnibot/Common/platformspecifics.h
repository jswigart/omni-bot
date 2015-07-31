#pragma once

#ifndef _PLATFORMSPECIFICS_H_
#define _PLATFORMSPECIFICS_H_

#ifdef WIN32
#include <unordered_map>
#else
#if defined (__GNUC__) && (__GNUC__ <= 2)
#include <hash_map.h>
#include <function.h>
namespace stdext
{
	using ::hash_map;
	using ::hash
		using ::equal_to;
};
#else
#include <ext/hash_map> // cs: deprecated message recommends unordered_map, but that relies on experimental -std=c++0x. tr1/unordered_map is an option ...
#include <ext/functional>
namespace stdext
{
	using __gnu_cxx::hash_map;
	using __gnu_cxx::hash;
	using __gnu_cxx::equal_to;
};
#endif
#endif

#endif
