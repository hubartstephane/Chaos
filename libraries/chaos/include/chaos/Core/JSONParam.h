namespace chaos
{
#ifdef CHAOS_FORWARD_DECLARATION

	class JSONParam;

#elif !defined CHAOS_TEMPLATE_IMPLEMENTATION

	// ==============================================================
	// JSONParam
	// ==============================================================

	class CHAOS_API JSONParam
	{
	public:

		/** constructor */
		JSONParam(nlohmann::json const & in_json);
		/** constructor */
		JSONParam(JSONParam const & src) = default;
		/** constructor */
		JSONParam(JSONParam && src) = default;

		/** conversion operator */
		operator nlohmann::json const& () const;

	protected:

		/** the real json object */
		nlohmann::json const& json;
	};

#endif

}; // namespace chaos