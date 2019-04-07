#pragma once

#include <chaos/StandardHeaders.h>
#include <chaos/ReferencedObject.h>
#include <chaos/ParticleLayer.h>
#include <chaos/GPURenderMaterial.h>
#include <chaos/TextureArrayAtlas.h>
#include <chaos/GPUProgramProvider.h>
#include <chaos/Renderable.h>
#include <chaos/Tickable.h>
#include <chaos/RenderableLayerSystem.h>
#include <chaos/ParticleTools.h>

namespace chaos
{

	// ==============================================================
	// ParticleManager
	// ==============================================================

	class ParticleManager : public Renderable
	{
		CHAOS_PARTICLE_ALL_FRIENDS

	public:

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
		template<typename ALLOCATION_TYPE, typename ...PARAMS>
		ParticleLayerBase * AddLayer(int render_order, TagType layer_id, char const * material_name, PARAMS... params)
		{






			ParticleLayerBase * result = ParticleLayer::CreateParticleLayer<ALLOCATION_TYPE>(material_name, params...);
			if (result == nullptr)
				return nullptr;
			DoAddLayer(result, render_order, layer_id);
			return result;
		}

		/** templated method to add a layer and set some values */
		template<typename ALLOCATION_TYPE, typename ...PARAMS>
		ParticleLayer * AddLayer(int render_order, TagType layer_id, GPURenderMaterial * render_material, PARAMS... params)
		{
			ParticleLayerBase * result = ParticleLayer::CreateParticleLayer<ALLOCATION_TYPE>(render_material, params...);
			if (result == nullptr)
				return nullptr;
			DoAddLayer(result, render_order, layer_id);
			return result;
		}

		/** remove a layer from the manager */
		void RemoveLayer(ParticleLayerBase * layer);

	protected:
		
		/** tick the manager */
		virtual bool DoTick(double delta_time) override;
		/** display all the particles */
		virtual int DoDisplay(Renderer * renderer, GPUProgramProviderBase const * uniform_provider, RenderParams const & render_params) const override;

		/** insert layer with some initialization */
		void DoAddLayer(ParticleLayerBase * layer, int render_order, TagType layer_id);

	protected:

		/** the layer system */
		RenderableLayerSystem layer_system;
		/** the texture atlas */
		shared_ptr<BitmapAtlas::TextureArrayAtlas> atlas;
	};

}; // namespace chaos

