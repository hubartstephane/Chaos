#pragma once

#include <chaos/StandardHeaders.h> 
#include <chaos/ParticleTools.h> 
#include <chaos/ParticleManager.h> 

// ===========================================================================
// the ID of the layers
// ===========================================================================

static int const BACKGROUND_LAYER_ID             = 0;
static int const BACKGROUND_GAMEOBJECT_LAYER_ID  = 1;
static int const GAMEOBJECT_LAYER_ID             = 2;
static int const BRICK_LAYER_ID                  = 3;
static int const BALL_LAYER_ID                   = 4;
static int const TEXT_LAYER_ID                   = 5;
static int const CHALLENGE_LAYER_ID              = 6;

// ===========================================================================
// Base Vertex class
// ===========================================================================

class VertexBase
{
public:

	glm::vec2 position;
	glm::vec3 texcoord;
	glm::vec4 color;
};

extern chaos::VertexDeclaration GetTypedVertexDeclaration(boost::mpl::identity<VertexBase>);

// ===========================================================================
// Background particle system
// ===========================================================================

class ParticleBackground
{
public:

	glm::vec4 color;
};

class ParticleBackgroundTrait : public chaos::ParticleLayerTrait<ParticleBackground, VertexBase>
{
public:

	size_t ParticleToVertex(ParticleBackground const * particle, VertexBase * vertices, size_t vertices_per_particle) const;
};

// ===========================================================================
// Object particle system
// ===========================================================================

class ParticleObject
{
public:

	chaos::box2 box;
	chaos::ParticleTexcoords texcoords;
	glm::vec4 color;
};

class ParticleObjectTrait : public chaos::ParticleLayerTrait<ParticleObject, VertexBase>
{
public:

	bool UpdateParticle(float delta_time, ParticleObject * particle);

	size_t ParticleToVertex(ParticleObject const * particle, VertexBase * vertices, size_t vertices_per_particle) const;
};




// ===========================================================================
// Brick particle system
// ===========================================================================

class ParticleBrick : public ParticleObject
{
public:

	int starting_life = 0;
	int life          = 0;
};

class ParticleBrickTrait : public chaos::ParticleLayerTrait<ParticleBrick, VertexBase>
{
public:

	bool UpdateParticle(float delta_time, ParticleBrick * particle);

	size_t ParticleToVertex(ParticleBrick const * particle, VertexBase * vertices, size_t vertices_per_particle) const;
};



// ===========================================================================
// Movable particle system
// ===========================================================================

class ParticleMovableObject : public ParticleObject
{
public:

	float delay_before_move;
	glm::vec2 velocity;
};

class ParticleMovableObjectTrait : public chaos::ParticleLayerTrait<ParticleMovableObject, VertexBase>
{
public:

	bool UpdateParticle(float delta_time, ParticleMovableObject * particle);

	size_t ParticleToVertex(ParticleMovableObject const * particle, VertexBase * vertices, size_t vertices_per_particle) const;

	void UpdateParticleVelocityFromCollision(chaos::box2 const & ball_box, chaos::box2 const & new_ball_box, glm::vec2 & velocity);


	class LudumGame * game = nullptr;
};









// ===========================================================================
// Challenge particle system
// ===========================================================================

class ParticleChallenge : public ParticleObject
{
public:

	class LudumChallenge * challenge = nullptr;
	size_t index = 0;
};

class ParticleChallengeTrait : public chaos::ParticleLayerTrait<ParticleChallenge, VertexBase>
{
public:

	size_t ParticleToVertex(ParticleChallenge const * particle, VertexBase * vertices, size_t vertices_per_particle) const;
};




