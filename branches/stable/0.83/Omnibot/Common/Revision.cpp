////////////////////////////////////////////////////////////////////////////////
// 
// $LastChangedBy$
// $LastChangedDate$
// $LastChangedRevision$
//
////////////////////////////////////////////////////////////////////////////////

#include "PrecompCommon.h"
#include "Revision.h"

namespace Revision
{
	String Number()
	{
		String RevisionNumber = "$LastChangedRevision$";
		size_t numStart = RevisionNumber.find_last_of(":")+2;
		size_t numEnd = RevisionNumber.find_last_of(" ");
		return RevisionNumber.substr(numStart,numEnd-numStart);
	}
	String Date()
	{
		String RevisionDate = "$LastChangedDate$";
		size_t dateStart = RevisionDate.find_last_of("(")+1;
		size_t dateEnd = RevisionDate.find_last_of(")");
		return RevisionDate.substr(dateStart,dateEnd-dateStart);
	}

};

































































































































