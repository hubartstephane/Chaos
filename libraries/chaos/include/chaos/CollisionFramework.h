#pragma once

#include <chaos/StandardHeaders.h>
#include <chaos/GLMTools.h>
#include <chaos/MathTools.h>
#include <chaos/GeometryFramework.h>

namespace chaos
{

	// ==============================================================================================
	// velocity functions
	// ==============================================================================================

	/** update the velocity by comparing position before and after collision */
	template<typename T>
	void UpdateVelocityFromCollision(T const & old_position, T const & new_position, T & velocity)
	{
		int dimension = velocity.length();

		for (int i = 0; i < dimension; ++i)
		{
			if (old_position[i] > new_position[i])
				velocity[i] = -abs(velocity[i]);
			else if (old_position[i] < new_position[i])
				velocity[i] = abs(velocity[i]);
		}
	}

	// ==============================================================================================
	// Restriction functions
	// ==============================================================================================

	template<typename T, int dimension>
	auto GetClosestPoint(type_box<T, dimension> const & b, typename type_box<T, dimension>::vec_type const & src)
	{
		// clamp X, Y, Z for all planes
		return glm::min(glm::max(src, b.position - b.half_size), b.position + b.half_size);
	}

	template<typename T, int dimension>
	auto GetClosestPoint(type_sphere<T, dimension> const & b, typename type_sphere<T, dimension>::vec_type const & src)
	{
		typename type_sphere<T, dimension>::vec_type result;











		return result;
	}

	template<typename T, int dimension>
	auto GetClosestPoint(type_triangle<T, dimension> const & b, typename type_sphere<T, dimension>::vec_type const & src)
	{
		typename type_triangle<T, dimension>::vec_type result;








		return result;
	}

	// ==============================================================================================
	// Restriction functions
	// ==============================================================================================

	template<typename T>
	bool MoveValue(T & bigger, T & smaller, bool move_big, T delta)
	{
		if (move_big)
			bigger += delta;
		else
			smaller -= delta;
		return true;
	}

	template<typename T, int dimension>
	bool RestrictToInside(type_box<T, dimension> & bigger, type_box<T, dimension> & smaller, bool move_big)
	{
		bool result = false;

		// if one is empty, nothing to do
		if (IsGeometryEmpty(bigger) || IsGeometryEmpty(smaller))
			return false;

		auto big_corners = GetBoxExtremums(bigger);
		auto small_corners = GetBoxExtremums(smaller);

		int const count = dimension;
		for (int i = 0; i < count; ++i)
		{
			// degenerated case : smaller ISNOT the smallest
			//  => just make 'smaller' fullfill 'bigger' surface
			//
			//               'smaller'
			//   +----------------------------------+ smaller fullfill fully the bigger surface
			//
			//         +---------------------+
			//                'bigger'

			if (smaller.half_size[i] >= bigger.half_size[i])
			{
				T delta1 = small_corners.first[i] - big_corners.first[i];

				if (delta1 > 0)
					result |= MoveValue(bigger.position[i], smaller.position[i], move_big, delta1);
				else
				{
					T delta2 = small_corners.second[i] - big_corners.second[i];
					if (delta2 < 0)
						result |= MoveValue(bigger.position[i], smaller.position[i], move_big, delta2);
				}
			}
			// normal case : smaller IS the smallest
			//
			//               'smaller'
			//         +---------------------+
			//
			//   +----------------------------------+ smaller fullfill inside the bigger surface
			//                'bigger'
			else
			{
				T delta1 = small_corners.first[i] - big_corners.first[i];
				if (delta1 < 0)
					result |= MoveValue(bigger.position[i], smaller.position[i], move_big, delta1);
				else
				{
					T delta2 = small_corners.second[i] - big_corners.second[i];
					if (delta2 > 0)
						result |= MoveValue(bigger.position[i], smaller.position[i], move_big, delta2);
				}
			}
		}
		return result;
	}

	template<typename T, int dimension>
	bool RestrictToOutside(type_box<T, dimension> & src, type_box<T, dimension> & target)
	{
		if (IsGeometryEmpty(src) || IsGeometryEmpty(target))
			return false;

		auto src_corners = GetBoxExtremums(src);
		auto target_corners = GetBoxExtremums(target);

		if (glm::any(glm::lessThan(src_corners.second, target_corners.first)))
			return false;
		if (glm::any(glm::greaterThan(src_corners.first, target_corners.second)))
			return false;

		// compute the minimum distance, and best direction (+X, -X, +Y ...) to move the box
		T   best_distance = (T)-1;
		int best_direction = -1;
		for (int i = 0; i < dimension; ++i)
		{
			// in positive direction (dist_pos is to be positive)
			T dist_pos = src_corners.second[i] - target_corners.first[i];
			if (dist_pos <= 0)
				return false; // no collision, nothing to do
			T dist_neg = target_corners.second[i] - src_corners.first[i];
			if (dist_neg <= 0)
				return false; // no collision, nothing to do

			if (best_distance < 0 || dist_pos < best_distance)
			{
				best_distance = dist_pos;
				best_direction = 2 * i;
			}
			// in negative direction (dist_neg is to be positive)
			if (best_distance < 0 || dist_neg < best_distance)
			{
				best_distance = dist_neg;
				best_direction = 2 * i + 1;
			}
		}

		// do the displacement
		if (best_direction >= 0)
		{
			if ((best_direction & 1) == 0)
				target.position[best_direction / 2] += best_distance; // positive direction
			else
				target.position[best_direction / 2] -= best_distance; // negative direction  
			return true;
		}
		return false;
	}

	template<typename T, int dimension>
	bool RestrictToInside(type_sphere<T, dimension> & bigger, type_sphere<T, dimension> & smaller, bool move_big)
	{
		// if one is empty, nothing to do
		if (IsGeometryEmpty(bigger) || IsGeometryEmpty(smaller))
			return false;

		// ensure smaller and bigger are coherent
		if (smaller.radius > bigger.radius)
			return false;

		auto delta_pos = smaller.position - bigger.position;
		T    l = glm::length(delta_pos);

		if (l + smaller.radius <= bigger.radius) // smaller fully inside bigger
			return false;

		T factor = ((bigger.radius - smaller.radius) / l);

		delta_pos = delta_pos * factor;

		if (move_big)
			bigger.position = smaller.position - delta_pos;
		else
			smaller.position = bigger.position + delta_pos;
		return true;
	}

	template<typename T, int dimension>
	bool RestrictToOutside(type_sphere<T, dimension> & src, type_sphere<T, dimension> & target)
	{
		if (IsGeometryEmpty(src) || IsGeometryEmpty(target))
			return false;

		auto delta_pos = target.position - src.position;
		T    l = glm::length(delta_pos);

		if (l - target.radius >= src.radius)
			return false;

		if (l != 0)
		{
			T factor = ((src.radius + target.radius) / l);
			delta_pos = delta_pos * factor;
		}
		else
		{
			auto new_direction = type_sphere<T, dimension>::vec_type(0);
			new_direction[0] = (T)1;

			T factor = (src.radius + target.radius);
			delta_pos = new_direction * factor;
		}

		target.position = src.position + delta_pos;

		return true;
	}

	// ==============================================================================================
	// Symetric Collisions
	// ==============================================================================================

	template<typename T, int dimension>
	bool Collide(type_box<T, dimension> const & src1, type_box<T, dimension> const & src2)
	{
		if (IsGeometryEmpty(src1) || IsGeometryEmpty(src2))
			return false;

		auto src1_corners = GetBoxExtremums(src1);
		auto src2_corners = GetBoxExtremums(src2);

		if (glm::any(glm::lessThan(src1_corners.second, src2_corners.first)))
			return false;
		if (glm::any(glm::greaterThan(src1_corners.first, src2_corners.second)))
			return false;

		return true;
	}

	template<typename T, int dimension>
	bool Collide(type_sphere<T, dimension> const & src1, type_sphere<T, dimension> const & src2)
	{
		if (IsGeometryEmpty(src1) || IsGeometryEmpty(src2))
			return false;

		return glm::distance2(src1.position, src2.position) <= MathTools::Square(src1.radius + src2.radius);
	}


#if 0

	// SEARCH SEPARATING PLANE
	//
	//         4 = (1 << 2)
	//               8 = (1 << 3)
	//
	//    5       1      9         
	//         |     |
	//  -------+-----+--------     1 = (1 << 0)
	//         |     |
	//    4    |  0  |   8
	//         |     |
	//  -------+-----+--------     2 = (1 << 1)
	//         |     |  
	//    6       2      10
	//
	//



#endif


	template<typename T, int dimension>
	bool HasSeparatingPlane(type_box<T, dimension> const & b, typename type_box<T, dimension>::vec_type const * vertices, int count, bool & has_collision)
	{
		has_collision = false;

		if (count == 0)
			return false;

		// the edge we are interresting in
		int edge_of_interrests = 1 | 2 | 4 | 8;

		// for each edge, we describe thhe tests to do 
		int   edge_component[]  = { 0, 0, 1 , 1 };
		float edge_multiplier[] = { -1.0f, 1.0f, -1.0f , 1.0f };
		float edge_value[]      = { b.position.x - b.position.x, b.position.x + b.position.x, b.position.y - b.position.y, b.position.y + b.position.y };

		// iterate over all vertices and eliminate some edges
		for (int i = 0; i < count ; ++i)
		{
			// no possible edge separator ?
			if (edge_of_interrests == 0) 
				return false;
			// iterate over all remaining edges
			int edges = edge_of_interrests;
			while (edges != 0)
			{
				// get the edge of interrest and remove it from current iteration
				int edge_index = chaos::MathTools::bsf(edges);
				edges &= ~(1 << edge_index);

				int   c = edge_component[edge_index];
				float m = edge_multiplier[edge_index];
				float v = edge_value[edge_index];

				// is edge still candidate => remove opposite edge
				if (m * vertices[i][c] > m * v) 
				{
					edge_of_interrests &= ~(1 << (edge_index ^ 1));
					edges &= ~(1 << (edge_index ^ 1));
				}
				// edge is not a good candidate : remove it
				else 
				{
					edge_of_interrests &= ~(1 << edge_index);
					edges &= ~(1 << edge_index);
				}
			}
		
			return false;
		}





	template<typename T, int dimension>
	bool Collide(type_obox<T, dimension> const & src1, type_obox<T, dimension> const & src2)
	{
		assert(0);







		return false; // TODO COLLISION
	}


	template<typename T, int dimension>
	bool Collide(type_triangle<T, dimension> const & src1, type_triangle<T, dimension> const & src2)
	{
		assert(0);






		return false; // TODO COLLISION
	}

	// ==============================================================================================
	// Collisions sphere/box
	// ==============================================================================================

#if 0

	template<typename T>
	bool Collide(type_box<T, 2> const & b, type_sphere<T, 2> const & s)
	{
		using box_type = type_box<T, 2>;
		using vec_type = box_type::vec2_type;

		// 1 : test whether any entry is null (sphere is faster call, first)
		if (IsGeometryEmpty(s))
			return false;
		if (IsGeometryEmpty(b))
			return false;

		// 2 : test whether the center of the sphere is inside the box
		if (Collide(s.position, b))
			return true;

		// 3 : test distance from the sphere center to the 4 corners of the box
		auto r2 = s.radius * s.radius;

		auto corners = GetBoxExtremums(b);
		auto const & A = corners.first;
		auto const & C = corners.second;

		vec_type V[4] =
		{
			A,
			vec_type(A.x, C.y),
			C,
			vec_type(C.x, A.y)
		};

		for (int i = 0; i < 4; ++i)
		{
			T dist = glm::distance2(V[i], s.position);
			if (dist <= r2)
				return true;
		}

		// 4 : test whether sphere center is too far from any edge
		if (s.position.x > b.position.x + b.half_size.x + s.radius)
			return false;
		if (s.position.x < b.position.x - b.half_size.x - s.radius)
			return false;
		if (s.position.y > b.position.y + b.half_size.y + s.radius)
			return false;
		if (s.position.y < b.position.y - b.half_size.y - s.radius)
			return false;
		return true;
	}

#endif


	template<typename T, int dimension>
	bool Collide(type_box<T, dimension> const & b, type_sphere<T, dimension> const & s)
	{
		if (IsGeometryEmpty(s) || IsGeometryEmpty(b))
			return false;

		auto pt = GetClosestPoint(b, s.position);
		return glm::distance2(pt, s.position) <= (s.radius * s.radius);
	}

	template<typename T, int dimension>
	bool Collide(type_sphere<T, dimension> const & s, type_box<T, dimension> const & b)
	{
		return Collide(b, s);
	}

	// ==============================================================================================
	// Collisions sphere/triangle
	// ==============================================================================================

	// TODO COLLISION       TRIANGLE 3 / SPHERE 3 ???



	template<typename T>
	bool Collide(type_triangle<T, 2> const & t, type_sphere<T, 2> const & s)
	{
		if (IsGeometryEmpty(s) || IsGeometryEmpty(t))
			return false;

		auto pt = GetClosestPoint(t, s.position);
		return glm::distance2(pt, s.position) <= (s.radius * s.radius);





#if 0



		auto r2 = s.radius * s.radius;

		// 2 : test whether any of the 3 vertices is inside the sphere
		for (int i = 0; i < 3; ++i)
			if (glm::length2(t[i] - s.position) <= r2) // do not use sphere::Contains(...) to avoid multiple computation of R * R
				return true;

		// 3 : test whether the sphere center is too far from one side of any edge
		//     the distance si given by
		//     length(cross(normalized_edge, e))
		//
		//     we can use a simplified version of cross product while in 2D
		//     => the cross will give a vector so that (X = 0, Y = 0)
		for (int i = 0; i < 3; ++i)
		{
			auto const & e1 = t[i];
			auto const & e2 = t[(i + 1) % 3];

			auto e1_S = s.position - e1;
			auto normalized_edge = glm::normalize(e2 - e1);

			auto d = GLMTools::Get2DCrossProductZ(normalized_edge, e1_S); // cross product, in plane, the only valid coordinate is Z = (x.y') - (x'y)
			if (d > s.radius)
				return false;
		}
#endif
		return true;
	}

	template<typename T, int dimension>
	bool Collide(type_sphere<T, dimension> const & s, type_triangle<T, dimension> const & t)
	{
		return Collide(t, s);
	}

	// ==============================================================================================
	// Collisions box/triangle
	// ==============================================================================================

	template<typename T, int dimension>
	bool Collide(type_box<T, dimension> const & b, type_triangle<T, dimension> const & t)
	{
		assert(0);







		return false; // TODO COLLISION
	}

	template<typename T, int dimension>
	bool Collide(type_triangle<T, dimension> const & t, type_box<T, dimension> const & b)
	{
		return Collide(b, t);
	}

	// ==============================================================================================
	// Collisions obox/triangle
	// ==============================================================================================

	template<typename T, int dimension>
	bool Collide(type_obox<T, dimension> const & b, type_triangle<T, dimension> const & t)
	{
		assert(0);





		return false;
	}

	template<typename T, int dimension>
	bool Collide(type_triangle<T, dimension> const & t, type_obox<T, dimension> const & b)
	{
		return Collide(b, t);
	}

	// ==============================================================================================
	// Collisions obox/sphere  
	// ==============================================================================================

	template<typename T, int dimension>
	bool Collide(type_obox<T, dimension> const & b, type_sphere<T, dimension> const & s)
	{
		assert(0);





		return false; // TODO COLLISION
	}

	template<typename T, int dimension>
	bool Collide(type_sphere<T, dimension> const & s, type_obox<T, dimension> const & b)
	{
		return Collide(b, s);
	}

	// ==============================================================================================
	// Collision point/box 
	// ==============================================================================================

	/** returns true whether the point is contained in the box */
	template<typename T, int dimension>
	bool Collide(typename type_box<T, dimension>::vec_type const & pt, type_box<T, dimension> const & b)
	{
		return glm::all(glm::lessThanEqual(glm::abs(pt - b.position), b.half_size));
	}

	template<typename T, int dimension>
	bool Collide(type_box<T, dimension> const & b, typename type_box<T, dimension>::vec_type const & pt)
	{
		return Collide(pt, b);
	}

	// ==============================================================================================
	// Collision point/obox 
	// ==============================================================================================

	template<typename T, int dimension>
	bool Collide(typename type_obox<T, dimension>::vec_type const & pt, type_obox<T, dimension> const & b)
	{
		// set point from global to local system
		auto transform = GetRotatorMatrix(-b.rotator);
		auto transformed_ptr = GLMTools::Mult(transform, pt - b.position);
		// now we can considere we are in a standard BOX
		return glm::all(glm::lessThanEqual(glm::abs(transformed_ptr), b.half_size));
	}

	template<typename T, int dimension>
	bool Collide(type_obox<T, dimension> const & b, typename type_obox<T, dimension>::vec_type const & pt)
	{
		return Collide(pt, b);
	}

	// ==============================================================================================
	// Collision point/triangles
	// ==============================================================================================

	/** returns true whether the point is contained in the triangle */
	template<typename T, int dimension>
	bool Collide(typename type_triangle<T, dimension>::vec_type const & pt, type_triangle<T, dimension> const & t)
	{
		using vec_type = typename type_triangle<T, dimension>::vec_type;

		// test whether the triangle is null
		if (IsGeometryEmpty(t))
			return false;

		// test whether the point is inside the edges
		for (int i = 0; i < 3; ++i)
		{
			vec_type const & e1 = t[i];
			vec_type const & e2 = t[(i + 1) % 3];

			vec_type e1_S = pt - e1;
			vec_type normalized_edge = glm::normalize(e2 - e1);

			auto d = GLMTools::Get2DCrossProductZ(normalized_edge, e1_S); // cross product, in plane, the only valid coordinate is Z = (x.y') - (x'y)
			if (d > 0.0f)
				return false;
		}
		return true;
	}

	template<typename T, int dimension>
	bool Collide(type_triangle<T, dimension> const & t, typename type_triangle<T, dimension>::vec_type const & pt)
	{
		return Collide(pt, t);
	}

	// ==============================================================================================
	// Collision point/sphere
	// ==============================================================================================

	/** returns true whether the point is contained in the sphere */
	template<typename T, int dimension>
	bool Collide(typename type_sphere<T, dimension>::vec_type const & pt, type_sphere<T, dimension> const & s)
	{
		return glm::distance2(pt, s.position) <= (s.radius * s.radius);
	}

	template<typename T, int dimension>
	bool Collide(type_sphere<T, dimension> const & s, typename type_sphere<T, dimension>::vec_type const & pt)
	{
		return Collide(pt, s);
	}

	// ==============================================================================================
	// Triangle preparation functions
	// ==============================================================================================

	// check the orientation of the triangle for future collision detection
	template<typename T>
	bool IsTriangleReadyForCollision(type_triangle<T, 2> const & t)
	{
		auto e1 = t.b - t.a;
		auto e2 = t.c - t.b;
		if (GLMTools::Get2DCrossProductZ(e2, e1) >= 0)
			return true;
		return false;
	}

	// ensure the orientation of the triangle is correct for further collision detection
	template<typename T>
	type_triangle<T, 2> PrepareTriangleForCollision(type_triangle<T, 2> const & t)
	{
		if (IsTriangleReadyForCollision(t))
			return t;
		return GetInvertedTriangle(t);
	}

}; // namespace chaos

