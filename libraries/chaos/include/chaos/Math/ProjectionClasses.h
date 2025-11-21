namespace chaos
{
#ifdef CHAOS_FORWARD_DECLARATION

	enum class ProjectionType;

	CHAOS_GEOMETRY_TEMPLATE(T, dimension)
	class projection_volume;

	CHAOS_GEOMETRY_TEMPLATE(T, dimension)
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

	template<std::floating_point T>
	class projection_volume<T, 2>
	{
	public:

		T left  = 0;
		T right = 0;
		T front = 0;
		T back  = 0;
	};

	template<std::floating_point T>
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

	template<std::floating_point T>
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

	template<std::floating_point T>
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

	CHAOS_GEOMETRY_TEMPLATE(T, dimension)
	projection_volume<T, dimension> GetProjectionVolume(perspective<T, dimension> const& persp)
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

		T fov_radian = persp.fov * T(M_PI) / T(180.0);

		if constexpr (dimension == 2)
		{
			T distance = T(0.5) * persp.width / std::tan(fov_radian * T(0.5)); // distance at which field of view is statisfied
			T ratio = T(0.5) * (persp.front / distance);

			result.left = -persp.width * ratio;
			result.right = -result.left;
		} 
		else if constexpr (dimension == 3)
		{
			T distance = T(0.5) * persp.height / std::tan(fov_radian * T(0.5)); // distance at which field of view is statisfied
			T ratio = T(0.5) * (persp.front / distance);

			result.left = -persp.width * ratio;
			result.right = -result.left;
			result.bottom = -persp.height * ratio;
			result.top = -result.bottom;
		}

		result.front  = persp.front;
		result.back   = persp.back;
		return result;
	}


	template<std::floating_point T>
	glm::mat<4, 4, T, glm::defaultp> GetProjectionMatrix(perspective<T, 3> const& persp)
	{
		return glm::perspectiveFov(persp.fov * T(M_PI) / T(180.0), T(persp.width), float(persp.height), persp.front, persp.back);
	}

	template<std::floating_point T>
	glm::mat<4, 4, T> GetProjectionMatrix(ProjectionType type, projection_volume<T, 3> const& vol)
	{
		switch (type)
		{
			case ProjectionType::PERSPECTIVE:
				return glm::frustum(vol.left, vol.right, vol.bottom, vol.top, vol.front, vol.back);
			case ProjectionType::ORTHOGRAPHIC:
				return glm::ortho(vol.left, vol.right, vol.bottom, vol.top, vol.front, vol.back);
			default:
				assert(0);
				return {};
		}
	}

	CHAOS_GEOMETRY_TEMPLATE(T, dimension)
	type_box_plane<T, dimension> GetProjectionPlanes(perspective<T, dimension> const& persp)
	{
		type_box_plane<T, dimension> result;

		return result;
	}

	CHAOS_GEOMETRY_TEMPLATE(T, dimension)
	type_box_plane<T, dimension> GetProjectionPlanes(ProjectionType type, projection_volume<T, dimension> const& vol)
	{
		using geometry   = type_geometric<T, dimension>;
		using vec_type   = typename geometry::vec_type;
		using plane_type = typename geometry::plane_type;

		type_box_plane<T, dimension> result;

		switch (type)
		{
		case ProjectionType::PERSPECTIVE:
		{
			



			break;
		}
		case ProjectionType::ORTHOGRAPHIC:
		{
			if constexpr (dimension == 2)
			{
				result.neg_x = { -T(1),  T(0), -vol.left };
				result.pos_x = { +T(1),  T(0), -vol.right };
				result.neg_y = {  T(0), -T(1), -vol.bottom };
				result.pos_y = {  T(0), +T(1), -vol.top };
			}
			else if constexpr (dimension == 3)
			{
				result.neg_x = { -T(1),  T(0), T(0), -vol.left };
				result.pos_x = { +T(1),  T(0), T(0), -vol.right };
				result.neg_y = {  T(0), -T(1), T(0), -vol.bottom };
				result.pos_y = {  T(0), +T(1), T(0), -vol.top };
			}
			break;
		}
		default:
			assert(0);
		}

		if constexpr (dimension == 2)
		{
			result.front = { T(0), -T(1), -vol.front};
			result.back  = { T(0), +T(1), -vol.back };
		}
		else if constexpr (dimension == 3)
		{
			result.front = { T(0), T(0), -T(1), -vol.front};
			result.back  = { T(0), T(0), +T(1), -vol.back };
		}

		return result;
	}

#endif

}; // namespace chaos