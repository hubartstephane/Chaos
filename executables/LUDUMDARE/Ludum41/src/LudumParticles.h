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
static int const TEXT_LAYER_ID                   = 3;
static int const CHALLENGE_LAYER_ID              = 4;

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

	chaos::ParticleCorners corners;
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
// Movable particle system
// ===========================================================================

class ParticleMovableObject : public ParticleObject
{
public:

	glm::vec2 velocity;
};

class ParticleMovableObjectTrait : public chaos::ParticleLayerTrait<ParticleMovableObject, VertexBase>
{
public:

	bool UpdateParticle(float delta_time, ParticleMovableObject * particle);

	size_t ParticleToVertex(ParticleMovableObject const * particle, VertexBase * vertices, size_t vertices_per_particle) const;
};

// ===========================================================================
// Challenge particle system
// ===========================================================================

class ParticleChallenge : public ParticleObject
{
public:

	class LudumSequenceChallenge * challenge = nullptr;
	size_t index = 0;
};

class ParticleChallengeTrait : public chaos::ParticleLayerTrait<ParticleChallenge, VertexBase>
{
public:

	size_t ParticleToVertex(ParticleChallenge const * particle, VertexBase * vertices, size_t vertices_per_particle) const;
};




