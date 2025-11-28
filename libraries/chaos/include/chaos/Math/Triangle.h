 namespace chaos
{
#ifdef CHAOS_FORWARD_DECLARATION

	CHAOS_GEOMETRY_TEMPLATE(DIMENSION, T) class triangle;
	CHAOS_GEOMETRY_DEFINE_GLMLIKE_TEMPLATES(triangle);

#elif !defined CHAOS_TEMPLATE_IMPLEMENTATION

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

	/** equality test function for triangles */
	CHAOS_GEOMETRY_TEMPLATE(DIMENSION, T)
	bool operator == (triangle<DIMENSION, T> const& t1, triangle<DIMENSION, T> const& t2)
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

	/** returns true whether the triangle is empty */
	CHAOS_GEOMETRY_TEMPLATE(DIMENSION, T)
	bool IsGeometryEmpty(triangle<DIMENSION, T> const& t)
	{
		if (t.a == t.b || t.a == t.c || t.b == t.c)
			return true;
		return false;
	}

	/** get the reversed triangle */
	CHAOS_GEOMETRY_TEMPLATE(DIMENSION, T)
	triangle<DIMENSION, T> GetInvertedTriangle(triangle<DIMENSION, T> const& t)
	{
		return triangle<DIMENSION, T>(t.a, t.c, t.b);
	}

#endif

}; // namespace chaos