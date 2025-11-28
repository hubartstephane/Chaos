namespace chaos
{
#ifdef CHAOS_FORWARD_DECLARATION

	enum class ProjectionType;

	CHAOS_GEOMETRY_TEMPLATE(DIMENSION, T) class projection_volume;
	CHAOS_GEOMETRY_TEMPLATE(DIMENSION, T) class perspective;

	CHAOS_GEOMETRY_DEFINE_GLMLIKE_TEMPLATES(projection_volume);
	CHAOS_GEOMETRY_DEFINE_GLMLIKE_TEMPLATES(perspective);

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
	class projection_volume<2, T> : public geometry<2, T>
	{
	public:

		T left  = 0;
		T right = 0;
		T front = 0;
		T back  = 0;
	};

	template<std::floating_point T>
	class projection_volume<3, T> : public geometry<3, T>
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
	class perspective<2, T> : public geometry<2, T>
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
	class perspective<3, T> : public geometry<3, T>
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

	CHAOS_GEOMETRY_TEMPLATE(DIMENSION, T)
	projection_volume<DIMENSION, T> GetProjectionVolume(perspective<DIMENSION, T> const& persp)
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

		projection_volume<DIMENSION, T> result;

		T fov_radian = persp.fov * T(M_PI) / T(180.0);

		if constexpr (DIMENSION == 2)
		{
			T distance = T(0.5) * persp.width / std::tan(fov_radian * T(0.5)); // distance at which field of view is statisfied
			T ratio = T(0.5) * (persp.front / distance);

			result.left = -persp.width * ratio;
			result.right = -result.left;
		} 
		else if constexpr (DIMENSION == 3)
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
	glm::mat<4, 4, T> GetProjectionMatrix(perspective<3, T> const& persp)
	{
		return glm::perspectiveFov(persp.fov * T(M_PI) / T(180.0), T(persp.width), float(persp.height), persp.front, persp.back);
	}

	template<std::floating_point T>
	glm::mat<4, 4, T> GetProjectionMatrix(ProjectionType type, projection_volume<3, T> const& vol)
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

	CHAOS_GEOMETRY_TEMPLATE(DIMENSION, T)
	box_plane<DIMENSION, T> GetProjectionPlanes(perspective<DIMENSION, T> const& persp)
	{
		return GetProjectionPlanes(ProjectionType::PERSPECTIVE, GetProjectionVolume(persp));
	}

	CHAOS_GEOMETRY_TEMPLATE(DIMENSION, T)
	box_plane<DIMENSION, T> GetProjectionPlanes(ProjectionType type, projection_volume<DIMENSION, T> const& vol)
	{
		using geometry_type   = geometry<DIMENSION, T>;
		using vec_type   = typename geometry_type::vec_type;
		using plane_type = typename geometry_type::plane_type;

		box_plane<DIMENSION, T> result;

		switch (type)
		{
		case ProjectionType::PERSPECTIVE:
		{
			if constexpr (DIMENSION == 2)
			{

			}
			else if constexpr (DIMENSION == 3)
			{
				T factor = vol.back / vol.front;

				auto ComputePlaneX = [](vec_type const& A, vec_type const& B)
				{
					vec_type AB = B - A;
					vec_type normal = glm::normalize(vec_type(-AB.z, T(0), AB.x));
					return plane_type(normal.x, normal.y, normal.z, T(0));
				};

				result.neg_x = ComputePlaneX(
					{ vol.left, T(0), vol.front },
					{ vol.left * factor, T(0), vol.back }
				);

				result.pos_x = -ComputePlaneX(
					{ vol.right, T(0), vol.front },
					{ vol.right * factor, T(0), vol.back }
				);

				auto ComputePlaneY = [](vec_type const& A, vec_type const& B)
				{
					vec_type AB = B - A;
					vec_type normal = glm::normalize(vec_type(T(0), -AB.z, AB.y));
					return plane_type(normal.x, normal.y, normal.z, T(0));
				};

				result.neg_y = ComputePlaneY(
					{ T(0), vol.bottom, vol.front },
					{ T(0), vol.bottom * factor, vol.back }
				);

				result.pos_y = -ComputePlaneY(
					{ T(0), vol.top, vol.front },
					{ T(0), vol.top * factor, vol.back }
				);
			}
			break;
		}
		case ProjectionType::ORTHOGRAPHIC:
		{
			if constexpr (DIMENSION == 2)
			{
				result.neg_x = { -T(1),  T(0), -vol.left };
				result.pos_x = { +T(1),  T(0), -vol.right };
				result.neg_y = {  T(0), -T(1), -vol.bottom };
				result.pos_y = {  T(0), +T(1), -vol.top };
			}
			else if constexpr (DIMENSION == 3)
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

		if constexpr (DIMENSION == 2)
		{
			result.neg_z = { T(0), -T(1), -vol.front};
			result.pos_z = { T(0), +T(1), -vol.back };
		}
		else if constexpr (DIMENSION == 3)
		{
			result.neg_z = { T(0), T(0), -T(1), -vol.front};
			result.pos_z = { T(0), T(0), +T(1), -vol.back };
		}

		return result;
	}

#endif

}; // namespace chaos