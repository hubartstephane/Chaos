#include "Ludum45PCH.h"
#include "Ludum45Game.h"

namespace toto
{

#define WITH_METHOD 1

	// ----------------------------------------------------

	class A
	{
	public:

		void DrawImGuiVariable(chaos::DrawImGuiVariableFlags flags = chaos::DrawImGuiVariableFlags::None)
		{
			ImGui::Text("Method A"); ImGui::SameLine();
			chaos::DrawImGuiVariable(i, flags);
		}

		int i = 777;
	};

	// ----------------------------------------------------

	class B
	{
	public:

		int i = 666;
	
	};

	void DrawImGuiVariableImpl(B & b, chaos::DrawImGuiVariableFlags flags = chaos::DrawImGuiVariableFlags::None)
	{
		ImGui::Text("Function B"); ImGui::SameLine();
		chaos::DrawImGuiVariable(b.i, flags);
	}

	// ----------------------------------------------------

	class C
	{
	public:

	};

}; // namespace toto

using ATYPE = toto::A;
using BTYPE = toto::B;
using CTYPE = toto::C;

CHAOS_GLOBAL_VARIABLE(ATYPE, A1);
CHAOS_GLOBAL_VARIABLE(BTYPE, B1);
CHAOS_GLOBAL_VARIABLE(CTYPE, C1);
CHAOS_GLOBAL_VARIABLE(const ATYPE, A2);
CHAOS_GLOBAL_VARIABLE(const BTYPE, B2);
CHAOS_GLOBAL_VARIABLE(const CTYPE, C2);

CHAOS_GLOBAL_VARIABLE(int, i1);
CHAOS_GLOBAL_VARIABLE(int const, i2);
CHAOS_GLOBAL_VARIABLE(float, f1);
CHAOS_GLOBAL_VARIABLE(float const, f2);
CHAOS_GLOBAL_VARIABLE(double, d1);
CHAOS_GLOBAL_VARIABLE(double const, d2);


CHAOS_GLOBAL_VARIABLE(glm::vec2, vec2_1);
CHAOS_GLOBAL_VARIABLE(glm::int3, int3_1);
CHAOS_GLOBAL_VARIABLE(glm::double4, double4_1);
CHAOS_GLOBAL_VARIABLE(std::string, string_1, "mutable string");

CHAOS_GLOBAL_VARIABLE(glm::vec2 const, vec2_2);
CHAOS_GLOBAL_VARIABLE(glm::int3 const, int_2);
CHAOS_GLOBAL_VARIABLE(glm::double4 const, double4_2);
CHAOS_GLOBAL_VARIABLE(std::string const, string_2, "const string");



int main(int argc, char ** argv, char ** env)
{
	return chaos::RunGame<LudumGame>(argc, argv, env);
}
