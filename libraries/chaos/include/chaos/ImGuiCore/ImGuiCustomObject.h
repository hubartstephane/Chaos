namespace chaos
{
#ifdef CHAOS_FORWARD_DECLARATION

	class ImGuCustomiObject;

#elif !defined CHAOS_TEMPLATE_IMPLEMENTATION

	/**
	 * ImGuiCustomObject: an ImGuiObject dedicated to use std::function to display its content
	 */

	class ImGuiCustomObject : public ImGuiObject
	{
	public:

		//CHAOS_DECLARE_OBJECT_CLASS(ImGuiCustomObject, ImGuiObject);

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