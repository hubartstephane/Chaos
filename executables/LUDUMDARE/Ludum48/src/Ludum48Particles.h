#pragma once

#include <chaos/Chaos.h>

// ===========================================================================
// VertexBase and ParticleBase
// ===========================================================================

using VertexBase = chaos::VertexDefault;

class ParticleBase : public  chaos::TMParticle
{
public:

	glm::vec2 velocity = glm::vec2(0.0f, 0.0f);
	float life = 0.0f;
	float duration = 0.0f;
};






// ===========================================================================
// ParticleAnimated
// ===========================================================================

class ParticleAnimated : public ParticleBase
{

public:

	//bool UpdateParticle(float delta_time);

public:
	int frame_index = 0;
	float animation_timer = 0.0f;
};

bool UpdateParticle(float delta_time, ParticleAnimated& particle);

class ParticleAnimatedLayerTrait : public chaos::ParticleLayerTrait<ParticleAnimated, VertexBase>
{
public:

#if 0
	static bool UpdateParticle(float delta_time, ParticleAnimated & particle);
#endif

};

// ===========================================================================
// ParticlePlayer
// ===========================================================================

class ParticlePlayer : public ParticleAnimated
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

CHAOS_REGISTER_CLASS2(ParticleBase, chaos::TMParticle);
CHAOS_REGISTER_CLASS2(ParticleAnimated, ParticleBase);
CHAOS_REGISTER_CLASS2(ParticlePlayer, ParticleAnimated);

