#pragma once

#include <chaos/StandardHeaders.h>

namespace chaos
{

	// ==============================================================================================
	// Some common typedefs for geometry
	// ==============================================================================================

	// the base template (two arguments, type and dimension)
	template<typename T, int dimension> class type_ray;
	template<typename T, int dimension> class type_box;
	template<typename T, int dimension> class type_obox;
	template<typename T, int dimension> class type_sphere;
	template<typename T, int dimension> class type_triangle;
	template<typename T, int dimension> class type_rotator; // this is not an object that describes a rotation, but a meta object that gives the rotation in a meta function
	// remove the dimension from the template (keep type)
	template<typename T> using type_ray2      = type_ray<T, 2>;
	template<typename T> using type_ray3      = type_ray<T, 3>;
	template<typename T> using type_box2      = type_box<T, 2>;
	template<typename T> using type_box3      = type_box<T, 3>;
	template<typename T> using type_obox2     = type_obox<T, 2>;
	template<typename T> using type_obox3     = type_obox<T, 3>;
	template<typename T> using type_sphere2   = type_sphere<T, 2>;
	template<typename T> using type_sphere3   = type_sphere<T, 3>;
	template<typename T> using type_triangle2 = type_triangle<T, 2>;
	template<typename T> using type_triangle3 = type_triangle<T, 3>;
	template<typename T> using type_rotator2  = type_rotator<T, 2>; // theses is not objects that describe a rotation, but meta objects that give the rotation in a meta function
	template<typename T> using type_rotator3  = type_rotator<T, 3>; //

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

	/** base class for geometry objects that have some commons definitions */
	template<typename T> 
	class type_geometric_base
	{
	public:

		/** the type of the components */
		using type = T;

		/** the type of 2D vectors */
		using vec2_type = glm::tvec2<type>;
		/** the type of 3D vectors */
		using vec3_type = glm::tvec3<type>;

		/** the type of 2D planes */
		using plane2_type = glm::tvec3<type>;
		/** the type of 3D planes */
		using plane3_type = glm::tvec4<type>;

		/** the type of rotation in 2D space (just an angle in RADIAN) */
		using rot2_type = typename type_rotator2<T>::type;
		/** the type of rotation in 3D space (a quaternion) */
		using rot3_type = typename type_rotator3<T>::type;

		/** the type of 2D ray */
		using ray2_type = type_ray2<type>;
		/** the type of 3D ray */
		using ray3_type = type_ray3<type>;

		/** the type of 2D sphere */
		using sphere2_type = type_sphere2<type>;
		/** the type of 3D sphere */
		using sphere3_type = type_sphere3<type>;

		/** the type of 2D box */
		using box2_type = type_box2<type>;
		/** the type of 3D box */
		using box3_type = type_box3<type>;

		/** the type of 2D obox */
		using obox2_type = type_obox2<type>;
		/** the type of 3D obox */
		using obox3_type = type_obox3<type>;

		/** the type of 2D triangle */
		using triangle2_type = type_triangle2<type>;
		/** the type of 3D triangle */
		using triangle3_type = type_triangle3<type>;
	};

	template<typename T, int dimension> class type_geometric;

	/** geometry class specialisation for 2 dimensions objects */
	template<typename T> 
	class type_geometric<T, 2> : protected type_geometric_base<T> // 'protected' because we don t want in 2D to have access to 3D data
	{
	public:

		/** the dimension of the space */
		static int const dimension = 2;

		/** the type of vector */
		using vec_type = vec2_type;
		/** the type of plane */
		using plane_type = plane2_type;
		/** the type of rotator */
		using rot_type = rot2_type;
		/** the type of ray */
		using ray_type = ray2_type;
		/** the type of sphere */
		using sphere_type = sphere2_type;
		/** the type of box */
		using box_type = box2_type;
		/** the type of obox */
		using obox_type = obox2_type;
		/** the type of triangle */
		using triangle_type = triangle2_type;
	};

	/** geometry class specialisation for 3 dimensions objects */
	template<typename T>
	class type_geometric<T, 3> : protected type_geometric_base<T> // 'protected' because we don t want in 3D to have access to 2D data
	{
	public:

		/** the dimension of the space */
		static int const dimension = 3;

		/** the type of vector */
		using vec_type = vec3_type;
		/** the type of plane */
		using plane_type = plane3_type;
		/** the type of rotator */
		using rot_type = rot3_type;
		/** the type of ray */
		using ray_type = ray3_type;
		/** the type of sphere */
		using sphere_type = sphere3_type;
		/** the type of box */
		using box_type = box3_type;
		/** the type of obox */
		using obox_type = obox3_type;
		/** the type of triangle */
		using triangle_type = triangle3_type;
	};

	// ==============================================================================================
	// box_base class
	// ==============================================================================================

	template<typename T, int dimension>
	class box_base : public type_geometric<T, dimension>
	{
	public:

		using vec_type = typename type_geometric<T, dimension>::vec_type;
		using type     = typename type_geometric<T, dimension>::type;

		/** constructor (empty box) */
		box_base() : half_size((T)-1.0f) {}
		/** copy constructor */
		box_base(box_base const & src) : position(src.position), half_size(src.half_size) {}
		/** other constructor */
		box_base(vec_type const & in_position, vec_type const & in_half_size) : position(in_position), half_size(in_half_size) {}

	public:

		/** the center of the box */
		vec_type position;
		/** the half size the box */
		vec_type half_size;
	};

	// ==============================================================================================
	// box class
	// ==============================================================================================

	template<typename T, int dimension>
	class type_box : public box_base<T, dimension>
	{
	public:

		using vec_type = typename box_base<T, dimension>::vec_type;
		using type     = typename box_base<T, dimension>::type;

		/** constructor (empty box) */
		type_box() {}
		/** copy constructor */
		type_box(type_box const & src) : box_base(src.position, src.half_size) {}
		/** other constructor */
		type_box(vec_type const & in_position, vec_type const & in_half_size) : box_base(in_position, in_half_size) {}
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
	class type_obox : public box_base<T, dimension>
	{
	public:

		using vec_type = typename box_base<T, dimension>::vec_type;
		using type     = typename box_base<T, dimension>::type;
		using rot_type = typename box_base<T, dimension>::rot_type;

		/** constructor (empty box) */
		type_obox() {}
		/** copy constructor */
		type_obox(type_obox const & src) : box_base(src.position, src.half_size), rotator(src.rotator) {}
		/** other constructor */
		type_obox(vec_type const & in_position, vec_type const & in_half_size, rot_type const & in_rotator) : box_base(in_position, in_half_size), rotator(in_rotator) {}

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
		type_sphere() : radius((T)-1.0f) {}
		/** copy constructor */
		type_sphere(type_sphere const & src) : position(src.position), radius(src.radius) {}
		/** other constructor */
		type_sphere(vec_type const & in_position, type in_radius) : position(in_position), radius(in_radius) {}

	public:

		/** the center of the circle */
		vec_type position;
		/** the radius of the circle */
		type radius;
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
		type_triangle() {}
		/** copy constructor */
		type_triangle(type_triangle const & src) : a(src.a), b(src.b), c(src.c) {}
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
		type_ray() {}
		/** copy constructor */
		type_ray(type_ray const & src) : position(src.position), direction(src.direction) {}
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
	// The final objects
	// ==============================================================================================

	// remove all arguments from template (the objects to use for real)
	using ray2 = type_ray2<float>;
	using ray3 = type_ray3<float>;
	using box2 = type_box2<float>;
	using box3 = type_box3<float>;
	using obox2 = type_obox2<float>;
	using obox3 = type_obox3<float>;
	using sphere2 = type_sphere2<float>;
	using sphere3 = type_sphere3<float>;
	using triangle2 = type_triangle2<float>;
	using triangle3 = type_triangle3<float>;
	using rotator2 = type_rotator2<float>::type; // this are ROTATION here (angle or quaternion)
	using rotator3 = type_rotator3<float>::type; //

}; // namespace chaos

