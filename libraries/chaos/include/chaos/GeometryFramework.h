#pragma once

#include <chaos/StandardHeaders.h>
#include <chaos/GLMTools.h>
#include <chaos/GeometryClasses.h>

namespace chaos
{
	// ==============================================================================================
	// GLM vector functions
	// ==============================================================================================

	/** cast a vector into another vector (arity and type may change) */
	template<typename VECTOR_TYPE1, typename VECTOR_TYPE2>
	static VECTOR_TYPE1 RecastVector(VECTOR_TYPE2 const& value)
	{
		VECTOR_TYPE1 result(0);
		size_t count = std::min(result.length(), value.length());
		for (size_t i = 0; i < count; ++i)
			result[i] = static_cast<VECTOR_TYPE1::value_type>(value[i]);

		return result;
	}

	// ==============================================================================================
	// box_base functions
	// ==============================================================================================

	// shu46 : half_size.x == 0  -> pas vide


	/** returns true whether the box is empty */
	template<typename T, int dimension>
	bool IsGeometryEmpty(box_base<T, dimension> const & b)
	{
		return glm::any(glm::lessThan(b.half_size, box_base<T, dimension>::vec_type((T)0.0f)));
	}

	/** set the box has an empty box */
	template<typename T, int dimension>
	void SetGeometryEmpty(box_base<T, dimension> & b)
	{
		b.half_size = box_base<T, dimension>::vec_type((T)-1.0f);
	}

	/** returns the perimeter of the box */
	template<typename T>
	T GetPerimeter(box_base<T, 2> const & b)
	{
		return static_cast<T>(4) * (b.half_size.x + b.half_size.y);
	}

	/** returns the surface of the box */
	template<typename T>
	T GetSurface(box_base<T, 2> const & b)
	{
		return static_cast<T>(4) * (b.half_size.x * b.half_size.y);
	}

	/** return the volume of the box */
	template<typename T>
	T GetVolume(box_base<T, 3> const & b)
	{
		return static_cast<T>(8) * b.half_size.x * b.half_size.y * b.half_size.z;
	}

	/** return the surface of the box */
	template<typename T>
	T GetSurface(box_base<T, 3> const & b)
	{
		return static_cast<T>(8) *((b.half_size.x * b.half_size.y) + (b.half_size.y * b.half_size.z) + (b.half_size.z * b.half_size.x));
	};

	/** returns the bounding circle for the box */
	template<typename T>
	type_sphere2<T> GetBoundingSphere(box_base<T, 2> const & b)
	{
		return IsGeometryEmpty(b) ? type_sphere2<T>() : type_sphere2<T>(b.position, glm::length(b.half_size));
	}

	/** returns the inner circle for the box */
	template<typename T>
	type_sphere2<T> GetInnerSphere(box_base<T, 2> const & b)
	{
		return IsGeometryEmpty(b) ? type_sphere2<T>() : type_sphere2<T>(b.position, GLMTools::GetMinComponent(b.half_size));
	}

	/** returns the bounding sphere for the box */
	template<typename T>
	type_sphere3<T> GetBoundingSphere(box_base<T, 3> const & b)
	{
		return IsGeometryEmpty(b) ? type_sphere3<T>() : type_sphere3<T>(b.position, glm::length(b.half_size));
	}

	/** returns the inner sphere for the box */
	template<typename T>
	type_sphere3<T> GetInnerSphere(box_base<T, 3> const & b)
	{
		return IsGeometryEmpty(b) ? type_sphere3<T>() : type_sphere3<T>(b.position, GLMTools::GetMinComponent(b.half_size));
	}

	/** returns the "aspect" of the box (width/height) */
	template<typename T>
	T GetBoxAspect(box_base<T, 2> const & b)
	{
		return (b.half_size.y) ? (b.half_size.x / b.half_size.y) : static_cast<T>(1);
	}

	/** reduce a rectangle with an aspect (we do not use box_base<> because we want the result to be the exact type of the entry) */
	template<typename BOX_TYPE>
	BOX_TYPE ShrinkBoxToAspect(BOX_TYPE const & src, typename BOX_TYPE::type aspect)
	{
		// any negative component
		if (IsGeometryEmpty(src))
			return src;
		// aspect already good
		BOX_TYPE::type effective_aspect = GetBoxAspect(src);
		if (effective_aspect == aspect)
			return src;
		// make the update
		BOX_TYPE result = src;
		if (effective_aspect > aspect) // width too large
			result.half_size.x = src.half_size.y * aspect;
		else if (effective_aspect < aspect) // height too large
			result.half_size.y = src.half_size.x / aspect;

		return result;
	}

	/** transform rectangle to have desire aspect (if a component is 0, change it, otherwise take more prioritize aspect) */
	template<typename BOX_TYPE>
	BOX_TYPE AlterBoxToAspect(BOX_TYPE const & src, typename BOX_TYPE::type aspect, bool prefere_update_width)
	{
		using type = typename BOX_TYPE::type;

		// any (non null) negative component
		if (IsGeometryEmpty(src))
			return src;
		// cannot have no size
		if (src.half_size.x == (type)0 && src.half_size.y == (type)0)
			return src;
		// width axis to update ?
		if (src.half_size.x == (type)0)
			prefere_update_width = true;
		else if (src.half_size.y == (type)0)
			prefere_update_width = false;
		// make the update
		BOX_TYPE result = src;
		if (prefere_update_width)
			result.half_size.x = src.half_size.y * aspect;
		else
			result.half_size.y = src.half_size.x / aspect;

		return result;
	}
	
	// shuvvf
	
	// compute the final size of the particle
	//
	// XXX: explanation of 'particle_size' member usage
	//      -if .x AND .y are 0     => use the particle size in the atlas
	//      -if .x AND .y are not 0 => override particle size in the atlas
	//      -if .x OR  .y is  0     => use the particle effective ratio to compute the 0 member value
	
	template<typename BOX_TYPE>
	BOX_TYPE MakeBoxMatchAspect(BOX_TYPE const & src, typename BOX_TYPE const & aspect_box, bool prefere_update_with)
	{
		// the reference box that gives the aspect may not be empty
		assert(!IsGeometryEmpty(aspect_box));
		
		if (IsGeometryEmpty(src))
			return aspect_box;
			
			
			
		
		
		
		
		
	}
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	

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
	type_box<T, dimension> operator & (type_box<T, dimension> const & b1, type_box<T, dimension> const & b2)
	{
		using vec_type = type_box<T, dimension>::vec_type;

		if (IsGeometryEmpty(b1) || IsGeometryEmpty(b2)) // any of the 2 is empty, intersection is empty
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
	type_box<T, dimension> operator | (type_box<T, dimension> const & b1, type_box<T, dimension> const & b2)
	{
		using vec_type = type_box<T, dimension>::vec_type;

		if (IsGeometryEmpty(b1)) // if one is empty, returns other
			return b2;
		if (IsGeometryEmpty(b2))
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

	/** get the corners of the box */
	template<typename T, int dimension>
	auto GetBoxCorners(type_box<T, dimension> const & b) // returns a std::pair<vec_type, vec_type>
	{
		if (!IsGeometryEmpty(b))
			return std::make_pair(b.position - b.half_size, b.position + b.half_size);
		return std::make_pair(b.position, b.position);
	}

	/** increase the box size with a single vertex */
	template<typename T, int dimension>
	void ExtendBox(type_box<T, dimension> & b, typename type_box<T, dimension>::vec_type const & v)
	{
		using vec_type = typename type_box<T, dimension>::vec_type;

		if (IsGeometryEmpty(b))
		{
			b.position  = v;
			b.half_size = vec_type((T)0.0f);
		}
		else
		{
			std::pair<vec_type, vec_type> corners = GetBoxCorners(b);
			corners.first = glm::min(corners.first, v);
			corners.second = glm::max(corners.second, v);
			b = type_box<T, dimension>(corners);
		}
	}

	template<typename T>
	auto GetBoxVertices(box_base<T, 2> const & b, typename box_base<T, 2>::vec_type * result, bool global = true) // expect an array of 4 elements
	{
		assert(result != nullptr);

		using vec_type = typename box_base<T, 2>::vec_type;

		T NX = - b.half_size.x;
		T PX = + b.half_size.x;

		T NY = - b.half_size.y;
		T PY = + b.half_size.y;

		result[0] = vec_type(NX, NY);
		result[1] = vec_type(PX, NY);
		result[2] = vec_type(NX, PY);
		result[3] = vec_type(PX, PY);

		if (global)
			for (int i = 0 ; i < 4 ; ++i)
				result[i] += b.position;

		return result;
	}

	template<typename T>
	auto GetBoxVertices(box_base<T, 3> const & b, typename box_base<T, 3>::vec_type * result, bool global = true) // expect an array of 8 elements
	{
		assert(result != nullptr);

		using vec_type = typename box_base<T, 3>::vec_type;

		T NX = - b.half_size.x;
		T PX = + b.half_size.x;

		T NY = - b.half_size.y;
		T PY = + b.half_size.y;

		T NZ = - b.half_size.z;
		T PZ = + b.half_size.z;

		result[0] = vec_type(NX, NY, NZ);
		result[1] = vec_type(PX, NY, NZ);
		result[2] = vec_type(NX, PY, NZ);
		result[3] = vec_type(PX, PY, NZ);
		result[4] = vec_type(NX, NY, PZ);
		result[5] = vec_type(PX, NY, PZ);
		result[6] = vec_type(NX, PY, PZ);
		result[7] = vec_type(PX, PY, PZ);

		if (global)
			for (int i = 0 ; i < 8 ; ++i)
				result[i] += b.position;

		return result;
	}

	/** encode a box2 into a vector4 */
	glm::vec4 EncodeBoxToVector(box2 const & src);

	// ==============================================================================================
	// obox functions
	// ==============================================================================================

	/** equality function for obox */
	template<typename T, int dimension>
	bool operator == (type_obox<T, dimension> const & b1, type_obox<T, dimension> const & b2)
	{
		return (b1.position == b1.position) && (b1.half_size == b2.half_size) && (b1.rotator == b2.rotator);
	}

	/** difference function for obox */
	template<typename T, int dimension>
	bool operator != (type_obox<T, dimension> const & b1, type_obox<T, dimension> const & b2)
	{
		return !(b1 == b2);
	}

	template<typename T>
	auto GetBoxVertices(type_obox<T, 2> const & b, typename box_base<T, 2>::vec_type * result, bool global = true) // expect an array of 4 elements
	{
		GetBoxVertices((box_base<T, 2> const &)b, result, false); // do not integrate translation because this would produce a wrong rotation/translation combinaison

		if (global)
		{
			auto transform = GetRotatorMatrix(b.rotator);
			for (int i = 0; i < 4; ++i)
			{
				result[i] = GLMTools::Mult(transform, result[i]);		
				result[i] += b.position;
			}
		}
		return result;
	}

	template<typename T>
	auto GetBoxVertices(type_obox<T, 3> const & b, typename box_base<T, 3>::vec_type * result, bool global = true) // expect an array of 8 elements
	{
		GetBoxVertices((box_base<T, 3> const &)b, result, false); // do not integrate translation because this would produce a wrong rotation/translation combinaison

		if (global)
		{
			auto transform = GetRotatorMatrix(b.rotator);
			for (int i = 0; i < 8; ++i)
			{
				result[i] = GLMTools::Mult(transform, result[i]);		
				result[i] += b.position;
			}
		}
	}
	
	template<typename T>
	type_box2<T> GetBoundingBox(type_obox2<T> const & b)
	{
		type_box2<T> result;

		// copy position
		result.position = b.position;
		// get all vertices and compute max values
		type_box2<T>::vec_type vertices[4];
		GetBoxVertices(b, vertices, false);

		type_box2<T>::vec_type max_vec = glm::tvec2<T>(-std::numeric_limits<T>::max(), -std::numeric_limits<T>::max());

		auto transform = GetRotatorMatrix(b.rotator);
		for (int i = 0; i < 4; ++i)
			max_vec = glm::max(max_vec, GLMTools::Mult(transform, vertices[i]));
		result.half_size = max_vec;

		return result;
	}

	template<typename T>
	type_box3<T> GetBoundingBox(type_obox3<T> const & b)
	{
		type_box3<T> result;

		// copy position
		result.position = b.position;

		// get all vertices and compute max values
		type_box3<T>::vec_type vertices[8];
		GetBoxVertices(b, vertices, false);

		type_box3<T>::vec_type max_vec = glm::tvec3<T>(-std::numeric_limits<T>::max(), -std::numeric_limits<T>::max(), -std::numeric_limits<T>::max());

		auto transform = GetRotatorMatrix(b.rotator);
		for (int i = 0; i < 8; ++i)
			max_vec = glm::max(max_vec, GLMTools::Mult(transform, vertices[i]));
		result.half_size = max_vec;

		return result;
	}

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

	/** returns true whether the triangle is empty */
	template<typename T, int dimension>
	bool IsGeometryEmpty(type_triangle<T, dimension> const & t)
	{
		if (t.a == t.b || t.a == t.c || t.b == t.c)
			return true;
		return false;
	}

	/** get the reversed triangle */
	template<typename T, int dimension>
	type_triangle<T, dimension> GetInvertedTriangle(type_triangle<T, dimension> const & t)
	{
		return type_triangle<T, dimension>(t.a, t.c, t.b);
	}

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
	// sphere/circle functions
	// ==============================================================================================

	/** returns true whether the circle is empty */
	template<typename T, int dimension>
	bool IsGeometryEmpty(type_sphere<T, dimension> const & c)
	{
		return (c.radius < 0);
	}

	/** set the circle has an empty circle */
	template<typename T, int dimension>
	void SetGeometryEmpty(type_sphere<T, dimension> & c)
	{
		c.radius = (T)-1.0f;
	}

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

		return IsGeometryEmpty(c) ? type_box2<T>() : type_box2<T>(c.position, vec_type(c.radius, c.radius));
	}

	/** returns the bounding box of the circle (square) */
	template<typename T>
	type_box2<T> GetInnerBox(type_sphere2<T> const & c)
	{
		using vec_type = typename type_sphere2<T>::vec_type;

		static double const INV_SQRT2 = 0.707106781186547; /* 1.0 / sqrtf(2.0) */

		return IsGeometryEmpty(c) ? type_box2<T>() : type_box2<T>(c.position, vec_type(c.radius * static_cast<T>(INV_SQRT2)));
	}

	template<typename T>
	type_box3<T> GetBoundingBox(type_sphere3<T> const & s)
	{
		using vec_type = typename type_sphere3<T>::vec_type;

		return IsGeometryEmpty(s) ? type_box3<T>() : type_box3<T>(s.position, vec_type(s.radius));
	}

	template<typename T>
	type_box3<T> GetInnerBox(type_sphere3<T> const & s)
	{
		using vec_type = typename type_sphere3<T>::vec_type;

		static double const INV_SQRT3 = 0.577350269189625; /* 1.0 / sqrtf(3.0) */

		return IsGeometryEmpty(s) ? type_box3<T>() : type_box3<T>(s.position, vec_type(s.radius * static_cast<T>(INV_SQRT3)));
	}

	/** returns intersection of 2 spheres */
	template<typename T, int dimension>
	type_sphere<T, dimension> operator & (type_sphere<T, dimension> const & s1, type_sphere<T, dimension> const & s2) // intersection
	{
		using vec_type = typename type_sphere<T, dimension>::vec_type;

		if (IsGeometryEmpty(s1) || IsGeometryEmpty(s2))
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
	type_sphere<T, dimension> operator | (type_sphere<T, dimension> const & s1, type_sphere<T, dimension> const & s2) // union
	{
		using vec_type = typename type_sphere<T, dimension>::vec_type;

		if (IsGeometryEmpty(s1))
			return s2;
		if (IsGeometryEmpty(s2))
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

	// ==============================================================================================
	// rotator functions
	// ==============================================================================================

	/** rotator to matrix */
	glm::mat4x4 GetRotatorMatrix(glm::quat const & rotator);
	/** rotator to matrix */
	glm::dmat4x4 GetRotatorMatrix(glm::dquat const & rotator);
	/** rotator to matrix */
	glm::mat4x4 GetRotatorMatrix(float rotator);
	/** rotator to matrix */
	glm::dmat4x4 GetRotatorMatrix(double rotator);

	// ==============================================================================================
	// plane functions
	// ==============================================================================================

	glm::vec2  GetPlaneNormal(glm::vec3  const & p);
	glm::vec3  GetPlaneNormal(glm::vec4  const & p);
	glm::dvec2 GetPlaneNormal(glm::dvec3 const & p);
	glm::dvec3 GetPlaneNormal(glm::dvec4 const & p);

	float  GetPlaneOffset(glm::vec3  const & p);
	float  GetPlaneOffset(glm::vec4  const & p);
	double GetPlaneOffset(glm::dvec3 const & p);
	double GetPlaneOffset(glm::dvec4 const & p);


	// ==============================================================================================
	// JSON functions
	// ==============================================================================================

		// vectors

	template<typename T, glm::precision P>
	bool SaveIntoJSON(nlohmann::json & json_entry, glm::tvec2<T, P> const & src)
	{
		json_entry = nlohmann::json::array();
		JSONTools::SetAttributeByIndex(json_entry, 0, src.x);
		JSONTools::SetAttributeByIndex(json_entry, 1, src.y);
		return true;
	}

	template<typename T, glm::precision P>
	bool LoadFromJSON(nlohmann::json const & json_entry, glm::tvec2<T, P> & dst)
	{
		if (json_entry.is_object())
		{
			JSONTools::GetAttribute(json_entry, "x", dst.x);
			JSONTools::GetAttribute(json_entry, "y", dst.y);
			return true;
		}
		else if (json_entry.is_array())
		{
			size_t count = std::min(json_entry.size(), (size_t)dst.length());
			for (size_t i = 0; i < count; ++i)
				dst[i] = json_entry[i].get<T>();
			return true;
		}
		return false;
	}

	template<typename T, glm::precision P>
	bool SaveIntoJSON(nlohmann::json & json_entry, glm::tvec3<T, P> const & src)
	{
		json_entry = nlohmann::json::array();
		JSONTools::SetAttributeByIndex(json_entry, 0, src.x);
		JSONTools::SetAttributeByIndex(json_entry, 1, src.y);
		JSONTools::SetAttributeByIndex(json_entry, 2, src.z);
		return true;
	}

	template<typename T, glm::precision P>
	bool LoadFromJSON(nlohmann::json const & json_entry, glm::tvec3<T, P> & dst)
	{
		if (json_entry.is_object())
		{
			JSONTools::GetAttribute(json_entry, "x", dst.x);
			JSONTools::GetAttribute(json_entry, "y", dst.y);
			JSONTools::GetAttribute(json_entry, "z", dst.z);
			return true;
		}
		else if (json_entry.is_array())
		{
			size_t count = std::min(json_entry.size(), (size_t)dst.length());
			for (size_t i = 0; i < count; ++i)
				dst[i] = json_entry[i].get<T>();
			return true;
		}
		return false;
	}

	template<typename T, glm::precision P>
	bool SaveIntoJSON(nlohmann::json & json_entry, glm::tvec4<T, P> const & src)
	{
		json_entry = nlohmann::json::array();
		JSONTools::SetAttributeByIndex(json_entry, 0, src.x);
		JSONTools::SetAttributeByIndex(json_entry, 1, src.y);
		JSONTools::SetAttributeByIndex(json_entry, 2, src.z);
		JSONTools::SetAttributeByIndex(json_entry, 3, src.w);
		return true;
	}

	template<typename T, glm::precision P>
	bool LoadFromJSON(nlohmann::json const & json_entry, glm::tvec4<T, P> & dst)
	{
		if (json_entry.is_object())
		{
			JSONTools::GetAttribute(json_entry, "x", dst.x);
			JSONTools::GetAttribute(json_entry, "y", dst.y);
			JSONTools::GetAttribute(json_entry, "z", dst.z);
			JSONTools::GetAttribute(json_entry, "w", dst.w);
			return true;
		}
		else if (json_entry.is_array())
		{
			size_t count = std::min(json_entry.size(), (size_t)dst.length());
			for (size_t i = 0; i < count ; ++i)
				dst[i] = json_entry[i].get<T>();
			return true;
		}
		return false;
	}
#if 0




	template<typename T, int dimension>
	bool SaveIntoJSON(nlohmann::json & json_entry, type_ray<T, dimension> const & src)
	{

	}

	template<typename T, int dimension>
	bool LoadFromJSON(nlohmann::json const & json_entry, type_ray<T, dimension> & dst)
	{
	}
	   	 

#endif

}; // namespace chaos

