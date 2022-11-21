#include "chaos/ChaosPCH.h"
#include "chaos/ChaosInternals.h"

#if 0

namespace chaos
{

	size_t ViewportWindow::GetViewportCount() const
	{
		return viewports.size();
	}

	AutoCastable<Viewport> ViewportWindow::GetViewport(size_t index)
	{
		return viewports[index].get();
	}

	AutoConstCastable<Viewport> ViewportWindow::GetViewport(size_t index) const
	{
		return viewports[index].get();
	}

	AutoCastable<Viewport> ViewportWindow::FindViewport(ObjectRequest request)
	{
		return request.FindObject(viewports);
	}

	AutoConstCastable<Viewport> ViewportWindow::FindViewport(ObjectRequest request) const
	{
		return request.FindObject(viewports);
	}

	AutoCastable<ViewportLayout> ViewportWindow::GetViewportLayout()
	{
		return viewport_layout.get();
	}

	AutoConstCastable<ViewportLayout> ViewportWindow::GetViewportLayout() const
	{
		return viewport_layout.get();
	}

	void ViewportWindow::OnWindowResize(glm::ivec2 size)
	{
		ComputeViewportPlacements(size);
	}

	void ViewportWindow::AddViewport(Viewport* viewport, bool compute_viewport_surfaces)
	{
		assert(viewport != nullptr);
		assert(viewport->window == nullptr);

		viewport->window = this;
		viewports.push_back(viewport);

		if (compute_viewport_surfaces)
			ComputeViewportPlacements(GetWindowSize());
	}

	void ViewportWindow::RemoveViewport(Viewport* viewport, bool compute_viewport_surfaces, bool keep_empty_place)
	{
		assert(viewport != nullptr);
		assert(viewport->window == this);

		viewport->window = nullptr;

		auto it = std::find(viewports.begin(), viewports.end(), viewport);
		if (keep_empty_place) // remove the element, but relace it with a empty zone
			*it = nullptr;
		else
			viewports.erase(it);

		if (compute_viewport_surfaces)
			ComputeViewportPlacements(GetWindowSize());
	}

	void ViewportWindow::SetViewportLayout(ViewportLayout* in_viewport_layout)
	{
		if (viewport_layout != nullptr)
			viewport_layout->window = nullptr;
		viewport_layout = in_viewport_layout;
		if (viewport_layout != nullptr)
			viewport_layout->window = this;
		ComputeViewportPlacements(GetWindowSize());
	}

	void ViewportWindow::ComputeViewportPlacements(glm::ivec2 size)
	{
		if (viewport_layout != nullptr)
		{
			size_t count = viewports.size();
			for (size_t i = 0; i < count; ++i)
				if (Viewport* viewport = viewports[i].get())
					viewport->SetViewportPlacement(viewport_layout->ComputeViewportPlacement(viewport, size, i, count));
		}
	}

	bool ViewportWindow::OnDraw(GPURenderer* renderer, WindowDrawParams const& draw_params, GPUProgramProviderInterface const * uniform_provider)
	{
		// clear the color buffers
		glm::vec4 clear_color = glm::vec4(0.0f, 1.0f, 0.0f, 0.0f);
		glClearBufferfv(GL_COLOR, 0, (GLfloat*)&clear_color);

		// clear the depth buffers
		float far_plane = 1000.0f;
		glClearBufferfi(GL_DEPTH_STENCIL, 0, far_plane, 0);
		glEnable(GL_DEPTH_TEST);
		glDisable(GL_CULL_FACE);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glDisable(GL_DEPTH_TEST);
		glDisable(GL_CULL_FACE);



		GPUDrawInterface<VertexDefault> DI(nullptr);

		box2 view = box2(std::make_pair(glm::vec2(0.0f, 0.0f), glm::vec2(100.0f , 100.0f)));

		GPUProgramProvider provider;
		provider.AddVariable("canvas_box", EncodeBoxToVector(view), GPUProgramProviderPassType::EXPLICIT);


		size_t index = 0;
		for (auto& viewport : viewports)
		{
			++index;
			if (viewport != nullptr)
			{
				ViewportPlacement const & placement = viewport->GetViewportPlacement();


				glViewport(placement.position.x, placement.position.y, placement.size.x, placement.size.y);

				glm::vec4 color(float(index) / float(viewports.size()), 0.0f, 0.0f, 0.5f);

				auto quad = DI.AddQuads();
				quad[0].color = color;
				quad[1].color = color;
				quad[2].color = color;
				quad[3].color = color;
				quad[0].position = { -50.0f,-50.0f };
				quad[1].position = { -50.0f, 50.0f };
				quad[2].position = {  50.0f, 50.0f };
				quad[3].position = {  50.0f,-50.0f };
				quad[0].flags = 0;
				quad[1].flags = 0;
				quad[2].flags = 0;
				quad[3].flags = 0;
				quad[0].texcoord = { 0.0f, 0.0f, -1.0f };
				quad[1].texcoord = { 0.0f, 0.0f, -1.0f };
				quad[2].texcoord = { 0.0f, 0.0f, -1.0f };
				quad[3].texcoord = { 0.0f, 0.0f, -1.0f };


				GPURenderParams render_params;
			//	render_params.viewport = viewport;
				//render_params.screen_size = window_size;
				DI.Display(renderer, &provider, render_params);
#if 0
				renderer->DrawFullscreenQuad()

				glScissor(placement.position.x, placement.position.y, placement.size.x, placement.size.y);
				glViewport(placement.position.x, placement.position.y, placement.size.x, placement.size.y);
				glClearBufferfv(GL_COLOR, 0, (GLfloat*)&color);
#endif




			}
		}




		return true;
	}

}; // namespace chaos


#endif