#include <stdio.h>
#include <stdlib.h>
#include <iostream>

#include <assimp/Importer.hpp> 
#include <assimp/scene.h>           
#include <assimp/postprocess.h>     

#include <irrKlang.h>

#include <ft2build.h>
#include FT_FREETYPE_H

#include <glm/glm.hpp>

#include <boost/thread/thread.hpp>
#include <boost/filesystem.hpp>
#include <boost/chrono.hpp>

#include <lua.hpp>

// must be included BEFORE windows.h
// because windows.h make a #define max(...) that collide with the lib
// GLEW include windows.h too
#include <gli/gli.hpp>
//#include "gtx/loader.hpp"

// must be included before gl/gl.h
#define GLEW_STATIC // because we are using static library else we should link with SHARED library, and copy the dll in concerned directory
#include <gl/glew.h>
#include <gl/wglew.h>


#include <glfw/glfw3.h>

#include <Windows.h> // required by gl/gl.h
#include <gl/gl.h>

#include "fmod.hpp"
#include "fmod_errors.h"

//#define ZLIB_WINAPI
#include <zlib.h>

#include "FreeImage.h"


#include <chaos/GeometryFramework.h>
#include <chaos/WinTools.h>
#include <chaos/LuaTools.h>


#include "tinyxml2.h"



void ParseXML(tinyxml2::XMLNode * node)
{
  if (node == nullptr)
    return;

  tinyxml2::XMLText    * t = node->ToText();
  tinyxml2::XMLElement * e = node->ToElement();
  if (t != nullptr)
  {
    std::cout << "TEXT:" << t->Value() << std::endl;
  }
  else if (e != nullptr)
  {
     std::cout << "ELEMENT:" << e->Value() << std::endl;

     tinyxml2::XMLAttribute const * att = e->FirstAttribute();
     while (att != nullptr)
     {
       std::cout << "ATTRIBUTE:" << att->Name() << " " << att->Value() << std::endl;
       att = att->Next();
     }
  }

  ParseXML(node->NextSibling());
  ParseXML(node->FirstChild());
}

void func()
{
  std::cout << "from another thread" << std::endl;
}

static void *l_alloc (void *ud, void *ptr, size_t /* osize */, size_t nsize) 
{
  if (nsize == 0) 
  {
    free(ptr);
    return NULL;
  }
  else
    return realloc(ptr, nsize);
}

#include <stdio.h>
#include <io.h>
#include <fcntl.h>
#include <windows.h>
#include <conio.h>




int _tmain(int argc, char ** argv, char ** env)
{
  chaos::WinTools::AllocConsoleAndRedirectStdOutput();



  // Test Freefont2
  FT_Library  library;
  if (!FT_Init_FreeType( &library ))
  {
    argc = argc;


  }
  

#if 0

  AllocConsole();
  HANDLE handle_out = GetStdHandle(STD_OUTPUT_HANDLE);
  int hCrt = _open_osfhandle((long) handle_out, _O_TEXT);
  FILE* hf_out = _fdopen(hCrt, "w");
  setvbuf(hf_out, NULL, _IONBF, 1);
  *stdout = *hf_out;

  HANDLE handle_in = GetStdHandle(STD_INPUT_HANDLE);
  hCrt = _open_osfhandle((long) handle_in, _O_TEXT);
  FILE* hf_in = _fdopen(hCrt, "r");
  setvbuf(hf_in, NULL, _IONBF, 128);
  *stdin = *hf_in;

  while(true);

  boost::filesystem::path application_path = boost::filesystem::path(argv[0]);
  application_path = application_path.parent_path();


  
  // Test GLEW

  GLenum err = glewInit();

  // Test GLM
  glm::fvec3 v = glm::fvec3(1.0f, 2.0f, 3.0f);
  v = v + v;

  // Test BOOST
  boost::filesystem::path p = boost::filesystem::current_path();

  boost::thread new_thread(func);
  new_thread.join();



  // Test FMOD
    FMOD::System *system = NULL;

  FMOD_RESULT fmod_res = FMOD::System_Create(&system);
  if (fmod_res == FMOD_OK)
  {

    system->release();
  }

  // Test LUA    
  lua_State *state = lua_newstate (l_alloc, NULL);
  if (state != NULL)
  {

    lua_close(state);
    state = NULL;
  }

    // Test GLI

  boost::filesystem::path dds_path = application_path / "resources" / "cube.dds";

  gli::texture2D TextureAtlasEntry(1, gli::RGBA8_UNORM, gli::texture2D::dimensions_type(256, 256)); 
  glm::u8vec4 * LinearAddress = TextureAtlasEntry.data<glm::u8vec4>(); 
  gli::storage StorageDDS(gli::load_dds(dds_path.string().c_str()));

  // Test GLFW3
  if (glfwInit())
  {


    
  }

    // Test IRRKLANG

  irrklang::ISoundDeviceList * deviceList = irrklang::createSoundDeviceList();
    if (deviceList != nullptr)
  {
    for (int i = 0 ; i < deviceList->getDeviceCount() ; ++i)
    printf("%d: [%s]  [%s]\n", i, deviceList->getDeviceDescription(i), deviceList->getDeviceID(i));
    deviceList->drop();
    deviceList = nullptr;
  }

  irrklang::ISoundEngine* engine = irrklang::createIrrKlangDevice();
  if (engine != NULL)
  {
    boost::filesystem::path ogg_path = application_path / "resources" / "getout.ogg";

    irrklang::ISound * sound = engine->play2D(ogg_path.string().c_str(), true /* looped */, false /*  start paused */, true /* track */, irrklang::ESM_AUTO_DETECT /* stearm mode */, true /* enable FX */);

    if (sound != NULL)
    {
      irrklang::ISoundEffectControl* fx = sound->getSoundEffectControl();
      if (fx != NULL)
      {
        // enable the echo sound effect for this sound
      //  fx->enableEchoSoundEffect();
      }
    }

    boost::this_thread::sleep_for(boost::chrono::milliseconds(1000));

    sound->drop();
    engine->drop();
  }


  // Test ZLIB
  z_stream strm;
    strm.zalloc = Z_NULL;
    strm.zfree = Z_NULL;
    strm.opaque = Z_NULL;
    strm.avail_in = 0;
    strm.next_in = Z_NULL;
    inflateInit(&strm);

  // Test ASSIMP
  Assimp::Importer importer;

  boost::filesystem::path assimp_path = application_path / "resources" / "ThreeCubesGreen.ASE";

  const aiScene* scene = importer.ReadFile( assimp_path.string().c_str(), 
        aiProcess_CalcTangentSpace       | 
        aiProcess_Triangulate            |
        aiProcess_JoinIdenticalVertices  |
        aiProcess_SortByPType);


  // Test FreeImage
  FreeImage_Initialise();
  boost::filesystem::path freeimage_path = application_path / "resources" / "logo.png";

  FREE_IMAGE_FORMAT FI_Format = FreeImage_GetFileType(freeimage_path.string().c_str(), 0);
  if (FI_Format != FIF_UNKNOWN)
  {
    FIBITMAP * FImg = FreeImage_Load(FI_Format, freeimage_path.string().c_str(), 0);
    if (FImg != nullptr)
    {
      unsigned int    width  = FreeImage_GetWidth(FImg);
      unsigned int    height = FreeImage_GetHeight(FImg);
      unsigned int    bpp    = FreeImage_GetBPP(FImg);
      FREE_IMAGE_TYPE type   = FreeImage_GetImageType(FImg);

      if (type == FIT_BITMAP)
      {
        BYTE * data = FreeImage_GetBits(FImg); // scanline DWORD aligned

      }
      FreeImage_Unload(FImg);
    }
  }
  FreeImage_DeInitialise();



  // Test tinyxml2

  boost::filesystem::path xml_path = application_path / "resources" / "HelloWorld.xml";



  tinyxml2::XMLDocument* doc = new tinyxml2::XMLDocument();
  if (doc != nullptr)
  {
    doc->LoadFile(xml_path.string().c_str());

    if (doc->ErrorID() == 0)
    {
      tinyxml2::XMLPrinter printer;
      doc->Print(&printer);      
      tinyxml2::XMLNode * root = doc->RootElement();
      ParseXML(root);
      std::cout << printer.CStr() << std::endl;
    }
    delete(doc);
  }




    // Test Chaos

  chaos::ray2 r2;
  chaos::ray3 r3;

  chaos::box2 b;

  b.half_size.x = 1;
  b.half_size.y = 2;

  bool bb = b.IsEmpty();

  float s = b.GetSurface();
  float p = b.GetPerimeter();

#endif

  return 0;
}
