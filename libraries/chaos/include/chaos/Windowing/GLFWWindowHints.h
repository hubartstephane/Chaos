namespace chaos
{
#ifdef CHAOS_FORWARD_DECLARATION

	class GLFWWindowHints;

#elif !defined CHAOS_TEMPLATE_IMPLEMENTATION

	/**
	* GLFWWindowHints : this represents hints for GLFWwindow creation
	*/

	class CHAOS_API GLFWWindowHints
	{
	public:

		/** gives set hints to GLFW */
		void ApplyHints() const;

	public:

		/** self description */
		int red_bits = 8;
		/** self description */
		int green_bits = 8;
		/** self description */
		int blue_bits = 8;
		/** self description */
		int alpha_bits = 8;
	};

	CHAOS_API bool SaveIntoJSON(nlohmann::json& json, GLFWWindowHints const& src);

	CHAOS_API bool LoadFromJSON(nlohmann::json const& json, GLFWWindowHints& dst);

#endif

}; // namespace chaos