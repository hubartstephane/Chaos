#pragma once

#include <chaos/StandardHeaders.h> 
#include <chaos/ParticleTools.h> 
#include <chaos/ParticleManager.h> 
#include <chaos/ParticleDefault.h> 
#include <chaos/GPUVertexDeclaration.h> 
#include <chaos/BitmapAtlas.h> 

#include <death/TiledMapParticle.h> 


// ===========================================================================
// VertexBase and ParticleBase
// ===========================================================================

class VertexBase : public chaos::ParticleDefault::Vertex
{
public:

};

chaos::GPUVertexDeclaration GetTypedVertexDeclaration(boost::mpl::identity<VertexBase>);

class ParticleBase : public  death::TiledMap::TileParticle
{
public:

	glm::vec2 velocity = glm::vec2(0.0f, 0.0f);

};

// ===========================================================================
// ParticleLideTrait
// ===========================================================================


class ParticleLifeTrait : public chaos::ParticleAllocationTrait<chaos::ParticleDefault::Particle, chaos::ParticleDefault::Vertex>
{
public:

};


// ===========================================================================
// ParticleBonus
// ===========================================================================


class ParticleBonus : public ParticleBase
{
public:

	

	chaos::TagType bonus_type;
};

class ParticleBonusTrait : public chaos::ParticleAllocationTrait<ParticleBonus, VertexBase>
{
public:

	class LayerTrait
	{
	public:

		class LudumGame * game = nullptr;
	};

	chaos::box2 BeginUpdateParticles(float delta_time, ParticleBonus * particle, size_t count, LayerTrait const * layer_trait) const;


	bool UpdateParticle(float delta_time, ParticleBonus * particle, chaos::box2 const & player_box, LayerTrait const * layer_trait) const;

	size_t ParticleToVertices(ParticleBonus const * particle, VertexBase * vertices, size_t vertices_per_particle, LayerTrait const * layer_trait) const;

};



// ===========================================================================
// ParticlePlayer
// ===========================================================================

class ParticlePlayer : public ParticleBase
{
public:

	float  orientation = 0.0f;
	float  image_timer = 0.0f;
	size_t current_frame = 0;
	

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

// ===========================================================================
// ParticlePlayer
// ===========================================================================

class ParticleFire : public ParticleBase
{
public:

	bool  player_ownership = true;
	bool  trample = false;
	float damage = 1.0f;
	float rotation = 0.0f;

	float lifetime = 10.0f;
};


class ParticleFireUpdateData
{
public:

	/** the camera box */
	chaos::box2 camera_box;
#if 0
	/** all the enemies */
	std::vector<ParticleEnemy*> enemies;
#endif
	/** the main player */
	class LudumPlayer * player = nullptr;
};


class ParticleFireTrait : public chaos::ParticleAllocationTrait<ParticleFire, VertexBase>
{
public:

	class LayerTrait
	{
	public:

		class LudumGame * game = nullptr;
	};

	ParticleFireUpdateData BeginUpdateParticles(float delta_time, ParticleFire * particle, size_t count, LayerTrait const * layer_trait) const;

	bool UpdateParticle(float delta_time, ParticleFire * particle, ParticleFireUpdateData const & update_data, LayerTrait const * layer_trait) const;

	size_t ParticleToVertices(ParticleFire const * particle, VertexBase * vertices, size_t vertices_per_particle, LayerTrait const * layer_trait) const;
};



