namespace chaos
{
#ifdef CHAOS_FORWARD_DECLARATION

	class JSONSerializableInterface;

#elif !defined CHAOS_TEMPLATE_IMPLEMENTATION

	class CHAOS_API JSONSerializableInterface
	{
	public:

		/** the processor may save its configuration into a JSON file */
		virtual bool SerializeIntoJSON(nlohmann::json * json) const;
		/** the processor may save its configuration from a JSON file */
		virtual bool SerializeFromJSON(JSONReadConfiguration config);
	};

#endif

}; // namespace chaos