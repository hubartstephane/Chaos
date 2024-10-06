#ifdef CHAOS_FORWARD_DECLARATION

#define CHAOS_PREMAIN static bool const BOOST_PP_CAT(premain_, __LINE__) = chaos::RegisterPremainCode

#elif !defined CHAOS_TEMPLATE_IMPLEMENTATION

namespace chaos
{
	/** register a Premain code */
	CHAOS_API bool RegisterPremainCode(int order, std::function<void()> const& func);
	/** call code for all code in premain */
	CHAOS_API void FlushPremainCalls();

}; // namespace chaos

#endif



