namespace chaos
{
#ifdef CHAOS_FORWARD_DECLARATION

	CHAOS_GEOMETRY_TEMPLATE(DIMENSION, T)
	using box_corners = Range<glm::vec<DIMENSION, T>>;

	CHAOS_GEOMETRY_DEFINE_GLMLIKE_TEMPLATES(box_corners);

#endif

}; // namespace chaos