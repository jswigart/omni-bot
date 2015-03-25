////////////////////////////////////////////////////////////////////////////////
//
// $LastChangedBy$
// $LastChangedDate$
// $LastChangedRevision$
//
////////////////////////////////////////////////////////////////////////////////

#include "D3_InterfaceFuncs.h"

namespace InterfaceFuncs
{
	const char *GetLocation(const Vector3f &_pos)
	{
		D3_Location data;
		data.mPosition[0] = _pos[0];
		data.mPosition[1] = _pos[1];
		data.mPosition[2] = _pos[2];
		data.mLocationName[0] = 0;
		MessageHelper msg(D3_MSG_GETLOCATION, &data, sizeof(data));
		InterfaceMsg(msg);
		return data.mLocationName ? data.mLocationName : "";
	}
};