
#include "chaos/Chaos.h"


enum class Truc : int
{
	None = 0,
	Truc1 = 1,
	Truc2 = 2,
	Truc3 = 4,
	Truc4 = 8
};

class AAA
{
public:

	void DrawImGuiVariable(chaos::DrawImGuiVariableFlags flags)
	{
		if (ImGui::CollapsingHeader("Configuration"))
		{
			chaos::DrawImGuiVariable(i, flags);
			chaos::DrawImGuiVariable(j, flags);
			chaos::DrawImGuiVariable(name, flags);
		}
	}

	int i = 666;
	int j = 667;
	std::string name;
};

static chaos::EnumTools::EnumMetaData<Truc> const Truc_metadata =
{
	{ Truc::Truc1, "Truc1" },
	{ Truc::Truc2, "Truc2" },
	{ Truc::Truc3, "Truc3" },
	{ Truc::Truc4, "Truc4" }
};

//CHAOS_DECLARE_ENUM_METHOD(Truc);
CHAOS_IMPLEMENT_ENUM_METHOD(Truc, &Truc_metadata);

CHAOS_GLOBAL_VARIABLE(Truc, mtruc);
CHAOS_GLOBAL_VARIABLE(bool, mybool);
CHAOS_GLOBAL_VARIABLE(int, myint, 666);
CHAOS_GLOBAL_VARIABLE(Truc, mytruc2, Truc::Truc3);
CHAOS_GLOBAL_VARIABLE(chaos::Hotpoint, hot);

CHAOS_GLOBAL_VARIABLE(AAA, aaa);



#if 0



template<typename T>

class ImGuiDrawVectorOperationProcessor
{
	/** possible operations to do on a vector */
	enum class VectorOperation
	{
		Delete,
		Duplicate,
		InsertBefore,
		InsertAfter
	};

	/** tool class for processing drag-and-drop */
	class DragAndDropPayload
	{

	public:

		/** constructor */
		DragAndDropPayload(std::vector<T>* in_target, size_t in_source_index) :
			target(in_target),
			source_index(in_source_index) {}

	public:

		/** the vector which is concerned by operations */
		void* target = nullptr;
		/** the index in the vector of the element as destination for drag-and-drop */
		std::optional<size_t> source_index;
		/** the index in the vector of the element as source for drag-and-drop */
		std::optional<size_t> destination_index;
	};

public:

	/** constructor */
	ImGuiDrawVectorOperationProcessor(std::vector<T>& in_target) :
		target(in_target) {}

	/** request for a deletion */
	void Delete(size_t index)
	{
		pending_operation = VectorOperation::Delete;
		target_index = index;
	}

	/** request for a duplication */
	void Duplicate(size_t index)
	{
		if constexpr (std::copy_constructible<T>)
		{
			pending_operation = VectorOperation::Duplicate;
			target_index = index;
		}
	}

	/** request for insertion (before) */
	void InsertBefore(size_t index)
	{
		if constexpr (std::is_default_constructible_v<T>)
		{
			pending_operation = VectorOperation::InsertBefore;
			target_index = index;
		}
	}

	/** request for insertion (after) */
	void InsertAfter(size_t index)
	{
		if constexpr (std::is_default_constructible_v<T>)
		{
			pending_operation = VectorOperation::InsertAfter;
			target_index = index;
		}
	}

	/** finalize the pending operation */
	void ResolvePendingOperation()
	{
		/** resolve drag and drop */
		if (DragAndDropPayload* drag_and_drop_payload = GetCurrentPlayload())
		{
			if (drag_and_drop_payload->source_index.has_value() && drag_and_drop_payload->destination_index.has_value())
			{
				size_t source_index = drag_and_drop_payload->source_index.value();
				size_t destination_index = drag_and_drop_payload->destination_index.value();
				if (source_index != destination_index)
				{
					T first_element = std::move(target[source_index]);

					if (source_index < destination_index)
					{
						for (size_t i = source_index; i < destination_index; ++i)
							target[i] = std::move(target[i + 1]);
					}
					else
					{
						for (size_t i = source_index; i > destination_index; --i)
							target[i] = std::move(target[i - 1]);
					}
					target[destination_index] = std::move(first_element);

					// update the preload data
					drag_and_drop_payload->source_index = destination_index;
					//drag_and_drop_payload->destination_index = destination_index;
				}
			}
			return;
		}

		/* resolve other operations */
		if (pending_operation.has_value())
		{
			switch (pending_operation.value())
			{
			case VectorOperation::Delete:
				return DoDelete();
			case VectorOperation::Duplicate:
				return DoDuplicate();
			case VectorOperation::InsertBefore:
				return DoInsertBefore();
			case VectorOperation::InsertAfter:
				return DoInsertAfter();
			default:
				assert(0);
			}
		}
	}

	/** start a drag-and-drop */
	bool CheckDragSource(size_t in_source_index, ImGuiDragDropFlags source_flags, LightweightFunction<void()> draw_tooltip_func = {})
	{
		if (ImGui::BeginDragDropSource(source_flags))
		{
			DragAndDropPayload drag_and_drop_payload(&target, in_source_index);
			ImGui::SetDragDropPayload("CHAOS_VECTOR_OPERATIONS", &drag_and_drop_payload, sizeof(DragAndDropPayload), ImGuiCond_Once);

			if (!(source_flags & ImGuiDragDropFlags_SourceNoPreviewTooltip))
			{
				ImGui::PushID((int)in_source_index); // ensure that whatever draw_tooltip_func does, there is not ID collision
				draw_tooltip_func();
				ImGui::PopID();
			}

			ImGui::EndDragDropSource();
			return true;
		}
		return false;
	}

	/** end a drag-and-drop */
	bool CheckDropOnTarget(size_t in_destination_index, ImGuiDragDropFlags target_flags)
	{
		if (ImGui::BeginDragDropTarget())
		{
			if (ImGuiPayload const* payload = ImGui::AcceptDragDropPayload("CHAOS_VECTOR_OPERATIONS", target_flags))
			{
				if (DragAndDropPayload* drag_and_drop_payload = (DragAndDropPayload*)payload->Data)
				{
					if (drag_and_drop_payload->target == &target)
					{
						drag_and_drop_payload->destination_index = in_destination_index;
						return true;
					}
				}
			}
			ImGui::EndDragDropTarget();
		}
		return false;
	}

protected:

	/** try to get the payload. only success if target_vector is the same than source_vector */
	DragAndDropPayload* GetCurrentPlayload()
	{
		if (ImGuiPayload const* payload = ImGui::GetDragDropPayload())
			if (StringTools::Strcmp("CHAOS_VECTOR_OPERATIONS", payload->DataType) == 0)
				if (DragAndDropPayload* result = (DragAndDropPayload*)payload->Data)
					if (result->target == &target)
						return result;
		return nullptr;
	}

	/** process delete operation */
	void DoDelete()
	{
		if (target_index < target.size())
			target.erase(target.begin() + target_index);
	}

	/** process duplicate operation */
	void DoDuplicate()
	{
		if constexpr (std::copy_constructible<T>)
			if (target_index < target.size())
				target.insert(target.begin() + target_index, target[target_index]);
	}

	/** process insert before operation */
	void DoInsertBefore()
	{
		if constexpr (std::is_default_constructible_v<T>)
			if (target_index < target.size())
				target.insert(target.begin() + target_index, std::move(T()));
	}

	/** process insert after operation */
	void DoInsertAfter()
	{
		if constexpr (std::is_default_constructible_v<T>)
			if (target_index < target.size())
				target.insert(std::next(target.begin() + target_index), std::move(T()));
	}

protected:

	/** the array we are working on */
	std::vector<T>& target;
	/** pending operation */
	std::optional<VectorOperation> pending_operation;
	/** target index */
	size_t target_index = 0;
	};







class AAA
{
public:

	//AAA():
	//	name("vide"){}

	AAA(char const* in_name) :
		name(in_name) {};


	void DrawImGuiVariable(chaos::DrawImGuiVariableFlags flags)
	{
		ImGui::Text("A: %s", name.c_str());
		ImGui::Text("B: %s", name.c_str());
		ImGui::Text("C: %s", name.c_str());
		ImGui::Text("D: %s", name.c_str());
	}

protected:

	std::string name;
};


template<typename T>
void DrawArray(std::vector<T>& values)
{
	ImGuiDrawVectorOperationProcessor<T> vector_operation_processor(values);

	for (size_t i = 0; i < values.size(); ++i)
	{
		ImGui::PushID((int)i);

		// render the drag and drop handler
		ImGui::Button(StringTools::Printf("[%02d]", i).c_str());

		// source for drag and drop
		ImGuiDragDropFlags source_flags = 0;
		source_flags |= ImGuiDragDropFlags_SourceNoDisableHover;     // Keep the source displayed as hovered
		source_flags |= ImGuiDragDropFlags_SourceNoHoldToOpenOthers; // Because our dragging is local, we disable the feature of opening foreign treenodes/tabs while dragging

		vector_operation_processor.CheckDragSource(i, source_flags, [&values, i]()
			{
				ImGui::BeginGroup();
				DrawImGuiVariable(values[i]);
				ImGui::EndGroup();
			});

		// target for drag and drop
		ImGuiDragDropFlags target_flags = 0;
		target_flags |= ImGuiDragDropFlags_AcceptBeforeDelivery;    // Don't wait until the delivery (release mouse button on a target) to do something

		vector_operation_processor.CheckDropOnTarget(i, target_flags);

		// render the object itself
		ImGui::SameLine();

		ImGui::BeginGroup();
		DrawImGuiVariable(values[i]);
		ImGui::EndGroup();

		// drop down popup for row operations
		ImGui::SameLine();
		if (ImGui::ArrowButton("##down", ImGuiDir_Down))
			ImGui::OpenPopup("row operation popup", ImGuiPopupFlags_MouseButtonRight);

		// operation popup
		if (ImGui::BeginPopup("row operation popup"))
		{
			if (ImGui::Selectable("Delete"))
				vector_operation_processor.Delete(i);
			if constexpr (std::copy_constructible<T>)
				if (ImGui::Selectable("Duplicate"))
					vector_operation_processor.Duplicate(i);
					if constexpr (std::is_default_constructible_v<T>)
						if (ImGui::Selectable("Insert Before"))
							vector_operation_processor.InsertBefore(i);
					if constexpr (std::is_default_constructible_v<T>)
						if (ImGui::Selectable("Insert After"))
							vector_operation_processor.InsertAfter(i);
					ImGui::EndPopup();
		}

		ImGui::PopID();
	}

	vector_operation_processor.ResolvePendingOperation();
}

#endif





#if 0

ImGui::BeginDisabled();

static glm::vec4 color = { 0.0f, 0.0f, 0.0f , 1.0f };
DrawImGuiVariableImpl(color, DrawImGuiVariableFlags::COLOR);


ImGui::EndDisabled();

//static float col[4] = { 0.0f, 0.0f, 0.0f , 0.0f};
//ImGui::ColorEdit4("colour", col, 0);

//static ImVec4 col2 = { 0.0f, 0.0f, 0.0f, 0.0f };
//ImGui::ColorButton("desc_id", col2);


if (WindowApplication* WA = Application::GetInstance())
{
	if (GPUResourceManager* RM = WA->GetGPUResourceManagerInstance())
	{
		if (GPUTexture const* T = RM->FindTexture("brightsideoflife"))
		{
			ImVec2 size = { 100, 100 };

			GPUTexture const* T2 = RM->FindTexture("background1");

			if (ImGui::ImageButton("", (ImTextureID)(intptr_t)T->GetResourceID(), size, { 0.0f, 1.0f }, { 1.0f, 0.0f }))
			{
				T = T;
			}

			if (ImGui::IsItemHovered())
			{
				ImGui::ImageButton("", (ImTextureID)(intptr_t)T2->GetResourceID(), size, { 0.0f, 1.0f }, { 1.0f, 0.0f });
			}
		}


		RM = RM;
	}
}
		}

		ImGui::Separator();



		static std::vector<AAA> names1 = { "name1" , "name2", "name3", "name4", "name5", "name6", "name7", "name8", "name9", "name10" };
		ImGui::PushID(1);
		DrawArray(names1);
		ImGui::PopID();

#endif


























		enum class PopupPlacementType
		{
			ScreenCenter,
			CursorPosition,
			FreePosition
		};

		class PopupPlacement
		{
		public:

			/** get an instance for a popup centered on screen */
			static PopupPlacement GetCenterOnScreenPlacement(bool in_movable = true, const glm::vec2 & in_alignment = {0.5f, 0.5f})
			{
				PopupPlacement result;
				result.placement_type = PopupPlacementType::ScreenCenter;
				result.movable        = in_movable;
				result.alignment      = in_alignment;
				return result;
			}
			/** get an instance for a popup placed on cursor position */
			static PopupPlacement GetCenterOnCursorPlacement(bool in_movable = true, const glm::vec2& in_alignment = { 0.0f, 0.0f })
			{
				PopupPlacement result;
				result.placement_type = PopupPlacementType::CursorPosition;
				result.movable        = in_movable;
				result.alignment      = in_alignment;
				return result;
			}

			/** get an instance for a popup at a given position */
			static PopupPlacement GetCenterOnPositionPlacement(const glm::vec2& in_position, bool in_movable = true, const glm::vec2& in_alignment = { 0.5f, 0.5f })
			{
				PopupPlacement result;
				result.placement_type = PopupPlacementType::FreePosition;
				result.movable        = in_movable;
				result.alignment      = in_alignment;
				result.position       = in_position;
				return result;
			}

		public:

			/** how to place the popup */
			PopupPlacementType placement_type = PopupPlacementType::ScreenCenter;
			/** whether the popup can be moved */
			bool movable = true;
			/** the position of the popup */
			glm::vec2 position = { 0.0f, 0.0f };
			/** the alignment of the popup */
			glm::vec2 alignment = { 0.5f, 0.5f };
		};

		enum class PopupState
		{
			Closed,
			Opening,
			Opened
		};


		template<typename T>
		class ImGuiPopupBase
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
				if constexpr (!std::is_same_v<void, T>)
					if (popup_state == PopupState::Closed)
						return {};

				if (popup_state == PopupState::Opening)
				{
					ImGui::OpenPopup(popup_name.c_str(), ImGuiPopupFlags_MouseButtonRight);

					ImVec2 position = { 0.0f, 0.0f };
					ImVec2 alignment = { popup_placement.alignment.x, popup_placement.alignment.y };

					if (popup_placement.placement_type == PopupPlacementType::ScreenCenter)
						position = ImGui::GetMainViewport()->GetCenter();
					else if (popup_placement.placement_type == PopupPlacementType::CursorPosition)
						position = ImGui::GetMousePos();
					else if (popup_placement.placement_type == PopupPlacementType::FreePosition)
						position = { popup_placement.position.x, popup_placement.position.y };

					ImGui::SetNextWindowPos(position, ImGuiCond_Always, alignment);

					popup_state = PopupState::Opened;
				}

				int flags = GetPopupFlags();
				if (!popup_placement.movable)
					flags |= ImGuiWindowFlags_NoMove;

				if (ImGui::BeginPopupModal(popup_name.c_str(), NULL, flags))
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

			/** get the current state of the popup */
			PopupState GetPopupState() const
			{
				return popup_state;
			}

			/** end popup lifetime */
			void Close()
			{
				if (popup_state == PopupState::Closed)
					return;

				if (popup_state == PopupState::Opened)
					ImGui::CloseCurrentPopup();

				popup_name = {};
				popup_state = PopupState::Closed;
			}

		protected:

			/** get the window flags */
			virtual int GetPopupFlags() const
			{
				return ImGuiWindowFlags_AlwaysAutoResize;
			}

			/** this method is to be override. It describes the popup content and a return value when the popup lifetime is over */
			virtual RESULT_TYPE DoProcess()
			{
				if constexpr (!std::is_same_v<T, void>)
					return {};
			}

			/** internal method called to open the popup. Derived class has to implement a public Open method that initialize the popup and calls DoOpen */
			bool DoOpen(std::string in_popup_name, const PopupPlacement & in_placement)
			{
				if (popup_state != PopupState::Closed)
					return false;

				popup_name      = std::move(in_popup_name);
				popup_placement = in_placement;
				popup_state     = PopupState::Opening;
				return true;
			}

		protected:

			/** the title of the popup */
			std::string popup_name;
			/** the state of the popup */
			PopupState popup_state = PopupState::Closed;
			/** placement behaviour of the popup */
			PopupPlacement popup_placement;
		};








		// ----------------------------------------------------------------------------------------

		class ImGuiErrorPopup : public ImGuiPopupBase<void>
		{
		public:

			void Open(std::string in_popup_name, std::string in_message, const PopupPlacement& in_placement = {})
			{
				if (DoOpen(std::move(in_popup_name), in_placement))
				{
					message = std::move(in_message);
				}
			}

		protected:

			virtual void DoProcess() override
			{
				if (ImGui::IsKeyDown(ImGuiKey::ImGuiKey_Escape))
				{
					Close();
				}

				ImGui::Text(message.c_str());
				if (ImGui::Button("\\uf071 Close"))
					Close();
			}

		protected:

			std::string message;
		};


















class WindowOpenGLTest : public chaos::Window
{
	CHAOS_DECLARE_OBJECT_CLASS(WindowOpenGLTest, chaos::Window);

protected:

	virtual bool OnDraw(chaos::GPURenderer* renderer, chaos::GPUProgramProviderInterface const* uniform_provider, chaos::WindowDrawParams const& draw_params) override
	{
		glm::vec4 clear_color(0.1f, 0.0f, 0.0f, 0.0f);
		glClearBufferfv(GL_COLOR, 0, (GLfloat*)&clear_color);

		float far_plane = 1000.0f;
		glClearBufferfi(GL_DEPTH_STENCIL, 0, far_plane, 0);
		return true;
	}

	virtual void OnDrawWindowImGuiContent()
	{

#if 0
		static ImFont* fontAwesome = nullptr;

		static bool b = false;
		if (!b)
		{
			b = true;

			chaos::Buffer buf = chaos::FileTools::LoadFile(chaos::Application::GetInstance()->GetResourcesPath() / "fonts/fontawesome-webfont.ttf");

			ImGuiIO& io = ImGui::GetIO();
			//ImFont* fontAwesome = io.Fonts->AddFontFromFileTTF("fontawesome-webfont.ttf", 16.0f, NULL, io.Fonts->GetGlyphRangesDefault());


			fontAwesome = io.Fonts->AddFontFromFileTTF(
				(chaos::Application::GetInstance()->GetResourcesPath() / "fonts/fontawesome-webfont.ttf").string().c_str(),
				16.0f);
				//NULL, 
				//io.Fonts->GetGlyphRangesDefault());


			//fontAwesome = io.Fonts->AddFontFromMemoryCompressedTTF(buf.data, buf.bufsize, 16.0f, NULL, io.Fonts->GetGlyphRangesDefault());


		}

		if (fontAwesome)
			ImGui::PushFont(fontAwesome);
#endif

		if (ImGui::Begin("help", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_AlwaysAutoResize))
		{

			if (ImGui::Button("show popup", { 200.0f , 200.0f }))
			{
				PopupPlacement placement = PopupPlacement::GetCenterOnScreenPlacement();
				placement.movable = true;
				error_popup.Open("error", "this is the error message", placement);
			}
			ImGui::End();
		}

#if 0
		if (fontAwesome)
			ImGui::PopFont();
#endif

		error_popup.Process();



	}

protected:

	ImGuiErrorPopup error_popup;
};


int main(int argc, char ** argv, char ** env)
{
	return chaos::RunWindowApplication<WindowOpenGLTest>(argc, argv, env);
}

