#include <chaos/Chaos.h>

class A : public chaos::Object 
{
	CHAOS_DECLARE_OBJECT_CLASS2(A, chaos::Object);

};

class B : public A 
{
	CHAOS_DECLARE_OBJECT_CLASS2(B, A);

};

class C : public B 
{
	CHAOS_DECLARE_OBJECT_CLASS2(C, B);

};

class D 
{

};






class BBB
{
public:

	int value = 666;
	chaos::SubClassOf<A> aaa;

};

bool SaveIntoJSON(nlohmann::json& json, BBB const& src)
{
	if (!json.is_object())
		json = nlohmann::json::object();
	chaos::JSONTools::SetAttribute(json, "value", src.value);
	chaos::JSONTools::SetAttribute(json, "aaa", src.aaa);
	return true;
}

bool LoadFromJSON(nlohmann::json const & json, BBB & dst)
{
	if (!json.is_object())
		return false;
	chaos::JSONTools::GetAttribute(json, "value", dst.value);
	chaos::JSONTools::GetAttribute(json, "aaa", dst.aaa);
	return true;
}








#if 0

template<typename T>
bool LoadFromJSON(nlohmann::json const& json, SubClassOf<T>& dst)
{
	if (!json.is_string())
		return false;
	json
	return true;
}

#if 0
if (!json.is_object())
return false;
JSONTools::GetAttribute(json, "bitmap_index", dst.bitmap_index);
#endif

#endif

class T {};

int CHAOS_MAIN(int argc, char ** argv, char ** env)
{
  chaos::WinTools::AllocConsoleAndRedirectStdOutput();


  chaos::SubClassOf<chaos::Object> a1 = A::GetStaticClass();
  chaos::SubClassOf<A> a2 = A::GetStaticClass();
  chaos::SubClassOf<B> a3 = A::GetStaticClass();
  chaos::SubClassOf<A> a4 = B::GetStaticClass();
  chaos::SubClassOf<B> a5 = B::GetStaticClass();


  chaos::SubClassOf<A> a6 = chaos::SubClassOf<B>();
  chaos::SubClassOf<B> a7 = chaos::SubClassOf<B>();
  chaos::SubClassOf<A> a8 = chaos::SubClassOf<A>();
  chaos::SubClassOf<B> a9 = chaos::SubClassOf<A>();


  bool b1 = a1;
  bool b2 = a2;
  bool b3 = a3;
  bool b4 = a4;
  bool b5 = a5;

  bool b6 = a6;
  bool b7 = a7;
  bool b8 = a8;
  bool b9 = a9;

  auto pp = chaos::Class::FindClass<T>();




  nlohmann::json e = "toto";

  nlohmann::json f = 3;


//  SubClassOf<chaos::Object> a = chaos::Object::GetStaticClass();
  chaos::SubClassOf<chaos::Object> b = A::GetStaticClass();


  chaos::SubClassOf<A> xxx = chaos::SubClassOf<A>();







	///
	{
		BBB bbb;
		bbb.aaa = chaos::SubClassOf<B>();

		nlohmann::json eee;
		SaveIntoJSON(eee, bbb);

		auto s1 = eee.dump(2);





		BBB ccc;
		LoadFromJSON(eee, ccc);




		s1 = s1;

	}







  chaos::WinTools::PressToContinue();

  return 0;
}



