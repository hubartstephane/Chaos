#ifdef CHAOS_FORWARD_DECLARATION

namespace chaos
{
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
	template<typename T, int dimension> class type_rotator; // this is not an object that describes a rotation, but a meta object that gives the rotation in a meta function

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
	
	//using rotator2 = float; // this are ROTATION here (angle or quaternion)
	//using rotator3 = glm::quat; //
	
	
	//using rotator2 = typename type_rotator2<float>::type; // this are ROTATION here (angle or quaternion)
	//using rotator3 = typename type_rotator3<float>::type; //

}; // namespace chaos

#else 

namespace chaos
{
	// ==============================================================================================
	// rotators
	// ==============================================================================================

	// XXX : depending whether we are in 2D or 3D a rotation can be described by a single float or a quaternion
	//       this code is here to provide some common interface
	//
	// XXX : for 3D rotations we use a quaternion. quaternions have a constructor that make them valid (constructor = 0 rotator)
	//       for 2D we simply use float/double. that means that this is NOT INITIALIZED by default !
	//       (I could use a class to embedd a single scalar but the conversion/addition ... operators implementation would be too tedious

	class zero_rotator
	{
	public:

		operator float() const { return 0.0f; }

		operator double() const { return 0.0; }

		operator glm::quat() const { return glm::quat(); }

		operator glm::dquat() const { return glm::dquat(); }
	};


	// base template
	template<typename T, int dimension> 
	class type_rotator;
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
		/** the type of vector */
		using vec_type = glm::tvec2<type>;
		/** the type of plane */
		using plane_type = glm::tvec3<type>;
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
	};

	template<typename T>
	class type_geometric<T, 3>
	{
	public:
		/** the dimension of the space */
		static constexpr int dimension = 3;
		/** the type of the components */
		using type = T;

		/** the type of vector */
		using vec_type = glm::tvec3<type>;
		/** the type of plane */
		using plane_type = glm::tvec4<type>;
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
		vec_type half_size = vec_type(T(-1));
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
		type_box(type_box_base<T, dimension> const & src) : type_box_base(src.position, src.half_size) {}
		/** other constructor */
		type_box(vec_type const & in_position, vec_type const & in_half_size) : type_box_base(in_position, in_half_size) {}
		/** construct a box from 2 points */
		type_box(std::pair<vec_type, vec_type> const & pts)
		{
			vec_type a = glm::min(pts.first, pts.second);
			vec_type b = glm::max(pts.first, pts.second);

			position = (b + a) / static_cast<T>(2);
			half_size = (b - a) / static_cast<T>(2);
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
		type_obox(type_box_base<T, dimension> const& src) : type_box_base(src.position, src.half_size){}
		/** other constructor */
		type_obox(vec_type const & in_position, vec_type const & in_half_size, rot_type const & in_rotator) : type_box_base(in_position, in_half_size), rotator(in_rotator) {}

	public:

		/** the angle/quaternion of rotation to apply to a box to have this obox */
		rot_type rotator = zero_rotator();
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

}; // namespace chaos

#endif // CHAOS_FORWARD_DECLARATION



