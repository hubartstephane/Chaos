#pragma once


#include "LudumParticles.h"
#include "LudumGame.h"

#include <chaos/CollisionFramework.h>
#include <chaos/ClassTools.h>

void DeclareParticleClasses()
{
	chaos::ClassTools::DeclareClass<ParticleObject>("ParticleObject");
	chaos::ClassTools::DeclareClass<ParticleBackground>("ParticleBackground");
	chaos::ClassTools::DeclareClass<ParticleBrick, ParticleObject>("ParticleBrick");
	chaos::ClassTools::DeclareClass<ParticleMovableObject, ParticleObject>("ParticleMovableObject");
	chaos::ClassTools::DeclareClass<ParticleChallenge, ParticleObject>("ParticleChallenge");
}