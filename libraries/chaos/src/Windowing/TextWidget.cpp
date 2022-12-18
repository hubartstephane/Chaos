#include "chaos/ChaosPCH.h"
#include "chaos/ChaosInternals.h"

namespace chaos
{
	bool TextWidget::OnDraw(GPURenderer* renderer, GPUProgramProviderInterface const* uniform_provider, WindowDrawParams const& draw_params)
	{
		
		if (mesh != nullptr)
		{
			//mesh->Display(renderer, uniform_provider)
		}







		return true;
	}

	void TextWidget::Initialize(char const * in_text)
	{
		if (StringTools::IsEmpty(in_text))
			mesh = nullptr;
		else
		{
			ParticleTextGenerator::GeneratorParams other_params;

			GPUDrawInterface<VertexDefault> DI(nullptr);
			DrawText(DI, in_text, other_params, {}, &bounding_box);
			mesh = DI.GetDynamicMesh(mesh.get());
		}
	}

}; // namespace chaos
