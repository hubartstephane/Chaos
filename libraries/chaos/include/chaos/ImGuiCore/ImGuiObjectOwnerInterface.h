namespace chaos
{
#ifdef CHAOS_FORWARD_DECLARATION

	class ImGuiObjectOwnerInterface;

#elif !defined CHAOS_TEMPLATE_IMPLEMENTATION

	/**
	 * ImGuiObjectOwnerInterface: an interface dedicated for Window to handle ImGuiObjects
	 **/

class ImGuiObjectOwnerInterface
{
public:

	using CreateImGuiObjectFunc = LightweightFunction<ImGuiObject* ()>;
	using EnumerateKnownImGuiObjectFunc = LightweightFunction<bool(char const*, char const *, CreateImGuiObjectFunc)>; // name, imgui menu path, create function

	/** create and add a new imgui object */
	template<typename IMGUIOBJECT, typename... PARAMS>
	IMGUIOBJECT* AddNewImGuiObject(char const* title, PARAMS... params)
	{
		static_assert(std::is_base_of_v<ImGuiObject, IMGUIOBJECT>);

		// ensure an object with same name does not already exist
		if (FindImGuiObject(title) != nullptr)
		{
			ImGuiLog::Warning("Window::AddNewImGuiObject(...) failure: an ImGuiObject named [%s] already exists", title);
			return nullptr;
		}

		// create and add the object
		IMGUIOBJECT* result = new IMGUIOBJECT(std::forward<PARAMS>(params)...);
		if (result != nullptr)
		{
			result->SetName(title);
			AddImGuiObject(result);
		}
		return result;
	}

	/** create and add a new imgui popup modal */
	template<typename IMGUIOBJECT, typename... PARAMS>
	IMGUIOBJECT* AddNewImGuiPopupModal(char const* title, PARAMS... params)
	{
		IMGUIOBJECT* result = AddNewImGuiObject<IMGUIOBJECT>(title, std::forward<PARAMS>(params)...);
		if (result != nullptr)
		{
			ImGuiObjectFlags flags = result->GetImGuiObjectFlags(); // ensure the ImGuiObject is marked as a modal popup whilst keeping flags it could naturaly have
			flags |=  ImGuiObjectFlags::PopupModalWindow;
			flags &= ~ImGuiObjectFlags::PopupWindow;
			flags &= ~ImGuiObjectFlags::FloatingWindow;
			flags &= ~ImGuiObjectFlags::FullViewport;
			result->SetImGuiObjectFlags(flags);
		}
		return result;
	}


	/** create and add a new imgui popup */
	template<typename IMGUIOBJECT, typename... PARAMS>
	IMGUIOBJECT* AddNewImGuiPopup(char const* title, PARAMS... params)
	{
		IMGUIOBJECT* result = AddNewImGuiObject<IMGUIOBJECT>(title, std::forward<PARAMS>(params)...);
		if (result != nullptr)
		{
			ImGuiObjectFlags flags = result->GetImGuiObjectFlags(); // ensure the ImGuiObject is marked as a modal popup whilst keeping flags it could naturaly have
			flags &= ~ImGuiObjectFlags::PopupModalWindow;
			flags |=  ImGuiObjectFlags::PopupWindow;
			flags &= ~ImGuiObjectFlags::FloatingWindow;
			flags &= ~ImGuiObjectFlags::FullViewport;
			result->SetImGuiObjectFlags(flags);
			result->SetWindowPlacement(ImGuiWindowPlacement::GetCenterOnCursorPlacement());
		}
		return result;
	}

	/** search whether a given ImGui object exists */
	bool IsKnownImGuiObjectVisible(char const* name) const;
	/** create a ImGuiObject from its name */
	bool SetKnownImGuiObjectVisibility(char const* name, bool visible);

	/** search an ImGui object by name */
	AutoCastable<ImGuiObject> FindImGuiObject(ObjectRequest request);
	/** search an ImGui object by name */
	AutoConstCastable<ImGuiObject> FindImGuiObject(ObjectRequest request) const;

	/** insert an ImGui object into list */
	void AddImGuiObject(ImGuiObject* imgui_object);
	/** remove an ImGui object from list */
	void RemoveImGuiObject(ImGuiObject* imgui_object);

protected:

	/** enumerate ImGuiObjects that the window can create */
	virtual bool EnumerateKnownImGuiObjects(EnumerateKnownImGuiObjectFunc func);
	/** create or destroy an ImGuiObject */
	void SetImGuiObjectInternalVisibility(bool visible, char const* name, CreateImGuiObjectFunc create_func);
	/** draw all ImGui objects */
	void OnDrawImGuiObjectOwnerContent();
	/** draw a menu for ImGuiObjects */
	void OnDrawImGuiObjectOwnerMenu(ImGuiMenuBuilder const & menu_builder);

protected:

	/** the imgui_objects handled by this window */
	std::vector<shared_ptr<ImGuiObject>> imgui_objects;
};
	

#endif

}; // namespace chaos