#ifdef CHAOS_FORWARD_DECLARATION

namespace chaos
{
	class Interface;

}; // namespace chaos

#elif !defined CHAOS_TEMPLATE_IMPLEMENTATION

namespace chaos
{
	/**
	* Interface: a base class for all interfaces
	*/

	class CHAOS_API Interface
	{

	public:

		/** destructor */
		virtual ~Interface() = default;
	};

}; // namespace chaos

#endif