////////////////////////////////////////////////////////////////////////////////
// 
// $LastChangedBy: drevil $
// $LastChangedDate: 2009-04-25 11:26:01 -0700 (Sat, 25 Apr 2009) $
// $LastChangedRevision: 3992 $
//
////////////////////////////////////////////////////////////////////////////////

#include "PrecompCommon.h"
#include "Revision.h"

namespace Revision
{
	String Number()
	{
		String RevisionNumber = "$LastChangedRevision: 3992 $";
		size_t numStart = RevisionNumber.find_last_of(":")+2;
		size_t numEnd = RevisionNumber.find_last_of(" ");
		return RevisionNumber.substr(numStart,numEnd-numStart);
	}
	String Date()
	{
		String RevisionDate = "$LastChangedDate: 2009-04-25 11:26:01 -0700 (Sat, 25 Apr 2009) $";
		size_t dateStart = RevisionDate.find_last_of("(")+1;
		size_t dateEnd = RevisionDate.find_last_of(")");
		return RevisionDate.substr(dateStart,dateEnd-dateStart);
	}
};



































































































































