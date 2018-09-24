--------------------------------------
Utilisation de Visual Studio 2012 = 11
--------------------------------------


=> NANA

  Semble etre une lib sympa pour faire de l'UI (necessite vs2013 ou VS2015)

  J'ai utilisé la solution .sln 2015 founie et compile : resultat dans build/bin



     quelques modification dans les .vcxproj/.sln

     passer le character set a "Not Set"
     dans code generation, utiliser RUNTIME LIBRARY en DLL/Debug DLL (comme les autres projets chaos)


=> FFMPEG 

  abandonne pour l instant l'idee de l utiliser. Compilation marche pas. Pas reussi a trouver les lib deja compilee (sauf utilisation de mingwin)

=>libxml2

  probleme avec cette lib
  
=> tinyxml2 

  se rabat sur cette lib qui semble bien marcher
  
  compilation du projet .sln
  
  
  Note : avec VS2015, j ai du convertir le .sln de 10 => 15 et tout recompiler
  


=> DXSDK

  si l install du SDK echoue avec S1023 c'est qu'un redistribuable visual 2010 plus recent est deja installe (https://support.microsoft.com/en-us/kb/2728613)
  
  -il faut le desinstalle
  
    MsiExec.exe /passive /X{F0C3E5D1-1ADE-321E-8167-68EF0DE699A5}
    MsiExec.exe /passive /X{1D8E6291-B0D5-35EC-8441-6616F567A0F7}  
  
  -puis installer le SDK 
  
  -et reinstalle le redistribuable visual 2010 (https://www.microsoft.com/en-us/download/details.aspx?id=26999)
  
    vcredist_x64.exe et vcredist_x86.exe
    
=> assimp (DXSDK semble necessaire)

  - cd cmake_file_shu_win32
  - cmake -G"Visual Studio 11" ..     (ca genere un projet uniquement pour x86)
  
  resultat apres build de la solution dans 
  
    \assimp-3.1.1\cmake_file_shu_win32\code\Release
    \assimp-3.1.1\cmake_file_shu_win32\code\Debug
    
    on creer un repertoire \assimp-3.1.1\lib32 et recopie le link
  
  
  - cd cmake_file_shu_win64
  - cmake -G"Visual Studio 11 Win64" ..     (ca genere un projet uniquement pour x64)


             

=> LUABIND

  semble obsolete par rapport a la nouvelle version de lua
  
    
  
  
  
  
=> ZLIB

  il faut prendre les deux archives   
  
    zlib124_masm_obj.zip
    zlib128.zip
    
  sinon on peut pas compiler x64 en release parcequ'il manque les fichiers ...obj obtenus a partir de ASM
  
  1/ en prends la solution, et on Compile zlibStat (bibliotheques statiques)
  
  2/ Finalement l'etape precedente ne marche pas trop
     On genere la solution avec CMAKE
     
     cmake -G"Visual Studio 11"  
     compile + garde copy de zlibstatic.lib / zlibstaticd.lib dans repetoire pour archi 32
     
     cmake -G"Visual Studio 11 Win64"
     compile + garde copy de zlibstatic.lib / zlibstaticd.lib dans repetoire pour archi 64           

=> IRRKLANG

  a sound library
  seems more 'free' than FMOD's
  
  I renamed installation directories
  
  


=> FMOD Studio 

  next than FMOD EX


=> FMOD ex      (WIN32 only ??)

   FMOD Ex semble etre legacy et le predecesseur de FMOD studio

  - fmodex.dll  sans LOG => fmodex_vc.lib
  - fmodexL.dll avec LOG => fmodex_vcL.lib
  





=> freetype2

  solution freetype.sln dans freetype-2.5.5\builds\windows\vc2010
  
  creer les les dans
  
      -freetype-2.5.5\objs\vc2010\Win32
      -freetype-2.5.5\objs\vc2010\x64
  

  

=> GLI

  Attention : Interet semble tres limité
  lecture uniquement de fichier DDS, et a partir du disk !!!

  header only lib
  
  BEWARE, windows.h sucks ... #define max(...)
  breaks gli (and glm ?)
  
  so this must be included before windows.h (so is for glew.h)
  
  
      #include <gli/gli.hpp>

      // must be included before gl/gl.h
      #define GLEW_STATIC // because we are using static library else we should link with SHARED library, and copy the dll in concerned directory
      #include <gl/glew.h>
      #include <gl/wglew.h>
      
      #include <Windows.h> // required by gl/gl.h
      #include <gl/gl.h>
      
      
      #include <glfw/glfw3.h>  

=> LUA

  il y a un fichier premake dedié a lua
  a copier dans le repertoire de lua et lancer


=> GLFW3

  i use the extraction of glfw-3.1.1.bin.WIN32.zip & glfw-3.1.1.bin.WIN64.zip
  that contains already the library for visual studio
  (instead of source code package)
  
=> BOOST

  building boost with
  
  ./bootstrap

  b2 address-model=64 => plein de warning "possible loss of data" 
  renommer stage/lib en stage/vc110/lib64
  
  b2 clean
  
  b2 address-model=32   
  renommer stage/lib en stage/vc110/lib32
  
  
  
  XXX :attention, le toolset par defaut (visual 11,12 ou 15  vc110, vc120 ou vc140) semble venir de la version installée
  
  
  
  
  
  
=> GLEW

  .. GLEW_MX est une version de glew qui utilise in contexte GLEWContext
  pour stocker les pointeurs de fonctions plutot que des variables globales
  
  
  Probleme : je veux utiliser
  
        Visual Studio 2012 = 11 
     
     alors que la solution fournie est
     
        Visual Studio 2013 = 12


    ouvre glew.sln
    manuellement dans les proprietes des 4 projets, changer le toolset de v120 vers v110
    
    edite a la main 
      F:\programmation\external\glew-1.12.0\build\vc12\glew_.....vcxproj
      
      et remplace ToolsVersion="12.0" par ToolsVersion="4.0"
      
      permet d'utiliser Visual Studio 2012 = 11
      a la place de Visual Studio 2013 = 12
      
    recompiler debug/release/Win32/x64
    
            
            ATTENTION : 
            ---------
            
            quand on utilise GLEW, on utilise soit la version STATIC, soit la version SHARED 
            du coup comme on utilise les memes headers, il faut annoncer ce qu on va utiliser            
            
              GLEW_STATIC pour static
              rien        pour importer SHARED (dllimport)
              GLEW_BUILD  pour exporter SHARED (dllexport) 
              
            
              quand on utilise la lib GLEW il faut faire comme suit
              
              #define GLEW_STATIC // si on est en static seulement, sinon rien
              #include <gl/glew.h>
              #include <gl/wglew.h>
              
             et pour le link
            
            -glew32sd.lib et glew32s.lib pour la lib STATIC   (d pour debug)
            
            -glew32d.lib et glew32.lib pour la version SHARED (d pour debug)
            
            
FREEIMAGE
---------

  -le pb de cette library est sur la gestion des GIF animé
  -l interface de lecture d'une image simple et d'une image animée n'est pas la meme
  
     FIBITMAP
     FIMULTIBITMAP
     
     => avec FIMULTIBITMAP, on peut lire plusieurs page d images
     
     => deux soucis se posent :
     
       - si on utilise un MEMORY (et donc un biuffer perso) pour decrypter l animation, il faut que sa durée de vie (et donc celle du buffer qu on fournit)
         soit plus grande que le FIMULTIBITMAP. Aie !
       
       - pire : on ne peut pas differencier une image a animée d'une image simple. 
         On peut utiliser FIMULTIBITMAP pour une image simple. On aura bien que le nombre de PAGES est 1
         PB : on arrive pas a locker cette page.
         
OPENCV
------

  en cours d evaluation
  une compilation manuelle pour win32 est necessaire
  pas reussie
  OpenCV n'a pas l'air de supporter GIF !!!
  abandonne OpenCV
  Retour a FreeImage

  