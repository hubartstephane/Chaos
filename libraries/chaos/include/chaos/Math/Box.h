 namespace chaos
{
#ifdef CHAOS_FORWARD_DECLARATION

	CHAOS_GEOMETRY_TEMPLATE(DIMENSION, T) class box_base;
	CHAOS_GEOMETRY_TEMPLATE(DIMENSION, T) class box;
	CHAOS_GEOMETRY_TEMPLATE(DIMENSION, T) class obox;
	CHAOS_GEOMETRY_TEMPLATE(DIMENSION, T) class aabox;     // aligned axis box

	CHAOS_GEOMETRY_DEFINE_GLMLIKE_TEMPLATES(box_base);
	CHAOS_GEOMETRY_DEFINE_GLMLIKE_TEMPLATES(box);
	CHAOS_GEOMETRY_DEFINE_GLMLIKE_TEMPLATES(obox);
	CHAOS_GEOMETRY_DEFINE_GLMLIKE_TEMPLATES(aabox);

#elif !defined CHAOS_TEMPLATE_IMPLEMENTATION

	CHAOS_GEOMETRY_TEMPLATE(DIMENSION, T)
	class box_base : public geometry<DIMENSION, T>
	{
	public:

		using geometry_type = geometry<DIMENSION, T>;
		using vec_type = typename geometry_type::vec_type;

		/** constructor (empty box) */
		box_base() = default;
		/** copy constructor */
		box_base(box_base const& src) = default;
		/** other constructor */
		box_base(vec_type const & in_position, vec_type const & in_half_size) : position(in_position), half_size(in_half_size) {}

	public:

		/** the center of the box */
		vec_type position;
		/** the half size the box */
		vec_type half_size = vec_type(-1);
	};

	CHAOS_GEOMETRY_TEMPLATE(DIMENSION, T)
	class box : public box_base<DIMENSION, T>
	{
	public:

		using geometry_type = geometry<DIMENSION, T>;
		using vec_type = typename geometry_type::vec_type;

		/** constructor (empty box) */
		box() = default;
		/** copy constructor */
		box(box const & src) = default;
		/** constructor from base (usefull for obox conversion) */
		box(box_base<DIMENSION, T> const & src) : box_base<DIMENSION, T>(src.position, src.half_size) {}
		/** other constructor */
		box(vec_type const & in_position, vec_type const & in_half_size) : box_base<DIMENSION, T>(in_position, in_half_size) {}
		/** construct a box from 2 points */
		box(std::pair<vec_type, vec_type> const & pts)
		{
			vec_type a = glm::min(pts.first, pts.second);
			vec_type b = glm::max(pts.first, pts.second);

			this->position = (b + a) / static_cast<T>(2);
			this->half_size = (b - a) / static_cast<T>(2);
		}
	};

	/** an oriented bounding box */
	CHAOS_GEOMETRY_TEMPLATE(DIMENSION, T)
	class obox : public box_base<DIMENSION, T>
	{
	public:

		using geometry_type = geometry<DIMENSION, T>;
		using vec_type = typename geometry_type::vec_type;
		using rot_type = typename geometry_type::rot_type;

		/** constructor (empty box) */
		obox() = default;
		/** copy constructor */
		obox(obox const& src) = default;
		/** constructor from base (usefull for box conversion) */
		obox(box_base<DIMENSION, T> const& src) : box_base<DIMENSION, T>(src.position, src.half_size){}
		/** other constructor */
		obox(vec_type const & in_position, vec_type const & in_half_size, rot_type const & in_rotation) : box_base<DIMENSION, T>(in_position, in_half_size), rotation(in_rotation) {}

	public:

		/** the angle/quaternion of rotation to apply to a box to have this obox */
		rot_type rotation = zero_rotator();
	};

	CHAOS_GEOMETRY_TEMPLATE(DIMENSION, T)
	class aabox : public geometry<DIMENSION, T>
	{
	public:

		using geometry_type = geometry<DIMENSION, T>;
		using vec_type = typename geometry_type::vec_type;

		/** constructor (empty box) */
		aabox() = default;
		/** copy constructor */
		aabox(aabox const& src) = default;
		/** other constructor */
		aabox(vec_type const& in_position, vec_type const& in_size) : position(in_position), size(in_size) {}
		/** construct a box from 2 points */
		aabox(std::pair<vec_type, vec_type> const& pts)
		{
			vec_type a = glm::min(pts.first, pts.second);
			vec_type b = glm::max(pts.first, pts.second);

			this->position = a;
			this->size     = (b - a);
		}

	public:

		/** the lower left corner of the box */
		vec_type position;
		/** the size the box */
		vec_type size = vec_type(-1);
	};

#endif

}; // namespace chaos