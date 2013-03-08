////////////////////////////////////////////////////////////////////////////////
//
// $LastChangedBy$
// $LastChangedDate$
// $LastChangedRevision$
//
////////////////////////////////////////////////////////////////////////////////

#include "Revision.h"

namespace Revision
{
	std::string Number()
	{
		std::string RevisionNumber = "$LastChangedRevision$";
		size_t numStart = RevisionNumber.find_last_of(":")+2;
		size_t numEnd = RevisionNumber.find_last_of(" ");
		return RevisionNumber.substr(numStart,numEnd-numStart);
	}
	std::string Date()
	{
		std::string RevisionDate = "$LastChangedDate$";
		size_t dateStart = RevisionDate.find_last_of("(")+1;
		size_t dateEnd = RevisionDate.find_last_of(")");
		return RevisionDate.substr(dateStart,dateEnd-dateStart);
	}
};