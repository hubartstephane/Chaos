#include <chaos/StandardHeaders.h> 
#include <death/Death.h> 
#include "Ludum44Game.h"

#include <chaos/EmptyClass.h>
#include <chaos/MetaProgramming.h>




#include <chaos/GeometryFramework.h>


class Particle1
{
public:


};

class Particle2
{
public:

	glm::vec2 velocity;
	glm::vec2 acceleration;
	chaos::box2 bounding_box;


};

template<typename PARTICLE_TYPE>
void AutomaticParticleUpate(float delta_time, PARTICLE_TYPE * particle)
{

}

template<typename PARTICLE_TYPE, typename VERTEX_TYPE>
void AutomaticParticleToVertex(float delta_time, PARTICLE_TYPE const * particle, VERTEX_TYPE * vertex_type)
{

}

// PARTICLE TEMPLATES
BOOST_DECLARE_HAS_MEMBER(has_bounding_box, bounding_box);
BOOST_DECLARE_HAS_MEMBER(has_velocity, velocity);
BOOST_DECLARE_HAS_MEMBER(has_acceleration, acceleration);
BOOST_DECLARE_HAS_MEMBER(has_age, age);
BOOST_DECLARE_HAS_MEMBER(has_lifetime, lifetime);
BOOST_DECLARE_HAS_MEMBER(has_rotation, rotation);
BOOST_DECLARE_HAS_MEMBER(has_rotation_speed, rotation_speed);
BOOST_DECLARE_HAS_MEMBER(has_texcoords, texcoords);
BOOST_DECLARE_HAS_MEMBER(has_color, color);
BOOST_DECLARE_HAS_MEMBER(has_bitmap_info, bitmap_info);

// VERTEX BASE
BOOST_DECLARE_HAS_MEMBER(has_position, position);
BOOST_DECLARE_HAS_MEMBER(has_texcoord, texcoord);
// BOOST_DECLARE_HAS_MEMBER(has_color, color); // already defined in part





/** a meta structure for generalized rotator initialization (angle in RAD or quaternion) */
template<typename T> 
struct make_rotator; // main definition
template<>
struct make_rotator<float>{ static float value() { return 0.0f; } };
template<>
struct make_rotator<double>{ static double value() { return 0.0; } };
template<>
struct make_rotator<glm::vec4>{ static glm::vec4 value() { return glm::vec4(1.0f, 0.0f, 0.0f, 0.0f); } };
template<>
struct make_rotator<glm::tvec4<double>>{ static glm::tvec4<double> value() { return glm::tvec4<double>(1.0, 0.0, 0.0, 0.0); } };

/** an oriented bounding box */
template<typename T, int dimension>
class type_obox : public chaos::type_geometric<T, dimension>
{
public:

	using vec_type = typename chaos::type_geometric<T, dimension>::vec_type;

	using rotator_type = typename chaos::type_geometric<T, dimension>::rotator_type;


	/** constructor (empty box) */
	type_obox() : half_size((T)-1.0f) {}
	/** copy constructor */
	type_obox(type_obox const & src) : position(src.position), half_size(src.half_size), rotator(src.rotator) {}
	/** other constructor */
	type_obox(vec_type const & in_position, vec_type const & in_half_size, rotator_type const & in_rotator) : position(in_position), half_size(in_half_size), rotator(in_rotator){}











	/** returns true whether the box is empty */
	bool IsGeometryEmpty() const
	{
		return glm::any(glm::lessThan(half_size, vec_type((T)0.0f)));
	}

	/** set the box has an empty box */
	void SetGeometryEmpty()
	{
		half_size = vec_type((T)-1.0f);
	}

public:


	/** the center of the box */
	vec_type position;
	/** the half size the box */
	vec_type half_size;
	/** the angle/quaternion of rotation to apply to a box to have this obox */
	rotator_type rotator = make_rotator<rotator_type>::value();
};




template<typename T> using type_obox2     = type_obox<T, 2>;
template<typename T> using type_obox3     = type_obox<T, 3>;

using obox2      = type_obox2<float>;
using obox3      = type_obox3<float>;





template<typename T, template<typename U, int DIMENSION> class BOX_TEMPLATE>
chaos::type_sphere3<T> GetBoundingSphereX(BOX_TEMPLATE<T, 3> const & b)
{
	return b.IsEmpty() ? chaos::type_sphere3<T>() : chaos::type_sphere3<T>(b.position, glm::length(b.half_size));

	return chaos::type_sphere3<T>();
}








int CHAOS_MAIN(int argc, char ** argv, char ** env)
{	
	
	obox2 o2;
	obox3 o3;


	auto s1 = chaos::GetBoundingSphere(chaos::box3());
	auto s2 = chaos::GetBoundingSphere(o3);









	return 0;


	auto a1 = has_velocity<Particle1>::value;
	auto a2 = has_velocity<Particle2>::value;

	auto b1 = boost::mpl::bool_<has_velocity<Particle1>::value>();
	auto b2 = boost::mpl::bool_<has_acceleration<Particle2>::value>();


	chaos::MyGLFW::SingleWindowApplicationParams params;
	params.monitor = nullptr;
	params.width = 500;
	params.height = 500;
	params.monitor_index = 0;
	death::RunGame<LudumGame>(argc, argv, env, params);
	return 0;
}


