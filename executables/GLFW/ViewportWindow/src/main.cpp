
#include "chaos/Chaos.h"

uint64_t render_stamp = 0;

chaos::shared_ptr<chaos::GPUFence> render_fence;

class WindowOpenGLTest : public chaos::ViewportWindow
{
	CHAOS_DECLARE_OBJECT_CLASS(WindowOpenGLTest, chaos::ViewportWindow);

protected:

	virtual bool OnInitialize(chaos::JSONReadConfiguration config) override
	{
		if (!chaos::ViewportWindow::OnInitialize(config))
			return false;

		if (chaos::ViewportGridLayout* layout = new chaos::ViewportGridLayout)
		{
			layout->SetMaxViewportCount(6, false);
			//layout->SetHorizontalFillMode(chaos::ViewportGridHorizontalFillMode::RIGHT_TO_LEFT, false);
			//layout->SetHorizontalFillMode(chaos::ViewportGridHorizontalFillMode::LEFT_TO_RIGHT, false);
			//layout->SetVerticalFillMode(chaos::ViewportGridVerticalFillMode::TOP_TO_BOTTOM, false);
			layout->SetVerticalFillMode(chaos::ViewportGridVerticalFillMode::BOTTOM_TO_TOP, false);
			//layout->SetOrientation(chaos::Orientation::VERTICAL);

			layout->SetMode(chaos::ViewportGridMode::EXPANDED);
			//layout->SetMode(chaos::ViewportGridMode::UNIFORM_PACKED);
			//layout->SetMode(chaos::ViewportGridMode::UNIFORM_CENTERED);

			SetViewportLayout(layout);

			for (int i = 0 ; i < 9; ++i)
				AddViewport(new chaos::Viewport, false);

			layout->UpdateWindowViewportPlacements();
		}
		return true;
	}

	virtual bool EnumerateInputActions(EnumerateKeyActionFunc in_action_enumerator) override
	{
		chaos::ViewportGridLayout* layout = GetViewportLayout();

		if (in_action_enumerator.CheckAndProcess(RequestKeyPressed(chaos::Key::E), "ViewportGridMode EXPANDED", [this, layout]()
		{
			layout->SetMode(chaos::ViewportGridMode::EXPANDED);
		}))
		{
			return true;
		}

		if (in_action_enumerator.CheckAndProcess(RequestKeyPressed(chaos::Key::P), "ViewportGridMode UNIFORM_PACKED", [this, layout]()
		{
			layout->SetMode(chaos::ViewportGridMode::UNIFORM_PACKED);
		}))
		{
			return true;
		}

		if (in_action_enumerator.CheckAndProcess(RequestKeyPressed(chaos::Key::C), "ViewportGridMode UNIFORM_CENTERED", [this, layout]()
		{
			layout->SetMode(chaos::ViewportGridMode::UNIFORM_CENTERED);
		}))
		{
			return true;
		}

		return chaos::Window::EnumerateInputActions(in_action_enumerator, in_context);
	}




	virtual bool OnKeyEventImpl(chaos::KeyEvent const& key_event) override
	{
		chaos::ViewportGridLayout* layout = GetViewportLayout();

		if (key_event.key == (int)chaos::Key::E)
			layout->SetMode(chaos::ViewportGridMode::EXPANDED);
		if (key_event.key == (int)chaos::Key::P)
			layout->SetMode(chaos::ViewportGridMode::UNIFORM_PACKED);
		if (key_event.key == (int)chaos::Key::C)
			layout->SetMode(chaos::ViewportGridMode::UNIFORM_CENTERED);

		if (key_event.key == (int)chaos::Key::V)
			layout->SetOrientation(chaos::Orientation::VERTICAL);
		if (key_event.key == (int)chaos::Key::H)
			layout->SetOrientation(chaos::Orientation::HORIZONTAL);

		if (key_event.key == (int)chaos::Key::LEFT)
			layout->SetHorizontalFillMode(chaos::ViewportGridHorizontalFillMode::RIGHT_TO_LEFT);

		if (key_event.key == (int)chaos::Key::RIGHT)
			layout->SetHorizontalFillMode(chaos::ViewportGridHorizontalFillMode::LEFT_TO_RIGHT);

		if (key_event.key == (int)chaos::Key::UP)
			layout->SetVerticalFillMode(chaos::ViewportGridVerticalFillMode::BOTTOM_TO_TOP);

		if (key_event.key == (int)chaos::Key::DOWN)
			layout->SetVerticalFillMode(chaos::ViewportGridVerticalFillMode::TOP_TO_BOTTOM);

		if (key_event.key == (int)chaos::Key::KP_1)
			layout->SetMaxViewportCount(0);

		if (key_event.key == (int)chaos::Key::KP_2)
			layout->SetMaxViewportCount(7);

		if (key_event.key == (int)chaos::Key::KP_3)
			layout->SetMaxViewportCount(11);

		return chaos::ViewportWindow::OnKeyEventImpl(key_event);
	}
};

int main(int argc, char ** argv, char ** env)
{
	return chaos::RunSimpleWindowApplication<WindowOpenGLTest>(argc, argv, env);
}