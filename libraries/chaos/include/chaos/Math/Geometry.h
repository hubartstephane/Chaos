 namespace chaos
{
#ifdef CHAOS_FORWARD_DECLARATION

	CHAOS_GEOMETRY_TEMPLATE(DIMENSION, T) class geometry;
	CHAOS_GEOMETRY_DEFINE_GLMLIKE_TEMPLATES(geometry);

#elif !defined CHAOS_TEMPLATE_IMPLEMENTATION

	CHAOS_GEOMETRY_TEMPLATE(DIMENSION, T)
	class geometry
	{
	public:
		/** the dimension of the space */
		static constexpr int dimension = DIMENSION;

		/** the type of the components */
		using type = T;
		/** the type for the geometry itself */
		using geometry_type = geometry<dimension, type>;
		/** the type for matrix */
		using mat_type = glm::tmat4x4<T>;
		/** the type of vector */
		using vec_type = glm::vec<dimension, type>;
		/** the type of plane */
		using plane_type = glm::vec<dimension + 1, type>;
		/** the type of rotator */
		using rot_type =  rotator<dimension, type>;
		/** the type of ray */
		using ray_type = ray<dimension, type>;
		/** the type of sphere */
		using sphere_type = sphere<dimension, type>;
		/** the type of box */
		using box_type = box<dimension, type>;
		/** the type of obox */
		using obox_type = obox<dimension, type>;
		/** the type of triangle */
		using triangle_type = triangle<dimension, type>;
		/** the type of aabox */
		using aabox_type = aabox<dimension, type>;
		/** the type of box plane */
		using box_plane_type = box_plane<dimension, type>;
	};

#endif

}; // namespace chaos