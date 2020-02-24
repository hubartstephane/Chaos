#pragma once

#include <chaos/StandardHeaders.h>
#include <chaos/GeometryFramework.h>

namespace chaos
{
	enum class ZonePartitionType : int
	{
		/** point is inside inner zone */
		NEAR = 0,
		/** point is betwen inner_zone and outer_zone */
		MIDDLE = 1,
		/** point is outside outer_zone */
		FAR = 2
	};
	
	enum class ZonePartitionChangeType : int
	{
		/** no zone of change */
		NOCHANGE = 0,
		/** was not in the near zone, and then enters in it */
		ENTER = 1,
		/** was not in the far zone and enters in it */
		LEAVE = 2
	};

	/**
	* DoubleZone : testing if a point enters or exits to trigger an action can be tedious if the 
	*              point change its zone too often. DoubleZone helps avoiding this issue by using
	*              using 2 zones
	*/

	template<typename ZONE_TYPE> 
	class DoubleZone
	{
	public:

		/** the type of the zone */
		using zone_type = ZONE_TYPE;
		/** the type of the vector */
		using vec_type = typename ZONE_TYPE::vec_type;

		/** returns the zone where is the point */
		ZonePartitionType GetPointZone(vec_type const & p) const
		{
			if (!inside(p, outer_zone))
				return ZonePartitionType::FAR;
			if (!inside(p, inner_zone))
				return ZonePartitionType::MIDDLE;
			return ZonePartitionType::NEAR;
		}

		/** returns the point zone changement */
		ZonePartitionChangeType CheckZoneChange(vec_type const & before, vec_type const & after) const
		{
			ZonePartitionType before_zone = GetPointZone(before);
			ZonePartitionType after_zone  = GetPointZone(after);
			return CheckZoneChange(before_zone, after_zone);
		}

		/** returns the point zone changement */
		static ZonePartitionChangeType CheckZoneChange(ZonePartitionType before, ZonePartitionType after)
		{  
			if (before_zone != after_zone)
			{
				if (after_zone == ZonePartitionType::NEAR)
					return ZonePartitionChangeType::ENTER;
				if (after_zone == ZonePartitionType::FAR)
					return ZonePartitionChangeType::LEAVE;
			}
			return ZonePartitionChangeType::NOCHANGE;
		}

	protected:

		/** the inner zone */
		zone_type inner_zone;
		/** the outer zone */
		zone_type outer_zone;
	};

}; // namespace chaos
