namespace chaos
{
#ifdef CHAOS_FORWARD_DECLARATION

	CHAOS_GEOMETRY_TEMPLATE(DIMENSION, T) class box_corners;
	CHAOS_GEOMETRY_DEFINE_GLMLIKE_TEMPLATES(box_corners);

#elif !defined CHAOS_TEMPLATE_IMPLEMENTATION

	/**
	 * box_corners: a class that deserves to hold the min and max bounding vertices of a box
	 */

	CHAOS_GEOMETRY_TEMPLATE(DIMENSION, T)
	class box_corners : public geometry<DIMENSION, T>
	{
	public:

		using geometry_type = geometry<DIMENSION, T>;
		using vec_type = geometry_type::vec_type;
		using box_base_type = box_base<DIMENSION, T>;
		using aabox_type = geometry_type::aabox_type;

		/** constructor (empty box) */
		box_corners() = default;
		/** copy constructor */
		box_corners(box_corners const& src) = default;
		/** other constructor */
		box_corners(vec_type const& in_center, vec_type const& in_half_size, CenterAndHalfSizeToken):
			min(in_center - in_half_size),
			max(in_center + in_half_size){}
		/** other constructor */
		box_corners(vec_type const& in_min, vec_type const& in_max, MinAndMaxPointsToken = {}):
			min(in_min), 
			max(in_max){}
		/** other constructor */
		box_corners(vec_type const& in_A, vec_type const& in_B, AnyTwoPointsToken):
			min(glm::min(in_A, in_B)),
			max(glm::max(in_A, in_B)){}
		/** other constructor */
		box_corners(vec_type const& in_min, vec_type const& in_size, MinCornerAndSizeToken):
			min(in_min), 
			max(in_min + in_size){}
		/** conversion constructor */
		box_corners(box_base_type const& src) : 
			box_corners(src.position, src.half_size, CenterAndHalfSize){}
		/** conversion constructor */
		box_corners(aabox_type const& src):
			box_corners(src.position, src.size, AnyTwoPoints){}

	public:

		/** the minimum corner */
		vec_type min = vec_type(+1);
		/** the maximum corner */
		vec_type max = vec_type(-1);
	};

	/** returns true whether the box is empty */
	CHAOS_GEOMETRY_TEMPLATE(DIMENSION, T)
	bool IsGeometryEmpty(box_corners<DIMENSION, T> const& b)
	{
		using geometry_type = geometry<DIMENSION, T>;
		return glm::any(glm::lessThan(b.max, b.min)); // a 0 size box is a valid box !
	}

	/** equality function for box_corners */
	CHAOS_GEOMETRY_TEMPLATE(DIMENSION, T)
	bool operator == (box_corners<DIMENSION, T> const& b1, box_corners<DIMENSION, T> const& b2)
	{
		if (IsGeometryEmpty(b1))
		{
			if (IsGeometryEmpty(b2))
				return true;
		}
		else if (IsGeometryEmpty(b2))
		{
			return false;
		}
		return (b1.min == b1.min) && (b1.max == b2.max);
	}

#endif

}; // namespace chaos