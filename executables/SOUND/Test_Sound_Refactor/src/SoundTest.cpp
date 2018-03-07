#include "SoundTest.h"


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







void ParticleLayerBase::TickParticles(float delta_time)
{
}

ParticleRange ParticleLayerBase::SpawnParticles(size_t count, boost::intrusive_ptr<ParticleRangeAllocation> * allocation)
{
	ParticleRange result;
	if (count > 0)
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
			
				*allocation = ra;
			}
		}
	}
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




}