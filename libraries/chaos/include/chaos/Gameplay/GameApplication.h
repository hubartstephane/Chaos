#ifdef CHAOS_FORWARD_DECLARATION

namespace chaos
{
	class GameApplication;

}; // namespace chaos

#else

namespace chaos
{
	class GameApplication : public MyGLFW::SingleWindowApplication
	{

	};


	template<typename GAME_TYPE>
	bool RunGame(int argc, char** argv, char** env, MyGLFW::SingleWindowApplicationParams params = {})
	{
		return MyGLFW::RunWindowApplication<TypedGameWindow<GAME_TYPE>>(argc, argv, env, params);
	}

}; // namespace chaos

#endif // CHAOS_FORWARD_DECLARATION