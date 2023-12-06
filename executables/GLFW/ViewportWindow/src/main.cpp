
#include "chaos/Chaos.h"

uint64_t render_stamp = 0;

chaos::shared_ptr<chaos::GPUFence> render_fence;

class WindowOpenGLTest : public chaos::ViewportWindow
{
	CHAOS_DECLARE_OBJECT_CLASS(WindowOpenGLTest, chaos::ViewportWindow);

protected:

	virtual bool InitializeFromConfiguration(nlohmann::json const * config) override
	{
		if (!chaos::ViewportWindow::InitializeFromConfiguration(config))
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

	virtual bool OnKeyEventImpl(chaos::KeyEvent const& event) override
	{
		chaos::ViewportGridLayout* layout = GetViewportLayout();

		if (event.key == (int)chaos::KeyboardButton::E)
			layout->SetMode(chaos::ViewportGridMode::EXPANDED);
		if (event.key == (int)chaos::KeyboardButton::P)
			layout->SetMode(chaos::ViewportGridMode::UNIFORM_PACKED);
		if (event.key == (int)chaos::KeyboardButton::C)
			layout->SetMode(chaos::ViewportGridMode::UNIFORM_CENTERED);

		if (event.key == (int)chaos::KeyboardButton::V)
			layout->SetOrientation(chaos::Orientation::VERTICAL);
		if (event.key == (int)chaos::KeyboardButton::H)
			layout->SetOrientation(chaos::Orientation::HORIZONTAL);

		if (event.key == (int)chaos::KeyboardButton::LEFT)
			layout->SetHorizontalFillMode(chaos::ViewportGridHorizontalFillMode::RIGHT_TO_LEFT);

		if (event.key == (int)chaos::KeyboardButton::RIGHT)
			layout->SetHorizontalFillMode(chaos::ViewportGridHorizontalFillMode::LEFT_TO_RIGHT);

		if (event.key == (int)chaos::KeyboardButton::UP)
			layout->SetVerticalFillMode(chaos::ViewportGridVerticalFillMode::BOTTOM_TO_TOP);

		if (event.key == (int)chaos::KeyboardButton::DOWN)
			layout->SetVerticalFillMode(chaos::ViewportGridVerticalFillMode::TOP_TO_BOTTOM);

		if (event.key == (int)chaos::KeyboardButton::KP_1)
			layout->SetMaxViewportCount(0);

		if (event.key == (int)chaos::KeyboardButton::KP_2)
			layout->SetMaxViewportCount(7);

		if (event.key == (int)chaos::KeyboardButton::KP_3)
			layout->SetMaxViewportCount(11);

		return true;
	}
};

int main(int argc, char ** argv, char ** env)
{
	chaos::WindowPlacementInfo placement_info;
	placement_info.size = { 300, 300 };

	return chaos::RunWindowApplication<WindowOpenGLTest>(argc, argv, env, placement_info);
}