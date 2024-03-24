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











class MyImGuiObject : public chaos::ImGuiObject
{
public:

	MyImGuiObject()
	{
		AnalyticMatrix S = MakeScaleMatrix("S");

		//m = TranslationAnalyticMatrix({ 3.0f, 5.0f, 7.0f });
		//m = m * m;


		m = MakeRotationMatrixZ("Rot");
	}

	virtual void OnDrawImGuiContent() override
	{
		DisplayAnalyticMatrix(m, "T * S");

		if (ImGui::Button("New"))
		{

		}
	}

	void DisplayAnalyticMatrix(AnalyticMatrix const& m, char const * name) const
	{
		ImGui::PushID(&m);
		if (name != nullptr)
			ImGui::SeparatorText(name);
		if (ImGui::BeginTable("AnalyticMatrix", 4, ImGuiTableFlags_Resizable | ImGuiTableFlags_RowBg))
		{
			for (size_t y = 0; y < 4; ++y)
			{
				for (size_t x = 0; x < 4; ++x)
				{
					ImGui::TableNextColumn();
					ImGui::Text("%s", ToString(m(x, y)).c_str());
				}
			}
			ImGui::EndTable();
		}
		if (ImGui::Button("Clipboard"))
			chaos::WinTools::CopyStringToClipboard(m.ToCppString("m").c_str());
		ImGui::PopID();
	}

protected:

	AnalyticMatrix m;
};






int main(int argc, char ** argv, char ** env)
{
	return chaos::RunImGuiApplication<MyImGuiObject>(argc, argv, env);
}