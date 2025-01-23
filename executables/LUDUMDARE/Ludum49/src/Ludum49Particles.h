#pragma once
#include "Ludum49PCH.h"
#include "Ludum49.h"



// ===========================================================================
// VertexBase and ParticleBase
// ===========================================================================

using VertexBase = VertexDefault;

class ParticleBase : public  TMParticle
{
public:



};


// ===========================================================================
// ParticlePlayer
// ===========================================================================

class ParticlePlayer : public ParticleBase
{
public:


};


class ParticlePlayerLayerTrait : public ParticleLayerTrait<ParticlePlayer, VertexBase>
{
public:

	bool UpdateParticle(float delta_time, ParticlePlayer & particle) const;

	void ParticleToPrimitives(ParticlePlayer const& particle, PrimitiveOutput<VertexDefault>& output) const;

	void UpdateRenderingStates(GPURenderContext* render_context, bool begin) const
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


CHAOS_REGISTER_CLASS(ParticleBase, TMParticle);
CHAOS_REGISTER_CLASS(ParticlePlayer, ParticleBase);

