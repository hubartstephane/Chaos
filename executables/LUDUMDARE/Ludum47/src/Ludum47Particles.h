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
// the position of the player/opponent inside the road
// ===========================================================================

class RacePosition
{
public:

	// current lap count
	int lap_count = 0;
	// last validated index
	int road_point = 0;
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


CHAOS_REGISTER_CLASS2(ParticleBase, death::TMParticle);
CHAOS_REGISTER_CLASS2(ParticlePlayer, ParticleBase);

