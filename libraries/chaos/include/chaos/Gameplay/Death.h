#ifdef CHAOS_FORWARD_DECLARATION

namespace chaos
{

}; // namespace chaos

#else

namespace chaos
{
	template<typename GAME_TYPE>
	bool RunGame(int argc, char ** argv, char ** env, MyGLFW::SingleWindowApplicationParams params = MyGLFW::SingleWindowApplicationParams())
	{
		return MyGLFW::RunWindowApplication<TypedGameWindow<GAME_TYPE>>(argc, argv, env, params);
	}

}; // namespace chaos

#endif // CHAOS_FORWARD_DECLARATION