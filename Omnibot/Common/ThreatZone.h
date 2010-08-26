////////////////////////////////////////////////////////////////////////////////
// 
// $LastChangedBy: DrEvil $
// $LastChangedDate: 2006-04-01 18:52:22 -0700 (Sat, 01 Apr 2006) $
// $LastChangedRevision: 1151 $
//
////////////////////////////////////////////////////////////////////////////////

#ifndef __THREATZONE_H__
#define __THREATZONE_H__

// class: ThreatZone
//		This class marks an area as dangerous.
class ThreatZone
{
public:

	static const obuint32 MAX_WEAPONS = 32;
	static const obuint32 MAX_CLASSES = 16;

	ThreatZone(obuint32 _navId, obint32 _weaponId, obint32 _classId);
	~ThreatZone();
private:
	ThreatZone();

	int		m_NavId;

	obuint16	m_WeaponKills[MAX_WEAPONS];
	obuint16	m_ClassKills[MAX_CLASSES];
};

// typedef: ThreatZonePtr
typedef boost::shared_ptr<ThreatZone> ThreatZonePtr;

#endif
