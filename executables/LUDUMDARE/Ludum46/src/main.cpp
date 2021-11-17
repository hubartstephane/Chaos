#include <chaos/Chaos.h>
#include "Ludum46Game.h"

#if 0
#define M_E        2.71828182845904523536   // e
#define M_LOG2E    1.44269504088896340736   // log2(e)
#define M_LOG10E   0.434294481903251827651  // log10(e)
#define M_LN2      0.693147180559945309417  // ln(2)
#define M_LN10     2.30258509299404568402   // ln(10)
#define M_PI       3.14159265358979323846   // pi
#define M_PI_2     1.57079632679489661923   // pi/2
#define M_PI_4     0.785398163397448309616  // pi/4
#define M_1_PI     0.318309886183790671538  // 1/pi
#define M_2_PI     0.636619772367581343076  // 2/pi
#define M_2_SQRTPI 1.12837916709551257390   // 2/sqrt(pi)
#define M_SQRT2    1.41421356237309504880   // sqrt(2)
#define M_SQRT1_2  0.707106781186547524401  // 1/sqrt(2)
#endif

class RealValue
{
public:

	/** copy construct */
	RealValue(RealValue const& src) = default;
	/** constructor with float */
	RealValue(float in_value):
		float_value(in_value),
		double_value(double(in_value)) {}
	/** constructor with double */
	RealValue(double in_value):
		float_value(float(in_value)),
		double_value(in_value){}

	/** conversion into float */
	operator float() const
	{
		return float_value;
	}
	/** conversion into double */
	operator double() const
	{
		return double_value;
	}

protected:

	float float_value;

	double double_value;
};

float operator + (float src1, RealValue const& src2)
{
	return src1 + float(src2);
}

float operator + (RealValue const & src1, float src2)
{
	return float(src1) + src2;
}

namespace MathConst
{
	RealValue PI = M_PI;

}

enum Toto
{
	Titi,
	Truc
};

class AAA
{
};

int CHAOS_MAIN(int argc, char ** argv, char ** env)
{
	{
		bool b1 = std::is_enum_v<chaos::ShaderType>;
		bool b2 = std::is_enum_v<Toto>;
		bool b3 = std::is_enum_v<int>;
		bool b4 = std::is_enum_v<AAA>;

		b1 = b1;
	}
	


#if 0
			//	glm::translate(glm::vec3(position, 0.0f)) * 
			//	GetRotatorMatrix(rotator) * 
			//	glm::scale(glm::vec3(scale, 1.0f));
#endif

	chaos::SceneTransform<float, 2> a;
	chaos::SceneTransform<float, 3> b;
	chaos::SceneTransform<double, 2> c;
	chaos::SceneTransform<double, 3> d;

	{

		auto ma1 = a.GetLocalToParent();
		auto mb1 = b.GetLocalToParent();
		auto mc1 = c.GetLocalToParent();
		auto md1 = d.GetLocalToParent();

		auto ma2 = a.GetParentToLocal();
		auto mb2 = b.GetParentToLocal();
		auto mc2 = c.GetParentToLocal();
		auto md2 = d.GetParentToLocal();

		ma2 = ma2;
	}

	glm::vec3 v = glm::vec3(0);
	glm::vec3 v1 = glm::vec3(1);


	//auto a = 2.0f + MathConst::PI;
	//auto b = MathConst::PI + 3.0f;

	chaos::WindowParams params;
	params.monitor = nullptr;
	params.width = 500;
	params.height = 500;
	params.monitor_index = 0;

	chaos::WindowHints hints;

	return chaos::RunGame<LudumGame>(argc, argv, env, params, hints);
}
