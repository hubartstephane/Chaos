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
}




ParticleRange ParticleLayerBase::SpawnParticles(size_t count, boost::intrusive_ptr<ParticleRangeAllocation> * allocation)
{


	ParticleRange result;
	if (count > 0 && particle_size.size() > 0)
	{
		result = DoSpawnParticles(count);	
		if (allocation != nullptr)
		{
			ParticleRangeAllocation * ra = new ParticleRangeAllocation;
			if (ra != nullptr)
			{
				ra->layer = this;
				ra->range_index = particles_ranges.size();
				range_allocations.push_back(ra);

				particles_ranges.push_back(result);		
			
				//
				*allocation = ra;
			}
		}
	}
	return result;  
}


ParticleRange ParticleLayerBase::DoSpawnParticles(size_t count)
{
	size_t current_particle_count = particles.size();

	// initialize the result
	ParticleRange result;
	result.start = current_particle_count;
	result.count = count;
	// create the particles and the suppression corresponding data
	size_t new_count = current_particle_count + count;
	particles.resize(new_count, particle_type());
	suppression_vector.resize(new_count, 0);

	return result;
}

ParticleRange ParticleLayerBase::DoSpawnParticles(size_t count)
{
	return ParticleRange();
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



	allocation->layer = nullptr;
	allocation->range_index = std::numeric_limits<size_t>::max();
}

// ==============================================================
// CALLBACKS
// ==============================================================