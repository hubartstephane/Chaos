 namespace chaos
{
#ifdef CHAOS_FORWARD_DECLARATION

	CHAOS_GEOMETRY_TEMPLATE(DIMENSION, T) class sphere;
	CHAOS_GEOMETRY_DEFINE_GLMLIKE_TEMPLATES(sphere);

#elif !defined CHAOS_TEMPLATE_IMPLEMENTATION

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

	CHAOS_GEOMETRY_TEMPLATE(DIMENSION, T)
	bool DoSaveIntoJSON(nlohmann::json* json, sphere<DIMENSION, T> const& src)
	{
		if (!PrepareSaveObjectIntoJSON(json))
			return false;
		JSONTools::SetAttribute(json, "position", src.position);
		JSONTools::SetAttribute(json, "radius", src.radius);
		return true;
	}

	CHAOS_GEOMETRY_TEMPLATE(DIMENSION, T)
	bool DoLoadFromJSON(JSONReadConfiguration config, sphere<DIMENSION, T>& dst)
	{
		JSONTools::GetAttribute(config, "position", dst.position);
		JSONTools::GetAttribute(config, "radius", dst.radius);
		return true;
	}

#endif

}; // namespace chaos