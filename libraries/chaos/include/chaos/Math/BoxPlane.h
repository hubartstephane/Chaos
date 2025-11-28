 namespace chaos
{
#ifdef CHAOS_FORWARD_DECLARATION

	CHAOS_GEOMETRY_TEMPLATE(DIMENSION, T) class box_plane; // a set of 4 or 6 clip planes
	CHAOS_GEOMETRY_DEFINE_GLMLIKE_TEMPLATES(box_plane);

#elif !defined CHAOS_TEMPLATE_IMPLEMENTATION

	/**
	 * box_plane: a class to hold the planes of a box or projection volume
	 */

	template<std::floating_point T>
	class box_plane<2, T> : public geometry<2, T>
	{
	public:

		static constexpr size_t plane_count = 4;

		using geometry_type = geometry<2, T>;
		using plane_type = typename geometry_type::plane_type;

		/** number of planes in the box */
		size_t size() const
		{
			return plane_count;
		}
		/** indexing the planes */
		plane_type const& operator [](size_t index) const
		{
			assert(index < plane_count);
			return (&neg_x)[index];
		}
		/** indexing the planes */
		plane_type& operator [](size_t index)
		{
			assert(index < plane_count);
			return (&neg_x)[index];
		}

		/** indexing the planes */
		plane_type const& operator [](Direction direction) const
		{
			return operator [](size_t(direction));
		}
		/** indexing the planes */
		plane_type & operator [](Direction direction)
		{
			return operator [](size_t(direction));
		}

		/** conversion to span */
		operator std::span<plane_type const>() const
		{
			return span();
		}
		/** conversion to span */
		operator std::span<plane_type>()
		{
			return span();
		}
		/** conversion to span */
		std::span<plane_type const> span() const
		{
			return std::span<plane_type const>(&neg_x, plane_count);
		}
		/** conversion to span */
		std::span<plane_type> span()
		{
			return std::span<plane_type>(&neg_x, plane_count);
		}
		/** begin operator */
		auto begin() const
		{
			return span().begin();
		}
		/** begin operator */
		auto begin()
		{
			return span().begin();
		}
		/** end operator */
		auto end() const
		{
			return span().end();
		}
		/** end operator */
		auto end()
		{
			return span().end();
		}

	public:

		/** the plane whose normal is facing toward negative X direction */
		plane_type neg_x;
		/** the plane whose normal is facing toward positive X direction */
		plane_type pos_x;
		/** the plane whose normal is facing toward negative Y direction */
		plane_type neg_y;
		/** the plane whose normal is facing toward positive Y direction */
		plane_type pos_y;
	};

	/**
	 * box_plane: a class to hold the planes of a box or projection volume
	 */

	template<std::floating_point T>
	class box_plane<3, T> : public geometry<3, T>
	{
	public:

		static constexpr size_t plane_count = 6;

		using geometry_type = geometry<3, T>;
		using plane_type = typename geometry_type::plane_type;

		/** number of planes in the box */
		size_t size() const
		{
			return plane_count;
		}
		/** indexing the planes */
		plane_type const& operator [](size_t index) const
		{
			assert(index < plane_count);
			return (&neg_x)[index];
		}
		/** indexing the planes */
		plane_type & operator [](size_t index)
		{
			assert(index < plane_count);
			return (&neg_x)[index];
		}

		/** indexing the planes */
		plane_type const& operator [](Direction direction) const
		{
			return operator [](size_t(direction));
		}
		/** indexing the planes */
		plane_type & operator [](Direction direction)
		{
			return operator [](size_t(direction));
		}

		/** conversion to span */
		operator std::span<plane_type const>() const
		{
			return span();
		}
		/** conversion to span */
		operator std::span<plane_type>()
		{
			return span();
		}
		/** conversion to span */
		std::span<plane_type const> span() const
		{
			return std::span<plane_type const>(&neg_x, plane_count);
		}
		/** conversion to span */
		std::span<plane_type> span()
		{
			return std::span<plane_type>(&neg_x, plane_count);
		}
		/** begin operator */
		auto begin() const
		{
			return span().begin();
		}
		/** begin operator */
		auto begin()
		{
			return span().begin();
		}
		/** end operator */
		auto end() const
		{
			return span().end();
		}
		/** end operator */
		auto end()
		{
			return span().end();
		}

	public:

		/** the plane whose normal is facing toward negative X direction */
		plane_type neg_x;
		/** the plane whose normal is facing toward positive X direction */
		plane_type pos_x;
		/** the plane whose normal is facing toward negative Y direction */
		plane_type neg_y;
		/** the plane whose normal is facing toward positive Y direction */
		plane_type pos_y;
		/** the plane whose normal is facing toward negative Z direction */
		plane_type neg_z;
		/** the plane whose normal is facing toward positive Z direction */
		plane_type pos_z;
	};

#endif

}; // namespace chaos