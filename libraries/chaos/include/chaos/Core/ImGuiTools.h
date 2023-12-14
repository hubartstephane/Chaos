namespace chaos
{
	namespace ImGuiTools
	{

#ifdef CHAOS_FORWARD_DECLARATION



#elif !defined CHAOS_TEMPLATE_IMPLEMENTATION

		/** ImGUI use 32 bits integers (or strings) for ID's. when using pointers as key, a dedicated function is necessary while pointers are 64 bits */
		void PushID(void* ptr);

#endif

	}; // namespace ImGuiTools

}; // namespace chaos