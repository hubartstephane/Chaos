#pragma once

#include <chaos/StandardHeaders.h> 
#include <chaos/ParticleTools.h> 
#include <chaos/ParticleManager.h> 
#include <chaos/ParticleDefault.h> 
#include <chaos/GPUVertexDeclaration.h> 
#include <chaos/BitmapAtlas.h> 
#include <chaos/Class.h> 

#include <death/TM.h> 




// ===========================================================================
// the car values
// ===========================================================================

class CarData
{
public: 

	float min_velocity = -50.0f;
	float max_velocity = 200.0f;
	float acceleration = 50.0f;
	float angular_velocity = 3.14f;
	float normal_deceleration = 30.0f;
	float break_deceleration = 100.0f;
};





// ===========================================================================
// the position of the player/opponent inside the road
// ===========================================================================

class RacePosition
{
public:

	bool operator < (RacePosition const& src) const
	{
		return (current_lap < src.current_lap) ||
			(current_lap == src.current_lap && current_road_point < src.current_road_point);
	}

	bool operator > (RacePosition const& src) const
	{
		return (current_lap > src.current_lap) ||
			(current_lap == src.current_lap && current_road_point > src.current_road_point);
	}

	bool operator == (RacePosition const& src) const
	{
		return (current_lap == src.current_lap && current_road_point == src.current_road_point);
	}

	// current lap count
	size_t current_lap = 0;
	// last validated index
	size_t current_road_point = 0;
};



// ===========================================================================
// VertexBase and ParticleBase
// ===========================================================================

using VertexBase = chaos::VertexDefault;

class ParticleBase : public  death::TMParticle
{
public:

	// velocity length in direction of the oriented particle
	float velocity = 0.0f;

	bool accelerate_pressed = false;

	bool break_pressed = false;
};


// ===========================================================================
// ParticlePlayer
// ===========================================================================

class ParticlePlayer : public ParticleBase
{
public:


};


class ParticlePlayerLayerTrait : public chaos::ParticleLayerTrait<ParticlePlayer, VertexBase>
{
public:

	bool UpdateParticle(float delta_time, ParticlePlayer & particle) const;

	void UpdateRenderingStates(chaos::GPURenderer* renderer, bool begin) const
	{
		if (begin)
		{
			glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			glDisable(GL_DEPTH_TEST);
			glDisable(GL_CULL_FACE);
		}
		else
		{
			glDisable(GL_BLEND);
			glEnable(GL_DEPTH_TEST);
			glEnable(GL_CULL_FACE);
		}
	}

public:

	class LudumGame* game = nullptr;
};






// ===========================================================================
// ParticlePlayer
// ===========================================================================

class ParticleOpponent : public ParticleBase
{
public:


};


class ParticleOpponentLayerTrait : public chaos::ParticleLayerTrait<ParticleOpponent, VertexBase>
{
public:

	bool UpdateParticle(float delta_time, ParticleOpponent& particle) const;

	

public:

	class LudumGame* game = nullptr;
};













CHAOS_REGISTER_CLASS2(ParticleBase, death::TMParticle);
CHAOS_REGISTER_CLASS2(ParticlePlayer, ParticleBase);
CHAOS_REGISTER_CLASS2(ParticleOpponent, ParticleBase);


