#include "chaos/Chaos.h"

enum class AnalyticExpressionType
{
	Value,
	Name,
	Add,
	Mul
};

// ===============================================================================

class AnalyticExpression
{
public:

	AnalyticExpression() = default;

	AnalyticExpression(float in_value)
	{
		value = in_value;
		type = AnalyticExpressionType::Value;
	}

	AnalyticExpression(std::string in_name)
	{
		name = std::move(in_name);
		type = AnalyticExpressionType::Name;
	}

	~AnalyticExpression();

public:

	AnalyticExpressionType type = AnalyticExpressionType::Value;

	float value = 0;

	std::string name;

	AnalyticExpression* operand1 = nullptr;

	AnalyticExpression* operand2 = nullptr;
};

// ===============================================================================

AnalyticExpression::~AnalyticExpression()
{
	delete(operand1);
	delete(operand2);
}

AnalyticExpression* Clone(AnalyticExpression const* src)
{
	if (src == nullptr)
		return nullptr;
	AnalyticExpression* result = new AnalyticExpression;
	if (result != nullptr)
	{
		result->type = src->type;
		switch (src->type)
		{
		case AnalyticExpressionType::Value:
			result->value = src->value;
			break;
		case AnalyticExpressionType::Name:
			result->name = src->name;
			break;
		case AnalyticExpressionType::Add:
		case AnalyticExpressionType::Mul:
			result->operand1 = Clone(src->operand1);
			result->operand2 = Clone(src->operand2);
			break;
		}
	}
	return result;
}

bool IsZero(AnalyticExpression const* src)
{
	if (src == nullptr)
		return true;
	if (src->type == AnalyticExpressionType::Value && src->value == 0.0f)
		return true;
	return false;
}

bool IsOne(AnalyticExpression const* src)
{
	if (src == nullptr)
		return false;
	if (src->type == AnalyticExpressionType::Value && src->value == 1.0f)
		return true;
	return false;
}

AnalyticExpression* Add(AnalyticExpression const* src1, AnalyticExpression const* src2)
{
	if (IsZero(src1))
		return Clone(src2);
	if (IsZero(src2))
		return Clone(src1);

	AnalyticExpression* result = new AnalyticExpression;
	if (result != nullptr)
	{
		if (src1->type == AnalyticExpressionType::Value && src2->type == AnalyticExpressionType::Value)
		{
			result->type = AnalyticExpressionType::Value;
			result->value = src1->value + src2->value;
		}
		else
		{
			result->type = AnalyticExpressionType::Add;
			result->operand1 = Clone(src1);
			result->operand2 = Clone(src2);
		}
	}
	return result;
}

AnalyticExpression* Mul(AnalyticExpression const* src1, AnalyticExpression const* src2)
{
	if (IsZero(src1) || IsZero(src2))
		return nullptr;
	if (IsOne(src1))
		return Clone(src2);
	if (IsOne(src2))
		return Clone(src1);
	AnalyticExpression* result = new AnalyticExpression;
	if (result != nullptr)
	{
		if (src1->type == AnalyticExpressionType::Value && src2->type == AnalyticExpressionType::Value)
		{
			result->type = AnalyticExpressionType::Value;
			result->value = src1->value * src2->value;
		}
		else
		{
			result->type = AnalyticExpressionType::Mul;
			result->operand1 = Clone(src1);
			result->operand2 = Clone(src2);
		}
	}
	return result;
}

std::string ToString(AnalyticExpression const* src)
{
	if (src == nullptr)
		return "0";
	if (src->type == AnalyticExpressionType::Value)
		return std::format("{}", src->value);
	if (src->type == AnalyticExpressionType::Name)
		return src->name;	
	if (src->type == AnalyticExpressionType::Add)
		return std::format("({} + {})", ToString(src->operand1), ToString(src->operand2));
	if (src->type == AnalyticExpressionType::Mul)
		return std::format("({} * {})", ToString(src->operand1), ToString(src->operand2));

	return "?";
}

// ===============================================================================

class AnalyticMatrix
{
public:

	AnalyticMatrix() = default;

	AnalyticMatrix(AnalyticMatrix const& src);

	AnalyticMatrix(AnalyticMatrix&& src);

	AnalyticMatrix(glm::mat4x4 const& src);

	~AnalyticMatrix();

	AnalyticMatrix& operator = (AnalyticMatrix const& src);

	AnalyticMatrix& operator = (AnalyticMatrix&& src);

	AnalyticMatrix & operator = (glm::mat4x4 const& src);

	AnalyticExpression*& operator () (size_t x, size_t y)
	{
		return components[x + y * 4];
	}

	AnalyticExpression const* const& operator () (size_t x, size_t y) const
	{
		return components[x + y * 4];
	}

	void Clear();

	std::string ToCppString(char const* matrix_name) const;

public:

	std::array<AnalyticExpression*, 16> components = { nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr };
};

// ===============================================================================

AnalyticMatrix::AnalyticMatrix(AnalyticMatrix const& src)
{
	for (size_t i = 0; i < 16; ++i)
		components[i] = Clone(src.components[i]);
}

AnalyticMatrix::AnalyticMatrix(AnalyticMatrix&& src)
{
	for (size_t i = 0; i < 16; ++i)
	{
		components[i] = src.components[i];
		src.components[i] = nullptr;
	}
}

AnalyticMatrix::AnalyticMatrix(glm::mat4x4 const& src)
{
	for (size_t y = 0; y < 4; ++y)
		for (size_t x = 0; x < 4; ++x)
			if (src[y][x] != 0.0f)
				operator () (x, y) = new AnalyticExpression(src[y][x]);
}

AnalyticMatrix::~AnalyticMatrix()
{
	Clear();
}

void AnalyticMatrix::Clear()
{
	for (AnalyticExpression*& component : components)
	{
		delete component;
		component = nullptr;
	}
}

AnalyticMatrix& AnalyticMatrix::operator = (AnalyticMatrix const& src)
{
	if (&src != this)
	{
		Clear();
		for (size_t i = 0; i < 16; ++i)
			components[i] = Clone(src.components[i]);
	}
	return *this;
}

AnalyticMatrix& AnalyticMatrix::operator = (AnalyticMatrix&& src)
{
	if (&src != this)
	{
		Clear();
		for (size_t i = 0; i < 16; ++i)
		{
			components[i] = src.components[i];
			src.components[i] = nullptr;
		}
	}
	return *this;
}

AnalyticMatrix& AnalyticMatrix::operator = (glm::mat4x4 const & src)
{
	Clear();
	for (size_t y = 0; y < 4; ++y)
		for (size_t x = 0; x < 4; ++x)
			if (src[y][x] != 0.0f)
				operator () (x, y) = new AnalyticExpression(src[y][x]);
	return *this;
}


AnalyticMatrix operator * (AnalyticMatrix const& m1, AnalyticMatrix const& m2)
{
	AnalyticMatrix result;

	for (size_t y = 0; y < 4; ++y)
	{
		for (size_t x = 0; x < 4; ++x)
		{
			AnalyticExpression* component = Mul(m1(0, y), m2(x, 0));
			for (size_t i = 1; i < 4; ++i)
				component = Add(component, Mul(m1(i, y), m2(x, i)));
			result(x, y) = component;
		}
	}
	return result;
}

std::string AnalyticMatrix::ToCppString(char const* matrix_name) const
{
	std::string result;
	for (size_t y = 0; y < 4; ++y)
	{
		for (size_t x = 0; x < 4; ++x)
		{
			if (result.length() > 0)
				result += "\r\n";
			result += std::format("{}({}, {}) = {};",
				matrix_name,
				x,
				y,
				ToString(operator()(x, y)));
		}
	}
	return result;
}

// ----------------------------------------------------------------------------------------

AnalyticMatrix MakeScaleMatrix(glm::vec3 const & v)
{
	return glm::scale(v);
}

AnalyticMatrix MakeScaleMatrix(std::string x, std::string y, std::string z)
{
	AnalyticMatrix result;
	result(0, 0) = new AnalyticExpression(std::move(x));
	result(1, 1) = new AnalyticExpression(std::move(y));
	result(2, 2) = new AnalyticExpression(std::move(z));
	result(3, 3) = new AnalyticExpression(1.0f);
	return result;
}

AnalyticMatrix MakeScaleMatrix(std::string const & name)
{
	return MakeScaleMatrix(
		std::format("{}.x", name),
		std::format("{}.y", name),
		std::format("{}.z", name)
	);
}

AnalyticMatrix MakeIdentityMatrix()
{
	return MakeScaleMatrix(glm::vec3(1.0f, 1.0f, 1.0f));
}

AnalyticMatrix MakeTranslationMatrix(glm::vec3 const& v)
{
	return AnalyticMatrix(glm::translate(v));
}

AnalyticMatrix MakeTranslationMatrix(std::string x, std::string y, std::string z)
{
	AnalyticMatrix result = MakeIdentityMatrix();
	result(0, 3) = new AnalyticExpression(std::move(x));
	result(1, 3) = new AnalyticExpression(std::move(y));
	result(2, 3) = new AnalyticExpression(std::move(z));
	return result;
}

AnalyticMatrix MakeTranslationMatrix(std::string const& name)
{
	return MakeTranslationMatrix(
		std::format("{}.x", name),
		std::format("{}.y", name),
		std::format("{}.z", name)
	);
}

AnalyticMatrix MakeRotationMatrixX(float value)
{
	float c = std::cos(value);
	float s = std::sin(value);

	AnalyticMatrix result;
	result(0, 0) = new AnalyticExpression(1.0f);
	result(1, 1) = new AnalyticExpression(c);
	result(2, 1) = new AnalyticExpression(s);
	result(1, 2) = new AnalyticExpression(-s);
	result(2, 2) = new AnalyticExpression(c);
	return result;
}

AnalyticMatrix MakeRotationMatrixX(char const * angle_name)
{
	AnalyticMatrix result;
	result(0, 0) = new AnalyticExpression(1.0f);
	result(1, 1) = new AnalyticExpression(std::format("cos({})", angle_name));
	result(2, 1) = new AnalyticExpression(std::format("sin({})", angle_name));
	result(1, 2) = new AnalyticExpression(std::format("-sin({})", angle_name));
	result(2, 2) = new AnalyticExpression(std::format("cos({})", angle_name));
	return result;
}

AnalyticMatrix MakeRotationMatrixY(float value)
{
	float c = std::cos(value);
	float s = std::sin(value);

	AnalyticMatrix result;
	
	result(0, 0) = new AnalyticExpression(c);
	result(2, 0) = new AnalyticExpression(-s);
	result(1, 1) = new AnalyticExpression(1.0f);
	result(0, 2) = new AnalyticExpression(s);
	result(2, 2) = new AnalyticExpression(c);
	return result;
}

AnalyticMatrix MakeRotationMatrixY(char const* angle_name)
{
	AnalyticMatrix result;	
	result(0, 0) = new AnalyticExpression(std::format("cos({})", angle_name));
	result(2, 0) = new AnalyticExpression(std::format("-sin({})", angle_name));
	result(1, 1) = new AnalyticExpression(1.0f);
	result(0, 2) = new AnalyticExpression(std::format("sin({})", angle_name));
	result(2, 2) = new AnalyticExpression(std::format("cos({})", angle_name));
	return result;
}

AnalyticMatrix MakeRotationMatrixZ(float value)
{
	float c = std::cos(value);
	float s = std::sin(value);

	AnalyticMatrix result;

	result(0, 0) = new AnalyticExpression(c);
	result(1, 0) = new AnalyticExpression(s);
	result(0, 1) = new AnalyticExpression(-s);
	result(1, 1) = new AnalyticExpression(c);
	result(2, 2) = new AnalyticExpression(1.0f);
	return result;
}

AnalyticMatrix MakeRotationMatrixZ(char const* angle_name)
{
	AnalyticMatrix result;
	result(0, 0) = new AnalyticExpression(std::format("cos({})", angle_name));
	result(1, 0) = new AnalyticExpression(std::format("sin({})", angle_name));
	result(0, 1) = new AnalyticExpression(std::format("-sin({})", angle_name));
	result(1, 1) = new AnalyticExpression(std::format("cos({})", angle_name));
	result(2, 2) = new AnalyticExpression(1.0f);
	return result;
}

// ----------------------------------------------------------------------------------------


#if 0

enum class ImGuiWindowPlacementType
{
	ScreenCenter,
	CursorPosition
};


template<typename T>
class ImGuiPopupBase
{
public:

	using RESULT_TYPE = std::conditional_t<
		std::is_same_v<T, void>,
		void,
		std::optional<T>
	>;

	virtual ImGuiWindowPlacementType GetPlacementType() const
	{
		//return ImGuiWindowPlacementType::ScreenCenter;
		return ImGuiWindowPlacementType::CursorPosition;
	}

	virtual RESULT_TYPE Process()
	{
		if (popup_name.length() > 0)
		{
			ImGui::SetNextWindowPos(popup_position, ImGuiCond_Always, popup_alignment);

			if (ImGui::BeginPopupModal(popup_name.c_str(), NULL, ImGuiWindowFlags_AlwaysAutoResize))
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
		}

		if constexpr (!std::is_same_v<void, T>)
		{
			return {};
		}
	}

	bool IsOpened() const
	{
		return popup_name.length() > 0;
	}

protected:

	virtual RESULT_TYPE DoProcess()
	{
		if constexpr (!std::is_same_v<T, void>)
		{
			return {};
		}
	}

	virtual void Close()
	{
		popup_name = {};
		ImGui::CloseCurrentPopup();
	}

	bool DoOpen(std::string in_popup_name)
	{
		if (!IsOpened())
		{
			ImGuiWindowPlacementType placement_type = GetPlacementType();
			if (placement_type == ImGuiWindowPlacementType::ScreenCenter)
			{
				popup_position  = ImGui::GetMainViewport()->GetCenter();
				popup_alignment = ImVec2(0.5f, 0.5f);
			}
			else if (placement_type == ImGuiWindowPlacementType::CursorPosition)
			{
				popup_position = ImGui::GetMousePos();
				popup_alignment = ImVec2(0.0f, 0.0f);
			}

			popup_name = std::move(in_popup_name);

			ImGui::OpenPopup(popup_name.c_str(), ImGuiPopupFlags_MouseButtonRight); // shuxxx ImGuiPopupFlags_MouseButtonRight
			return true;
		}
		return false;
	}

protected:

	std::string popup_name;

	ImVec2 popup_position;

	ImVec2 popup_alignment;
};


// ----------------------------------------------------------------------------------------

class ImGuiErrorPopup : public ImGuiPopupBase<void>
{
public:

	void Open(std::string in_popup_name, std::string in_message)
	{
		if (DoOpen(std::move(in_popup_name)))
		{
			message = std::move(in_message);
		}
	}

protected:

	virtual void DoProcess() override
	{
		ImGui::Text(message.c_str());
		if (ImGui::Button("Close"))
			Close();
	}

	virtual void Close() override
	{
		message = {};
		ImGuiPopupBase::Close();
	}

protected:

	std::string message;
};

// ----------------------------------------------------------------------------------------

class ImGuiNameSelectionPopup : public ImGuiPopupBase<std::string>
{
public:

	void Open(std::string in_popup_name)
	{
		DoOpen(std::move(in_popup_name));
	}

protected:

	virtual std::optional<std::string> DoProcess() override
	{
		std::optional<std::string> result;

		ImGui::Text("new matrix name");

		chaos::ImGuiTools::InputText("", name);
		if (ImGui::Button("Validate"))
		{
			result.emplace(std::move(name));
			Close();
		}

		ImGui::SameLine();
		if (ImGui::Button("Close"))
		{
			result = "";
			Close();
		}

		return result;
	}

	virtual void Close() override
	{
		name = {};
		ImGuiPopupBase::Close();
	}

protected:

	std::string name;
};



// ----------------------------------------------------------------------------------------

enum class MatrixCreationType
{
	Identity,
	Scale,
	Translation,
	RotationX,
	RotationY,
	RotationZ
};

class ImGuiNewMatrixPopup : public ImGuiPopupBase<MatrixCreationType>
{
public:

	void Open(std::string in_popup_name)
	{
		DoOpen(std::move(in_popup_name));
	}

protected:

	std::optional<MatrixCreationType> DoProcess() override
	{
		std::optional<MatrixCreationType> result;

		if (ImGui::Selectable("Identity"))
		{
			result = MatrixCreationType::Identity;
			Close();
		}
		if (ImGui::Selectable("Scale"))
		{
			result = MatrixCreationType::Scale;
			Close();
		}
		if (ImGui::Selectable("Translation"))
		{
			result = MatrixCreationType::Translation;
			Close();
		}
		if (ImGui::Selectable("Rotation X"))
		{
			result = MatrixCreationType::RotationX;
			Close();
		}
		if (ImGui::Selectable("Rotation Y"))
		{
			result = MatrixCreationType::RotationY;
			Close();
		}
		if (ImGui::Selectable("Rotation Z"))
		{
			result = MatrixCreationType::RotationZ;
			Close();
		}

		return result;
	}
};

#endif

// ----------------------------------------------------------------------------------------











// ----------------------------------------------------------------------------------------

enum class IterationState
{
	Continue,
	Stop
};

class NamedMatrix
{
public:

	std::string name;
	AnalyticMatrix matrix;
};


class MyImGuiObject : public chaos::ImGuiObject
{
public:

	MyImGuiObject()
	{
		AnalyticMatrix S = MakeScaleMatrix("S");
		AnalyticMatrix T = MakeTranslationMatrix("T");

		matrixes.push_back({ "toto", S });
		matrixes.push_back({ "titi", T });
	}

	virtual void OnDrawImGuiContent(chaos::Window * window) override
	{
		for (NamedMatrix const& m : matrixes)
		{
			if (DisplayAnalyticMatrix(m) == IterationState::Stop)
			{
				break;
			}
			ImGui::Separator();
		}

		ImGui::Text("tergndslkjnlgkjnfg"); ImGui::SameLine();
		if (ImGui::ArrowButton("##down", ImGuiDir_Down))
			new_matrix_popup.Open("New Matrix");
			//new_matrix_popup.Open();


		if (std::optional<MatrixCreationType> new_matrix_name = new_matrix_popup.Process())
		{
			int i = 0;
			++i;

		}
		
		
		//new_matrix_popup.Display();
#if 0
		if (std::optional<std::string> new_matrix_name = name_selection_popup.Display())
		{
			auto it = std::ranges::find_if(matrixes, [&name = new_matrix_name](NamedMatrix const& M)
			{
				return chaos::StringTools::Stricmp(name, M.name) == 0;
			});
			if (it != matrixes.end())
			{
				ErrorPopup.Open(std::format("A matrix named [{}] already exits", name));
			}
			else
			{
				
			}

		}
#endif

	}

	IterationState DisplayAnalyticMatrix(NamedMatrix const& m)
	{
		ImGui::PushID(&m);

		ImGui::TextWrapped(m.name.c_str());

		if (ImGui::BeginTable("AnalyticMatrix", 4, ImGuiTableFlags_Resizable | ImGuiTableFlags_RowBg))
		{
			for (size_t y = 0; y < 4; ++y)
			{
				for (size_t x = 0; x < 4; ++x)
				{
					ImGui::TableNextColumn();
					ImGui::Text("%s", ToString(m.matrix(x, y)).c_str());
				}
			}
			ImGui::EndTable();
		}

		ImGui::PopID();

		return IterationState::Continue;
	}













#if 0

			ImGui::OpenPopup("new matrix", ImGuiPopupFlags_MouseButtonRight);

		bool new_identity_matrix = false;
		bool new_scale_matrix = false;
		bool new_translation_matrix = false;
		bool new_rotation_x_matrix = false;
		bool new_rotation_y_matrix = false;
		bool new_rotation_z_matrix = false;

		if (ImGui::BeginPopup("new matrix"))
		{
			if (ImGui::Selectable("Identity"))
			{
				new_identity_matrix = true;
			}
			if (ImGui::Selectable("Scale"))
			{
				new_scale_matrix = true;
			}
			if (ImGui::Selectable("Translation"))
			{
				new_translation_matrix = true;
			}
			if (ImGui::Selectable("Rotation X"))
			{
				new_rotation_x_matrix = true;
			}
			if (ImGui::Selectable("Rotation Y"))
			{
				new_rotation_y_matrix = true;
			}
			if (ImGui::Selectable("Rotation Z"))
			{
				new_rotation_z_matrix = true;
			}
			ImGui::EndPopup();
		}

		auto DisplayNamePopupAndCreateMatrix = [this](bool raise_popup, char const * title, chaos::LightweightFunction<AnalyticMatrix(char const*)> func)
		{
#if 0
			DisplayNameSelectionPopup(raise_popup, title, [this, &func](char const* name)
			{
				NamedMatrix NewMatrix;
				NewMatrix.name = name;
				NewMatrix.matrix = func(name);
				matrixes.push_back(std::move(NewMatrix));
			});
#endif
		};

		DisplayNamePopupAndCreateMatrix(new_identity_matrix, "Identity", [this](char const * name)
		{
			return MakeIdentityMatrix();
		});

		DisplayNamePopupAndCreateMatrix(new_scale_matrix, "Scale", [this](char const* name)
		{
			return MakeScaleMatrix(name);
		});

		DisplayNamePopupAndCreateMatrix(new_translation_matrix, "Translation", [this](char const* name)
		{
			return MakeTranslationMatrix(name);
		});

		DisplayNamePopupAndCreateMatrix(new_rotation_x_matrix, "Rotation##x", [this](char const* name)
		{
			return MakeRotationMatrixX(name);
		});

		DisplayNamePopupAndCreateMatrix(new_rotation_y_matrix, "Rotation##y", [this](char const* name)
		{
			return MakeRotationMatrixY(name);
		});

		DisplayNamePopupAndCreateMatrix(new_rotation_z_matrix, "Rotation##z", [this](char const* name)
		{
			return MakeRotationMatrixZ(name);
		});
	}



	bool DisplayAnalyticMatrix(NamedMatrix const& m)
	{
		ImGui::PushID(&m);

		static ImVec2 p;
		ImGui::TextWrapped(m.name.c_str()); ImGui::SameLine();

		// popup for actions
		static std::optional<NamedMatrix const*> matrix_to_duplicate;
		if (ImGui::ArrowButton("##down", ImGuiDir_Down))
		{
			p = ImGui::GetCursorPos();
			matrix_to_duplicate.reset();
			ImGui::OpenPopup("element operation", ImGuiPopupFlags_MouseButtonRight);
		}

		ImGui::SetNextWindowPos(p, ImGuiCond_Always, ImVec2(0.0f, 0.0f));

		bool duplicate_matrix = false;
		bool delete_matrix = false;
		bool has_duplicated = false;
		if (ImGui::BeginPopup("element operation"))
		{
			if (ImGui::Selectable("delete"))
			{
				delete_matrix = true;
			}
			else if (ImGui::Selectable("duplicate"))
			{
				duplicate_matrix = true;
				matrix_to_duplicate = &m;
			}
			else if (ImGui::Selectable("copy to clipboard"))
			{
				chaos::WinTools::CopyStringToClipboard(m.matrix.ToCppString("m").c_str());
			}
			ImGui::EndPopup();
		}

		if (delete_matrix)
		{
			size_t index = &m - &matrixes[0];
			matrixes.erase(matrixes.begin() + index);
		}
		else
		{
			DisplayNameSelectionPopup(duplicate_matrix, "Duplicate", [this, &matrix_to_duplicate = matrix_to_duplicate, &has_duplicated](char const* name) mutable
			{
				NamedMatrix NewMatrix;
				NewMatrix.name = name;
				NewMatrix.matrix = matrix_to_duplicate.value()->matrix;
				matrixes.push_back(std::move(NewMatrix));
				matrix_to_duplicate.reset();
				has_duplicated = true;
			});

			if (!has_duplicated) // don't displace matrix because the current matrix parameter may become invalid due to vector resize
			{
				if (ImGui::BeginTable("AnalyticMatrix", 4, ImGuiTableFlags_Resizable | ImGuiTableFlags_RowBg))
				{
					for (size_t y = 0; y < 4; ++y)
					{
						for (size_t x = 0; x < 4; ++x)
						{
							ImGui::TableNextColumn();
							ImGui::Text("%s", ToString(m.matrix(x, y)).c_str());
						}
					}
					ImGui::EndTable();
				}
			}
		}
		ImGui::PopID();

		return delete_matrix || has_duplicated;
	}

#endif

protected:

	ImGuiNameSelectionPopup name_selection_popup;

	ImGuiNewMatrixPopup new_matrix_popup;

	std::vector<NamedMatrix> matrixes;
};

// ----------------------------------------------------------------------------------------

int main(int argc, char ** argv, char ** env)
{
	return chaos::RunImGuiApplication<MyImGuiObject>(argc, argv, env);
}