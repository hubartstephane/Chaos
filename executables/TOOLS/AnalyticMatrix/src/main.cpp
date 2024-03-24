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
	if (src->type == MatrixComponentType::Add)
		return std::format("({} + {})", ToString(src->operand1), ToString(src->operand2));
	if (src->type == MatrixComponentType::Mul)
		return std::format("({} * {})", ToString(src->operand1), ToString(src->operand2));

	return "?";
}


// ===============================================================================

class Matrix
{
public:

	Matrix() = default;

	Matrix(Matrix const& src);

	Matrix(Matrix&& src);

	~Matrix();

	Matrix& operator = (Matrix const& src);
	Matrix& operator = (Matrix && src);

	MatrixComponent*& operator () (size_t x, size_t y)
	{
		return components[x + y * 4];
	}

	MatrixComponent const* const& operator () (size_t x, size_t y) const
	{
		return components[x + y * 4];
	}

	void Clear();

public:

	std::array<MatrixComponent*, 16> components = { nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr };
};

// ===============================================================================

Matrix::Matrix(Matrix const& src)
{
	for (size_t i = 0; i < 16; ++i)
		components[i] = Clone(src.components[i]);
}

Matrix::Matrix(Matrix&& src)
{
	for (size_t i = 0; i < 16; ++i)
	{
		components[i] = src.components[i];
		src.components[i] = nullptr;
	}
}

Matrix& Matrix::operator = (Matrix const& src)
{
	if (&src != this)
	{
		Clear();
		for (size_t i = 0; i < 16; ++i)
			components[i] = Clone(src.components[i]);
	}
	return *this;
}

Matrix& Matrix::operator = (Matrix && src)
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


void Matrix::Clear()
{
	for (MatrixComponent * & component : components)
	{
		delete component;
		component = nullptr;
	}
}

Matrix::~Matrix()
{
	Clear();
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

Matrix ScaleMatrix(glm::vec3 const & v)
{
	Matrix result;
	result(0, 0) = new MatrixComponent(v.x);
	result(1, 1) = new MatrixComponent(v.y);
	result(2, 2) = new MatrixComponent(v.z);
	result(3, 3) = new MatrixComponent(1.0f);
	return result;
}

Matrix ScaleMatrix(std::string x, std::string y, std::string z)
{
	Matrix result;
	result(0, 0) = new MatrixComponent(std::move(x));
	result(1, 1) = new MatrixComponent(std::move(y));
	result(2, 2) = new MatrixComponent(std::move(z));
	result(3, 3) = new MatrixComponent(1.0f);
	return result;
}


Matrix IdentityMatrix()
{
	return ScaleMatrix({ 1.0f, 1.0f, 1.0f });
}

Matrix TranslationMatrix(glm::vec3 const& v)
{
	Matrix result = IdentityMatrix();
	result(0, 3) = new MatrixComponent(v.x);
	result(1, 3) = new MatrixComponent(v.y);
	result(2, 3) = new MatrixComponent(v.z);
	return result;
}

Matrix TranslationMatrix(std::string x, std::string y, std::string z)
{
	Matrix result = IdentityMatrix();
	result(0, 3) = new MatrixComponent(std::move(x));
	result(1, 3) = new MatrixComponent(std::move(y));
	result(2, 3) = new MatrixComponent(std::move(z));
	return result;
}

class MyImGuiObject : public chaos::ImGuiObject
{
public:

	MyImGuiObject()
	{
		//m = TranslationMatrix({ 3.0f, 5.0f, 7.0f });
		//m = m * m;

		m = TranslationMatrix("A", "B", "C");

		m = m * m;
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
	return chaos::RunImGuiApplication<MyImGuiObject>(argc, argv, env);
}