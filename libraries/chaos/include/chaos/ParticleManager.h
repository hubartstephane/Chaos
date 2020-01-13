#pragma once

#include <chaos/StandardHeaders.h>
#include <chaos/ReferencedObject.h>
#include <chaos/ParticleLayer.h>
#include <chaos/GPUClasses.h>
#include <chaos/GPURenderMaterial.h>
#include <chaos/TextureArrayAtlas.h>
#include <chaos/GPUProgramProvider.h>
#include <chaos/GPURenderable.h>
#include <chaos/GPUBuffer.h>
#include <chaos/Tickable.h>
#include <chaos/GPURenderableLayerSystem.h>
#include <chaos/ParticleTools.h>
#include <chaos/GPUBufferCache.h>

namespace chaos
{

	// ==============================================================
	// ParticleManager
	// ==============================================================

	class ParticleManager : public GPURenderable
	{
		CHAOS_PARTICLE_ALL_FRIENDS

	public:

        /** constructor */
        ParticleManager();

		/** change the bitmap atlas */
		void SetTextureAtlas(BitmapAtlas::TextureArrayAtlas * in_atlas);

		/** Search a layer by its name */
		ParticleLayerBase * FindLayer(char const * name);
		/** Search a layer by its name */
		ParticleLayerBase const * FindLayer(char const * name) const;
		/** Search a layer by its id */
		ParticleLayerBase * FindLayer(TagType id);
		/** Search a layer by its id */
		ParticleLayerBase const * FindLayer(TagType id) const;

		/** templated method to add a layer and set some values */
		template<typename ALLOCATION_TRAIT, typename ...PARAMS>
		ParticleLayerBase * AddLayer(int render_order, TagType layer_id, char const * material_name, PARAMS... params)
		{
			// find the optional GPURenderMaterial
			GPURenderMaterial * render_material = nullptr;
			if (material_name != nullptr)
			{
				GPUResourceManager * resource_manager = MyGLFW::SingleWindowApplication::GetGPUResourceManagerInstance();
				if (resource_manager == nullptr)
					return nullptr;
				render_material = resource_manager->FindRenderMaterial(material_name);
				if (render_material == nullptr)
					return nullptr;
			}
			// create the layer
			return AddLayer<ALLOCATION_TRAIT>(render_order, layer_id, render_material, params...);
		}

		/** templated method to add a layer and set some values */
		template<typename ALLOCATION_TRAIT, typename ...PARAMS>
		ParticleLayerBase * AddLayer(int render_order, TagType layer_id, GPURenderMaterial * render_material, PARAMS... params)
		{
			ParticleLayerBase * result = new ParticleLayer<ALLOCATION_TRAIT>(params...);
			if (result == nullptr)
				return nullptr;
			result->SetRenderMaterial(render_material);
			DoAddLayer(result, render_order, layer_id);			
			return result;
		}

		/** remove a layer from the manager */
		void RemoveLayer(ParticleLayerBase * layer);

        /** gets the buffer cache */
        GPUBufferCache& GetBufferCache() { return buffer_cache; }
        /** gets the buffer cache */
        GPUBufferCache const& GetBufferCache() const { return buffer_cache; }
        /** gets the vertex array cache */
        GPUVertexArrayCache* GetVertexArrayCache() { return vertex_array_cache.get();}
        /** gets the vertex array cache */
        GPUVertexArrayCache const * GetVertexArrayCache() const { return vertex_array_cache.get(); }

	protected:
		
		/** tick the manager */
		virtual bool DoTick(double delta_time) override;
		/** display all the particles */
		virtual int DoDisplay(GPURenderer * renderer, GPUProgramProviderBase const * uniform_provider, GPURenderParams const & render_params) override;

		/** insert layer with some initialization */
		void DoAddLayer(ParticleLayerBase * layer, int render_order, TagType layer_id);

	protected:

		/** the layer system */
		GPURenderableLayerSystem layer_system;
		/** the texture atlas */
		shared_ptr<BitmapAtlas::TextureArrayAtlas> atlas;
        /** the vertex array cache */
        shared_ptr<GPUVertexArrayCache> vertex_array_cache;
        /** a cache for GPUBuffer */
        GPUBufferCache buffer_cache;
	};

}; // namespace chaos

