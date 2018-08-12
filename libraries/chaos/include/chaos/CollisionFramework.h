#pragma once

#include <chaos/StandardHeaders.h>
#include <chaos/GLMTools.h>
#include <chaos/MathTools.h>
#include <chaos/GeometryFramework.h>

namespace chaos
{

	// ==============================================================================================
	// Restriction function
	// ==============================================================================================

	template<typename T, int dimension>
	bool RestrictToInside(type_box<T, dimension> & bigger, type_box<T, dimension> & smaller, bool move_big)
	{
		bool result = false;

		// if one is empty, nothing to do
		if (bigger.IsEmpty() || smaller.IsEmpty())
			return false;

		// ensure smaller and bigger are coherent
		if (glm::any(glm::greaterThan(smaller.half_size, bigger.half_size)))
			return false;

		auto big_corners = bigger.GetCorners();
		auto small_corners = smaller.GetCorners();

		int const count = dimension;
		for (int i = 0; i < count; ++i)
		{
			T delta1 = small_corners.first[i] - big_corners.first[i];
			if (delta1 < 0)
			{
				if (move_big)
					bigger.position[i] += delta1;
				else
					smaller.position[i] -= delta1;
				result = true;
			}
			else
			{
				T delta2 = small_corners.second[i] - big_corners.second[i];
				if (delta2 > 0)
				{
					if (move_big)
						bigger.position[i] += delta2;
					else
						smaller.position[i] -= delta2;
					result = true;
				}
			}
		}
		return result;
	}

	template<typename T, int dimension>
	bool RestrictToOutside(type_box<T, dimension> & src, type_box<T, dimension> & target)
	{
		if (src.IsEmpty() || target.IsEmpty())
			return false;

		auto src_corners = src.GetCorners();
		auto target_corners = target.GetCorners();

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
		if (bigger.IsEmpty() || smaller.IsEmpty())
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
		if (src.IsEmpty() || target.IsEmpty())
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
	// Collision function
	// ==============================================================================================

	template<typename T, int dimension>
	bool Collide(type_box<T, dimension> const & src1, type_box<T, dimension> const & src2)
	{
		if (src1.IsEmpty() || src2.IsEmpty())
			return false;

		auto src1_corners = src1.GetCorners();
		auto src2_corners = src2.GetCorners();

		if (glm::any(glm::lessThan(src1_corners.second, src2_corners.first)))
			return false;
		if (glm::any(glm::greaterThan(src1_corners.first, src2_corners.second)))
			return false;

		return true;
	}

	template<typename T, int dimension>
	bool Collide(type_sphere<T, dimension> const & src1, type_sphere<T, dimension> const & src2)
	{
		if (src1.IsEmpty() || src2.IsEmpty())
			return false;

		return glm::length2(src1.position - src2.position) <= MathTools::Square(src1.radius + src2.radius);
	}

	template<typename T>
	bool Collide(type_sphere<T, 2> const & s, type_box<T, 2> const & b)
	{
		return Collide(b, s);
	}

	template<typename T>
	bool Collide(type_box<T, 2> const & b, type_sphere<T, 2> const & s)
	{
		using box_type = type_box<T, 2>;
		using vec_type = box_type::vec2_type;

		// 1 : test whether any entry is null (sphere is faster call, first)
		if (s.IsEmpty())
			return false;
		if (b.IsEmpty())
			return false;

		// 2 : test whether the center of the sphere is inside the box
		if (b.Contains(s.position))
			return true;

		// 3 : test distance from the sphere center to the 4 corners of the box
		auto r2 = s.radius * s.radius;

		auto corners = b.GetCorners();
		auto const & A = corners.first;
		auto const & C = corners.second;

		vec_type V[4] = 
		{
			A,
			vec_type(A.x, C.y),
			C,
			vec_type(C.x, A.y)
		};

		for (int i = 0 ; i < 4; ++i)
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

	template<typename T>
	bool Collide(type_sphere<T, 2> const & s, type_triangle<T, 2> const & t)
	{
		return Collide(t, s);
	}

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
		return t.GetInvertedTriangle();
	}

	template<typename T>
	bool Collide(type_triangle<T, 2> const & t, type_sphere<T, 2> const & s)
	{
		// 1 : test whether any entry is null (sphere is faster call, first)
		if (s.IsEmpty())
			return false;
		if (t.IsEmpty())
			return false;

		auto r2 = s.radius * s.radius;

		// 2 : test whether any of the 3 vertices is inside the sphere
		if (glm::length2(t.a - s.position) <= r2)
			return true;
		if (glm::length2(t.b - s.position) <= r2) // do not use sphere::Contains(...) to avoid multiple computation of R * R
			return true;
		if (glm::length2(t.c - s.position) <= r2)
			return true;

		// 3 : test whether the sphere center is too far from one side of any edge
		//     the distance si given by
		//     length(cross(normalized_edge, e))
		//
		//     we can use a simplified version of cross product while in 2D
		//     => the cross will give a vector so that (X = 0, Y = 0)
		auto const * V = &t.a;
		for (int i = 0; i < 3; ++i)
		{
			auto const & e1 = V[i];
			auto const & e2 = V[(i + 1) % 3];

			auto e1_S = s.position - e1;
			auto normalized_edge = glm::normalize(e2 - e1);

			auto d = GLMTools::Get2DCrossProductZ(normalized_edge, e1_S); // cross product, in plane, the only valid coordinate is Z = (x.y') - (x'y)
			if (d > s.radius)
				return false;
		}
		return true;
	}

}; // namespace chaos

