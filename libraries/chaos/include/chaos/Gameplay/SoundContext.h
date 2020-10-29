#ifdef CHAOS_FORWARD_DECLARATION

namespace chaos
{
	namespace SoundContext
	{

	}; // namespace SoundContext

}; // namespace chaos

#else

namespace chaos
{
	namespace SoundContext
	{
		CHAOS_DECLARE_TAG(GAME);
		CHAOS_DECLARE_TAG(LEVEL);
		CHAOS_DECLARE_TAG(PAUSEMENU);
		CHAOS_DECLARE_TAG(MAINMENU);
	}
}; // namespace chaos

#endif // CHAOS_FORWARD_DECLARATION







