#include <chaos/Chaos.h>

class A : public chaos::Object 
{
	CHAOS_OBJECT_DECLARE_CLASS2(A, chaos::Object);

};

class B : public A 
{
	CHAOS_OBJECT_DECLARE_CLASS2(B, A);

};

class C : public B 
{
	CHAOS_OBJECT_DECLARE_CLASS2(C, B);

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

bool SaveIntoJSON(nlohmann::json& json_entry, BBB const& src)
{
	if (!json_entry.is_object())
		json_entry = nlohmann::json::object();
	chaos::JSONTools::SetAttribute(json_entry, "value", src.value);
	chaos::JSONTools::SetAttribute(json_entry, "aaa", src.aaa);
	return true;
}

bool LoadFromJSON(nlohmann::json const & json_entry, BBB & dst)
{
	if (!json_entry.is_object())
		return false;
	chaos::JSONTools::GetAttribute(json_entry, "value", dst.value);
	chaos::JSONTools::GetAttribute(json_entry, "aaa", dst.aaa);
	return true;
}








#if 0

template<typename T>
bool LoadFromJSON(nlohmann::json const& json_entry, SubClassOf<T>& dst)
{
	if (!json_entry.is_string())
		return false;
	json_entry
	return true;
}

#if 0
if (!json_entry.is_object())
return false;
JSONTools::GetAttribute(json_entry, "bitmap_index", dst.bitmap_index);
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



