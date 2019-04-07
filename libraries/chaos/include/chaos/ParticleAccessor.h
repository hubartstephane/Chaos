#pragma once

#include <chaos/StandardHeaders.h>

namespace chaos
{

		// ==============================================================
		// ParticleAccessorBase
		// ==============================================================

	template<typename PARTICLE_TYPE, typename BUFFER_TYPE>
	class ParticleAccessorBase
	{
	public:

		/** default constructor */
		ParticleAccessorBase(){}

		/** constructor */
		ParticleAccessorBase(BUFFER_TYPE * in_buffer, size_t in_count, size_t in_particle_size) :
			buffer(in_buffer), count(in_count), particle_size(in_particle_size)
		{
			assert((count > 0) ^ (buffer == nullptr));				
		}
		/** pointer indirection */
		PARTICLE_TYPE & operator * () const
		{
			assert(count > 0);
			return *((PARTICLE_TYPE*)buffer);
		}
		/** pointer inference */
		PARTICLE_TYPE * operator -> () const
		{
			if (count == 0)
				return nullptr;
			return ((PARTICLE_TYPE*)buffer);
		}
		/** array accessor */
		PARTICLE_TYPE & operator [](size_t index) const
		{
			assert(index < count);
			char * b = (char*)buffer;
			return *((PARTICLE_TYPE*)(b + index * particle_size));
		}

		/** get the number of particle */
		size_t GetCount() const 
		{
			return count;
		}
		/** get the particle size */
		size_t GetParticleSize() const
		{
			return particle_size;
		}

	protected:

		/** the start of the buffer */
		BUFFER_TYPE * buffer = nullptr;
		/** the number of particles in that buffer */
		size_t count = 0;
		/** the real particle size (not PARTICLE_TYPE) */
		size_t particle_size = 0;
	};

	// ==============================================================
	// ParticleAccessor / ParticleConstAccessor
	// ==============================================================

	template<typename PARTICLE_TYPE>
	class ParticleConstAccessor : public ParticleAccessorBase<PARTICLE_TYPE const, void const>
	{
	public:

		/** default constructor */
		ParticleConstAccessor() {}
		/** constructor */
		ParticleConstAccessor(void const * in_buffer, size_t in_count, size_t in_particle_size) :
			ParticleAccessorBase<PARTICLE_TYPE const, void const>(in_buffer, in_count, in_particle_size)
		{
		}
	};

	template<typename PARTICLE_TYPE>
	class ParticleAccessor : public ParticleAccessorBase<PARTICLE_TYPE, void>
	{
	public:

		/** default constructor */
		ParticleAccessor() {}
		/** constructor */
		ParticleAccessor(void * in_buffer, size_t in_count, size_t in_particle_size) :
			ParticleAccessorBase<PARTICLE_TYPE, void>(in_buffer, in_count, in_particle_size)
		{
		}
		/** convert into a const accessor */
		operator ParticleConstAccessor<PARTICLE_TYPE>() const
		{
			return ParticleConstAccessor<PARTICLE_TYPE>(buffer, count, particle_size);
		}
	};
}; // namespace chaos

