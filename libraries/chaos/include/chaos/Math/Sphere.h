 namespace chaos
{
#ifdef CHAOS_FORWARD_DECLARATION

	CHAOS_GEOMETRY_TEMPLATE(DIMENSION, T) class sphere;
	CHAOS_GEOMETRY_DEFINE_GLMLIKE_TEMPLATES(sphere);

#elif !defined CHAOS_TEMPLATE_IMPLEMENTATION

	/**
	 * sphere: self describing
	 */
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

	/** equality function for circle */
	CHAOS_GEOMETRY_TEMPLATE(DIMENSION, T)
	bool operator == (sphere<DIMENSION, T> const& c1, sphere<DIMENSION, T> const& c2)
	{
		if (IsGeometryEmpty(c1) != IsGeometryEmpty(c2))
			return false;
		return (c1.position == c2.position) && (c1.radius == c2.radius);
	}

	/** returns true whether the circle is empty */
	CHAOS_GEOMETRY_TEMPLATE(DIMENSION, T)
	bool IsGeometryEmpty(sphere<DIMENSION, T> const& c)
	{
		return (c.radius < 0);
	}

	/** returns the perimeter of the circle */
	template<std::floating_point T>
	T GetPerimeter(sphere<2, T> const& c)
	{
		assert(!IsGeometryEmpty(c));
		return static_cast<T>(2.0 * M_PI) * c.radius;
	}

	/** returns the surface of the circle */
	template<std::floating_point T>
	T GetSurface(sphere<2, T> const& c)
	{
		assert(!IsGeometryEmpty(c));
		return static_cast<T>(M_PI) * c.radius * c.radius;
	}

	/** returns the volume of the sphere */
	template<std::floating_point T>
	T GetVolume(sphere<3, T> const& s)
	{
		assert(!IsGeometryEmpty(s));
		return static_cast<T>((4.0 / 3.0) * M_PI) * s.radius * s.radius * s.radius;
	}

	/** returns the surface of the sphere */
	template<std::floating_point T>
	T GetSurface(sphere<3, T> const& s)
	{
		assert(!IsGeometryEmpty(s));
		return static_cast<T>(4.0 * M_PI) * s.radius * s.radius;
	}

	/** returns intersection of 2 spheres */
	CHAOS_GEOMETRY_TEMPLATE(DIMENSION, T)
	sphere<DIMENSION, T> operator & (sphere<DIMENSION, T> const& s1, sphere<DIMENSION, T> const& s2) // intersection
	{
		using geometry_type = geometry<DIMENSION, T>;
		using vec_type = typename geometry_type::vec_type;

		if (IsGeometryEmpty(s1) || IsGeometryEmpty(s2))
			return sphere<DIMENSION, T>();
		if (s1.position == s2.position)
			return sphere<DIMENSION, T>(s1.position, glm::min(s1.radius, s2.radius));

		vec_type delta_pos = s2.position - s1.position;   // vector that go from center 1 to center 2
		T        distance  = glm::length(delta_pos);      // length of such a vector

		if (distance >= s1.radius + s2.radius) // sphere too far => returns the empty sphere
			return sphere<DIMENSION, T>();

		T t1 = s1.radius / distance;  // positive
		T t2 = s2.radius / distance;  // positive

		T a = glm::max(-t1, static_cast<T>(1) - t2);
		T b = glm::min(t1, static_cast<T>(1) + t2);

		return sphere<DIMENSION, T>(
			s1.position + ((b + a) / static_cast<T>(2)) * delta_pos,
			((b - a) / static_cast<T>(2)) * distance);
	}

	/** returns union of 2 spheres */
	CHAOS_GEOMETRY_TEMPLATE(DIMENSION, T)
	sphere<DIMENSION, T> operator | (sphere<DIMENSION, T> const& s1, sphere<DIMENSION, T> const& s2) // union
	{
		using geometry_type = geometry<DIMENSION, T>;
		using vec_type = typename geometry_type::vec_type;

		if (IsGeometryEmpty(s1))
			return s2;
		if (IsGeometryEmpty(s2))
			return s1;
		if (s1.position == s2.position)
			return sphere<DIMENSION, T>(s1.position, glm::max(s1.radius, s2.radius));

		vec_type delta_pos = s2.position - s1.position;    // vector that go from center 1 to center 2
		T        distance = glm::length(delta_pos);       // length of such a vector

		T t1 = s1.radius / distance;  // positive
		T t2 = s2.radius / distance;  // positive

		T a = glm::min(-t1, static_cast<T>(1) - t2);
		T b = glm::max(t1, static_cast<T>(1) + t2);

		return sphere<DIMENSION, T>(
			s1.position + ((b + a) / static_cast<T>(2)) * delta_pos,
			((b - a) / static_cast<T>(2)) * distance);
	}

	/** save a sphere into JSON */
	CHAOS_GEOMETRY_TEMPLATE(DIMENSION, T)
	bool DoSaveIntoJSON(nlohmann::json* json, sphere<DIMENSION, T> const& src)
	{
		if (!PrepareSaveObjectIntoJSON(json))
			return false;
		JSONTools::SetAttribute(json, "position", src.position);
		JSONTools::SetAttribute(json, "radius", src.radius);
		return true;
	}

	/** load a sphere from JSON */
	CHAOS_GEOMETRY_TEMPLATE(DIMENSION, T)
	bool DoLoadFromJSON(JSONReadConfiguration config, sphere<DIMENSION, T>& dst)
	{
		JSONTools::GetAttribute(config, "position", dst.position);
		JSONTools::GetAttribute(config, "radius", dst.radius);
		return true;
	}

#endif

}; // namespace chaos