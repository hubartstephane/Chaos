namespace chaos
{
#ifdef CHAOS_FORWARD_DECLARATION

	class StreamToDebugOutputBuf;

#elif !defined CHAOS_TEMPLATE_IMPLEMENTATION

	/**
	 * StreamToDebugOutputBuf: class used to redirect std::stream to debug output
	 */

	class CHAOS_API StreamToDebugOutputBuf : public std::streambuf
	{
	protected:

		/** override */
		virtual std::streamsize xsputn(const char* s, std::streamsize n) override;
		/** override */
		virtual int overflow(int c) override;
	};

#endif

}; // namespace chaos