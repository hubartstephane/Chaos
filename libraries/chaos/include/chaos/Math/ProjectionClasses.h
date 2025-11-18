namespace chaos
{
#ifdef CHAOS_FORWARD_DECLARATION

	enum class ProjectionType;

	template<typename T, int dimension>
	class projection_volume;

	template<typename T, int dimension>
	class perspective;

#elif !defined CHAOS_TEMPLATE_IMPLEMENTATION

	/**
	 * ProjectionType: describe the kind of camera projection is used
	 */

	enum class ProjectionType : int
	{
		PERSPECTIVE,
		ORTHOGRAPHIC
	};

	/**
	 * projection_volume: describe the clipping volume for a camera projection
	 */

	template<typename T>
	class projection_volume<T, 2>
	{
	public:

		T left  = 0;
		T right = 0;
		T front = 0;
		T back  = 0;
	};

	template<typename T>
	class projection_volume<T, 3>
	{
	public:

		T left   = 0;
		T right  = 0;
		T bottom = 0;
		T top    = 0;
		T front  = 0;
		T back   = 0;
	};

	/**
	 * perspective: parameters to describe a perspective projection in a user friendly way
	 */

	template<typename T>
	class perspective<T, 2>
	{
	public:

		/** angle in degree of the field of view */
		T fov   = 0;
		/** width of the field of view (at fov distance) */
		T width = 0;
		/** distance to the near plane (at fov distance) */
		T front = 0;
		/** distance to the far plane */
		T back  = 0;
	};

	template<typename T>
	class perspective<T, 3>
	{
	public:

		/** angle in degree of the field of view */
		T fov    = 0;
		/** width of the field of view (at fov distance) */
		T width  = 0;
		/** height of the field of view (at fov distance) */
		T height = 0;
		/** distance to the near plane */
		T front  = 0;
		/** distance to the far plane */
		T back   = 0;
	};

	/**
	 * Some functions
	 */

	template<typename T, int dimension>
	projection_volume<T, dimension> GetProjectionVolume(perspective<T, dimension> const& p)
	{
		//          +
		//          | \ 
		//          |   \
		//   height |     \
		//   ------ |       \
		//     2    +---------+          tan(fov/2) = 0.5 * height / distance
		//          | distance
		//          |     /
		//          |   /
		//          | /
        //          +			

		projection_volume<T, dimension> result;

		T fov_radian = p.fov * T(M_PI) / T(180.0);

		if constexpr (dimension == 2)
		{
			T distance = T(0.5) * p.width / std::tan(fov_radian * T(0.5)); // distance at which field of view is statisfied
			T ratio = T(0.5) * (p.front / distance);

			result.left = -p.width * ratio;
			result.right = -result.left;
		} 
		else if constexpr (dimension == 3)
		{
			T distance = T(0.5) * p.height / std::tan(fov_radian * T(0.5)); // distance at which field of view is statisfied
			T ratio = T(0.5) * (p.front / distance);

			result.left = -p.width * ratio;
			result.right = -result.left;
			result.bottom = -p.height * ratio;
			result.top = -result.bottom;
		}

		result.front  = p.front;
		result.back   = p.back;
		return result;
	}

#endif

}; // namespace chaos