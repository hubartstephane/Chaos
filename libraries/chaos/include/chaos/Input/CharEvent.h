namespace chaos
{
#ifdef CHAOS_FORWARD_DECLARATION

	class CharEvent;

#elif !defined CHAOS_TEMPLATE_IMPLEMENTATION

	/**
	* CharEvent: a char event
	*/

	class CHAOS_API CharEvent
	{

	public:

		/** the char value */
		unsigned int char_value = 0;
	};

#endif

}; // namespace chaos