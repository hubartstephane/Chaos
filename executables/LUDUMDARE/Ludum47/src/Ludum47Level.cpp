#include "Ludum47Level.h"
#include "Ludum47LevelInstance.h"
#include "Ludum47Game.h"
#include "Ludum47Player.h"
#include "Ludum47GameInstance.h"

#include <chaos/GLMTools.h>
#include <chaos/ParticleDefault.h>
#include <chaos/GeometryFramework.h>
#include <chaos/ParticleSpawner.h>
#include <chaos/StringTools.h>


#include <death/TM.h>



bool LudumSpeedIndication::Initialize(death::TMLayerInstance* in_layer_instance, chaos::TiledMap::GeometricObject const* in_geometric_object, death::TMObjectReferenceSolver& reference_solver)
{
	if (!death::TMObject::Initialize(in_layer_instance, in_geometric_object, reference_solver))
		return false;


	return true;
}

bool LudumRoad::Initialize(death::TMLayerInstance* in_layer_instance, chaos::TiledMap::GeometricObject const* in_geometric_object, death::TMObjectReferenceSolver& reference_solver)
{
	if (!death::TMObject::Initialize(in_layer_instance, in_geometric_object, reference_solver))
		return false;

	std::vector<glm::vec2> const* src_points = nullptr;

	if (chaos::TiledMap::GeometricObjectPolygon const* pn = in_geometric_object->GetObjectPolygon())
		src_points = &pn->points;
	else if (chaos::TiledMap::GeometricObjectPolyline const * pl = in_geometric_object->GetObjectPolyline())
		src_points = &pl->points;

	if (src_points == nullptr)
		return false;

	for (glm::vec2 const& p : *src_points)
	{
		RoadPoint rp;
		rp.position = p;
		points.push_back(rp);
	}
	return true;
}

void LudumRoad::OnLevelStarted()
{
	death::TMObject::OnLevelStarted();

	if (layer_instance != nullptr && layer_instance->GetLevelInstance() != nullptr)
	{
		LudumLevelInstance* level_instance = layer_instance->GetLevelInstance();
		if (level_instance != nullptr)
		{
			death::TMLayerInstance* li = level_instance->FindLayerInstance("SpeedIndications");
			if (li != nullptr)
			{
				size_t count = li->GetObjectCount();
				for (size_t i = 0 ; i < count ; ++i)					
				{
					LudumSpeedIndication* indication = auto_cast(li->GetObject(i));
					if (indication != nullptr)
					{
						auto b = indication->GetBoundingBox(true);

						indication = indication;



					}
				}
			}
		}
	}
}




// =============================================================
// LudumLevel implementation
// =============================================================

LudumLevel::LudumLevel()
{
	level_instance_class = LudumLevelInstance::GetStaticClass();
}

chaos::ParticleLayerBase * LudumLevel::DoCreateParticleLayer(death::TMLayerInstance * layer_instance)
{
	LudumGame * ludum_game = auto_cast(layer_instance->GetGame());

	std::string const & layer_name = layer_instance->GetTiledLayer()->name;

	if (chaos::StringTools::Stricmp(layer_name, "PlayerAndCamera") == 0)
	{
		ParticlePlayerLayerTrait layer_trait;
		layer_trait.game = ludum_game;
		return new chaos::ParticleLayer<ParticlePlayerLayerTrait>(layer_trait);
	}




	return death::TMLevel::DoCreateParticleLayer(layer_instance);
}


death::TMObjectFactory LudumLevel::DoGetObjectFactory(death::TMLayerInstance * in_layer_instance, chaos::TiledMap::TypedObject const * in_typed_object)
{
	if (in_typed_object->IsObjectOfType("Road"))
		return DEATH_MAKE_OBJECT_FACTORY(return new LudumRoad(););

	if (in_typed_object->IsObjectOfType("SpeedIndication"))
		return DEATH_MAKE_OBJECT_FACTORY(return new LudumSpeedIndication(););

	return death::TMLevel::DoGetObjectFactory(in_layer_instance, in_typed_object);
}


bool LudumLevel::Initialize(chaos::TiledMap::Map* in_tiled_map)
{
	if (!death::TMLevel::Initialize(in_tiled_map))
		return false;



	return true;
}
