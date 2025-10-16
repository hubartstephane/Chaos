#include "chaos/Chaos.h"

class WindowOpenGLTest : public chaos::Window
{
	CHAOS_DECLARE_OBJECT_CLASS(WindowOpenGLTest, chaos::Window);

protected:


	void DrawGraphic(char const * title, chaos::LightweightFunction<float(float)> func)
	{
		constexpr size_t VALUE_COUNT = 256;

		float x[VALUE_COUNT];
		float y[VALUE_COUNT];

		for (size_t i = 0; i < VALUE_COUNT; ++i)
		{
			x[i] = float(i) / float(VALUE_COUNT - 1);
			y[i] = func(x[i]);
		}

		ImPlot::PlotLine(title, x, y, VALUE_COUNT);
	}

	void OnDrawImGuiContent() override
	{
		chaos::Window::OnDrawImGuiContent();

		chaos::ImGuiTools::FullViewportWindow("fullscreen", 0, [this]()
		{
			int plot_style =
				ImPlotFlags_NoMenus |
				ImPlotFlags_NoTitle |
				//ImPlotFlags_NoLegend |
				ImPlotFlags_NoInputs |
				//ImPlotFlags_NoMouseText |
				ImPlotFlags_NoBoxSelect;

			if (ImPlot::BeginPlot("function", ImVec2(-1, -1), plot_style))
			{
				ImPlot::PushStyleVar(ImPlotStyleVar_FillAlpha, 0.25f);

				int axis_x_style =
					ImPlotAxisFlags_NoLabel |
					ImPlotAxisFlags_NoMenus;

				int axis_y_style =
					ImPlotAxisFlags_NoLabel |
					ImPlotAxisFlags_NoMenus;

				ImPlot::SetupAxes("x", "F(x)", axis_x_style, axis_y_style);
				ImPlot::SetupAxisLimits(ImAxis_X1,  0.0f, 1.0f, ImPlotCond_Always);
				ImPlot::SetupAxisLimits(ImAxis_Y1, -1.2f, 1.2f, ImPlotCond_Always);

				ImPlot::PushColormap(ImPlotColormap_Deep);

				DrawGraphic("linear", [](float x)
				{
					return x;
				});

				DrawGraphic("sinus", [](float x)
				{
					return std::sin(x * 2.0f * float(M_PI));
				});

				ImPlot::PopColormap();
					ImPlot::PopStyleVar();
					ImPlot::EndPlot();
			}
		});
	}
};

int main(int argc, char** argv, char** env)
{
	return chaos::RunSimpleWindowApplication<WindowOpenGLTest>(argc, argv, env);
}