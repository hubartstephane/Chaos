#include "chaos/Chaos.h"

class WindowOpenGLTest : public chaos::Window
{
	CHAOS_DECLARE_OBJECT_CLASS(WindowOpenGLTest, chaos::Window);

protected:


	void DrawGraphic(char const * title, char const * tip, chaos::LightweightFunction<float(float)> func)
	{
		constexpr size_t VALUE_COUNT = 256;

		float x[VALUE_COUNT];
		float y[VALUE_COUNT];

		for (size_t i = 0; i < VALUE_COUNT; ++i)
		{
			float power = 1.0f;

			x[i] = float(i) / float(VALUE_COUNT - 1);
			y[i] = chaos::MathTools::Lerp(std::pow(func(x[i]), power), 0.0f, 1.0f);
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

				DrawGraphic("Sin", "f(x) = sin(x.pi/2) / sin(pi/2)", [](float x)
				{
					return
						std::sin(0.5f * float(M_PI) * x) /
						std::sin(0.5f * float(M_PI));
				});

				DrawGraphic("Cos", "f(x) = 1 - cos(x.pi)/2", [](float x)
				{
					return (1.0f - std::cos(float(M_PI) * x)) * 0.5f;
				});

				DrawGraphic("Linear", "f(x) = x", [](float x)
				{
					return x;
				});

				DrawGraphic("Ease", "f(x) = (3.x^2) - (2.x^3)", [](float x)
				{
					return chaos::MathTools::Ease(x);
				});

				DrawGraphic("Easier", "f(x) = (6.x^5) - (15.x^4) + (10.x^3)", [](float x)
				{
					return 
						   6.0f * (x * x * x * x * x)
						- 15.0f * (x * x * x * x)
						+ 10.0f * (x * x * x);
				});

				DrawGraphic("X^2", "f(x) = x^2", [](float x)
				{
					return x * x;
				});

				DrawGraphic("X^3", "f(x) = x^3", [](float x)
				{
					return x * x * x;
				});

				DrawGraphic("X^5", "f(x) = x^5", [](float x)
				{
					return x * x * x * x * x;
				});

				DrawGraphic("Exp(x, k=3, p=5)", "f(x) = (e^(k.x^p) - 1) / (e^k - 1)", [](float x)
				{
					float k = 5.0f;
					float power = 3.0f;
					return 
						(std::pow(float(M_E), k * std::pow(x, power)) - 1) / 
						(std::pow(float(M_E), k) - 1);
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