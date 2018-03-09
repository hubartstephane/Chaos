#include "SoundTest.h"

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
// CALLBACKS
// ==============================================================

ParticleLayerBase::~ParticleLayerBase()
{
	DetachAllParticleAllocations();
}

void ParticleLayerBase::DetachAllParticleAllocations()
{
	while (range_allocations.size())
		RemoveParticleAllocation(range_allocations[range_allocations.size() - 1]);
}

size_t ParticleLayerBase::GetParticleCount() const
{
	if (particle_size == 0)
		return 0;
	return particles.size() / particle_size;
}

size_t ParticleLayerBase::GetParticleCount(ParticleRange range) const
{
	if (particle_size == 0)
		return 0;
	return range.count / particle_size;
}

void * ParticleLayerBase::GetParticleBuffer(ParticleRange range)
{
	if (particle_size == 0)
		return nullptr;
	return &particles[range.start * particle_size];
}

void const * ParticleLayerBase::GetParticleBuffer(ParticleRange range) const
{
	if (particle_size == 0)
		return nullptr;
	return &particles[range.start * particle_size];
}

void ParticleLayerBase::TickParticles(float delta_time)
{	
	// update the particles themselves
	UpdateParticles(delta_time);
	// destroy the particles that are to be destroyed
	DestroyObsoletParticles();
	// finalization of the particle destruction
	UpdateParticleRanges();
}

ParticleRangeAllocation * ParticleLayerBase::SpawnParticlesAndKeepRange(size_t count, bool particles_owner)
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

ParticleRange ParticleLayerBase::SpawnParticles(size_t count)
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
		suppression_vector.resize(new_count, 0);	
	}
	return result;
}

void ParticleLayerBase::MarkParticlesToDestroy(size_t start, size_t count) 
{
	// clamp the range
	size_t suppression_count = suppression_vector.size();
	if (start >= suppression_count)
		return;
	size_t end = min(start + count, suppression_count);
	// mark the particles to destroy    
	while (start != end)
		suppression_vector[start++] = DESTROY_PARTICLE_MARK; 
}

void ParticleLayerBase::RemoveParticleAllocation(ParticleRangeAllocation * allocation)
{
	assert(allocation != nullptr);

	// mark the range to be destroyed
	if (allocation->particles_owner)
	{
		ParticleRange range = allocation->GetParticleRange();
		MarkParticlesToDestroy(range.start, range.count);
	}
	// displace range and allocation
	size_t last_index = range_allocations.size() - 1;
	if (allocation->range_index < last_index)
	{
		range_allocations[allocation->range_index] = range_allocations[last_index]; // replace the allocation
		particles_ranges[allocation->range_index]  = particles_ranges[last_index];	
	}
	range_allocations.pop_back();
	particles_ranges.pop_back();

	// reset the object
	*allocation = ParticleRangeAllocation();
}

void ParticleLayerBase::UpdateParticles(float delta_time)
{

}

void ParticleLayerBase::DestroyObsoletParticles()
{

}

void ParticleLayerBase::UpdateParticleRanges()
{
	// update the ranges (code is useless from one TICK to the next. the only important value is NUMERIC LIMIT)
	size_t range_count = particles_ranges.size();
	for (size_t i = 0; i < range_count; ++i)
	{
		// read the range
		size_t start = particles_ranges[i].start;
		size_t end   = start + particles_ranges[i].count;
		// apply the suppression count
		start -= suppression_vector[start]; 
		end   -= suppression_vector[end];
		// update the structure
		particles_ranges[i].start = start; 
		particles_ranges[i].count = end - start;
	}
	// reset the suppression vector
	size_t suppression_count = suppression_vector.size();
	for (size_t i = 0; i < suppression_count; ++i)
		suppression_vector[i] = 0;
}




// ==============================================================
// CALLBACKS
// ==============================================================

