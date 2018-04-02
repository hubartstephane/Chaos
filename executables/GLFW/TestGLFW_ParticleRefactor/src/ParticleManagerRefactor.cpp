#include "ParticleManagerRefactor.h"

// ==============================================================
// PARTICLE RANGE ALLOCATION
// ==============================================================

ParticleRangeAllocation::~ParticleRangeAllocation()
{
	RemoveFromLayer();
}

void ParticleRangeAllocation::RemoveFromLayer()
{
	if (layer == nullptr)
		return;
	layer->RemoveParticleAllocation(this);
}

ParticleRange ParticleRangeAllocation::GetParticleRange() const
{
	if (layer == nullptr)
		return ParticleRange();
	return layer->particles_ranges[range_index];
}

size_t ParticleRangeAllocation::GetParticleCount() const
{
	return GetParticleRange().count;
}

void * ParticleRangeAllocation::GetParticleBuffer()
{
	if (layer == nullptr)
		return nullptr;
	return layer->GetParticleBuffer(layer->particles_ranges[range_index]);
}

void const * ParticleRangeAllocation::GetParticleBuffer() const
{
	if (layer == nullptr)
		return nullptr;
	return layer->GetParticleBuffer(layer->particles_ranges[range_index]);
}

// ==============================================================
// PARTICLE LAYER DESC
// ==============================================================

size_t ParticleLayerDesc::GetParticleSize() const
{
	return 0;
}

size_t ParticleLayerDesc::GetVertexSize() const
{
	return 0;
}

void ParticleLayerDesc::UpdateParticles(float delta_time, void * particles, size_t particle_count)
{

}

size_t ParticleLayerDesc::DestroyObsoletParticles(void * particles, size_t particle_count, size_t * deletion_vector)
{
	return 0;
}

bool ParticleLayerDesc::HasParticleLimitedLifeTime() const
{
	return false;
}

bool ParticleLayerDesc::AreParticlesDynamic() const
{
	return false;
}

// ==============================================================
// PARTICLE LAYER
// ==============================================================

ParticleLayer::ParticleLayer(ParticleLayerDesc * in_layer_desc)
	: layer_desc(in_layer_desc)
{
	assert(in_layer_desc != nullptr);
	particle_size = layer_desc->GetParticleSize();
	vertex_size  = layer_desc->GetVertexSize();
}

ParticleLayer::~ParticleLayer()
{
	DetachAllParticleAllocations();
}

void ParticleLayer::DetachAllParticleAllocations()
{
	while (range_allocations.size())
		RemoveParticleAllocation(range_allocations[range_allocations.size() - 1]);
}

size_t ParticleLayer::GetParticleCount() const
{
	if (particle_size == 0)
		return 0;
	return particles.size() / particle_size;
}

size_t ParticleLayer::GetParticleCount(ParticleRange range) const
{
	if (particle_size == 0)
		return 0;
	return range.count / particle_size;
}

void * ParticleLayer::GetParticleBuffer(ParticleRange range)
{
	if (particle_size == 0)
		return nullptr;
	return &particles[range.start * particle_size];
}

void const * ParticleLayer::GetParticleBuffer(ParticleRange range) const
{
	if (particle_size == 0)
		return nullptr;
	return &particles[range.start * particle_size];
}

bool ParticleLayer::HasParticleLimitedLifeTime() const
{
	return layer_desc->HasParticleLimitedLifeTime();
}

bool ParticleLayer::AreParticlesDynamic() const
{
	return layer_desc->AreParticlesDynamic();
}

void ParticleLayer::Pause(bool in_paused)
{
	paused = in_paused;
}

bool ParticleLayer::IsPaused() const
{
	return paused;
}

void ParticleLayer::Show(bool in_visible)
{
	visible = in_visible;
}

bool ParticleLayer::IsVisible() const
{
	return visible;
}

void ParticleLayer::TickParticles(float delta_time)
{
	// no particles, nothing to do
	size_t particle_count = GetParticleCount();
	if (particle_count == 0)
		return;
	// early exit
	if (IsPaused())
		return;
	// update the particles themselves
	if (AreParticlesDynamic())
	{
		UpdateParticles(delta_time);
		require_GPU_update = true;
	}
	// destroy the particles that are to be destroyed
	if (pending_kill_particles > 0 || HasParticleLimitedLifeTime())
	{
		size_t new_particle_count = DestroyObsoletParticles();
		if (new_particle_count != particle_count)
		{
			UpdateParticleRanges(new_particle_count);
			require_GPU_update = true;
		}
		pending_kill_particles = 0;
	}
}

void ParticleLayer::UpdateParticles(float delta_time)
{
	layer_desc->UpdateParticles(delta_time, &particles[0], GetParticleCount());
}

size_t ParticleLayer::DestroyObsoletParticles()
{
	return layer_desc->DestroyObsoletParticles(&particles[0], GetParticleCount(), &deletion_vector[0]);
}

ParticleRangeAllocation * ParticleLayer::SpawnParticlesAndKeepRange(size_t count, bool particles_owner)
{
	ParticleRange range = SpawnParticles(count);
	if (range.count == 0)
		return nullptr;

	ParticleRangeAllocation * result = new ParticleRangeAllocation;

	result->layer = this;
	result->range_index = particles_ranges.size();
	result->particles_owner = particles_owner;

	particles_ranges.push_back(range);
	range_allocations.push_back(result);

	return result;
}

ParticleRange ParticleLayer::SpawnParticles(size_t count)
{
	ParticleRange result;
	if (count > 0 && particle_size > 0)
	{
		size_t current_particle_count = GetParticleCount();

		// initialize the result
		result.start = current_particle_count;
		result.count = count;
		// create the particles and the suppression corresponding data
		size_t new_count = current_particle_count + count;
		particles.resize(new_count * particle_size, 0);
		deletion_vector.resize(new_count, 0);	
	}
	return result;
}

void ParticleLayer::MarkParticlesToDestroy(size_t start, size_t count) 
{
	// clamp the range
	size_t suppression_count = deletion_vector.size();
	if (start >= suppression_count)
		return;
	size_t end = min(start + count, suppression_count);
	// mark the particles to destroy    
	while (start != end)
		deletion_vector[start++] = DESTROY_PARTICLE_MARK;
	// count the number of particles to destroy
	pending_kill_particles += count;
}

void ParticleLayer::RemoveParticleAllocation(ParticleRangeAllocation * allocation)
{
	assert(allocation != nullptr);

	// mark the range to be destroyed
	if (allocation->particles_owner)
	{
		ParticleRange range = allocation->GetParticleRange();
		MarkParticlesToDestroy(range.start, range.count);
	}
	// displace range and allocation
	size_t last_index  = range_allocations.size() - 1;
  size_t range_index = allocation->range_index;
	if (allocation->range_index < last_index)
	{
		range_allocations[range_index] = range_allocations[last_index]; // replace the allocation
		particles_ranges[range_index]  = particles_ranges[last_index];	
    range_allocations[range_index]->range_index = range_index;
	}
	range_allocations.pop_back();
	particles_ranges.pop_back();

	// reset the object
	*allocation = ParticleRangeAllocation();
}

void ParticleLayer::UpdateParticleRanges(size_t new_particle_count)
{
	// update the ranges (code is useless from one TICK to the next. the only important value is NUMERIC LIMIT)
	size_t range_count = particles_ranges.size();
	for (size_t i = 0; i < range_count; ++i)
	{
		ParticleRange & range = particles_ranges[i];
		if (range.count == 0)
			continue;
		// read the range
		size_t start = range.start;
		size_t end   = start + range.count - 1;
		// apply the suppression count
		start -= deletion_vector[start];
		end   -= deletion_vector[end];
		// update the structure
		range.start = start;
		range.count = end - start + 1;
	}
	// resize some vectors
	particles.resize(new_particle_count * particle_size);
	deletion_vector.resize(new_particle_count);
	// reset the suppression vector
	for (size_t i = 0; i < new_particle_count; ++i)
		deletion_vector[i] = 0;
}


// ==============================================================
// PARTICLE MANAGER
// ==============================================================

void ParticleManager::SetTextureAtlas(chaos::BitmapAtlas::TextureArrayAtlas * in_atlas)
{
	atlas = in_atlas;
}

void ParticleManager::Display(chaos::GPUProgramVariableProvider * uniform_provider)
{

}

















#if 0
GLuint framebuffer = 0;
glGenFramebuffers(1, &framebuffer);

GLuint depth_renderbuffer = 0;
glGenRenderbuffers(1, &depth_renderbuffer);

glNamedRenderbufferStorage(depth_renderbuffer, GL_DEPTH24_STENCIL8, 512, 512);

GLuint color_renderbuffer = 0;
glGenRenderbuffers(1, &color_renderbuffer);
glNamedRenderbufferStorage(color_renderbuffer, GL_RGBA, 512, 512);

glNamedFramebufferRenderbuffer(framebuffer, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, color_renderbuffer);


glNamedFramebufferTexture(framebuffer, GL_COLOR_ATTACHMENT0, texture_id, texture_level);

glCheckNamedFramebufferStatus(framebuffer, GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE;

#endif
