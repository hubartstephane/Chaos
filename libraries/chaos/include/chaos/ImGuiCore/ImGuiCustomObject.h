namespace chaos
{
#ifdef CHAOS_FORWARD_DECLARATION

	class ImGuiCustomObject;

#elif !defined CHAOS_TEMPLATE_IMPLEMENTATION

	/**
	 * ImGuiCustomObject: an ImGuiObject dedicated to use std::function to display its content
	 */

	CHAOS_API class ImGuiCustomObject : public ImGuiObject
	{
	public:

		/** constructor (without window) */
		ImGuiCustomObject(std::function<void()> in_draw_function);
		/** constructor (with window) */
		ImGuiCustomObject(std::function<void(Window*)> in_draw_function_with_window);

		/** override */
		virtual void OnDrawImGuiContent(Window* window) override;

	protected:

		/** the drawing function */
		std::function<void()> draw_function;
		/** the drawing function with a window parameter */
		std::function<void(Window*)> draw_function_with_window;
	};



#endif

}; // namespace chaos