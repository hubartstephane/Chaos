 namespace chaos
{
#ifdef CHAOS_FORWARD_DECLARATION

	CHAOS_GEOMETRY_TEMPLATE(DIMENSION, T) class getrotatortype;

	class zero_rotator;

#elif !defined CHAOS_TEMPLATE_IMPLEMENTATION

	// ==============================================================================================
	// rotators
	// ==============================================================================================

	// XXX : depending whether we are in 2D or 3D a rotation can be described by a single float or a quaternion
	//       this code is here to provide some common interface
	//
	// XXX : for 3D rotations we use a quaternion. quaternions have a constructor that make them valid (constructor = 0 rotator)
	//       for 2D we simply use float/double. that means that this is NOT INITIALIZED by default !
	//       (I could use a class to embedd a single scalar but the conversion/addition ... operators implementation would be too tedious

	class CHAOS_API zero_rotator
	{
	public:

		operator float() const { return 0.0f; }
		operator double() const { return 0.0; }
		operator glm::quat() const { return glm::quat(1.0f, 0.0f, 0.0f, 0.0f); } // BEWARE: first argument is W !!! want a normalized quaternion (0,0,0,0 is invalid)
		operator glm::dquat() const { return glm::dquat(1.0, 0.0, 0.0, 0.0); }   // BEWARE: first argument is W !!! want a normalized quaternion (0,0,0,0 is invalid)
	};


	// getrotatortype specializations
	template<> class getrotatortype<2, float>  : public boost::mpl::identity<float> {};
	template<> class getrotatortype<2, double> : public boost::mpl::identity<double> {};
	template<> class getrotatortype<3, float>  : public boost::mpl::identity<glm::quat> {};
	template<> class getrotatortype<3, double> : public boost::mpl::identity<glm::dquat> {};

	CHAOS_GEOMETRY_TEMPLATE(DIMENSION, T) using rotator = typename getrotatortype<DIMENSION, T>::type;

	template<std::floating_point T>	using trotator2 = typename getrotatortype<2, T>::type;	
	template<std::floating_point T>	using trotator3 = typename getrotatortype<3, T>::type;

	using rotator2  = typename trotator2<float>;
	using rotator3  = typename trotator3<float>;
	using drotator2 = typename trotator2<double>;
	using drotator3 = typename trotator3<double>;

#endif

}; // namespace chaos