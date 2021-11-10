namespace chaos
{
#ifdef CHAOS_FORWARD_DECLARATION

	template<typename T, int dimension>
	class SceneTransform;

#elif !defined CHAOS_TEMPLATE_IMPLEMENTATION

	template<typename T, int dimension>
	class SceneTransform
	{
		using vec_type = typename type_geometric<T, dimension>::vec_type;
		using rot_type = typename type_geometric<T, dimension>::rot_type;
		using mat_type = typename type_geometric<T, dimension>::mat_type;

	public:

		/** gets the local to parent transform */
		mat_type GetLocalToParent() const
		{
			mat_type t, r, s;

			if constexpr (dimension == 2)
				t = glm::translate(glm::tvec3<T>(position, 0));
			else if constexpr (dimension == 2)
				t = glm::translate(position);

			r = GetRotatorMatrix(rotator);

			if constexpr (dimension == 2)
				t = glm::scale(glm::tvec3<T>(scale, 1));
			else if constexpr (dimension == 2)
				t = glm::scale(position);

			return t * r * s;
		}
		/** gets the parent to local */
		mat_type GetParentToLocal() const
		{
			mat_type s, r, t;

			if constexpr (dimension == 2)
				t = glm::scale(glm::tvec3<T>(T(1) / scale, 1));
			else if constexpr (dimension == 2)
				t = glm::scale(T(1) / scale);

			r = GetRotatorMatrix(-rotator);

			if constexpr (dimension == 2)
				t = glm::translate(glm::tvec3<T>(-position, 0));
			else if constexpr (dimension == 2)
				t = glm::translate(-position);

			return s * r * t;
		}

	public:

		/** the position */
		vec_type position = vec_type(0);
		/** the scale */
		vec_type scale = vec_type(1);
		/** the rotator */
		rot_type rotator = zero_rotator();
	};

#endif

}; // namespace chaos