#include "chaos/Chaos.h"

class WindowOpenGLTest : public chaos::Window
{
	CHAOS_DECLARE_OBJECT_CLASS(WindowOpenGLTest, chaos::Window);

protected:


	void DrawGraphic(char const * title, char const * tip, chaos::InterpolationType interpolation_type)
	{
		constexpr size_t VALUE_COUNT = 256;

		float x[VALUE_COUNT];
		float y[VALUE_COUNT];

		for (size_t i = 0; i < VALUE_COUNT; ++i)
		{
			x[i] = float(i) / float(VALUE_COUNT - 1);
			y[i] = chaos::Interpolate(interpolation_type, x[i], 0.0f, 1.0f);
		}

		ImPlot::PlotLine(title, x, y, VALUE_COUNT);

		if (ImPlot::IsLegendEntryHovered(title))
			ImGui::SetTooltip(tip);
	}

	void OnDrawImGuiContent() override
	{
		chaos::Window::OnDrawImGuiContent();

		chaos::ImGuiTools::FullViewportWindow("fullscreen", 0, [this]()
		{
			int plot_style =
				ImPlotFlags_NoMenus |
				ImPlotFlags_NoTitle |
				ImPlotFlags_NoInputs |
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
				ImPlot::SetupAxisTicks(ImAxis_X1, 0.0, 1.0f, 11);

				ImPlot::SetupAxisLimits(ImAxis_Y1, 0.0f, 1.0f, ImPlotCond_Always);
				ImPlot::SetupAxisTicks(ImAxis_Y1, 0.0f, 1.0f, 11);

				ImPlot::PushColormap(ImPlotColormap_Deep);

				DrawGraphic("Sin", "f(x) = sin(x.pi/2) / sin(pi/2)", chaos::InterpolationType::Sin);

				DrawGraphic("Linear", "f(x) = x", chaos::InterpolationType::Linear);

				DrawGraphic("Ease", "f(x) = (3.x^2) - (2.x^3)", chaos::InterpolationType::Ease);

				DrawGraphic("Easier", "f(x) = (6.x^5) - (15.x^4) + (10.x^3)", chaos::InterpolationType::Easier);

				DrawGraphic("X^2", "f(x) = x^2", chaos::InterpolationType::Pow2);

				DrawGraphic("X^3", "f(x) = x^3", chaos::InterpolationType::Pow3);

				DrawGraphic("X^5", "f(x) = x^5", chaos::InterpolationType::Pow5);

				DrawGraphic("Exp(x, k=3, p=5)", "f(x) = (e^(k.x^p) - 1) / (e^k - 1)", chaos::InterpolationType::Exp);

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