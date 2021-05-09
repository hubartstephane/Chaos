namespace chaos
{
#ifdef CHAOS_FORWARD_DECLARATION

	class MyZLib;

#elif !defined CHAOS_TEMPLATE_IMPLEMENTATION

	class MyZLib
	{
	public:

		/** encoding method */
		Buffer<char> Encode(Buffer<char> const& src);
		/** decoding method */
		Buffer<char> Decode(Buffer<char> const& src);
	};

#endif

}; // namespace chaos