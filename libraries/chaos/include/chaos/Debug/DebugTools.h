#ifdef CHAOS_FORWARD_DECLARATION

namespace chaos
{
	namespace DebugTools
	{

	}; // namespace DebugTools

}; // namespace chaos

#else 

namespace chaos
{
	namespace DebugTools
	{
		/** display the callstack of the current application */
		void DisplayCallStack(std::ostream & stream);

	}; // namespace DebugTools

}; // namespace chaos

#endif // CHAOS_FORWARD_DECLARATION


