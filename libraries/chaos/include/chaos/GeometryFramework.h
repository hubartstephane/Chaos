#pragma once

#include <chaos/StandardHeaders.h>
#include <chaos/GLMTools.h>

namespace chaos
{

	// ==============================================================================================
	// Some common typedefs for geometry
	// ==============================================================================================

	template<typename T, int dimension> class type_ray;
	template<typename T, int dimension> class type_box;
	template<typename T, int dimension> class type_obox;
	template<typename T, int dimension> class type_sphere;
	template<typename T, int dimension> class type_triangle;

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

	using ray2      = type_ray2<float>;
	using ray3      = type_ray3<float>;
	using box2      = type_box2<float>;
	using box3      = type_box3<float>;
	using obox2     = type_obox2<float>;
	using obox3     = type_obox3<float>;
	using sphere2   = type_sphere2<float>;
	using sphere3   = type_sphere3<float>;
	using triangle2 = type_triangle2<float>;
	using triangle3 = type_triangle3<float>;

	// ==============================================================================================
	// rotator initializer
	// ==============================================================================================

	// XXX : depending whether we are in 2D or 3D a rotation can be described by a single float or a quaternion
	//       this code is here to provide some common initialization interface

	/** a meta structure for generalized rotator initialization (angle in RAD or quaternion) */
	template<typename T> 
	struct make_rotator; // main definition
	template<>
	struct make_rotator<float>{ static float value() { return 0.0f; } };
	template<>
	struct make_rotator<double>{ static double value() { return 0.0; } };
	template<>
	struct make_rotator<glm::vec4>{ static glm::vec4 value() { return glm::vec4(1.0f, 0.0f, 0.0f, 0.0f); } };
	template<>
	struct make_rotator<glm::tvec4<double>>{ static glm::tvec4<double> value() { return glm::tvec4<double>(1.0, 0.0, 0.0, 0.0); } };

	// ==============================================================================================
	// geometry classes
	// ==============================================================================================

	/**
	* base class for geometry objects that have some commons definitions
	*/

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

		/** the type of rotation in 2D space (just an angle in RADIAN) */
		using rotator2_type = type;
		/** the type of rotation in 3D space (a quaternion) */
		using rotator3_type = glm::tvec4<type>;
	};

	template<typename T, int dimension> class type_geometric;

	/**
	* geometry class specialisation for 2 dimensions objects
	*/

	template<typename T> 
	class type_geometric<T, 2> : public type_geometric_base<T>
	{
	public:

		/** the dimension of the space */
		static int const dimension = 2;
		/** the type of vector */
		using vec_type = vec2_type;
		/** the type of rotator */
		using rotator_type = rotator2_type;
	};

	/**
	* geometry class specialisation for 3 dimensions objects
	*/

	template<typename T>
	class type_geometric<T, 3> : public type_geometric_base<T>
	{
	public:

		/** the dimension of the space */
		static int const dimension = 3;
		/** the type of vector */
		using vec_type = vec3_type;
		/** the type of rotator */
		using rotator_type = rotator3_type;
	};


	// ==============================================================================================
	// boxes classes
	// ==============================================================================================

	template<typename T, int dimension>
	class type_box : public type_geometric<T, dimension>
	{
	public:

		using vec_type = typename type_geometric<T, dimension>::vec_type;

		/** constructor (empty box) */
		type_box() : half_size((T)-1.0f) {}
		/** copy constructor */
		type_box(type_box const & src) : position(src.position), half_size(src.half_size) {}
		/** other constructor */
		type_box(vec_type const & in_position, vec_type const & in_half_size) : position(in_position), half_size(in_half_size) {}
		/** construct a box from 2 points */
		type_box(std::pair<vec_type, vec_type> const & pts)
		{
			vec_type a = glm::min(pts.first, pts.second);
			vec_type b = glm::max(pts.first, pts.second);

			position  = (b + a) / static_cast<T>(2);
			half_size = (b - a) / static_cast<T>(2);
		}

		/** get the corners of the box */
		std::pair<vec_type, vec_type> GetCorners() const
		{
			if (!IsEmpty())
				return std::make_pair(position - half_size, position + half_size);
				
			return std::make_pair(position, position);						
		}

		/** returns true whether the box is empty */
		bool IsEmpty() const
		{
			return glm::any(glm::lessThan(half_size, vec_type((T)0.0f)));
		}

		/** set the box has an empty box */
		void SetEmpty()
		{
			half_size = vec_type((T)-1.0f);
		}

		/** increase the box size with a single vertex */
		void Extend(vec_type const & v)
		{
			if (IsEmpty())
			{
				position = v;
				half_size = vec_type((T)0.0f);
			}
			else
			{
				std::pair<vec_type, vec_type> corners = GetCorners();
				corners.first  = glm::min(corners.first, v);
				corners.second = glm::max(corners.second, v);
				*this = type_box(corners);
			}
		}

		/** returns true whether the point is contained in the box */
		bool Contains(vec_type const & pt) const
		{
			return glm::all(glm::lessThanEqual(glm::abs(pt - position), half_size));
		}

	public:

		/** the center of the box */
		vec_type position;
		/** the half size the box */
		vec_type half_size;
	};

	// ==============================================================================================
	// box functions
	// ==============================================================================================

	/** equality function for box */
	template<typename T, int dimension>
	bool operator == (type_box<T, dimension> const & b1, type_box<T, dimension> const & b2)
	{
		return (b1.position == b1.position) && (b1.half_size == b2.half_size);
	}

	/** difference function for box */
	template<typename T, int dimension>
	bool operator != (type_box<T, dimension> const & b1, type_box<T, dimension> const & b2)
	{
		return !(b1 == b2);
	}

	/** intersection of 2 boxes */
	template<typename T, int dimension>
	type_box<T, dimension> operator & (const type_box<T, dimension> & b1, const type_box<T, dimension> & b2)
	{
		using vec_type = type_box<T, dimension>::vec_type;

		if (b1.IsEmpty() || b2.IsEmpty()) // any of the 2 is empty, intersection is empty
			return type_box<T, dimension>();

		vec_type A1 = b1.position + b1.half_size;
		vec_type B2 = b2.position - b2.half_size;

		if (glm::any(glm::lessThanEqual(A1, B2)))
			return type_box<T, dimension>();

		vec_type B1 = b1.position - b1.half_size;
		vec_type A2 = b2.position + b2.half_size;

		if (glm::any(glm::lessThanEqual(A2, B1)))
			return type_box<T, dimension>();

		vec_type A = glm::min(A1, A2);
		vec_type B = glm::max(B1, B2);

		return type_box<T, dimension>(std::make_pair(A, B));
	}

	/** union of 2 boxes */
	template<typename T, int dimension>
	type_box<T, dimension> operator | (const type_box<T, dimension> & b1, const type_box<T, dimension> & b2)
	{
		using vec_type = type_box<T, dimension>::vec_type;

		if (b1.IsEmpty()) // if one is empty, returns other
			return b2;
		if (b2.IsEmpty())
			return b1;

		vec_type A1 = b1.position + b1.half_size;
		vec_type A2 = b2.position + b2.half_size;

		vec_type B1 = b1.position - b1.half_size;
		vec_type B2 = b2.position - b2.half_size;

		vec_type A = glm::max(A1, A2);
		vec_type B = glm::min(B1, B2);

		return type_box<T, dimension>(std::make_pair(A, B));
	}

	/** returns one of the sub-boxes obtained by splitting the src */
	template<typename T>
	type_box2<T> GetSplitBox(type_box2<T> const & b, int i, int j)
	{
		using vec2_type = type_box2<T>::vec2_type;

		assert((i == 0) || (i == 1));
		assert((j == 0) || (j == 1));
		i = (i << 1) - 1;
		j = (j << 1) - 1;
		type_box2<T> new_half_size = b.half_size / static_cast<T>(2);

		return type_box2<T>(
			b.position + new_half_size * vec2_type(static_cast<T>(i), static_cast<T>(j)),
			new_half_size);
	}

	/** returns one of the sub-boxes obtained by splitting the src */
	template<typename T>
	type_box3<T> GetSplitBox(type_box3<T> const & b, int i, int j, int k)
	{
		using vec3_type = type_box3<T>::vec3_type;

		assert((i == 0) || (i == 1));
		assert((j == 0) || (j == 1));
		assert((k == 0) || (k == 1));
		i = (i << 1) - 1;
		j = (j << 1) - 1;
		k = (k << 1) - 1;
		vec3_type new_half_size = b.half_size / static_cast<T>(2);

		return type_box3<T>(
			b.position + new_half_size * vec3_type(static_cast<T>(i), static_cast<T>(j), static_cast<T>(k)),
			new_half_size);
	}


	// ==============================================================================================
	// oriented boxes classes
	// ==============================================================================================

	/** an oriented bounding box */
	template<typename T, int dimension>
	class type_obox : public type_geometric<T, dimension>
	{
	public:

		using vec_type = typename type_geometric<T, dimension>::vec_type;

		using rotator_type = typename type_geometric<T, dimension>::rotator_type;


		/** constructor (empty box) */
		type_obox() : half_size((T)-1.0f) {}
		/** copy constructor */
		type_obox(type_obox const & src) : position(src.position), half_size(src.half_size), rotator(src.rotator) {}
		/** other constructor */
		type_obox(vec_type const & in_position, vec_type const & in_half_size, rotator_type const & in_rotator) : position(in_position), half_size(in_half_size), rotator(in_rotator){}











		/** returns true whether the box is empty */
		bool IsEmpty() const
		{
			return glm::any(glm::lessThan(half_size, vec_type((T)0.0f)));
		}

		/** set the box has an empty box */
		void SetEmpty()
		{
			half_size = vec_type((T)-1.0f);
		}

	public:


		/** the center of the box */
		vec_type position;
		/** the half size the box */
		vec_type half_size;
		/** the angle/quaternion of rotation to apply to a box to have this obox */
		rotator_type rotator = make_rotator<rotator_type>::value();
	};

	// ==============================================================================================
	// obox functions
	// ==============================================================================================





	// ==============================================================================================
	// box & obox functions
	// ==============================================================================================

	// XXX : i introduced 'int DIMENSION' in the BOX_TEMPLATE else when calling it seems to resolve to type_geometric !!
	//       need some research to understand more clearly

	/** returns the perimeter of the box */
	template<typename T, template<typename U, int DIMENSION> class BOX_TEMPLATE> 
	T GetPerimeter(BOX_TEMPLATE<T, 2> const & b)
	{
		return static_cast<T>(4) * (b.half_size.x + b.half_size.y);
	}

	/** returns the surface of the box */
	template<typename T, template<typename U, int DIMENSION> class BOX_TEMPLATE>
	T GetSurface(BOX_TEMPLATE<T, 2> const & b)
	{
		return static_cast<T>(4) * (b.half_size.x * b.half_size.y);
	}

	/** return the volume of the box */
	template<typename T, template<typename U, int DIMENSION> class BOX_TEMPLATE>
	T GetVolume(BOX_TEMPLATE<T, 3> const & b)
	{
		return static_cast<T>(8) * b.half_size.x * b.half_size.y * b.half_size.z;
	}

	/** return the surface of the box */
	template<typename T, template<typename U, int DIMENSION> class BOX_TEMPLATE>
	T GetSurface(BOX_TEMPLATE<T, 3> const & b)
	{
		return static_cast<T>(8) *((b.half_size.x * b.half_size.y) + (b.half_size.y * b.half_size.z) + (b.half_size.z * b.half_size.x));
	};

	/** returns the bounding circle for the box */
	template<typename T, template<typename U, int DIMENSION> class BOX_TEMPLATE>
	type_sphere2<T> GetBoundingCircle(BOX_TEMPLATE<T, 2> const & b)
	{
		return b.IsEmpty() ? type_sphere2<T>() : type_sphere2<T>(b.position, glm::length(b.half_size));
	}

	/** returns the inner circle for the box */
	template<typename T, template<typename U, int DIMENSION> class BOX_TEMPLATE>
	type_sphere2<T> GetInnerCircle(BOX_TEMPLATE<T, 2> const & b)
	{
		return b.IsEmpty() ? type_sphere2<T>() : type_sphere2<T>(b.position, GLMTools::GetMinComponent(b.half_size));
	}

	/** returns the bounding sphere for the box */
	template<typename T, template<typename U, int DIMENSION> class BOX_TEMPLATE>
	type_sphere3<T> GetBoundingSphere(BOX_TEMPLATE<T, 3> const & b)
	{
		return b.IsEmpty() ? type_sphere3<T>() : type_sphere3<T>(b.position, glm::length(b.half_size));
	}

	/** returns the inner sphere for the box */
	template<typename T, template<typename U, int DIMENSION> class BOX_TEMPLATE>
	type_sphere3<T> GetInnerSphere(BOX_TEMPLATE<T, 3> const & b)
	{
		return b.IsEmpty() ? type_sphere3<T>() : type_sphere3<T>(b.position, GLMTools::GetMinComponent(b.half_size));
	}

	/** returns the "aspect" of the box (width/height) */
	template<typename T, template<typename U, int DIMENSION> class BOX_TEMPLATE>
	T GetBoxAspect(BOX_TEMPLATE<T, 2> const & b)
	{
		return (b.half_size.y) ? (b.half_size.x / b.half_size.y) : static_cast<T>(1);
	}








	// ==============================================================================================
	// triangle classes
	// ==============================================================================================

	template<typename T, int dimension>
	class type_triangle : public type_geometric<T, dimension>
	{
	public:

		using vec_type = typename type_geometric<T, dimension>::vec_type;

		/** default constructor */
		type_triangle() {}
		/** copy constructor */
		type_triangle(type_triangle const & src) : a(src.a), b(src.b), c(src.c) {}
		/** constructor from points */
		type_triangle(vec_type const & in_a, vec_type const & in_b, vec_type const & in_c) : a(in_a), b(in_b), c(in_c) {}

		/** returns true whether the triangle is empty */
		bool IsEmpty() const
		{
			if (a == b || a == c || b == c)
				return true;
			return false;
		}

		/** get the reversed triangle */
		type_triangle GetInvertedTriangle() const
		{
			return type_triangle(a, c, b);
		}

		/** returns true whether the point is contained in the triangle */
		bool Contains(vec_type const & pt) const
		{
			// test whether the triangle is null
			if (IsEmpty())
				return false;

			// test whether the point is inside the edges
			vec_type const * V = &a;
			for (int i = 0; i < 3; ++i)
			{
				vec_type const & e1 = V[i];
				vec_type const & e2 = V[(i + 1) % 3];

				vec_type e1_S = pt - e1;
				vec_type normalized_edge = glm::normalize(e2 - e1);

				auto d = GLMTools::Get2DCrossProductZ(normalized_edge, e1_S); // cross product, in plane, the only valid coordinate is Z = (x.y') - (x'y)
				if (d > 0.0f)
					return false;
			}
			return true;    
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
	// triangles functions
	// ==============================================================================================

	/** equality test function for triangles */
	template<typename T, int dimension>
	bool operator == (type_triangle<T, dimension> const & t1, type_triangle<T, dimension> const & t2)
	{
		if (t1.a == t2.a)
		{
			if (t1.b == t2.b && t1.c == t2.c)
				return true;
			if (t1.c == t2.b && t1.b == t2.c)
				return true;
		}
		if (t1.a == t2.b)
		{
			if (t1.b == t2.a && t1.c == t2.c)
				return true;
			if (t1.c == t2.a && t1.b == t2.c)
				return true;
		}
		if (t1.a == t2.c)
		{
			if (t1.b == t2.a && t1.c == t2.b)
				return true;
			if (t1.c == t2.a && t1.b == t2.b)
				return true;
		}
		return false;
	}

	/** difference test function for triangle */
	template<typename T, int dimension>
	bool operator != (type_triangle<T, dimension> const & t1, type_triangle<T, dimension> const & t2)
	{
		return !(t1 == t2);
	}

	// ==============================================================================================
	// ray classes
	// ==============================================================================================

	template<typename T, int dimension>
	class type_ray : public type_geometric<T, dimension>
	{
	public:

		using vec_type = typename type_geometric<T, dimension>::vec_type;

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
	// ray functions
	// ==============================================================================================

	/** equality function for ray */
	template<typename T, int dimension>
	bool operator == (type_ray<T, dimension> const & r1, type_ray<T, dimension> const & r2)
	{
		return (r1.position == r2.position) && (r1.direction == r2.direction);
	}

	/** difference function for ray */
	template<typename T, int dimension>
	bool operator != (type_ray<T, dimension> const & r1, type_ray<T, dimension> const & r2)
	{
		return !(r1 == r2);
	}

	// ==============================================================================================
	// sphere/circle classes
	// ==============================================================================================

	template<typename T, int dimension>
	class type_sphere : public type_geometric<T, dimension>
	{
	public:

		using vec_type = typename type_geometric<T, dimension>::vec_type;
		using type     = typename type_geometric<T, dimension>::type;

		/** constructor (empty circle) */
		type_sphere() : radius((T)-1.0f) {}
		/** copy constructor */
		type_sphere(type_sphere const & src) : position(src.position), radius(src.radius) {}
		/** other constructor */
		type_sphere(vec_type const & in_position, type in_radius) : position(in_position), radius(in_radius) {}

		/** returns true whether the circle is empty */
		bool IsEmpty() const
		{
			return (radius < 0);
		}

		/** set the circle has an empty circle */
		void SetEmpty()
		{
			radius = (T)-1.0f;
		}

		/** returns true whether the point is contained in the circle */
		bool Contains(vec_type const & pt) const
		{
			return glm::length2(pt - position) <= radius * radius;
		}

	public:

		/** the center of the circle */
		vec_type position;
		/** the radius of the circle */
		type radius;
	};


	// ==============================================================================================
	// sphere/circle functions
	// ==============================================================================================

	/** equality function for circle */
	template<typename T, int dimension>
	bool operator == (type_sphere<T, dimension> const & c1, type_sphere<T, dimension> const & c2)
	{
		return (b1.position == b1.position) && (b1.radius == b2.radius);
	}

	/** difference function for circle */
	template<typename T, int dimension>
	bool operator != (type_sphere<T, dimension> const & c1, type_sphere<T, dimension> const & c2)
	{
		return !(c1 == c2);
	}

	/** returns the perimeter of the circle */
	template<typename T>
	float GetPerimeter(type_sphere2<T> const & c)
	{
		return static_cast<T>(2.0 * M_PI) * c.radius;
	}

	/** returns the surface of the circle */
	template<typename T>
	float GetSurface(type_sphere2<T> const & c)
	{
		return static_cast<T>(M_PI) * c.radius * c.radius;
	}

	/** returns the volume of the sphere */
	template<typename T>
	T GetVolume(type_sphere3<T> const & s)
	{
		return static_cast<T>((4.0 / 3.0) * M_PI) * s.radius * s.radius * s.radius;
	}

	/** returns the surface of the sphere */
	template<typename T>
	float GetSurface(type_sphere3<T> const & s)
	{
		return static_cast<T>(4.0 * M_PI) * s.radius * s.radius;
	}


	/** returns the bounding box of the circle */
	template<typename T>
	type_box2<T> GetBoundingBox(type_sphere2<T> const & c)
	{
		using vec_type = typename type_sphere2<T>::vec_type;

		return c.IsEmpty() ? type_box2<T>() : type_box2<T>(c.position, vec_type(c.radius, c.radius));
	}

	/** returns the bounding box of the circle (square) */
	template<typename T>
	type_box2<T> GetInnerBox(type_sphere2<T> const & c)
	{
		using vec_type = typename type_sphere2<T>::vec_type;

		static double const INV_SQRT2 = 0.707106781186547; /* 1.0 / sqrtf(2.0) */

		return c.IsEmpty() ? type_box2<T>() : type_box2<T>(c.position, vec_type(c.radius * static_cast<T>(INV_SQRT2)));
	}

	template<typename T>
	type_box3<T> GetBoundingBox(type_sphere3<T> const & s)
	{
		using vec_type = typename type_sphere3<T>::vec_type;

		return s.IsEmpty() ? type_box3<T>() : type_box3<T>(s.position, vec_type(s.radius));
	}

	template<typename T>
	type_box3<T> GetInnerBox(type_sphere3<T> const & s)
	{
		using vec_type = typename type_sphere3<T>::vec_type;

		static double const INV_SQRT3 = 0.577350269189625; /* 1.0 / sqrtf(3.0) */

		return s.IsEmpty() ? type_box3<T>() : type_box3<T>(s.position, vec_type(s.radius * static_cast<T>(INV_SQRT3)));
	}

	/** returns intersection of 2 spheres */
	template<typename T, int dimension>
	type_sphere<T, dimension> operator & (const type_sphere<T, dimension> & s1, const type_sphere<T, dimension> & s2) // intersection
	{
		using vec_type = typename type_sphere<T, dimension>::vec_type;

		if (s1.IsEmpty() || s2.IsEmpty())
			return type_sphere<T, dimension>();
		if (s1.position == s2.position)
			return type_sphere<T, dimension>(s1.position, glm::min(s1.radius, s2.radius));

		vec_type delta_pos = s2.position - s1.position;   // vector that go from center 1 to center 2  
		T        distance  = glm::length(delta_pos);      // length of such a vector

		if (distance >= s1.radius + s2.radius) // sphere too far => returns the empty sphere
			return type_sphere<T, dimension>();

		T t1 = s1.radius / distance;  // positive     
		T t2 = s2.radius / distance;  // positive 

		T a = glm::max(-t1, static_cast<T>(1) - t2);
		T b = glm::min( t1, static_cast<T>(1) + t2);

		return type_sphere<T, dimension>(
			s1.position + ((b + a) / static_cast<T>(2)) * delta_pos,
			((b - a) / static_cast<T>(2)) * distance);
	}

	/** returns union of 2 spheres */
	template<typename T, int dimension>
	type_sphere<T, dimension> operator | (const type_sphere<T, dimension> & s1, const type_sphere<T, dimension> & s2) // union
	{
		using vec_type = typename type_sphere<T, dimension>::vec_type;

		if (s1.IsEmpty())
			return s2;
		if (s2.IsEmpty())
			return s1;
		if (s1.position == s2.position)
			return type_sphere<T, dimension>(s1.position, glm::max(s1.radius, s2.radius));

		vec_type delta_pos = s2.position - s1.position;    // vector that go from center 1 to center 2  
		T        distance  = glm::length(delta_pos);       // length of such a vector

		T t1 = s1.radius / distance;  // positive   
		T t2 = s2.radius / distance;  // positive   

		T a = glm::min(-t1, static_cast<T>(1) - t2);
		T b = glm::max( t1, static_cast<T>(1) + t2);

		return type_sphere<T, dimension>(
			s1.position + ((b + a) / static_cast<T>(2)) * delta_pos,
			((b - a) / static_cast<T>(2)) * distance);
	}

	/** reduce a rectangle with an aspect */
	box2 ShrinkBoxToAspect(box2 src, float aspect);
	/** transform rectangle to have desire aspect (if a component is 0, change it, otherwise take more prioritize aspect) */
	box2 AlterBoxToAspect(box2 src, float aspect, bool prefer_width_update = true);


	/** encode a box2 into a vector4 */
	glm::vec4 EncodeBoxToVector(chaos::box2 const & src);

}; // namespace chaos

