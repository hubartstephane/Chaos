#pragma once

#include <chaos/Chaos.h>

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

	float reaction_value = 30.0f;
	float reaction_decrease = 100.0f;
};





// ===========================================================================
// the position of the player/opponent inside the road
// ===========================================================================

class RacePosition
{
public:

	bool IsCompleted() const
	{
		return completed;
	}

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

	// completed
	bool completed = false;
	// current lap count
	size_t current_lap = 0;
	// last validated index
	size_t current_road_point = 0;
};



// ===========================================================================
// VertexBase and ParticleBase
// ===========================================================================

using VertexBase = chaos::VertexDefault;

class ParticleBase : public  chaos::TMParticle
{
public:

	// velocity length in direction of the oriented particle
	float velocity = 0.0f;

	glm::vec2 collision_direction = { 0.0f, 0.0f };

	float     collision_reaction_intensity = 0.0f;

};



// ===========================================================================
// ParticleSmoke
// ===========================================================================

class ParticleSmoke : public  chaos::TMParticle
{
public:

	float lifetime = 1.0f;
	float duration = 1.0f; // value not update
	glm::vec2 velocity = { 0.0f, 0.0f };
	float angular_velocity = 1.0f;

};


class ParticleSmokeLayerTrait : public chaos::ParticleLayerTrait<ParticleSmoke, VertexBase>
{
public:

	bool UpdateParticle(float delta_time, ParticleSmoke& particle) const;

public:

	class LudumGame* game = nullptr;
};


// ===========================================================================
// ParticlePlayer
// ===========================================================================

class ParticlePlayer : public ParticleBase
{
public:

	bool accelerate_pressed = false;

	bool break_pressed = false;
};


class ParticlePlayerLayerTrait : public chaos::ParticleLayerTrait<ParticlePlayer, VertexBase>
{
public:

	bool UpdateParticle(float delta_time, ParticlePlayer & particle) const;

	void ParticleToPrimitives(ParticlePlayer const& particle, chaos::PrimitiveOutput<chaos::VertexDefault>& output) const;

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










CHAOS_REGISTER_CLASS(ParticleSmoke, chaos::TMParticle);
CHAOS_REGISTER_CLASS(ParticleBase, chaos::TMParticle);
CHAOS_REGISTER_CLASS(ParticlePlayer, ParticleBase);
CHAOS_REGISTER_CLASS(ParticleOpponent, ParticleBase);


