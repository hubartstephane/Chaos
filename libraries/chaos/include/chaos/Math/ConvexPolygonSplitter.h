namespace chaos
{
#if !defined CHAOS_FORWARD_DECLARATION && !defined CHAOS_TEMPLATE_IMPLEMENTATION

	class LineIntersectionResult
	{
	public:

		glm::vec2 intersection = glm::vec2(0.0f, 0.0f);

		float t1 = 0.0f;
		float t2 = 0.0f;

		float t1_infinite_min = 0.0f;
		float t1_infinite_max = 0.0f;
		float t2_infinite_min = 0.0f;
		float t2_infinite_max = 0.0f;

		bool has_solution = false;
		bool has_infinite_solutions = false;

	};

	class PolygonInfo
	{
	public:

		/** the index of the first point in the shared vector of points */
		size_t first = 0;
		/** the number of points in the polygon */
		size_t count = 0;
	};

	class PolygonSet
	{
	public:

		/** the points shared by all the polygons */
		std::vector<glm::vec2> points;
		/** the info defining all the polygons */
		std::vector<PolygonInfo> polygons;
	};


	class ConvexPolygonCollision
	{

	public:

		static bool IsPolygonConvex(std::vector<glm::vec2> const& polygon, bool* reversed_convex = nullptr)
		{
			assert(polygon.size() >= 3);

			bool has_positive = false;
			bool has_negative = false;

			size_t polygon_count = polygon.size();
			for (size_t i = 0; i < polygon_count; ++i)
			{
				glm::vec2 const& A = polygon[i];
				glm::vec2 const& B = polygon[(i + 1) % polygon_count];
				glm::vec2 const& C = polygon[(i + 2) % polygon_count];

				float value = GLMTools::Get2DCrossProductZ(B - A, C - B);

				has_positive |= (value > 0.0f);
				has_negative |= (value < 0.0f);

				if (has_negative)
				{
					if (reversed_convex == nullptr) // early exit : user only wants to know if Polygon is convex and positive
						return false;
					if (has_positive) // early exit : we already have all information we wanted
						break;
				}
			}

			if (reversed_convex != nullptr)
				*reversed_convex = has_negative & !has_positive; // always negative direction (convex but in reverse order)

			return (has_negative ^ has_positive); // always in the same direction
		}

		static bool Collide(std::vector<glm::vec2> const& polygon1, std::vector<glm::vec2> const& polygon2)
		{
			if (HasSeparatingEdge(polygon1, polygon2))
				return false;
			if (HasSeparatingEdge(polygon2, polygon1))
				return false;
			return true;
		}

	protected:

		static bool HasSeparatingEdge(std::vector<glm::vec2> const& points, std::vector<glm::vec2> const& polygon)
		{
			size_t polygon_count = polygon.size();
			size_t points_count = points.size();
			for (size_t i = 0; i < polygon_count; ++i)
			{
				glm::vec2 const& A = polygon[i];
				glm::vec2 const& B = polygon[(i + 1) % polygon_count];

				glm::vec2 AB = B - A;

				size_t j = 0;
				for (; j < points_count; ++j)
				{
					glm::vec2 const& P = points[i];
					glm::vec2 AP = P - A;

					if (GLMTools::Get2DCrossProductZ(AB, AP) < 0.0f)
						break;
				}
				if (j == points_count) // all points are outside relatively to current edge
					return true;
			}
			return false;
		}





	};


	class ConvexPolygonSplitter
	{
	public:

#if 0
		X  X
			Y  Y
			Z  Z
			X  X
			Y  Y

			YZ' - ZY'
			ZX' - XZ'
			XY' - YX'
#endif  


			/** search whether C and D are in the same sie relatively to [A, B] */
			static bool ArePointsSameSide(glm::vec2 const& A, glm::vec2 const& B, glm::vec2 const& C, glm::vec2 const& D)
		{
			glm::vec2 AB = B - A;
			glm::vec2 AC = C - A;
			glm::vec2 AD = D - A;

			float Z1 = GLMTools::Get2DCrossProductZ(AB, AC);
			float Z2 = GLMTools::Get2DCrossProductZ(AB, AD);

			return (Z1 * Z2 > 0.0f);
		}

		/** search whether there is an intersection between [A, B] and [C, D] */
		static LineIntersectionResult GetIntersection(glm::vec2 const& A, glm::vec2 const& B, glm::vec2 const& C, glm::vec2 const& D, bool check_for_segments = true)
		{
			LineIntersectionResult result;

			// no degenerated use case
			assert(A != B);
			assert(C != D);

			// early exit for segments
			if (check_for_segments)
			{
				if (ArePointsSameSide(A, B, C, D))
					return result;
				if (ArePointsSameSide(C, D, A, B))
					return result;
			}

			// special case : lines are colinear
			//   infinity or 0 solutions

			glm::vec2 CA = A - C;
			glm::vec2 AB = B - A;
			glm::vec2 CD = D - C;

			if (GLMTools::Get2DCrossProductZ(AB, CD) == 0.0f) // colinear
			{
				if (GLMTools::Get2DCrossProductZ(AB, CA) == 0.0f) // four points are aligned
				{
					glm::vec2 AC = C - A;
					glm::vec2 AD = D - A;
					glm::vec2 CB = C - B;

					result.has_solution = true;
					result.has_infinite_solutions = true;

					float AB_length = glm::length(AB);
					float CD_length = glm::length(CD);

					float t1_1 = MathTools::GetSign(glm::dot(AC, AB)) * glm::length(AC) / AB_length;
					float t1_2 = MathTools::GetSign(glm::dot(AD, AB)) * glm::length(AD) / AB_length;
					float t2_1 = MathTools::GetSign(glm::dot(CA, CD)) * glm::length(CA) / CD_length;
					float t2_2 = MathTools::GetSign(glm::dot(CB, CD)) * glm::length(CB) / CD_length;

					result.t1_infinite_min = std::min(t1_1, t1_2);
					result.t1_infinite_max = std::max(t1_1, t1_2);

					result.t2_infinite_min = std::min(t2_1, t2_2);
					result.t2_infinite_max = std::max(t2_1, t2_2);

					return result; // infinity of solutions
				}
				else
				{
					result.has_solution = false;
					result.has_infinite_solutions = false;
					return result; // no solution
				}
			}

			// The intersection point is both on AB and CD:
			//   P = A + t1.AB
			//   P = C + t2.CD
			//
			//  (A - C) + t1.AB = t2.CD   => 2 equations (1 for x, 1 for y)
			//
			//  CAx + t1.ABx = t2.CDx
			//  CAy + t1.ABy = t2.CDy

			float t1 = 0.0f;
			float t2 = 0.0f;

			// Simple case : cannot divide CD.x 
			if (CD.x == 0.0f)
			{
				assert(AB.x != 0.0f); // else lines would be colinear
				assert(CD.y != 0.0f); // else this would be a degenerated case

				t1 = -(CA.x / AB.x);
				t2 = +(CA.y + t1 * AB.y) / CD.y;
			}
			// Other simple case : cannot divide by CD.y
			else if (CD.y == 0.0f)
			{
				assert(AB.y != 0.0f); // else lines would be colinear
				assert(CD.x != 0.0f); // else this would be a degenerated case

				t1 = -(CA.y / AB.y);
				t2 = +(CA.x + t1 * AB.x) / CD.x;
			}

			// make substitutions
			//
			//      CAx + t1.ABx
			// t2 = ------------
			//          CDx
			//
			//                CAx + t1.ABx
			// CAy + t1.ABy = ------------ . CDy
			//                     CDx
			//
			// (while we can divide by CDy)
			//
			//   CAy.CDx                ABy.CDx
			// [ ------- - CAx ] + t1.[-------- - ABx] = 0
			//     CDy                    CDy
			//
			//  K1 + t1.K2 = 0
			//
			else
			{
				float K0 = (CD.x / CD.y);
				float K1 = (CA.y * K0) - CA.x;
				float K2 = (AB.y * K0) - AB.x;

				assert(K2 != 0.0f); // the no solution, infinite solutions is given with colinear points that should already have been threated previously

				t1 = -(K1 / K2);
				t2 = +(CA.x + t1 * AB.x) / CD.x;
			}

			result.has_solution = true;
			result.has_infinite_solutions = false;

			result.t1 = t1;
			result.t2 = t2;

			result.t1_infinite_min = t1;
			result.t1_infinite_max = t1;
			result.t2_infinite_min = t2;
			result.t2_infinite_max = t2;

			result.intersection = A + t1 * AB;

			return result;
		}

		bool SplitPolygonNoIntersection(std::vector<glm::vec2> const& polygon, PolygonSet& result)
		{
			// For each edge of the polygon, search whether it intersects with another edge
			size_t point_count = polygon.size();
			for (size_t i = 0; i < point_count; ++i)
			{
				glm::vec2 const& A = polygon[i];
				glm::vec2 const& B = polygon[(i + 1) % point_count];

				for (size_t j = 1; j < point_count; ++j)
				{
					glm::vec2 const& C = polygon[(i + j) % point_count];
					glm::vec2 const& D = polygon[(i + j + 1) % point_count];

					LineIntersectionResult intersection = GetIntersection(A, B, C, D, true);
					if (!intersection.has_solution)
						continue;



				}
			}


			return true;
		}


		bool SearchPointInsidePolygon(std::vector<glm::vec2> const& src, glm::vec2& result)
		{






			return true;
		}

		bool SplitIntoConvexPolygons(std::vector<glm::vec2> const& src)
		{






			return true;
		}


	};

#endif 

}; // namespace chaos