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
#include <chaos/GLMTools.h>


#include <chaos/ParticleManager.h>


// ==============================================================
// Particles 
// ==============================================================

class ParticleExample
{
public:

	chaos::box2 box;
	glm::vec2 velocity;
	chaos::ParticleTexcoords texcoords;
	float lifetime;
	float remaining_time;
};

class VertexExample
{
public:

	glm::vec2 position;
	glm::vec3 texcoord;
	glm::vec4 color;
};

chaos::VertexDeclaration GetTypedVertexDeclaration(boost::mpl::identity<VertexExample>)
{
	chaos::VertexDeclaration result;
	result.Push(chaos::SEMANTIC_POSITION, 0, chaos::TYPE_FLOAT2);
	result.Push(chaos::SEMANTIC_TEXCOORD, 0, chaos::TYPE_FLOAT3);
	result.Push(chaos::SEMANTIC_COLOR,    0, chaos::TYPE_FLOAT4);
	return result;
}

class ParticleExampleTrait : public chaos::ParticleLayerTrait<ParticleExample, VertexExample>
{
public:

	//bool UpdateParticle(float delta_time, ParticleExample * particle)
	bool UpdateParticle(chaos::TypedUpdateParticleData<ParticleExample> & data)
	{
		data.particle->box.position += data.particle->velocity * data.delta_time;
		data.particle->remaining_time -= data.delta_time;
		return (data.particle->remaining_time <= 0.0f);
	}

	size_t ParticleToVertex(ParticleExample const * particle, VertexExample * vertices, size_t vertices_per_particle) const
	{
		chaos::ParticleTools::GenerateBoxParticle(particle->box, particle->texcoords, vertices);

		float alpha = particle->remaining_time / particle->lifetime;
		for (size_t i = 0 ; i < 6 ; ++i)
			vertices[i].color = glm::vec4(1.0f, 0.5f, 0.25f, alpha);

		if (rand() % 5 == 0) // flickering particles (not always rendered)
			return 0;
		
		return vertices_per_particle;
	}


};

using ParticleLayerDescExample = chaos::TypedParticleLayerDesc<ParticleExampleTrait>;

// ==============================================================
// Application 
// ==============================================================

class MyGLFWWindowOpenGLTest1 : public chaos::MyGLFW::Window
{
	int const LAYER_COUNT = 5;
	int const MATERIAL_COUNT = 3;

	float VIEWPORT_WANTED_ASPECT = (16.0f / 9.0f);
	float WORLD_X = 1000.0f;

protected:

	virtual bool OnDraw(glm::ivec2 size) override
	{
		// clear the buffers
		glClearColor(0.4f, 0.4f, 0.4f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		float far_plane = 1000.0f;
		glClearBufferfi(GL_DEPTH_STENCIL, 0, far_plane, 0);
		
		// change  the viewport 
		chaos::GLTools::SetViewportWithAspect(size, VIEWPORT_WANTED_ASPECT);

		//
		chaos::DisableLastReferenceLost<chaos::GPUProgramProvider> uniform_provider;

		glm::vec2 world_size     = glm::vec2(WORLD_X, WORLD_X / VIEWPORT_WANTED_ASPECT);
		glm::vec2 world_position = glm::vec2(0.0f, 0.0f);

		glm::vec3 scale = glm::vec3(2.0f / world_size.x, 2.0f / world_size.y, 1.0f);
		glm::vec3 tr    = glm::vec3(-world_position.x, -world_position.y, 0.0f); 

		glm::mat4 local_to_cam =  glm::scale(scale) * glm::translate(tr);

		uniform_provider.AddVariableValue("local_to_cam", local_to_cam);

		// draw
		particle_manager->Display(&uniform_provider);

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
		particle_manager->Tick((float)delta_time);

		return true; // no redraw
	}

	virtual void OnMouseButton(int button, int action, int modifier) override
	{
		if (action != GLFW_PRESS)
			return;

		double mouse_x = 0.0;
		double mouse_y = 0.0;
		glfwGetCursorPos(glfw_window, &mouse_x, &mouse_y);

		if (button == 0)
		{
			int layer_index    = rand() % LAYER_COUNT;
			int material_index = rand() % MATERIAL_COUNT;

			chaos::ParticleLayer * particle_layer = particle_manager->FindLayer(material_index + layer_index * MATERIAL_COUNT);
			if (particle_layer != nullptr)
			{
				int particle_count = rand() % 50 + 5;
				chaos::ParticleRangeAllocation * range = particle_layer->SpawnParticlesAndKeepRange(particle_count);
				range_allocations.push_back(range);

				size_t pc = particle_layer->GetParticleCount(range->GetParticleRange());

				ParticleExample * particles = (ParticleExample*)particle_layer->GetParticleBuffer(range->GetParticleRange());
				if (particles != nullptr)
				{
					glm::vec2 center = 
						(2.0f * (chaos::GLMTools::RandVec2() - glm::vec2(0.5f, 0.5f))) * 0.5f * glm::vec2(WORLD_X, WORLD_X / VIEWPORT_WANTED_ASPECT);
					
				//	glm::vec2 center;
				//	center.x = (float)mouse_x;
				//	center.y = (float)mouse_y;
					
					InitializeParticles(particles, pc, center);
				}
			}
		}
		else
		{
			size_t count = range_allocations.size();
			if (count > 0)
			{
				size_t r = (rand() % count);

				if (button == 1)
				{
					range_allocations[r] = range_allocations.back();
					range_allocations.pop_back();
				}
				else
				{
					range_allocations[r]->Show(!range_allocations[r]->IsVisible());
					//range_allocations[r]->Pause(!range_allocations[r]->IsPaused());
				}
			}
		}
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
		assert(MATERIAL_COUNT == 3);

		particle_manager = new chaos::ParticleManager;

		for (int i = 0; i < LAYER_COUNT; ++i)
		{
			for (int j = 0; j < MATERIAL_COUNT; ++j)
			{
				chaos::ParticleLayer * particle_layer = particle_manager->AddLayer(new ParticleLayerDescExample());
					
				particle_layer->SetRenderOrder(i);
				particle_layer->SetLayerID(j + i * MATERIAL_COUNT);

				particle_layer->SetRenderMaterial(materials[j]);
			}
		}
		return true;
	}

	void InitializeParticles(ParticleExample * particles, size_t count, glm::vec2 const & center)
	{
		
		float WORLD_HEIGHT = 0.5f * WORLD_X / VIEWPORT_WANTED_ASPECT;

		for (size_t i = 0; i < count; ++i)
		{		
			float size  = WORLD_HEIGHT * chaos::MathTools::RandFloat() * 0.04f;
			float alpha = chaos::MathTools::RandFloat() * 6.28f;
			float speed = WORLD_HEIGHT * chaos::MathTools::RandFloat() * 0.1f;
			float lifetime = 2.0f + chaos::MathTools::RandFloat() * 2.0f;

			particles[i].box.position = center;
			particles[i].box.half_size = 0.5f * glm::vec2(size, size);
			particles[i].velocity = glm::vec2(
				speed * chaos::MathTools::Cos(alpha),
				speed * chaos::MathTools::Sin(alpha));			
			particles[i].lifetime = lifetime;
			particles[i].remaining_time = lifetime;			
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


