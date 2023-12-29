#include "Ludum45PCH.h"
#include "Ludum45Game.h"

class A
{
public:

	void DrawImGuiVariable(chaos::DrawImGuiVariableFlags flags = chaos::DrawImGuiVariableFlags::None) const
	{
		ImGui::Text("I am A here");
	}
};


	void DrawImGuiVariableImpl(A& a, chaos::DrawImGuiVariableFlags flags = chaos::DrawImGuiVariableFlags::None)
	{
		ImGui::Text("I am A");
	}


CHAOS_GLOBAL_VARIABLE(A, aaa);
CHAOS_GLOBAL_VARIABLE(glm::vec2, v2);
CHAOS_GLOBAL_VARIABLE(glm::int3, i3);
CHAOS_GLOBAL_VARIABLE(glm::double4, d4);
CHAOS_GLOBAL_VARIABLE(std::string, str, "toto truc");


int main(int argc, char ** argv, char ** env)
{
	return chaos::RunGame<LudumGame>(argc, argv, env);
}
