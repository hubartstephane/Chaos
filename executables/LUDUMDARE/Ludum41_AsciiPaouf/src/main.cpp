#include "Ludum41PCH.h"
#include "Ludum41Game.h"

using namespace chaos;

template<BasicInputType BASIC_INPUT_TYPE>
class InputDeviceQueryResult
{

public:

	using state_type = InputTypeToStateType_t<BASIC_INPUT_TYPE>;

	operator std::optional<state_type> const & () const
	{
		return input_state;
	}

public:

	std::optional<state_type> input_state;
};


void fff(std::optional<KeyState> const & state)
{

}


int main(int argc, char ** argv, char ** env)
{
	InputDeviceQueryResult<Key> r;
	fff(r);

	chaos::GameApplicationData game_application_data;
	game_application_data.game_class = LudumGame::GetStaticClass();
	return chaos::RunApplication<chaos::GameApplication>(argc, argv, env, &game_application_data);
}