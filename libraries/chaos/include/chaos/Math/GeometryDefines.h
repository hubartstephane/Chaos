 namespace chaos
{
#ifdef CHAOS_FORWARD_DECLARATION

	// template 'header' for geometry functions
	#define CHAOS_GEOMETRY_TEMPLATE(DIMENSION, T, ...)\
		template<int DIMENSION, std::floating_point T, __VA_ARGS__> requires (DIMENSION == 2 || DIMENSION == 3)

	 // defines 'using' & templates that mimic glm conventions
	 //   box2
	 //   box3
	 //   dbox2
	 //   dbox3
	 //   tbox2<float>
	 //   tbox3<float>
	 //   ...
	#define CHAOS_GEOMETRY_DEFINE_GLMLIKE_TEMPLATES(TYPENAME)\
		template<std::floating_point T> using t##TYPENAME##2 = TYPENAME<2, T>;\
		template<std::floating_point T> using t##TYPENAME##3 = TYPENAME<3, T>;\
		using TYPENAME##2 = t##TYPENAME##2<float>;\
		using TYPENAME##3 = t##TYPENAME##3<float>;\
		using d##TYPENAME##2 = t##TYPENAME##2<double>;\
		using d##TYPENAME##3 = t##TYPENAME##3<double>;

#endif

}; // namespace chaos