namespace chaos
{
#ifdef CHAOS_FORWARD_DECLARATION

	class ImGuiUserMessageObject;

	class ImGuiUserMessageEntry;

#elif !defined CHAOS_TEMPLATE_IMPLEMENTATION

	/**
	 * ImGuiUserMessageLine: an entry for the display
	 */

	class ImGuiUserMessageLine
	{
	public:

		/** message to display */
		std::string content;
		/** remaining lifetime (< 0 for infinite) */
		float remaining_lifetime = -1.0f;
		/** color of the line */
		glm::vec4 color = {1.0f, 1.0f, 1.0f, 1.0f};
	};

	/**
	* ImGuiUserMessageObject: used to display some messages with timeout and colors
	**/

	class CHAOS_API ImGuiUserMessageObject : public ImGuiObject
	{
	public:

		/** Add a line in the log */
		void AddLine(std::string content, float lifetime = -1.0f, glm::vec4 const & color = {1.0f, 1.0f, 1.0f, 1.0f});
		/** clear all the lines */
		void Clear();

		/** override */
		virtual void OnDrawImGuiContent(Window* window) override;

	protected:

		/** the lines of the logs */
		std::vector<ImGuiUserMessageLine> lines;
	};

#endif

}; // namespace chaos