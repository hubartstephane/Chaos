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

#endif

}; // namespace chaos