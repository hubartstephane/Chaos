#include "chaos/Chaos.h"


enum class Operator
{
	Add,
	Mul
};

enum class MatrixComponentType
{
	Value,
	Name,
	Add,
	Mul
};

// ===============================================================================

class MatrixComponent
{
public:

	MatrixComponent() = default;

	MatrixComponent(float in_value)
	{
		value = in_value;
		type = MatrixComponentType::Value;
	}

	MatrixComponent(std::string in_name)
	{
		name = std::move(in_name);
		type = MatrixComponentType::Name;
	}

	~MatrixComponent();

public:

	MatrixComponentType type = MatrixComponentType::Value;

	float value = 0;

	std::string name;

	MatrixComponent* operand1 = nullptr;

	MatrixComponent* operand2 = nullptr;
};

// ===============================================================================

MatrixComponent::~MatrixComponent()
{
	delete(operand1);
	delete(operand2);
}

MatrixComponent* Clone(MatrixComponent const* src)
{
	if (src == nullptr)
		return nullptr;
	MatrixComponent* result = new MatrixComponent;
	if (result != nullptr)
	{
		result->type = src->type;
		switch (src->type)
		{
		case MatrixComponentType::Value:
			result->value = src->value;
			break;
		case MatrixComponentType::Name:
			result->name = src->name;
			break;
		case MatrixComponentType::Add:
		case MatrixComponentType::Mul:
			result->operand1 = Clone(src->operand1);
			result->operand2 = Clone(src->operand2);
			break;
		}
	}
	return result;
}

bool IsZero(MatrixComponent const* src)
{
	if (src == nullptr)
		return true;
	if (src->type == MatrixComponentType::Value && src->value == 0.0f)
		return true;
	return false;
}

bool IsOne(MatrixComponent const* src)
{
	if (src == nullptr)
		return false;
	if (src->type == MatrixComponentType::Value && src->value == 1.0f)
		return true;
	return false;
}

MatrixComponent* Add(MatrixComponent const* src1, MatrixComponent const* src2)
{
	if (IsZero(src1))
		return Clone(src2);
	if (IsZero(src2))
		return Clone(src1);

	MatrixComponent* result = new MatrixComponent;
	if (result != nullptr)
	{
		if (src1->type == MatrixComponentType::Value && src2->type == MatrixComponentType::Value)
		{
			result->type = MatrixComponentType::Value;
			result->value = src1->value + src2->value;
		}
		else
		{
			result->type = MatrixComponentType::Add;
			result->operand1 = Clone(src1);
			result->operand2 = Clone(src2);
		}
	}
	return result;
}

MatrixComponent* Mul(MatrixComponent const* src1, MatrixComponent const* src2)
{
	if (IsZero(src1) || IsZero(src2))
		return nullptr;
	if (IsOne(src1))
		return Clone(src2);
	if (IsOne(src2))
		return Clone(src1);
	MatrixComponent* result = new MatrixComponent;
	if (result != nullptr)
	{
		if (src1->type == MatrixComponentType::Value && src2->type == MatrixComponentType::Value)
		{
			result->type = MatrixComponentType::Value;
			result->value = src1->value * src2->value;
		}
		else
		{
			result->type = MatrixComponentType::Mul;
			result->operand1 = Clone(src1);
			result->operand2 = Clone(src2);
		}
	}
	return result;
}

std::string ToString(MatrixComponent const* src)
{
	if (src == nullptr)
		return "0";
	if (src->type == MatrixComponentType::Value)
		return std::format("{}", src->value);
	if (src->type == MatrixComponentType::Name)
		return src->name;


	return "?";
}


// ===============================================================================

class Matrix
{
public:

	Matrix() = default;

	Matrix(Matrix const& in_src);

	Matrix(Matrix&& in_src);

	~Matrix();

	MatrixComponent*& operator () (size_t x, size_t y)
	{
		return components[x + y * 4];
	}

	MatrixComponent const* const& operator () (size_t x, size_t y) const
	{
		return components[x + y * 4];
	}

public:

	std::array<MatrixComponent*, 16> components = { nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr };
};

// ===============================================================================

Matrix::Matrix(Matrix const& in_src)
{
	for (size_t i = 0; i < 16; ++i)
		if (in_src.components[i] != nullptr)
			components[i] = Clone(in_src.components[i]);
}

Matrix::Matrix(Matrix&& in_src)
{
	for (size_t i = 0; i < 16; ++i)
	{
		components[i] = in_src.components[i];
		in_src.components[i] = nullptr;
	}
}

Matrix::~Matrix()
{
	for (MatrixComponent* component : components)
		delete component;
}


Matrix operator * (Matrix const& m1, Matrix const& m2)
{
	Matrix result;

	for (size_t y = 0; y < 4; ++y)
	{
		for (size_t x = 0; x < 4; ++x)
		{
			MatrixComponent* component = Mul(m1(0, y), m2(x, 0));
			for (size_t i = 1; i < 4; ++i)
				component = Add(component, Mul(m1(i, y), m2(x, i)));
			result(x, y) = component;
		}
	}
	return result;
}



class MyImGuiObject : public chaos::ImGuiObject
{
public:

	MyImGuiObject()
	{
		for (size_t i = 0; i < 4; ++i)
			m(i, i) = new MatrixComponent(float(i + 1));

		for (size_t y = 0; y < 4; ++y)
		{
			for (size_t x = 0; x < 4; ++x)
			{
				std::string c = ToString(m(x, y));
			}
		}
	}

	virtual void OnDrawImGuiContent() override
	{
		DisplayMatrix(m);
	}

	void DisplayMatrix(Matrix const& m) const
	{
		if (ImGui::BeginTable("Matrix", 4, ImGuiTableFlags_Resizable | ImGuiTableFlags_RowBg))
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


	}

protected:

	Matrix m;
};






int main(int argc, char ** argv, char ** env)
{
	return chaos:: RunImGuiApplication<MyImGuiObject>(argc, argv, env);
}