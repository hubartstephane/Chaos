 namespace chaos
{
	#define CHAOS_GEOMETRY_TEMPLATE(DIMENSION, T, ...) template<int DIMENSION, std::floating_point T, __VA_ARGS__> requires (DIMENSION == 2 || DIMENSION == 3)

	#define CHAOS_GEOMETRY_DEFINE_GLMLIKE_TEMPLATES(TYPENAME)\
		template<std::floating_point T> using t##TYPENAME##2 = TYPENAME<2, T>;\
		template<std::floating_point T> using t##TYPENAME##3 = TYPENAME<3, T>;\
		using TYPENAME##2 = TYPENAME<2, float>;\
		using TYPENAME##3 = TYPENAME<3, float>;\
		using d##TYPENAME##2 = TYPENAME<2, double>;\
		using d##TYPENAME##3 = TYPENAME<3, double>;

#ifdef CHAOS_FORWARD_DECLARATION

	// ==============================================================================================
	// Some common typedefs for geometry
	// ==============================================================================================

	// the base template (two arguments, type and dimension)
	CHAOS_GEOMETRY_TEMPLATE(DIMENSION, T) class geometry;
	CHAOS_GEOMETRY_TEMPLATE(DIMENSION, T) class ray;
	CHAOS_GEOMETRY_TEMPLATE(DIMENSION, T) class box_base;
	CHAOS_GEOMETRY_TEMPLATE(DIMENSION, T) class box;
	CHAOS_GEOMETRY_TEMPLATE(DIMENSION, T) class obox;
	CHAOS_GEOMETRY_TEMPLATE(DIMENSION, T) class sphere;
	CHAOS_GEOMETRY_TEMPLATE(DIMENSION, T) class triangle;
	CHAOS_GEOMETRY_TEMPLATE(DIMENSION, T) class aabox;     // aligned axis box
	CHAOS_GEOMETRY_TEMPLATE(DIMENSION, T) class box_plane; // a set of 4 or 6 clip planes
	CHAOS_GEOMETRY_TEMPLATE(DIMENSION, T) class getrotatortype;

	// for our geometric classes (ray, box, ...) defines using & templates that mimic glm conventions
	//   box2
	//   box3
	//   dbox2
	//   dbox3
	//   tbox2<float>
	//   tbox3<float>
	//   ...

	CHAOS_GEOMETRY_DEFINE_GLMLIKE_TEMPLATES(geometry);
	CHAOS_GEOMETRY_DEFINE_GLMLIKE_TEMPLATES(ray);
	CHAOS_GEOMETRY_DEFINE_GLMLIKE_TEMPLATES(box_base);
	CHAOS_GEOMETRY_DEFINE_GLMLIKE_TEMPLATES(box);
	CHAOS_GEOMETRY_DEFINE_GLMLIKE_TEMPLATES(obox);
	CHAOS_GEOMETRY_DEFINE_GLMLIKE_TEMPLATES(sphere);
	CHAOS_GEOMETRY_DEFINE_GLMLIKE_TEMPLATES(triangle);
	CHAOS_GEOMETRY_DEFINE_GLMLIKE_TEMPLATES(aabox);
	CHAOS_GEOMETRY_DEFINE_GLMLIKE_TEMPLATES(box_plane);

	class zero_rotator;

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


	// getrotatortype specializations
	template<> class getrotatortype<2, float>  : public boost::mpl::identity<float> {};
	template<> class getrotatortype<2, double> : public boost::mpl::identity<double> {};
	template<> class getrotatortype<3, float>  : public boost::mpl::identity<glm::quat> {};
	template<> class getrotatortype<3, double> : public boost::mpl::identity<glm::dquat> {};

	CHAOS_GEOMETRY_TEMPLATE(DIMENSION, T) using rotator = typename getrotatortype<DIMENSION, T>::type;

	template<std::floating_point T>	using trotator2 = typename getrotatortype<2, T>::type;	
	template<std::floating_point T>	using trotator3 = typename getrotatortype<3, T>::type;

	using rotator2  = typename getrotatortype<2, float>::type;
	using rotator3  = typename getrotatortype<3, float>::type;
	using drotator2 = typename getrotatortype<2, double>::type;
	using drotator3 = typename getrotatortype<3, double>::type;
	
	// ==============================================================================================
	// geometry class
	// ==============================================================================================

	CHAOS_GEOMETRY_TEMPLATE(DIMENSION, T)
	class geometry
	{
	public:
		/** the dimension of the space */
		static constexpr int dimension = DIMENSION;

		/** the type of the components */
		using type = T;
		/** the type for the geometry itself */
		using geometry_type = geometry<dimension, type>;
		/** the type for matrix */
		using mat_type = glm::tmat4x4<T>;
		/** the type of vector */
		using vec_type = glm::vec<dimension, type>;
		/** the type of plane */
		using plane_type = glm::vec<dimension + 1, type>;
		/** the type of rotator */
		using rot_type =  rotator<dimension, type>;
		/** the type of ray */
		using ray_type = ray<dimension, type>;
		/** the type of sphere */
		using sphere_type = sphere<dimension, type>;
		/** the type of box */
		using box_type = box<dimension, type>;
		/** the type of obox */
		using obox_type = obox<dimension, type>;
		/** the type of triangle */
		using triangle_type = triangle<dimension, type>;
		/** the type of aabox */
		using aabox_type = aabox<dimension, type>;
		/** the type of box plane */
		using box_plane_type = box_plane<dimension, type>;
	};

	// ==============================================================================================
	// box_base class
	// ==============================================================================================

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

	// ==============================================================================================
	// box class
	// ==============================================================================================

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

	// ==============================================================================================
	// obox class
	// ==============================================================================================

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

	// ==============================================================================================
	// aabox class
	// ==============================================================================================

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

	// ==============================================================================================
	// sphere/circle classes
	// ==============================================================================================

	CHAOS_GEOMETRY_TEMPLATE(DIMENSION, T)
	class sphere : public geometry<DIMENSION, T>
	{
	public:

		using geometry_type = geometry<DIMENSION, T>;
		using vec_type = typename geometry_type::vec_type;
		using type = typename geometry_type::type;

		/** constructor (empty circle) */
		sphere() = default;
		/** copy constructor */
		sphere(sphere const& src) = default;
		/** other constructor */
		sphere(vec_type const & in_position, type in_radius) : position(in_position), radius(in_radius) {}

	public:

		/** the center of the circle */
		vec_type position;
		/** the radius of the circle */
		type radius = type(-1);
	};

	// ==============================================================================================
	// triangle classes
	// ==============================================================================================

	CHAOS_GEOMETRY_TEMPLATE(DIMENSION, T)
	class triangle : public geometry<DIMENSION, T>
	{
	public:

		using geometry_type = geometry<DIMENSION, T>;
		using vec_type = typename geometry_type::vec_type;

		/** default constructor */
		triangle() = default;
		/** copy constructor */
		triangle(triangle const& src) = default;
		/** constructor from points */
		triangle(vec_type const & in_a, vec_type const & in_b, vec_type const & in_c) : a(in_a), b(in_b), c(in_c) {}

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

	CHAOS_GEOMETRY_TEMPLATE(DIMENSION, T)
	class ray : public geometry<DIMENSION, T>
	{
	public:

		using geometry_type = geometry<DIMENSION, T>;
		using vec_type = typename geometry_type::vec_type;

		/** default constructor */
		ray() = default;
		/** copy constructor */
		ray(ray const& src) = default;
		/** other constructor */
		ray(vec_type const & in_position, vec_type const & in_direction) : position(in_position), direction(in_direction) {}
		/** construct a ray from 2 points */
		ray(std::pair<vec_type, vec_type> const & pts) : position(pts.first), direction(glm::normalize(pts.second - pts.first)) {}

	public:

		/** the starting position of the ray in space */
		vec_type position;
		/** the direction of the ray in space */
		vec_type direction;
	};

// ==============================================================================================
// box_plane classes
// ==============================================================================================

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