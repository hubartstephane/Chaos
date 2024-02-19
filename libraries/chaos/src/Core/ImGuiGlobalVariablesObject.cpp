#include "chaos/ChaosPCH.h"
#include "chaos/ChaosInternals.h"

namespace chaos
{
	class ImGuiDrawVectorHelper;

	class ImGuiVectorOperationsSolverBase
	{
	public:

		virtual void Process(void* in_vector, size_t in_source_index, size_t in_destination_index) = 0;
	};

	template<typename T>
	class ImGuiVectorOperationsSolver : public ImGuiVectorOperationsSolverBase
	{
	public:

		static ImGuiVectorOperationsSolverBase* GetInstance()
		{
			static ImGuiVectorOperationsSolver<T> result;
			return &result;
		}

		virtual void Process(void* in_vector, size_t in_source_index, size_t in_destination_index) override
		{
			std::vector<T> & typed_vector = *(std::vector<T> *)in_vector;

			// displace elements in the array
			T source_element = std::move(typed_vector[in_source_index]);

			if (in_source_index < in_destination_index)
			{
				for (size_t i = in_source_index; i < in_destination_index; ++i)
					typed_vector[i] = std::move(typed_vector[i + 1]);
			}
			else
			{
				for (size_t i = in_source_index; i > in_destination_index; --i)
					typed_vector[i] = std::move(typed_vector[i - 1]);
			}
			typed_vector[in_destination_index] = std::move(source_element);

			// update the preload data
			if (ImGuiDrawVectorHelper* payload = ImGuiDrawVectorHelper::GetCurrentPlayload(typed_vector))
			{
				payload->source_index = in_destination_index;
				payload->destination_index = in_destination_index;
			}
		}
	};


	class ImGuiDrawVectorHelper
	{

	protected:

		template<typename T>
		friend class ImGuiVectorOperationsSolver;

		/** constructor */
		template<typename T>
		ImGuiDrawVectorHelper(std::vector<T> * in_source_vector, size_t in_source_index):
			source_vector(in_source_vector),
			source_index(in_source_index),
			solver(ImGuiVectorOperationsSolver<T>::GetInstance())
		{
		}

		/** try to get the payload. only success if target_vector is the same than source_vector */
		template<typename T>
		static class ImGuiDrawVectorHelper* GetCurrentPlayload(std::vector<T>& in_target_vector)
		{
			if (ImGuiPayload const* payload = ImGui::GetDragDropPayload())
				if (StringTools::Strcmp("CHAOS_VECTOR_OPERATIONS", payload->DataType) == 0)
					if (ImGuiDrawVectorHelper* result = (ImGuiDrawVectorHelper*)payload->Data)
						if (result->source_vector == &in_target_vector)
							return result;
			return nullptr;
		}

	public:

		/** start a drag-and-drop */
		template<typename T>
		static bool CheckDragSource(std::vector<T> & in_source_vector, size_t in_source_index, ImGuiDragDropFlags source_flags, LightweightFunction<void()> draw_tooltip_func = {})
		{
			if (ImGui::BeginDragDropSource(source_flags))
			{
				ImGuiDrawVectorHelper data(&in_source_vector, in_source_index);
				ImGui::SetDragDropPayload("CHAOS_VECTOR_OPERATIONS", &data, sizeof(ImGuiDrawVectorHelper), ImGuiCond_Once);

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
		template<typename T>
		static bool CheckDropOnTarget(std::vector<T> & in_target_vector, size_t in_destination_index, ImGuiDragDropFlags target_flags)
		{
			if (ImGui::BeginDragDropTarget())
			{
				if (ImGuiPayload const * payload = ImGui::AcceptDragDropPayload("CHAOS_VECTOR_OPERATIONS", target_flags))
				{
					if (ImGuiDrawVectorHelper* helper = (ImGuiDrawVectorHelper*)payload->Data)
					{
						if (helper->source_vector == &in_target_vector)
						{
							helper->destination_index = in_destination_index;
							return true;
						}
					}
				}
				ImGui::EndDragDropTarget();
			}
			return false;
		}

		/** finalize pending operations */
		template<typename T>
		static bool SolveVectorOperations(std::vector<T> & in_vector)
		{
			if (ImGuiDrawVectorHelper const * payload = GetCurrentPlayload(in_vector))
			{
				if (payload->source_index.has_value() && payload->destination_index.has_value())
				{
					size_t source_index = payload->source_index.value();
					size_t destination_index = payload->destination_index.value();
					if (source_index != destination_index)
						payload->solver->Process(payload->source_vector, source_index, destination_index);
					return true;
				}
			}
			return false;
		}

	public:

		/** the vector which is concerned by operations */
		void * source_vector = nullptr;
		/** the index in the vector of the element as destination for drag-and-drop */
		std::optional<size_t> source_index;
		/** the index in the vector of the element as source for drag-and-drop */
		std::optional<size_t> destination_index;
		/** the operator for solving drag-and-drop */
		ImGuiVectorOperationsSolverBase* solver = nullptr;
	};


	template<typename T>
	class ImGuiDrawVectorOperationProcessor
	{
		enum class VectorOperation
		{
			Delete,
			Duplicate,
			InsertBefore,
			InsertAfter
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
			pending_operation = VectorOperation::Duplicate;
			target_index = index;
		}

		/** request for insertion (before) */
		void InsertBefore(size_t index)
		{
			pending_operation = VectorOperation::InsertBefore;
			target_index = index;
		}

		/** request for insertion (after) */
		void InsertAfter(size_t index)
		{
			pending_operation = VectorOperation::InsertAfter;
			target_index = index;
		}

		/** finalize the pending operation */
		void ResolvePendingOperation()
		{
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

	protected:

		/** process delete operation */
		void DoDelete()
		{

		}

		/** process duplicate operation */
		void DoDuplicate()
		{

		}

		/** process insert before operation */
		void DoInsertBefore()
		{

		}

		/** process insert after operation */
		void DoInsertAfter()
		{

		}

	protected:

		/** the array we are working on */
		std::vector<T> & target;
		/** pending operation */
		std::optional<VectorOperation> pending_operation;
		/** target index */
		size_t target_index = 0;
	};










	template<typename T>
	void DrawArray(std::vector<T>& values)
	{
		ImGuiDrawVectorOperationProcessor<T> vector_operation_processor(values);

		for (size_t i = 0; i < values.size(); ++i)
		{
			ImGui::PushID((int)i);

			// render the drag and drop handler
			ImGui::Text("[%02d]", (int)i);

			// source for drag and drop
			ImGuiDragDropFlags source_flags = 0;
			source_flags |= ImGuiDragDropFlags_SourceNoDisableHover;     // Keep the source displayed as hovered
			source_flags |= ImGuiDragDropFlags_SourceNoHoldToOpenOthers; // Because our dragging is local, we disable the feature of opening foreign treenodes/tabs while dragging

			ImGuiDrawVectorHelper::CheckDragSource(values, i, source_flags, [&values, i]()
			{
				ImGui::BeginGroup();
				ImGui::Dummy({ 200.0f, 1.0f }); ImGui::SameLine();
				ImGui::Text("toto");
				ImGui::Dummy({ 200.0f, 1.0f });
				DrawImGuiVariable(values[i]);
				ImGui::EndGroup();
			});

			// target for drag and drop
			ImGuiDragDropFlags target_flags = 0;
			target_flags |= ImGuiDragDropFlags_AcceptBeforeDelivery;    // Don't wait until the delivery (release mouse button on a target) to do something
			ImGuiDrawVectorHelper::CheckDropOnTarget(values, i, target_flags);

			// render the object itself
			ImGui::SameLine();

			ImGui::BeginGroup();
			ImGui::Dummy({ 200.0f, 1.0f }); ImGui::SameLine();
			ImGui::Text("toto");
			ImGui::Dummy({ 200.0f, 1.0f });
			DrawImGuiVariable(values[i]);
			ImGui::EndGroup();

			// drop down popup for row operations
			ImGui::SameLine();
			if (ImGui::ArrowButton("##down", ImGuiDir_Down))
			{
				ImGui::OpenPopup("row operation popup", ImGuiPopupFlags_MouseButtonRight);
			}

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

		ImGuiDrawVectorHelper::SolveVectorOperations(values);
	}





	void ImGuiGlobalVariablesObject::OnDrawImGuiContent()
	{
		ImGui::Dummy({ 200.0f, 1.0f });
		if (ImGui::BeginTable("Global Variables", 2, ImGuiTableFlags_Resizable))
		{
			for (GlobalVariableBase* variable : chaos::GlobalVariableManager::GetInstance()->GetVariables())
			{
				if (variable != nullptr)
				{
					if (GlobalVariableInfo const* variable_info = variable->GetVariableInfo())
					{
						if (GlobalVariableImGuiRendererBase const* imgui_renderer = variable_info->GetImGuiRenderer())
						{
							if (imgui_renderer->CanDrawVariable())
							{
								ImGui::TableNextColumn();
								ImGui::Text("%s", variable->GetName());

								ImGui::TableNextColumn();
								imgui_renderer->DrawVariable(variable);

							}
						}
					}
				}
			}
			ImGui::EndTable();


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

		static std::vector<std::string> names1 = { "name1" , "name1", "name1", "name4", "name5", "name6", "name7", "name8", "name9", "name10" };
		ImGui::PushID(1);
		DrawArray(names1);
		ImGui::PopID();

		ImGui::Separator();
		return;
		static std::vector<std::string> names2 = { "xxxxxxx1" , "xxxxxxx2", "xxxxxxx3", "xxxxxxx4", "xxxxxxx5", "xxxxxxx6" };
		ImGui::PushID(1);
		DrawArray(names2);
		ImGui::PopID();


	}

}; // namespace chaos