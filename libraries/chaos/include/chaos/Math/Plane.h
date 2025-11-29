#if !define CHAOS_FORWARD_DECLARATION and !defined CHAOS_TEMPLATE_IMPLEMENTATION

namespace chaos
{
	template<int PLANE_DIMENSION, std::floating_point T> requires (PLANE_DIMENSION == 3 || PLANE_DIMENSION == 4)
	glm::vec<PLANE_DIMENSION - 1, T> GetPlaneNormal(glm::vec<PLANE_DIMENSION, T> const& src)
	{
		return *(glm::vec<PLANE_DIMENSION - 1, T>*)&src;
	}

	template<int PLANE_DIMENSION, std::floating_point T> requires (PLANE_DIMENSION == 3 || PLANE_DIMENSION == 4)
	T GetPlaneOffset(glm::vec<PLANE_DIMENSION, T> const& src)
	{
		return src[PLANE_DIMENSION - 1];
	}

	template<int PLANE_DIMENSION, std::floating_point T> requires (PLANE_DIMENSION == 3 || PLANE_DIMENSION == 4)
	void NormalizePlane(glm::vec<PLANE_DIMENSION, T> & src)
	{
		 auto normal = GetPlaneNormal(src);
		 T len = glm::length2(normal);
		 if (len != T(0) && len != T(1))
			 src /= std::sqrt(len);
	}

	template<int PLANE_DIMENSION, std::floating_point T> requires (PLANE_DIMENSION == 3 || PLANE_DIMENSION == 4)
	glm::vec<PLANE_DIMENSION, T> GetNormalizedPlane(glm::vec<PLANE_DIMENSION, T> const & src)
	{
		auto normal = GetPlaneNormal(src);
		T len = glm::length2(normal);
		if (len != T(0) && len != T(1))
			return src / std::sqrt(len);
		return src;
	}

}; // namespace chaos

#endif