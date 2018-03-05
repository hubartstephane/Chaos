#pragma once

#include <chaos/ReferencedObject.h>
#include <chaos/RenderMaterial.h>

// -----------------------------------------------------------------

class SpawnParticleResult
{

};

class SpriteLayerBase
{
public:

  /** ticking the particle system */
  virtual void TickParticles(float delta_time) {}

  //virtual SpawnParticleResult SpawnParticles(size_t count);

protected:

  /** the order of the layer in the manager */
  int order = 0;
  /** the material used to render the layer */
  boost::intrusive_ptr<chaos::RenderMaterial> render_material;
};

// -----------------------------------------------------------------

template<typename PARTICLE_DESC_TYPE>
class SpriteLayer : public SpriteLayerBase
{
public:

  using particle_desc_type = PARTICLE_DESC_TYPE;
  using particle_type = typename PARTICLE_DESC_TYPE::particle_type;
  using vertex_type = typename PARTICLE_DESC_TYPE::vertex_type;

  /** constructor */
  SpriteLayer(particle_desc_type in_desc = particle_desc_type()) : 
    particle_desc(in_desc) 
  {
  }

  /** the ticking method */
  virtual void TickParticles(float delta_time)
  {
    // update the particles themselves
    DoUpdateParticles(delta_time);
    // destroy the particles that are to be destroyed
    DoDestroyParticles(delta_time);




  }

protected:

  void DoUpdateParticles(float delta_time)
  {
    size_t count = particles.size();
    if (count == 0)
      return;

    particle_type * p = &particles[0];
    for (size_t i = 0; i < count; ++i)
      particle_desc.UpdateParticle(delta_time, p + i);
  }

  void DoDestroyParticles(float delta_time)
  {
    size_t count = particles.size();
    if (count == 0)
      return;

    particle_type * p = &particles[0];

    size_t i = 0;
    size_t j = 0;
    while (i < count)
    {
      if (!particle_desc.MustDestroyParticle(p + i))
      {
        if (i != j)
          p[j] = p[i];
        ++j;
      }
      ++i;
    }
    particles.resize(j);
  }

protected:

  /** the particle behavior description */
  particle_desc_type particle_desc;
  /** the array containing the particles */
  std::vector<particle_type> particles;
};

// -----------------------------------------------------------------

template<typename PARTICLE_TYPE, typename VERTEX_TYPE>
class ParticleLayerDescBase
{
public:

  using particle_type = typename PARTICLE_TYPE;
  using vertex_type = typename VERTEX_TYPE;

  bool MustDestroyParticle(particle_type const * p)
  {
    return false;
  }

  void UpdateParticle(float delta_time, particle_type * p)
  {

  }
};

class ParticleExample
{

};

class VertexExample
{

};

class ParticleLayerDescExample : public ParticleLayerDescBase<ParticleExample, VertexExample>
{


};

// -----------------------------------------------------------------

class SpriteManager : public chaos::ReferencedObject
{


};