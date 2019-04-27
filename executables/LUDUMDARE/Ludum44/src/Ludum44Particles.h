#pragma once

#include <chaos/StandardHeaders.h> 
#include <chaos/ParticleTools.h> 
#include <chaos/ParticleManager.h> 
#include <chaos/ParticleDefault.h> 
#include <chaos/GPUVertexDeclaration.h> 


#include <death/TiledMapParticle.h> 


static float const DANGER_RADIUS_RATIO = 0.3f;
static float const WAKEN_RADIUS_RATIO  = 1.0f;

// ===========================================================================
// VertexBase and ParticleBase
// ===========================================================================

class VertexBase : public chaos::ParticleDefault::Vertex
{
public:

	//glm::vec2 attraction_position;
	//glm::vec2 particle_center;
};

chaos::GPUVertexDeclaration GetTypedVertexDeclaration(boost::mpl::identity<VertexBase>);

class ParticleBase : public death::TiledMap::TileParticle
{
public:


};


// ===========================================================================
// ParticlePlayer
// ===========================================================================

class ParticlePlayer : public ParticleBase
{
public:

	glm::vec2 velocity = glm::vec2(0.0f, 0.0f);
	glm::vec2 acceleration = glm::vec2(0.0f, 0.0f);
	float life = 0.0f;

};

class ParticlePlayerTrait : public chaos::ParticleAllocationTrait<ParticlePlayer, VertexBase>
{
public:

	class LayerTrait
	{
	public:

		class LudumGame * game = nullptr;
	};


	bool UpdateParticle(float delta_time, ParticlePlayer * particle, LayerTrait const * layer_trait) const;

	size_t ParticleToVertices(ParticlePlayer const * particle, VertexBase * vertices, size_t vertices_per_particle, LayerTrait const * layer_trait) const;

};



