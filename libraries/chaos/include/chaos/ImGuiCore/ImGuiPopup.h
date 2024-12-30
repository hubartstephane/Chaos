namespace chaos
{
#ifdef CHAOS_FORWARD_DECLARATION

	enum class ImGuiPopupState;

	namespace details
	{
		class ImGuiPopupBase;
	};

	template<typename T>
	class ImGuiPopup;

#elif !defined CHAOS_TEMPLATE_IMPLEMENTATION

	/** the current state of the popup */
	enum class ImGuiPopupState
	{
		Closed,
		Opening,
		Opened
	};

	/** base class for a popup */
	namespace details
	{
		class ImGuiPopupBase
		{
		public:

			/** get the current state of the popup */
			ImGuiPopupState GetPopupState() const;
			
			/** get the window flags */
			int GetPopupFlags() const;
			/** get the window flags */
			void SetPopupFlags(int in_flags);

			/** end popup lifetime */
			void Close();

		protected:


			/** internal method called to open the popup. Derived class has to implement a public Open method that initialize the popup and calls DoOpen */
			bool DoOpen(std::string in_popup_name, const ImGuiWindowPlacement& in_placement);
			/** prepare imgui popup placement */
			void PrepareModalPlacement();

		protected:

			/** the title of the popup */
			std::string popup_name;
			/** the state of the popup */
			ImGuiPopupState popup_state = ImGuiPopupState::Closed;
			/** placement behaviour of the popup */
			ImGuiWindowPlacement popup_placement;
			/** the window flags */
			int imgui_flags = ImGuiWindowFlags_AlwaysAutoResize;
		};

	}; // namespace details

	/** base class for a popup */
	template<typename T>
	class ImGuiPopup : public details::ImGuiPopupBase
	{
	public:

		/** the return type of the popup */
		using RESULT_TYPE = std::conditional_t<
			std::is_same_v<T, void>,
			void,
			std::optional<T>
		>;

		/** entry point of the window loop */
		virtual RESULT_TYPE Process()
		{
			// early exit
			if (popup_state == ImGuiPopupState::Closed)
			{
				if constexpr (!std::is_same_v<void, T>)
					return {};
				else
					return;
			}
			// set the window placement
			PrepareModalPlacement();

			// open the popup
			if (ImGui::BeginPopupModal(popup_name.c_str(), NULL, imgui_flags))
			{
				if constexpr (std::is_same_v<void, T>)
				{
					DoProcess();
					ImGui::EndPopup();
				}
				else
				{
					RESULT_TYPE result = DoProcess();
					ImGui::EndPopup();
					return result;
				}
			}

			if constexpr (!std::is_same_v<void, T>)
				return {};
		}

	protected:

		/** this method is to be override. It describes the popup content and a return value when the popup lifetime is over */
		virtual RESULT_TYPE DoProcess()
		{
			if constexpr (!std::is_same_v<T, void>)
				return {};
		}
	};

#endif

}; // namespace chaos