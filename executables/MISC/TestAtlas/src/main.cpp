#include <chaos/SimpleWin32Class.h>
#include <chaos/SimpleWin32Window.h>
#include <chaos/WinTools.h>
#include <chaos/StringTools.h>
#include <chaos/TextureAtlas.h>
#include <chaos/TextureAtlasHTMLGenerator.h>
#include <chaos/MathTools.h>
#include <chaos/Application.h>
#include <chaos/FileTools.h>
#include <chaos/FontTools.h>
#include <chaos/FontAtlas.h>

#include <chaos/Metaprogramming.h>
#include <chaos/EmptyClass.h>

static int ATLAS_BPP = 0;

void TestAtlasReload(boost::filesystem::path const & filename)
{
  chaos::TextureAtlas atlas;
  atlas.LoadAtlas(filename);
}

void TestAtlasDebugMode(boost::filesystem::path const & dest_p, boost::filesystem::path const & resources_path)
{
  chaos::TextureAtlasInput input;

  input.AddFakeImageSource("A");
  input.AddFakeImageSource("B");
  input.AddFakeImageSource("C");
  input.AddFakeImageSource("D");
  input.AddFakeImageSource("E");
  input.AddFakeImageSource("F");
  input.AddFakeImageSource("G");
  input.AddFakeImageSource("H");
  input.AddFakeImageSource("I");
  input.AddFakeImageSource("J");

  chaos::TextureAtlas                atlas;
  chaos::TextureAtlasGenerator       generator;
  chaos::TextureAtlasGeneratorParams params = chaos::TextureAtlasGeneratorParams(256, 256, 3, ATLAS_BPP);
  
  if (generator.ComputeResult(input, atlas, params))
  {  
    chaos::TextureAtlasHTMLOutputParams html_params;
    html_params.show_header       = true;
    html_params.show_atlas_header = true;
    html_params.texture_scale     = 3.0f;
    html_params.auto_refresh      = false;

    boost::filesystem::path html_path = dest_p / "Atlas_Final.html";
    chaos::TextureAtlasHTMLGenerator::OutputToHTMLFile(atlas, html_path.string().c_str(), html_params);

    boost::filesystem::path dst_dir = dest_p / "AtlasResultFake" / "MyAtlas.x";
    atlas.SaveAtlas(dst_dir);
  }
}

void TestAtlasNormalMode(boost::filesystem::path const & dest_p, boost::filesystem::path const & resources_path)
{
  // XXX : the resources directory contains
  //        - a bmp image whose extension has been swapped to ".txt" => detected has an image
  //        - a text file                                            => not detected has an image
  // correct behavior 

  chaos::TextureAtlasGeneratorParams params = chaos::TextureAtlasGeneratorParams(512, 512, 10, ATLAS_BPP);

  boost::filesystem::path result_path = dest_p / "AtlasResult" / "MyAtlas.json";

  chaos::TextureAtlasGenerator::CreateAtlasFromDirectory(resources_path, result_path, params);
}

void TestAtlasFont(boost::filesystem::path const & dest_p, boost::filesystem::path const & resources_path)
{ 
  boost::filesystem::path font_path = resources_path / "unispace bold italic.ttf";

  boost::filesystem::path dst_dir1 = dest_p / "AtlasResultFont" / "MyAtlas";
  boost::filesystem::path dst_dir2 = dest_p / "AtlasResultFontReloaded" / "MyAtlas";

  chaos::FontAtlas          atlas;
  chaos::FontAtlasGenerator generator;
  chaos::FontAtlasInput     input;
  input.AddFont(nullptr, font_path.string().c_str(), nullptr, true, chaos::FontAtlasFontParams());
  input.AddTextureFilesFromDirectory(resources_path / "ButtonImages");

  chaos::FontAtlasGeneratorParams params = chaos::FontAtlasGeneratorParams(512, 512, 10, ATLAS_BPP);
  if (generator.ComputeResult(input, atlas, params))
    atlas.SaveAtlas(dst_dir1);

  chaos::FontAtlas atlas_reloaded;
  if (atlas_reloaded.LoadAtlas(dst_dir1))
    atlas_reloaded.SaveAtlas(dst_dir2);
}




class BB
{

};

class Base : public chaos::ConditionnalAddVirtualDestructor<boost::mpl::true_, BB>
{
public:

	~Base()
  {
    std::cout << "Base::destructor" << std::endl;
  }

};

class C : public Base
{
public:

	~C()
	{
		std::cout << "C::destructor" << std::endl;
	}
};

bool IsTrue(boost::mpl::true_)
{
	return true;
}

bool IsTrue(boost::mpl::false_)
{
	return false;
}





int _tmain(int argc, char ** argv, char ** env)
{
  chaos::Application::Initialize<chaos::Application>(argc, argv, env);

  chaos::WinTools::AllocConsoleAndRedirectStdOutput();

  //BB * b = new chaos::ConditionnalAddVirtualDestructor<boost::mpl::true_, Base>;

	Base * b = new C;

  delete(b);

	b = nullptr;


	bool t1 = IsTrue(chaos::has_nocopy_tag<chaos::NoCopy>::type());
	bool t2 = IsTrue(chaos::has_nocopy_tag<chaos::VirtualDestructor>::type());

//  B b;
//  B b2;

	{

	using vec = boost::mpl::vector<chaos::EmptyClass, chaos::NoCopy, int>;

	using has1 = chaos::meta::has_satisfying_element<vec, boost::is_same<int,boost::mpl::_> >;
	using has2 = chaos::meta::has_satisfying_element<vec, boost::is_same<float,boost::mpl::_> >;
	//using has3 = chaos::has_satisfying_element<vec, boost::mpl::identity<boost::mpl::true_ >>;
	//using has4 = chaos::has_satisfying_element<vec, boost::mpl::identity<boost::mpl::false_ >>;


	using has3 = chaos::meta::has_satisfying_element<vec, chaos::has_nocopy_tag<boost::mpl::_> >;
	using has4 = chaos::meta::has_satisfying_element<vec, chaos::has_virtual_destructor_tag<boost::mpl::_> >;

	bool t1 = IsTrue(has1::type());
	bool t2 = IsTrue(has2::type());
	bool t3 = IsTrue(has3::type());
	bool t4 = IsTrue(has4::type());
	//bool t3 = IsTrue(has3::type());
	//bool t4 = IsTrue(has4::type());

	int i = 0;
	++i;
	//using has1 = chaos::has_satisfying_element<vec, boost::mpl::true_>;
	//using has2 = chaos::has_satisfying_element<vec, boost::mpl::false_>;

	//bool t1 = IsTrue(has1::type());
	//bool t2 = IsTrue(has2());

#if 0

	using h1 = boost::is_same<
		boost::mpl::end,
		boost::mpl::find_if<vec, chaos::has_nocopy<boost::mpl::_>
		>;
	using h2 = boost::mpl::find_if<vec, chaos::has_nocopy<boost::mpl::_>>;

	bool t1 = IsTrue(h1::type());
	bool t2 = IsTrue(h2::type());
#endif
	}
	return 0;


#if 0

  chaos::NamedObjectArray<chaos::NamedObject> m1;
  chaos::NamedObjectArray<chaos::NamedObject*> m2;

  chaos::NamedObject * p1 = m1.GetElementByName(nullptr);
  chaos::NamedObject * p2 = m2.GetElementByName(nullptr);

  chaos::NamedObject * p3 = m1.GetElementByTag(0);
  chaos::NamedObject * p4 = m2.GetElementByTag(0);


	using b1 = chaos::meta::add_uniq_pointer<int *>;
	using b2 = chaos::meta::add_uniq_pointer<int>;

	b1 i;
	b2 ii;

#endif


  chaos::MathTools::ResetRandSeed();

  boost::filesystem::path dst_p;
  if (chaos::FileTools::CreateTemporaryDirectory("TestAtlas", dst_p))
  {
    boost::filesystem::path resources_path = chaos::Application::GetInstance()->GetResourcesPath();

    TestAtlasDebugMode(dst_p, resources_path);
    TestAtlasNormalMode(dst_p, resources_path / "Images");
    TestAtlasFont(dst_p, resources_path);

    TestAtlasReload(dst_p / "AtlasResult" / "MyAtlas");

    chaos::WinTools::ShowFile(dst_p.string().c_str());
  }

  chaos::Application::Finalize();

  return 0;
}


