namespace chaos
{
#ifdef CHAOS_FORWARD_DECLARATION

	// ==============================================================================================
	// Some common typedefs for geometry
	// ==============================================================================================

	// the base template (two arguments, type and dimension)
	template<typename T, int dimension> class type_geometric;
	template<typename T, int dimension> class type_ray;
	template<typename T, int dimension> class type_box_base;
	template<typename T, int dimension> class type_box;
	template<typename T, int dimension> class type_obox;
	template<typename T, int dimension> class type_sphere;
	template<typename T, int dimension> class type_triangle;
	template<typename T, int dimension> class type_rotator;   // this is not an object that describes a rotation, but a meta object that gives the rotation in a meta function
	template<typename T, int dimension> class type_aabox;     // aligned axis box
	template<typename T, int dimension> class type_box_plane; // a set of 4 or 6 clip planes

	class zero_rotator;

	// ==============================================================================================
	// The final objects
	// ==============================================================================================

	// remove all arguments from template (the objects to use for real)
	using ray2 = type_ray<float, 2>;
	using ray3 = type_ray<float, 3>;
	using box2 = type_box<float, 2>;
	using box3 = type_box<float, 3>;
	using obox2 = type_obox<float, 2>;
	using obox3 = type_obox<float, 3>;
	using sphere2 = type_sphere<float, 2>;
	using sphere3 = type_sphere<float, 3>;
	using triangle2 = type_triangle<float, 2>;
	using triangle3 = type_triangle<float, 3>;

	using rotator2 = float; // this are ROTATION here (angle or quaternion)
	using rotator3 = glm::quat;

	using aabox2  = type_aabox<float, 2>;
	using aabox3  = type_aabox<float, 3>;
	using aabox2i = type_aabox<int, 2>; // for box and obox it does not make many sense to have an integer specification
	using aabox3i = type_aabox<int, 3>; // (because half_size does not fit well with int)

	using box_plane2 = type_box_plane<float, 2>;
	using box_plane3 = type_box_plane<float, 3>;

#elif !defined CHAOS_TEMPLATE_IMPLEMENTATION

	// ==============================================================================================
	// rotators
	// ==============================================================================================

	// XXX : depending whether we are in 2D or 3D a rotation can be described by a single float or a quaternion
	//       this code is here to provide some common interface
	//
	// XXX : for 3D rotations we use a quaternion. quaternions have a constructor that make them valid (constructor = 0 rotator)
	//       for 2D we simply use float/double. that means that this is NOT INITIALIZED by default !
	//       (I could use a class to embedd a single scalar but the conversion/addition ... operators implementation would be too tedious

	class CHAOS_API zero_rotator
	{
	public:

		operator float() const { return 0.0f; }

		operator double() const { return 0.0; }

		operator glm::quat() const { return glm::quat(1.0f, 0.0f, 0.0f, 0.0f); } // BEWARE: first argument is W !!! want a normalized quaternion (0,0,0,0 is invalid)

		operator glm::dquat() const { return glm::dquat(1.0, 0.0, 0.0, 0.0); }   // BEWARE: first argument is W !!! want a normalized quaternion (0,0,0,0 is invalid)
	};


	// base template
	template<typename T, int dimension>
	class type_rotator : public boost::mpl::identity<void> {};
	// specialization
	template<>
	class type_rotator<float, 2> : public boost::mpl::identity<float> {};
	// specialization
	template<>
	class type_rotator<double, 2> : public boost::mpl::identity<double> {};
	// specialization
	template<>
	class type_rotator<float, 3> : public boost::mpl::identity<glm::quat> {};
	// specialization
	template<>
	class type_rotator<double, 3> : public boost::mpl::identity<glm::dquat> {};

	// ==============================================================================================
	// geometric class
	// ==============================================================================================

	template<typename T>
	class type_geometric<T, 2>
	{
	public:
		/** the dimension of the space */
		static constexpr int dimension = 2;

		/** the type of the components */
		using type = T;
		/** the type for matrix */
		using mat_type = glm::tmat4x4<T, glm::highp>;
		/** the type of vector */
		using vec_type = glm::vec<dimension, type>;
		/** the type of plane */
		using plane_type = glm::vec<dimension + 1, type>;
		/** the type of rotator */
		using rot_type = typename type_rotator<type, dimension>::type;
		/** the type of ray */
		using ray_type = type_ray<type, dimension>;
		/** the type of sphere */
		using sphere_type = type_sphere<type, dimension>;
		/** the type of box */
		using box_type = type_box<type, dimension>;
		/** the type of obox */
		using obox_type = type_obox<type, dimension>;
		/** the type of triangle */
		using triangle_type = type_triangle<type, dimension>;
		/** the type of aabox */
		using aabox_type = type_aabox<type, dimension>;
		/** the type of box plane */
		using box_plane_type = type_box_plane<type, dimension>;
	};

	template<typename T>
	class type_geometric<T, 3>
	{
	public:
		/** the dimension of the space */
		static constexpr int dimension = 3;

		/** the type of the components */
		using type = T;
		/** the type for matrix */
		using mat_type = glm::tmat4x4<T, glm::defaultp>;
		/** the type of vector */
		using vec_type = glm::vec<dimension, type>;
		/** the type of plane */
		using plane_type = glm::vec<dimension + 1, type>;
		/** the type of rotator */
		using rot_type = typename type_rotator<type, dimension>::type;
		/** the type of ray */
		using ray_type = type_ray<type, dimension>;
		/** the type of sphere */
		using sphere_type = type_sphere<type, dimension>;
		/** the type of box */
		using box_type = type_box<type, dimension>;
		/** the type of obox */
		using obox_type = type_obox<type, dimension>;
		/** the type of triangle */
		using triangle_type = type_triangle<type, dimension>;
		/** the type of aabox */
		using aabox_type = type_aabox<type, dimension>;
		/** the type of box plane */
		using box_plane_type = type_box_plane<type, dimension>;
	};

	// ==============================================================================================
	// type_box_base class
	// ==============================================================================================

	template<typename T, int dimension>
	class type_box_base : public type_geometric<T, dimension>
	{
	public:

		using vec_type = typename type_geometric<T, dimension>::vec_type;
		using type     = typename type_geometric<T, dimension>::type;

		/** constructor (empty box) */
		type_box_base() = default;
		/** copy constructor */
		type_box_base(type_box_base const& src) = default;
		/** other constructor */
		type_box_base(vec_type const & in_position, vec_type const & in_half_size) : position(in_position), half_size(in_half_size) {}

	public:

		/** the center of the box */
		vec_type position;
		/** the half size the box */
		vec_type half_size = vec_type(-1);
	};

	// ==============================================================================================
	// box class
	// ==============================================================================================

	template<typename T, int dimension>
	class type_box : public type_box_base<T, dimension>
	{
	public:

		using vec_type = typename type_box_base<T, dimension>::vec_type;
		using type     = typename type_box_base<T, dimension>::type;

		/** constructor (empty box) */
		type_box() = default;
		/** copy constructor */
		type_box(type_box const & src) = default;
		/** constructor from base (usefull for obox conversion) */
		type_box(type_box_base<T, dimension> const & src) : type_box_base<T, dimension>(src.position, src.half_size) {}
		/** other constructor */
		type_box(vec_type const & in_position, vec_type const & in_half_size) : type_box_base<T, dimension>(in_position, in_half_size) {}
		/** construct a box from 2 points */
		type_box(std::pair<vec_type, vec_type> const & pts)
		{
			vec_type a = glm::min(pts.first, pts.second);
			vec_type b = glm::max(pts.first, pts.second);

			this->position = (b + a) / static_cast<T>(2);
			this->half_size = (b - a) / static_cast<T>(2);
		}
	};

	// ==============================================================================================
	// obox class
	// ==============================================================================================

	/** an oriented bounding box */
	template<typename T, int dimension>
	class type_obox : public type_box_base<T, dimension>
	{
	public:

		using vec_type = typename type_box_base<T, dimension>::vec_type;
		using type     = typename type_box_base<T, dimension>::type;
		using rot_type = typename type_box_base<T, dimension>::rot_type;

		/** constructor (empty box) */
		type_obox() = default;
		/** copy constructor */
		type_obox(type_obox const& src) = default;
		/** constructor from base (usefull for box conversion) */
		type_obox(type_box_base<T, dimension> const& src) : type_box_base<T, dimension>(src.position, src.half_size){}
		/** other constructor */
		type_obox(vec_type const & in_position, vec_type const & in_half_size, rot_type const & in_rotator) : type_box_base<T, dimension>(in_position, in_half_size), rotator(in_rotator) {}

	public:

		/** the angle/quaternion of rotation to apply to a box to have this obox */
		rot_type rotator = zero_rotator();
	};

	// ==============================================================================================
	// type_aabox class
	// ==============================================================================================

	template<typename T, int dimension>
	class type_aabox : public type_geometric<T, dimension>
	{
	public:

		using vec_type = typename type_geometric<T, dimension>::vec_type;
		using type = typename type_geometric<T, dimension>::type;

		/** constructor (empty box) */
		type_aabox() = default;
		/** copy constructor */
		type_aabox(type_aabox const& src) = default;
		/** other constructor */
		type_aabox(vec_type const& in_position, vec_type const& in_size) : position(in_position), size(in_size) {}
		/** construct a box from 2 points */
		type_aabox(std::pair<vec_type, vec_type> const& pts)
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

	// ==============================================================================================
	// sphere/circle classes
	// ==============================================================================================

	template<typename T, int dimension>
	class type_sphere : public type_geometric<T, dimension>
	{
	public:

		using vec_type = typename type_geometric<T, dimension>::vec_type;
		using type = typename type_geometric<T, dimension>::type;

		/** constructor (empty circle) */
		type_sphere() = default;
		/** copy constructor */
		type_sphere(type_sphere const& src) = default;
		/** other constructor */
		type_sphere(vec_type const & in_position, type in_radius) : position(in_position), radius(in_radius) {}

	public:

		/** the center of the circle */
		vec_type position;
		/** the radius of the circle */
		type radius = type(-1);
	};

	// ==============================================================================================
	// triangle classes
	// ==============================================================================================

	template<typename T, int dimension>
	class type_triangle : public type_geometric<T, dimension>
	{
	public:

		using vec_type = typename type_geometric<T, dimension>::vec_type;
		using type = typename type_geometric<T, dimension>::type;

		/** default constructor */
		type_triangle() = default;
		/** copy constructor */
		type_triangle(type_triangle const& src) = default;
		/** constructor from points */
		type_triangle(vec_type const & in_a, vec_type const & in_b, vec_type const & in_c) : a(in_a), b(in_b), c(in_c) {}

		/** access to vertices */
		vec_type & operator [] (int i)
		{
			assert(i >= 0 && i < 3);
			return *(&a + i);
		}

		/** access to vertices */
		vec_type const & operator [] (int i) const
		{
			assert(i >= 0 && i < 3);
			return *(&a + i);
		}

	public:

		/** first point of the triangle */
		vec_type a;
		/** second point of the triangle */
		vec_type b;
		/** third point of the triangle */
		vec_type c;
	};

	// ==============================================================================================
	// ray classes
	// ==============================================================================================

	template<typename T, int dimension>
	class type_ray : public type_geometric<T, dimension>
	{
	public:

		using vec_type = typename type_geometric<T, dimension>::vec_type;
		using type = typename type_geometric<T, dimension>::type;

		/** default constructor */
		type_ray() = default;
		/** copy constructor */
		type_ray(type_ray const& src) = default;
		/** other constructor */
		type_ray(vec_type const & in_position, vec_type const & in_direction) : position(in_position), direction(in_direction) {}
		/** construct a ray from 2 points */
		type_ray(std::pair<vec_type, vec_type> const & pts) : position(pts.first), direction(glm::normalize(pts.second - pts.first)) {}

	public:

		/** the starting position of the ray in space */
		vec_type position;
		/** the direction of the ray in space */
		vec_type direction;
	};

// ==============================================================================================
// type_box_plane classes
// ==============================================================================================

	template<typename T>
	class type_box_plane<T, 2> : public chaos::type_geometric<T, 2>
	{
	public:

		static constexpr size_t plane_count = 4;

		using plane_type = chaos::type_geometric<T, 2>::plane_type;

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

	template<typename T>
	class type_box_plane<T, 3> : public chaos::type_geometric<T, 3>
	{
	public:

		static constexpr size_t plane_count = 6;

		using plane_type = chaos::type_geometric<T, 3>::plane_type;

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