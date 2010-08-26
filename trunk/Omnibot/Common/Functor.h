////////////////////////////////////////////////////////////////////////////////
// 
// $LastChangedBy: drevil $
// $LastChangedDate: 2009-03-22 16:08:41 -0700 (Sun, 22 Mar 2009) $
// $LastChangedRevision: 3946 $
//
////////////////////////////////////////////////////////////////////////////////

#ifndef __FUNCTOR_H__
#define __FUNCTOR_H__

// class: Functor
//		Base functor class, used for overloading operator() to do custom processing
class Functor
{
public:
    virtual int operator()()=0;
};

typedef boost::shared_ptr<Functor> FunctorPtr;
typedef std::list<FunctorPtr> FunctorList;
typedef std::map<std::string, FunctorPtr> FunctorMap;

//////////////////////////////////////////////////////////////////////////

struct toLower
{	
	char operator() (char c) const  
	{
#ifdef WIN32
		return (char)tolower(c); 
#else
		return std::tolower(c); 
#endif
	}
};

struct toUpper
{
	char operator() (char c) const  
	{
#ifdef WIN32
		return (char)toupper(c); 
#else
		return std::toupper(c); 
#endif
	}
};

#endif
