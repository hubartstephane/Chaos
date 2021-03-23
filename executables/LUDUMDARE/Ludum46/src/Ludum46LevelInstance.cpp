#include <chaos/Chaos.h>

#include "Ludum46Level.h"
#include "Ludum46LevelInstance.h"
#include "Ludum46Game.h"
#include "Ludum46Player.h"
#include "Ludum46GameInstance.h"
#include "Ludum46PlayerDisplacementComponent.h"

// =============================================================
// LudumLevelInstance implementation
// =============================================================

LudumLevelInstance::LudumLevelInstance()
{
	player_displacement_component_class = LudumPlayerDisplacementComponent::GetStaticClass();
}

void LudumLevelInstance::CreateCameras()
{
	chaos::TMLevelInstance::CreateCameras();

	size_t camera_count = cameras.size();
	for (size_t i = 0; i < camera_count; ++i)
	{
		cameras[i]->SetSafeZone(glm::vec2(0.6f, 0.6f));
		cameras[i]->AddComponent(new chaos::FollowPlayerCameraComponent(0));
		cameras[i]->AddComponent(new chaos::ShakeCameraComponent(0.15f, 0.05f, 0.15f, true, true));
		cameras[i]->AddComponent(new chaos::SoundListenerCameraComponent());
	}
}


template<typename VERTEX_TYPE>
class DrawInterface : public chaos::PrimitiveOutput<VERTEX_TYPE>
{
public:

	DrawInterface(chaos::GPUVertexDeclaration* in_vertex_declaration = nullptr, size_t in_vertex_requirement_evaluation = chaos::PrimitiveOutputBase::MIN_VERTEX_ALLOCATION) :
		PrimitiveOutput(&dynamic_mesh, nullptr, nullptr, in_vertex_declaration, in_vertex_requirement_evaluation)
	{
	}

	DrawInterface(chaos::ObjectRequest render_material_request, size_t in_vertex_requirement_evaluation = chaos::PrimitiveOutputBase::MIN_VERTEX_ALLOCATION) :
		PrimitiveOutput(&dynamic_mesh, nullptr, nullptr, render_material_request, in_vertex_requirement_evaluation)
	{
	}

	void Display(chaos::GPURenderer* renderer, chaos::GPUProgramProviderBase const* uniform_provider, chaos::GPURenderParams const& render_params, bool clear_mesh = true)
	{
		if (clear_mesh)
			Flush();
		else
			FlushMeshElement();
		dynamic_mesh.Display(renderer, uniform_provider, render_params);
	}

	chaos::GPUDynamicMesh* ExtractMesh()
	{
		chaos::GPUDynamicMesh* result = new chaos::GPUDynamicMesh();
		if (result != nullptr)
		{
			Flush();
			swap(*result, dynamic_mesh);
		}
		return result;
	}

	chaos::QuadPrimitive<VERTEX_TYPE> DrawText(char const* text, chaos::ParticleTextGenerator::GeneratorParams const& params = {})
	{
		//GeneratorResult& result,


		return {};
	}

protected:

	chaos::GPUDynamicMesh dynamic_mesh;
};


template<typename VERTEX_TYPE>
void color(chaos::Primitive<VERTEX_TYPE>& primitive, glm::vec4 const& c)
{
	for (VERTEX_TYPE& v : primitive)
		v.color = c;
}

template<typename VERTEX_TYPE>
void position(chaos::Primitive<VERTEX_TYPE>& primitive, glm::vec2 const& p)
{
	for (VERTEX_TYPE& v : primitive)
		v.position = c;
}

template<typename VERTEX_TYPE>
void flags(chaos::Primitive<VERTEX_TYPE>& primitive,int f)
{
	for (VERTEX_TYPE& v : primitive)
		v.flags = f;
}

template<typename VERTEX_TYPE>
void texcoord(chaos::QuadPrimitive<VERTEX_TYPE>& primitive, int f)
{
	for (VERTEX_TYPE& v : primitive)
		v.flags = f;
}

template<typename VERTEX_TYPE>
void texcoord(chaos::TrianglePairPrimitive<VERTEX_TYPE>& primitive, int f)
{
	//for (VERTEX_TYPE& v : primitive)
	//	v.flags = f;
}





template<typename VERTEX_TYPE>
void bounding_box(chaos::QuadPrimitive<VERTEX_TYPE>& primitive, int f)
{
	for (VERTEX_TYPE& v : primitive)
		v.flags = f;
}

template<typename VERTEX_TYPE>
void bounding_box(chaos::TrianglePairPrimitive<VERTEX_TYPE>& primitive, int f)
{
	//for (VERTEX_TYPE& v : primitive)
	//	v.flags = f;
}





int LudumLevelInstance::DoDisplay(chaos::GPURenderer* renderer, chaos::GPUProgramProviderBase const* uniform_provider, chaos::GPURenderParams const& render_params)
{
	int result = chaos::TMLevelInstance::DoDisplay(renderer, uniform_provider, render_params);

	glPointSize(10.0f);

	DrawInterface<chaos::VertexDefault> DI("screenspace1");

	glDisable(GL_DEPTH_TEST);

	auto Lines = DI.AddLines(1);

	Lines[0].color = glm::vec4(1.0f, 0.0f, 0.0f, 1.0f);
	Lines[0].flags = 0;
	Lines[0].texcoord = { -1.0f, -1.0f , -1 };
	Lines[0].position.x = 0.0f;
	Lines[0].position.y = 0.0f;

	Lines[1].color = glm::vec4(1.0f, 0.0f, 0.0f, 1.0f);
	Lines[1].flags = 0;
	Lines[1].texcoord = { -1.0f, -1.0f , -1 };
	Lines[1].position.x = 700.0f;
	Lines[1].position.y = 400.0f;



	color(Lines, { 1.0f, 0.0f, 1.0f, 1.0f });

	//chaos::shared_ptr<chaos::GPUDynamicMesh> m = DI.ExtractMesh();
	//m->Display(renderer, uniform_provider, render_params);



	DI.Display(renderer, uniform_provider, render_params);





#if 0


	chaos::GPUResourceManager* resource_manager = chaos::WindowApplication::GetGPUResourceManagerInstance();
	if (resource_manager != nullptr)
	{
		chaos::GPURenderMaterial* material1 = resource_manager->FindRenderMaterial("screenspace1");
		chaos::GPURenderMaterial* material2 = resource_manager->FindRenderMaterial("screenspace2");
		if (material1 != nullptr && material2 != nullptr)
		{
			chaos::shared_ptr<chaos::GPUDynamicMesh> dynamic_mesh = new chaos::GPUDynamicMesh();
			if (dynamic_mesh != nullptr)
			{
				chaos::shared_ptr<chaos::GPUVertexDeclaration> vertex_declaration = new chaos::GPUVertexDeclaration;
				if (vertex_declaration != nullptr)
				{
					chaos::GetTypedVertexDeclaration(vertex_declaration.get(), boost::mpl::identity<chaos::VertexDefault>());

					chaos::PrimitiveOutput<chaos::VertexDefault> output(dynamic_mesh.get(), nullptr, vertex_declaration.get(), material2, 0);

					int i = 0;

					glDisable(GL_DEPTH_TEST);

					auto Lines = output.AddLines(1);

					Lines[0].color = glm::vec4(1.0f, 0.0f, 0.0f, 1.0f);
					Lines[0].flags = 0;
					Lines[0].texcoord = { -1.0f, -1.0f , -1 };
					Lines[0].position.x = 0.0f;
					Lines[0].position.y = 0.0f;

					Lines[1].color = glm::vec4(1.0f, 0.0f, 0.0f, 1.0f);
					Lines[1].flags = 0;
					Lines[1].texcoord = { -1.0f, -1.0f , -1 };
					Lines[1].position.x = 700.0f;
					Lines[1].position.y = 400.0f;


					output.SetRenderMaterial(material1);

					i = 0;
					
					auto quad = output.AddQuads(1);
					for (auto& p : quad)
					{
						static float pos[] = {-50.0f, -50.0f, 50.0f, -50.0f, 50.0f, 50.0f, -50.0f, 50.0f};

						p.color = glm::vec4(0.0f, 1.0f, 0.0f, 1.0f);
						p.flags = 0;
						p.texcoord = { -1.0f, -1.0f , -1 };
						p.position.x = pos[i * 2];
						p.position.y = pos[i * 2 + 1];
						++i;
					}


					output.SetRenderMaterial(material2);

					auto Points = output.AddPoints(100);
					for (auto& p : Points)
					{
						p.color = glm::vec4(1.0f, 0.0f, 0.0f, 1.0f);
						p.flags = 0;
						p.texcoord = { -1.0f, -1.0f , -1 };
						p.position.x = -500.0f + float(i) * 50.0f;
						p.position.y = 0.0f;
						++i;
					}







					output.Flush();




					dynamic_mesh->Display(renderer, uniform_provider, render_params);
				}
			}
		}
	}


#endif

	return result;
}

bool LudumLevelInstance::DoTick(float delta_time)
{
	chaos::TMLevelInstance::DoTick(delta_time);


	// completed ?
	if (completion_timer > 0.0f)
		completion_timer = std::max(0.0f, completion_timer - delta_time);

	flame_health = std::max(0.0f, flame_health - delta_time * flame_health_lost_per_second);

	return true;
}

bool LudumLevelInstance::Initialize(chaos::Game * in_game, chaos::Level * in_level)
{
	if (!chaos::TMLevelInstance::Initialize(in_game, in_level))
		return false;

	LudumLevel* ludum_level = auto_cast(in_level);
	if (ludum_level != nullptr)
	{
		flame_health = ludum_level->flame_initial_health;
		flame_initial_health = ludum_level->flame_initial_health;
		flame_health_lost_per_second = ludum_level->flame_health_lost_per_second;
	}	
	return true;
}

int LudumLevelInstance::GetCurrentSoulCount() const
{
	chaos::TMLayerInstance const * layer_instance = FindLayerInstance("Souls", true);
	if (layer_instance != nullptr && layer_instance->GetParticleLayer() != nullptr)
		return (int)layer_instance->GetParticleLayer()->GetParticleCount();
	return 0;
}

int LudumLevelInstance::GetPotentialSoulCount() const
{
	int result = 0;

	chaos::TMLayerInstance const* layer_instance = FindLayerInstance("Objects", true);
	if (layer_instance != nullptr)
	{
		size_t count = layer_instance->GetObjectCount();
		for (size_t i = 0; i < count; ++i)
		{
			SoulSpawner const * soul_spawner = auto_cast(layer_instance->GetObject(i));
			if (soul_spawner != nullptr)
			{
				int remaining_count = soul_spawner->GetRemainingParticleCount();
				if (remaining_count < 0) // infinite case
					return -1;
				result += remaining_count;
			}
		}
	}
	return result;
}

bool LudumLevelInstance::IsPlayerDead(chaos::Player* player)
{
	if (chaos::TMLevelInstance::IsPlayerDead(player))
		return true;

	if (flame_health <= 0.0f)
		return true;

#if 0 // now maps are no more coherent
	LudumPlayer* ludum_player = auto_cast(player);

	LudumLevel * ludum_level = GetLevel();

	if (ludum_level != nullptr && ludum_player != nullptr)
	{
		int potential_soul_count = GetPotentialSoulCount();
		if (potential_soul_count < 0) // No END
			return false;


		int current_soul_count = GetCurrentSoulCount();
		if (current_soul_count + potential_soul_count + ludum_player->burned_souls < ludum_level->required_souls)
			return true;
	}
#endif
	return false;
}

bool LudumLevelInstance::CheckLevelCompletion() const
{
	LudumLevel const * ludum_level = GetLevel();

	LudumPlayer const * ludum_player = GetPlayer(0);
	if (ludum_player != nullptr && ludum_level != nullptr)
	{
		if (ludum_player->burned_souls >= ludum_level->required_souls)
		{
			if (completion_timer < 0.0f)
			{
				completion_timer = completion_delay; // forced to be mutable !??
			}
			return true;
		}
	}
	return false;
}

bool LudumLevelInstance::CanCompleteLevel() const
{
	if (completion_timer == 0.0f)
		return true;
	return false;
}


void LudumLevelInstance::SpawnBloodParticles(chaos::box2 const& box, int particles_count)
{
	chaos::ParticleSpawner spawner = GetParticleSpawner("Blood", "Blood");
	if (spawner.IsValid())
	{
		spawner.SpawnParticles(particles_count, false, [this, box, &spawner](chaos::ParticleAccessor<ParticleBlood> accessor)
		{
			for (ParticleBlood& p : accessor)
			{
				p.bounding_box = box;
				p.color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);

				float angle = chaos::MathTools::RandFloat() * (float)M_PI;
				p.velocity = chaos::MathTools::RandFloat(50.0f, 100.0f) * glm::vec2(std::cos(angle), std::sin(angle));
				p.bitmap_info = spawner.GetBitmapInfo();

				p.duration = 3.0f;
				p.life = 0.0f;
				p.acceleration.y = -30.0f;
			}		
		});
	}

}

void LudumLevelInstance::SpawnBurnedSoulParticles(chaos::box2 const& box, int particles_count)
{
	LudumPlayer* ludum_player = GetPlayer(0);
	if (ludum_player != nullptr)
		ludum_player->SetScore(10, true); // increment score

	chaos::ParticleSpawner spawner = GetParticleSpawner("BurnedSouls", "BurnedSoulAnim");
	if (spawner.IsValid())
	{
		spawner.SpawnParticles(particles_count, false, [this, box, &spawner](chaos::ParticleAccessor<ParticleBurnedSoul> accessor)
		{
			for (ParticleBurnedSoul& p : accessor)
			{
				p.bounding_box = box;
				p.color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
				p.velocity = glm::vec2(0.0f, chaos::MathTools::RandFloat(50.0f, 100.0f) );
				p.duration = 6.0f;
				p.life = 0.0f;
				p.offset_t = chaos::MathTools::RandFloat() * (float)M_PI;
				p.bitmap_info = spawner.GetBitmapInfo();

				p.acceleration.y = 0.0f;
			}
		});
	}
}