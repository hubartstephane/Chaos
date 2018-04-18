#include <chaos/StandardHeaders.h> 
#include <chaos/FileTools.h> 
#include <chaos/Buffer.h> 
#include <chaos/LogTools.h> 
#include <chaos/MyGLFWSingleWindowApplication.h> 
#include <chaos/MyGLFWWindow.h> 
#include <chaos/WinTools.h> 
#include <chaos/Application.h>
#include <chaos/IrrklangTools.h>
#include <chaos/MathTools.h>
#include <chaos/SoundManager.h>
#include <chaos/JSONTools.h>
#include <chaos/FileTools.h>
#include <chaos/GLTextureTools.h>
#include <chaos/TextureLoader.h>
#include <chaos/GPUProgramLoader.h>
#include <chaos/VertexDeclaration.h>
#include <chaos/ParticleTools.h>
#include <chaos/Hotpoint.h>

#include <chaos/ParticleManager.h>


// ==============================================================
// Particles 
// ==============================================================

class ParticleExample
{
public:

	glm::vec2 position;
	glm::vec2 size;
	chaos::BitmapAtlas::BitmapTexcoords texcoords;
};

class VertexExample
{
public:

	glm::vec2 position;
	glm::vec3 texcoord;
	glm::vec4 color;
};

class ParticleExampleTrait : public chaos::ParticleLayerTrait<ParticleExample, VertexExample>
{
public:

	bool IsParticleObsolet(ParticleExample * p)
	{
		return false;
	}

	void UpdateParticle(float delta_time, ParticleExample * particle)
	{
		particle = particle;

	}

	void ParticleToVertex(ParticleExample const * particle, VertexExample * vertices) const
	{
		chaos::ParticleCorners corners = chaos::ParticleTools::GetParticleCorners(particle->position, particle->size, chaos::Hotpoint::CENTER);

		chaos::ParticleTools::GenerateBoxParticle(corners, particle->texcoords, vertices);
	}

	chaos::VertexDeclaration GetVertexDeclaration() const
	{
		chaos::VertexDeclaration result;
		result.Push(chaos::SEMANTIC_POSITION, 0, chaos::TYPE_FLOAT2);
		result.Push(chaos::SEMANTIC_TEXCOORD, 0, chaos::TYPE_FLOAT3);
		result.Push(chaos::SEMANTIC_COLOR,    0, chaos::TYPE_FLOAT3);
		return result;
	}
};

using ParticleLayerDescExample = chaos::TParticleLayerDesc<ParticleExampleTrait>;







// ==============================================================
// Application 
// ==============================================================

class MyGLFWWindowOpenGLTest1 : public chaos::MyGLFW::Window
{

protected:

	virtual bool OnDraw(glm::ivec2 size) override
	{
		glClearColor(0.0f, 0.0, 0.0, 0.0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


		particle_manager->Display(nullptr);

		return true;
	}

	virtual void Finalize() override
	{
		particle_manager = nullptr;
		particle_layers.clear();
		range_allocations.clear();
	}

	virtual bool Tick(double delta_time) override
	{
		particle_manager->Tick(0.0f);

		return true; // no redraw
	}

	virtual void OnMouseButton(int button, int action, int modifier) override
	{




	}

	virtual bool InitializeFromConfiguration(nlohmann::json const & config, boost::filesystem::path const & config_path) override
	{
		chaos::MyGLFW::SingleWindowApplication * application = chaos::MyGLFW::SingleWindowApplication::GetGLFWApplicationInstance();
		if (application == nullptr)
			return false;

		chaos::GPUResourceManager * gpu_manager = application->GetGPUResourceManager();

		chaos::RenderMaterial * RM1 = gpu_manager->FindRenderMaterial("mat1");
		chaos::RenderMaterial * RM2 = gpu_manager->FindRenderMaterial("mat2");
		chaos::RenderMaterial * RM3 = gpu_manager->FindRenderMaterial("mat3");

		chaos::RenderMaterial * materials[] = { RM1, RM2, RM3 };

		particle_manager = new chaos::ParticleManager;

		int const LAYER_COUNT    = 5;
		int const MATERIAL_COUNT = 5;
		for (int i = 0; i < LAYER_COUNT; ++i)
		{
			for (int j = 0; j < MATERIAL_COUNT; ++j)
			{
				chaos::ParticleLayer * particle_layer = new chaos::ParticleLayer(new ParticleLayerDescExample());
				particle_layer->SetRenderOrder(i);
				particle_layer->SetLayerID(j + i * MATERIAL_COUNT);

				int material_index = rand() % 3;
				int particle_count = rand() % 50 + 5;

				particle_layer->SetRenderMaterial(materials[material_index]);

				chaos::ParticleRangeAllocation * range = particle_layer->SpawnParticlesAndKeepRange(particle_count);
				range_allocations.push_back(range);

				size_t pc = particle_layer->GetParticleCount(range->GetParticleRange());
				
				ParticleExample * particles = (ParticleExample*)particle_layer->GetParticleBuffer(range->GetParticleRange());
				if (particles)
					InitializeParticles(particles, pc);

				particle_manager->AddLayer(particle_layer);
			}
		}
		return true;
	}

	void InitializeParticles(ParticleExample * particles, size_t count)
	{
		for (size_t i = 0; i < count; ++i)
		{
			particles[i].position = glm::vec3(0.0f, 0.0f, 0.0f);

		}
		


	}

	virtual void TweakHints(chaos::MyGLFW::WindowHints & hints, GLFWmonitor * monitor, bool pseudo_fullscreen) const override
	{
		chaos::MyGLFW::Window::TweakHints(hints, monitor, pseudo_fullscreen);
		hints.toplevel = 0;
		hints.decorated = 1;
	}

protected:

	boost::intrusive_ptr<chaos::ParticleManager> particle_manager;

	std::vector<boost::intrusive_ptr<chaos::ParticleLayer>> particle_layers;
	std::vector<boost::intrusive_ptr<chaos::ParticleRangeAllocation>> range_allocations;
};



// ===============================================


int _tmain(int argc, char ** argv, char ** env)
{
	chaos::MyGLFW::SingleWindowApplicationParams params;
	params.monitor = nullptr;
	params.width = 500;
	params.height = 500;
	params.monitor_index = 0;
	chaos::MyGLFW::RunWindowApplication<MyGLFWWindowOpenGLTest1>(argc, argv, env, params);

	return 0;
}


