////////////////////////////////////////////////////////////////////////////////
// 
// $LastChangedBy: DrEvil $
// $LastChangedDate: 2006-04-01 18:52:22 -0700 (Sat, 01 Apr 2006) $
// $LastChangedRevision: 1151 $
//
////////////////////////////////////////////////////////////////////////////////

#ifndef __RESPONSECURVE_H__
#define __RESPONSECURVE_H__

// class: ResponseCurve
class ResponseCurve
{
public:

	bool LoadCurve();
	bool SaveCurve();

	float CalculateValue(float _value);

	typedef struct Point_t
	{
		float x, y;
	} Point;

	ResponseCurve();
	virtual ~ResponseCurve();
protected:
	typedef std::vector<Point> PointList;
	PointList		m_PointList;
};

#endif
