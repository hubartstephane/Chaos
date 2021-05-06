#ifdef CHAOS_FORWARD_DECLARATION

namespace chaos
{
	class MyZLib;

}; // namespace chaos

#elif defined CHAOS_TEMPLATE_IMPLEMENTATION


#else

namespace chaos
{
	class MyZLib
	{
	public:

		/** encoding method */
		Buffer<char> Encode(Buffer<char> const & src);
		/** decoding method */
		Buffer<char> Decode(Buffer<char> const & src);
	};

}; // namespace chaos

#endif CHAOS_FORWARD_DECLARATION