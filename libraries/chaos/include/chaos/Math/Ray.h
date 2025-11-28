 namespace chaos
{
#ifdef CHAOS_FORWARD_DECLARATION

	CHAOS_GEOMETRY_TEMPLATE(DIMENSION, T) class ray;
	CHAOS_GEOMETRY_DEFINE_GLMLIKE_TEMPLATES(ray);

#elif !defined CHAOS_TEMPLATE_IMPLEMENTATION

	CHAOS_GEOMETRY_TEMPLATE(DIMENSION, T)
	class ray : public geometry<DIMENSION, T>
	{
	public:

		using geometry_type = geometry<DIMENSION, T>;
		using vec_type = typename geometry_type::vec_type;

		/** default constructor */
		ray() = default;
		/** copy constructor */
		ray(ray const& src) = default;
		/** other constructor */
		ray(vec_type const & in_position, vec_type const & in_direction) : position(in_position), direction(in_direction) {}
		/** construct a ray from 2 points */
		ray(std::pair<vec_type, vec_type> const & pts) : position(pts.first), direction(glm::normalize(pts.second - pts.first)) {}

	public:

		/** the starting position of the ray in space */
		vec_type position;
		/** the direction of the ray in space */
		vec_type direction;
	};

#endif

}; // namespace chaos