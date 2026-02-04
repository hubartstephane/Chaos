namespace chaos
{
#ifdef CHAOS_FORWARD_DECLARATION

	enum class ZonePartitionType;

	enum class ZonePartitionChangeType;

	template<typename ZONE_TYPE>
	class DoubleZone;

#elif !defined CHAOS_TEMPLATE_IMPLEMENTATION

	enum class ZonePartitionType : int
	{
		/** point is inside inner zone */
		NearZone = 0,
		/** point is betwen inner_zone and outer_zone */
		MiddleZone = 1,
		/** point is outside outer_zone */
		FarZone = 2
	};

	enum class ZonePartitionChangeType : int
	{
		/** no zone of change */
		None = 0,
		/** was not in the near zone, and then enters in it */
		Enter = 1,
		/** was not in the far zone and enters in it */
		Leave = 2
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

		/** constructor */
		DoubleZone() = default;
		/** copy constructor */
		DoubleZone(DoubleZone const& src) = default;
		/** initialization constructor */
		DoubleZone(zone_type const& in_zone_type, zone_type const& in_outer_zone) :
			zone_type(in_zone_type), outer_zone(in_outer_zone)
		{
		}

		/** returns the zone where is the point */
		ZonePartitionType GetPointZone(vec_type const& p) const
		{
			if (!inside(p, outer_zone))
				return ZonePartitionType::FarZone;
			if (!inside(p, inner_zone))
				return ZonePartitionType::MiddleZone;
			return ZonePartitionType::NearZone;
		}

		/** returns the point zone changement */
		ZonePartitionChangeType CheckZoneChange(vec_type const& before, vec_type const& after) const
		{
			ZonePartitionType before_zone = GetPointZone(before);
			ZonePartitionType after_zone = GetPointZone(after);
			return CheckZoneChange(before_zone, after_zone);
		}

		/** returns the point zone changement */
		static ZonePartitionChangeType CheckZoneChange(ZonePartitionType before, ZonePartitionType after)
		{
			if (before != after)
			{
				if (after == ZonePartitionType::NearZone)
					return ZonePartitionChangeType::Enter;
				if (after == ZonePartitionType::FarZone)
					return ZonePartitionChangeType::Leave;
			}
			return ZonePartitionChangeType::None;
		}

	public:

		/** the inner zone */
		zone_type inner_zone;
		/** the outer zone */
		zone_type outer_zone;
	};

#endif

}; // namespace chaos