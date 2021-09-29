namespace chaos
{
#ifdef CHAOS_FORWARD_DECLARATION

	class JSONSerializable;

#elif !defined CHAOS_TEMPLATE_IMPLEMENTATION

	class JSONSerializable
	{
	public:

		/** the processor may save its configuration into a JSON file */
		virtual bool SerializeIntoJSON(nlohmann::json& json) const;
		/** the processor may save its configuration from a JSON file */
		virtual bool SerializeFromJSON(nlohmann::json const& json);
	};

#endif

}; // namespace chaos