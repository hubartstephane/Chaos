#ifdef CHAOS_FORWARD_DECLARATION

namespace chaos
{
	class JSONSerializable;

}; // namespace chaos

#elif defined CHAOS_TEMPLATE_IMPLEMENTATION


#else 

namespace chaos
{

	class JSONSerializable
	{
	public:

		/** the processor may save its configuration into a JSON file */
		virtual bool SerializeIntoJSON(nlohmann::json& json_entry) const;
		/** the processor may save its configuration from a JSON file */
		virtual bool SerializeFromJSON(nlohmann::json const& json_entry);
	};

}; // namespace chaos

#endif // CHAOS_FORWARD_DECLARATION


