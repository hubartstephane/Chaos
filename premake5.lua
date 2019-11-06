-- =============================================================================
-- Some windows command reminders
-- =============================================================================

-- WTF : the os.execute(...) seems to switch / and \ on arguments for windows ...
--       causing the command to fail
-- 
-- MKLINK /H   new_file   src_file                  (link between files)
-- MKLINK /J   new_dir    src_dir                   (link between directories)

TYPE_EXECUTABLE       = 0
TYPE_LIBRARY          = 1
TYPE_EXTERNAL_LIBRARY = 2
TYPE_RESOURCES        = 3

BUILD_TARGET       = _ACTION
ROOT_PATH          = path.getdirectory(_SCRIPT)
SOLUTION_PATH      = path.join(ROOT_PATH, "..", "premaked_solutions", BUILD_TARGET)
EXTERNAL_PATH      = path.join(ROOT_PATH, "..", "external")
BUILD_PATH         = path.join(ROOT_PATH, ".." , "build", BUILD_TARGET)
BUILD_TOOLS_PATH   = path.join(ROOT_PATH, "build_tools") 
COPY_SCRIPT        = path.join(BUILD_TOOLS_PATH, "mycopy.py")
ZIP_SCRIPT         = path.join(BUILD_TOOLS_PATH, "myzip.py")
DOXYGEN_SCRIPT     = path.join(BUILD_TOOLS_PATH, "mydoxygen.py")
PROJ_NAME          = ""
PROJECT_PATH       = ""
PROJECT_SRC_PATH   = ""
PROJECT_BUILD_PATH = ""

CURRENT_GROUP      = nil

MYPROJECTS = {}

DISPLAY_ROOT_ENVIRONMENT = false
DISPLAY_ENVIRONMENT      = true
DISPLAY_DEPENDENCIES     = true

DEBUG   = "DEBUG"
RELEASE = "RELEASE"
x32     = "x32"
x64     = "x64"
win32   = "win32"
win64   = "win64"

INDENT = 1

-- ============================================================================= 
-- Type functions
-- =============================================================================

function IsTable(value)
  return (type(value) == "table")
end

function IsNil(value)
  return (type(value) == "nil")
end

function IsString(value)
  return (type(value) == "string")
end

function IsNumber(value)
  return (type(value) == "number")
end

-- ============================================================================= 
-- Functions to display a line
-- =============================================================================

function GetIndentText(indent)
  local txt = ""
  for i = 1, indent do
    txt = "  " .. txt  
  end 
  return txt  
end

function Output(txt)
  local ident_txt = GetIndentText(INDENT)  
  local lines     = string.explode(txt, "\n")
  for i in pairs(lines) do
    print(ident_txt .. "[" .. lines[i] .. "]")
  end    
end

-- ============================================================================= 
-- Get a string representation of an object
-- =============================================================================

function GetDebugRepresentationString(obj)
  if (IsNil(obj)) then
    return "nil"    
  end            
  if (IsNumber(obj)) then
    return tostring(obj)    
  end
  if (IsString(obj)) then
    return "\"" .. obj .. "\""    
  end
  if (IsTable(obj)) then
    local result = "{"
    local first  = true
    local k                  
    for k in pairs(obj) do
      if (not first) then
        result = result .. ", "
      end                  
      result = result .. GetDebugRepresentationString(k) .. " => " .. GetDebugRepresentationString(obj[k])          
      first = false             
    end        
    result = result .. "}"    
    return result  
  end                    
end

-- ============================================================================= 
-- Functions to display the environment
-- =============================================================================

function DisplayRootEnvironment()
  if (DISPLAY_ROOT_ENVIRONMENT) then
    Output("=======================================================================")
    Output("PREMAKE5 VERSION   : " .. _PREMAKE_VERSION)
    Output("ROOT_PATH          : " .. ROOT_PATH)
    Output("EXTERNAL_PATH      : " .. EXTERNAL_PATH)    
    Output("BUILD_TOOLS_PATH   : " .. BUILD_TOOLS_PATH)
    Output("COPY_SCRIPT        : " .. COPY_SCRIPT)
    Output("ZIP_SCRIPT         : " .. ZIP_SCRIPT)   
    Output("DOXYGEN_SCRIPT     : " .. DOXYGEN_SCRIPT)    
    Output("=======================================================================")
    Output("")
  end 
end

function DisplayEnvironment()
  if (DISPLAY_ENVIRONMENT) then
    Output("=======================================================================")
    Output("SCRIPT             : " .. _SCRIPT)
    Output("PROJ_NAME          : " .. PROJ_NAME)
    Output("PROJECT_PATH       : " .. PROJECT_PATH)
    Output("PROJECT_SRC_PATH   : " .. PROJECT_SRC_PATH)
    Output("PROJECT_BUILD_PATH : " .. PROJECT_BUILD_PATH)      
    Output("=======================================================================")
    Output("")
  end  
end

DisplayRootEnvironment()
DisplayEnvironment()

-- =============================================================================
-- An option to gain time and avoiding generation of doxygen documentation
-- CMD : premake5.exe vs2012 --no-auto-doc
-- =============================================================================

newoption {
   trigger     = "no-auto-doc",
   description = "Do not generate doxygen documentation when project is being built (do it manually)"
}

local AUTO_DOCUMENTATION = true 
if (_OPTIONS["no-auto-doc"]) then
  if (_OPTIONS["no-auto-doc"] == "") then
    AUTO_DOCUMENTATION = false
  elseif (string.lower(_OPTIONS["no-auto-doc"]) == "true") then
    AUTO_DOCUMENTATION = false
  end       
end

-- =============================================================================
-- Dependant project names
-- =============================================================================

function GetDependantDocumentationProjName(proj_name)
  return "Documentation of " .. proj_name
end

function GetDependantResourceProjName(proj_name)
  return "Resources of " .. proj_name
end

-- =============================================================================
-- Function to require copying a file or directory
-- =============================================================================

function TransformResourcePath(proj, value, plat, conf)

  if (not IsNil(value)) then
    local filename = value[plat][conf]

    local relative_path = true
    if (string.sub(filename, 1, 1) == "@") then -- the file is to be copied directly in the same directory than the executable itself
      relative_path = false
      filename      = string.sub(filename, 2)       
    end    

    local full_filename = path.join(proj.root_path, filename)
        
    if (not relative_path) then
      filename = path.getname(filename) -- remove the path before the filename    
    end        
    
    Output("DECLARE RESOURCE [" .. full_filename .. "] => [" .. filename .. "] for " .. plat .. " " .. conf)            
  
    value[plat][conf] = {filename, full_filename}          
  end      
end

function DeclareResourceHelper(proj, filename)

  local tmp = GetPlatConfArray(filename)
 
  TransformResourcePath(proj, tmp, x32, DEBUG)
  TransformResourcePath(proj, tmp, x32, RELEASE)  
  TransformResourcePath(proj, tmp, x64, DEBUG)      
  TransformResourcePath(proj, tmp, x64, RELEASE)
  
  table.insert(proj.tocopy[x32][DEBUG],   tmp[x32][DEBUG])   
  table.insert(proj.tocopy[x32][RELEASE], tmp[x32][RELEASE])
  table.insert(proj.tocopy[x64][DEBUG],   tmp[x64][DEBUG]) 
  table.insert(proj.tocopy[x64][RELEASE], tmp[x64][RELEASE])
end

function DeclareResource(filename)

  DeclareResourceHelper(FindProject(), filename)
   
end

-- =============================================================================
-- Function to initialize project for DEBUG
-- =============================================================================

function DebugConf(plat)
  configuration {DEBUG, plat}
    defines { DEBUG }
    defines { "_DEBUG" }
    symbols "On"
    flags { "MultiProcessorCompile" } 
end

-- =============================================================================
-- Function to initialize project for DEBUG
-- =============================================================================

function ReleaseConf(plat)
  configuration {RELEASE, plat}
    defines { "NDEBUG" }
    defines { RELEASE }
    defines { "_RELEASE" }
    optimize "On" 
    flags { "MultiProcessorCompile" } 
end

-- =============================================================================
-- Function to find a project in MYPROJECTS
-- =============================================================================

function FindProject(name) -- by default, this returns the current project
  name = name or PROJ_NAME 
  name = string.upper(name)
  for k in pairs(MYPROJECTS) do
    if (MYPROJECTS[k].name == name) then
      return MYPROJECTS[k]     
    end       
  end   
  return nil
end

-- =============================================================================
-- DeclareExternalLib
-- =============================================================================

function PrefixPathArray(src, prefix)
  prefix = prefix or EXTERNAL_PATH
  if (src[x32][DEBUG]) then   
    src[x32][DEBUG]   = path.join(prefix, src[x32][DEBUG])
  end  
  if (src[x64][DEBUG]) then  
    src[x64][DEBUG]   = path.join(prefix, src[x64][DEBUG])
  end  
  if (src[x32][RELEASE]) then  
    src[x32][RELEASE] = path.join(prefix, src[x32][RELEASE])
  end  
  if (src[x64][RELEASE]) then  
    src[x64][RELEASE] = path.join(prefix, src[x64][RELEASE])
  end  
  return src        
end

function DeclareExternalLib(external_name, inc_path, lib_path, libname, tocopy)

  Output("DECLARE EXTERNAL LIB [" .. external_name .. "]")

  local result = {}  
  table.insert(MYPROJECTS, result)
            
  result.name        = string.upper(external_name)
  result.proj_type   = TYPE_EXTERNAL_LIBRARY
  result.root_path   = EXTERNAL_PATH
  result.includedirs      = PrefixPathArray(GetPlatConfArray(inc_path), EXTERNAL_PATH)   
  result.targetdir        = PrefixPathArray(GetPlatConfArray(lib_path), EXTERNAL_PATH)
  result.libname          = GetPlatConfArray(libname)
  result.additionnal_libs = GetPlatConfArray({}) 
  
  result.dependencies = {}
  
  result.tocopy = GetPlatConfArray({})
  
  if (not IsNil(tocopy)) then
    DeclareResourceHelper(result, tocopy)
  end            
    
  return result    
end


-- =============================================================================
-- Function to get a platform/configuration array
-- =============================================================================

function HasPlatformKey(value)
  return (IsTable(value) and (not IsNil(value[x32]) or not IsNil(value[x64])))
end

function HasConfigKey(value)
  return (IsTable(value) and (not IsNil(value[DEBUG]) or not IsNil(value[RELEASE])))
end

function DeepCopy(value)
  if (IsTable(value)) then
    return table.deepcopy(value)  
  end  
  return value
end

function GetPlatConfArrayHelper(value, plat, conf)
  if (HasPlatformKey(value)) then
    if (HasConfigKey(value[plat])) then
      return value[plat][conf]      
    end
    return value[plat]    
  elseif (HasConfigKey(value)) then  
    if (HasPlatformKey(value[conf])) then
      return value[conf][plat]      
    end
    return value[conf]     
  end 
  return value           
end

function GetPlatConfArray(value)

   local result = { 
      x32 = { DEBUG = DeepCopy(GetPlatConfArrayHelper(value, x32, DEBUG)) , RELEASE = DeepCopy(GetPlatConfArrayHelper(value, x32, RELEASE)) }, 
      x64 = { DEBUG = DeepCopy(GetPlatConfArrayHelper(value, x64, DEBUG)) , RELEASE = DeepCopy(GetPlatConfArrayHelper(value, x64, RELEASE)) } 
   }      
   return result;    
end

-- =============================================================================
-- WindowedApp : entry point WindowsApp
-- =============================================================================

function GenDoxygen()
  FindProject().gendoxygen = true
end

-- =============================================================================
-- onConfig
-- =============================================================================

function onConfig(in_kind, plat, conf, proj)

   -- a message that is display when the projection is being build
   buildmessage("Make in_kind for plat config' : %{prj.name}")

   -- where the result EXE/LIB is been saved
   local targ = path.join(PROJECT_BUILD_PATH, conf, plat)
   targetdir(targ)
   
   proj.targetdir[plat][conf] = targ     
                                        
   -- where the includes are for the project
   local inc = path.join(PROJECT_SRC_PATH, "include")   
   includedirs(inc)
   
   proj.includedirs[plat][conf] = inc     
   
   --defines("CHAOS_PROJECT_PATH='".. PROJECT_PATH.."'")
   --defines("CHAOS_PROJECT_SRC_PATH='".. PROJECT_SRC_PATH.."'")     
   --defines("CHAOS_PROJECT_BUILD_PATH='".. targ .. "'")
   
           
   defines("CHAOS_PROJECT_PATH=\"".. PROJECT_PATH.."\"")          
   defines("CHAOS_PROJECT_SRC_PATH=\"".. PROJECT_SRC_PATH.."\"")     
   defines("CHAOS_PROJECT_BUILD_PATH=\"".. targ .. "\"")        
      
    
    characterset("ASCII")  
end

-- =============================================================================
-- ResourceProject : generate a non-visual project (just a data project or system)
-- =============================================================================

function ResourceProject()
                             
  local result = {}  
  table.insert(MYPROJECTS, result)
            
  result.name             = string.upper(PROJ_NAME)
  result.proj_type        = TYPE_RESOURCES
  result.path             = PROJECT_PATH    
  result.root_path        = PROJECT_SRC_PATH         
  result.build_path       = PROJECT_BUILD_PATH
  result.lua_project      = nil                
  result.targetdir        = GetPlatConfArray(nil);
  result.includedirs      = GetPlatConfArray(nil); 
  result.tocopy           = GetPlatConfArray({});
  result.gendoxygen       = false
  result.additionnal_libs = GetPlatConfArray({})  
    
  result.inc_path     = GetPlatConfArray(inc_path)      
  result.src_path     = GetPlatConfArray(src_path)
  result.res_path     = GetPlatConfArray(res_path)
  result.dependencies = {}
  
  return result          
    
end

-- =============================================================================
-- CppProject : entry point for all C++ Applications
-- =============================================================================

function CppProject(in_kind, proj_type)

  -- the name of the group
  local group_name = path.join(CURRENT_GROUP, PROJ_NAME)
  if (CURRENT_GROUP ~= nil) then
    group(group_name)   
  end
  
  -- create a project for the resources
  local resource_proj_name = GetDependantResourceProjName(PROJ_NAME)
  project (resource_proj_name)
    kind("Makefile")  
    
  
  -- create the project it self
  project(PROJ_NAME)
   
    local proj_location = path.join(SOLUTION_PATH, PROJECT_PATH)  
  
    location (proj_location)  
  
    local result = {}  
    table.insert(MYPROJECTS, result)
            
    result.name             = string.upper(PROJ_NAME)
    result.proj_type        = proj_type
    result.path             = PROJECT_PATH    
    result.root_path        = PROJECT_SRC_PATH         
    result.build_path       = PROJECT_BUILD_PATH
    result.lua_project      = project()               
    result.targetdir        = GetPlatConfArray(nil);
    result.includedirs      = GetPlatConfArray(nil); 
    result.tocopy           = GetPlatConfArray({});
    result.gendoxygen       = false
    result.group_name       = group_name
    result.proj_location    = proj_location
    result.additionnal_libs = GetPlatConfArray({});           
         
    kind(in_kind)
    
    language "C++"
    
    -- change entry point for windows (avoid WinMain to main)
    if (os.target() == "windows") then
      if (proj_type == TYPE_EXECUTABLE) then
        entrypoint "mainCRTStartup"
       end    
    end            
       
    local inc_path = path.join(PROJECT_SRC_PATH, "include")           
    local src_path = path.join(PROJECT_SRC_PATH, "src")
    local res_path = path.join(PROJECT_SRC_PATH, "resources")    
      
    -- some files including *.cpp, *.c, *.hpp, *.h
    local src_h   = path.join(PROJECT_SRC_PATH, "**.h")
    local src_hpp = path.join(PROJECT_SRC_PATH, "**.hpp")
    local src_c   = path.join(PROJECT_SRC_PATH, "**.c")
    local src_cpp = path.join(PROJECT_SRC_PATH, "**.cpp")              
    files {src_h, src_hpp, src_c, src_cpp}                      
                        
    DebugConf(x32)
      onConfig(in_kind, x32, DEBUG, result)
        
    ReleaseConf(x32)
      onConfig(in_kind, x32, RELEASE, result)        
        
    DebugConf(x64)
      onConfig(in_kind, x64, DEBUG, result)        
        
    ReleaseConf(x64)
      onConfig(in_kind, x64, RELEASE, result)
      
    result.inc_path     = GetPlatConfArray(inc_path)      
    result.src_path     = GetPlatConfArray(src_path)
    result.res_path     = GetPlatConfArray(res_path)
    result.dependencies = {}
  
  return result        
end

-- =============================================================================
-- WindowedApp : entry point WindowedApp
-- =============================================================================

function WindowedApp()

  local result = CppProject("WindowedApp", TYPE_EXECUTABLE)
  DeclareResource("config.lua")
  --DependOnLib("COMMON RESOURCES")       
  DisplayEnvironment()
  return result    
end

-- =============================================================================
-- WindowedApp : entry point WindowsApp
-- =============================================================================

function WindowedApp()
  local result = CppProject("WindowedApp", TYPE_EXECUTABLE)
  DeclareResource("config.lua")  
  --DependOnLib("COMMON RESOURCES")     
  DisplayEnvironment()
  return result    
end       

-- =============================================================================
-- StaticLib : entry point for libraries
-- =============================================================================

function StaticLib()
  local result = CppProject("StaticLib", TYPE_LIBRARY)
  result.libname = GetPlatConfArray(result.name)
  DisplayEnvironment()
  GenDoxygen()  
  return result          
end

-- =============================================================================
-- SharedLib : entry point for libraries
-- =============================================================================

function SharedLib()
  local result = CppProject("SharedLib", TYPE_LIBRARY)
  result.libname = GetPlatConfArray(result.name)
  DisplayEnvironment()
  GenDoxygen()  
  return result          
end

-- =============================================================================
-- DependOnStandardLib : declare a dependency
-- =============================================================================

function DependOnStandardLib(libname)

  if os.target() ~= "windows" then
    return
  end    

  local proj = FindProject()
  
  if (IsTable(libname)) then
    for i in pairs(libname) do
      DependOnStandardLib(libname[i])
    end
  else  
    table.insert(proj.additionnal_libs[x32][DEBUG],   libname)   
    table.insert(proj.additionnal_libs[x32][RELEASE], libname) 
    table.insert(proj.additionnal_libs[x64][DEBUG],   libname)    
    table.insert(proj.additionnal_libs[x64][RELEASE], libname)
  end
end

-- =============================================================================
-- Depend : declare a dependency
-- =============================================================================

function DependOnLib(libname)
  if (IsTable(libname)) then
    for i in pairs(libname) do
      DependOnLib(libname[i])
    end
  else 
    libname = libname string.upper(libname)
    table.insert(FindProject().dependencies, libname)
  end
end

-- =============================================================================
-- Test whether a file exists or not
-- =============================================================================

function FileExists(filename)
  local f = io.open(filename,"r")
  if (f ~= nil) then
    io.close(f)
    return true
  else
    return false
  end
end

-- =============================================================================
-- Do a file if it exists
-- =============================================================================

  -- XXX : there is a difference between 
  --       loadfile() + file() => same working directory (same environment ?)
  --       dofile()            => seems to start a new VM in new environment (new working directory)

function ProcessFile(filename, new_context)
  new_context = new_context or nil
  if (FileExists(filename)) then
    if (not new_context) then  
      local file = loadfile(filename)
      file()
    else
      dofile(filename)    
    end           
  end
end

function StoreEnvironment(names, result)
  result = result or {}
  for k in pairs(names) do   
    result[names[k]] = _G[names[k]]  
  end 
  return result
end

function RestoreEnvironment(env)
  for k in pairs(env) do 
    _G[k] = env[k]  
  end  
end  

function ProcessSubPremake(proj_name, sub_path, filename)

  INDENT = INDENT + 1

  local env = StoreEnvironment({"PROJECT_PATH", "PROJ_NAME", "PROJECT_SRC_PATH", "BUILD_PATH"})
  
  PROJ_NAME = string.upper(path.getbasename(proj_name))
    
  if (sub_path == nil) then
    Output("SUBDIRECTORY [" .. proj_name .. "]  sub_path = [nil]")           
    PROJECT_PATH       = path.join(PROJECT_PATH, path.getbasename(proj_name))
    PROJECT_SRC_PATH   = path.join(ROOT_PATH, PROJECT_PATH)
    PROJECT_BUILD_PATH = path.join(BUILD_PATH, PROJECT_PATH)
  else
    Output("SUBDIRECTORY [" .. proj_name .. "]  sub_path = [" .. sub_path .. "]")  
    PROJECT_PATH       = nil
    PROJECT_SRC_PATH   = sub_path  -- should only be executed for external libraries 
    PROJECT_BUILD_PATH = nil   
  end          
                                 
  filename = filename or "subpremake5.lua"                  
  ProcessFile(path.join(PROJECT_SRC_PATH, filename), false)
  RestoreEnvironment(env)
  
  INDENT = INDENT - 1  
        
end
   
-- =============================================================================
-- Project description
--
-- in some commands we can use some jockers such as
--   %{sln.name}
--   %{prj.location}
--
-- they are listed below: 
--  sln.name
--
--  prj.name
--  prj.language
--  
--  cfg.architecture
--  cfg.buildcfg
--  cfg.buildtarget
--  cfg.kind
--  cfg.linktarget
--  cfg.longname
--  cfg.objdir
--  cfg.platform
--  cfg.shortname
--  cfg.system
--  
--  file.abspath
--  file.basename
--  file.name
--  file.relpath
--       These values are available on build and link targets
--  target.abspath
--  target.basename
--  target.bundlename
--  target.bundlepath
--  target.directory
--  target.extension
--  target.name
--  target.prefix
--  target.relpath
--  target.suffix
--
-- And generic strings for portable commands:
--  {CHDIR}
--  {COPY}
--  {DELETE}
--  {ECHO}
--  {MKDIR}
--  {MOVE}
--  {RMDIR}
--  {TOUCH}
-- =============================================================================

solution "DeathAndChaos"
  platforms { x32, x64 }
  configurations { DEBUG , RELEASE }

  location (SOLUTION_PATH) -- where the visual studio project file is been created  

  if os.target() == "windows" then
    defines { "WINDOWS" }
  end    
  
  if os.target() == "linux" then
    defines { "LINUX" }
  end
   
  CURRENT_GROUP = nil
  ProcessSubPremake("common resources")
  
  CURRENT_GROUP = nil  
  --ProcessSubPremake("external", EXTERNAL_PATH, "external_premake5.lua")
  ProcessSubPremake("external", ".", "external_premake5.lua")
  
  CURRENT_GROUP = "libraries"         
  ProcessSubPremake("libraries")
         
  CURRENT_GROUP = "executables"
  ProcessSubPremake("executables")  

-- =============================================================================
-- Propagate all dependencies
-- =============================================================================

local some_changes = true

while(some_changes) do
  some_changes = false
  for i in pairs(MYPROJECTS) do
    local proj = MYPROJECTS[i]
    for j in pairs(proj.dependencies) do
      local other_proj = FindProject(proj.dependencies[j])
      if (other_proj) then
        for k in pairs(other_proj.dependencies) do
          if (not table.contains(proj.dependencies, other_proj.dependencies[k])) then
            table.insert(proj.dependencies, other_proj.dependencies[k])
            some_changes = true                                                            
          end                
        end                
      end
    end     
  end
end

-- =============================================================================
-- Display all dependencies
-- =============================================================================

if (DISPLAY_DEPENDENCIES) then
  Output("=======================================================================")
  Output("Dependencies")
  Output("=======================================================================")
  for i in pairs(MYPROJECTS) do
    local proj = MYPROJECTS[i]
    if (#proj.dependencies > 0) then
      Output("Project ["..proj.name.."] :")
      for j in pairs(proj.dependencies) do
        Output ("  depends on ["..proj.dependencies[j].."]")    
      end          
    end
  end
  Output("=======================================================================")
end  

-- =============================================================================
-- Fully propagate dependencies
-- =============================================================================

function CopyResourceFiles(dst_proj, src_proj, plat, conf, proj_visible) -- dst_proj is the project that wants resources
  local p = project() 
  if (dst_proj.proj_type == TYPE_EXECUTABLE) then    
    filter { "configurations:" .. conf, "platforms:" .. plat}
    local all_files = src_proj.tocopy[plat][conf]    
    if (all_files) then
      project(GetDependantResourceProjName(p.name))
      if (src_proj.proj_location ~= nil) then
        location(src_proj.proj_location)
      end      
      filter { "configurations:" .. conf, "platforms:" .. plat}                  
      for v, data in ipairs(all_files) do
        local filename      = data[1]
        local full_filename = data[2]	       
        local dst_name      = path.join(dst_proj.targetdir[plat][conf], filename)
        
	      if (proj_visible) then		                         
          if (os.isfile(full_filename)) then
            files(full_filename)
          elseif (os.isdir(full_filename)) then
            files(path.join(full_filename, "**"))                  
          end
        end                             
                             
        local build_command_str = ("\"" .. COPY_SCRIPT .. "\" \"" .. full_filename .. "\" \"" .. dst_name .. "\"")
        buildcommands (build_command_str)
      end                     
    end
  end
  project(p.name)        
end

function ResolveDependency(proj, other_proj, plat, conf)

  if (other_proj.proj_type == TYPE_LIBRARY or other_proj.proj_type == TYPE_EXTERNAL_LIBRARY) then -- only resolve dependencies with libraries
    filter { "configurations:" .. conf, "platforms:" .. plat}
    
    local inc_dir = other_proj.includedirs[plat][conf]
    if (inc_dir) then    
      includedirs(inc_dir)
      if (DISPLAY_DEPENDENCIES) then
        Output("ResolveDependency [" .. proj.name .. "] includedirs [" .. inc_dir .. "] for " .. plat .. " " .. conf)
      end          
    end
    
    local target_dir = other_proj.targetdir[plat][conf] 
    if (target_dir) then                
      libdirs(target_dir)
      if (DISPLAY_DEPENDENCIES) then        
        Output("ResolveDependency [" .. proj.name .. "] libdirs     [" .. target_dir .. "] for " .. plat .. " " .. conf)
      end                  
    end
    
    
    if (proj.proj_type == TYPE_EXECUTABLE) then  
    
      local additionnal_libs = other_proj.additionnal_libs[plat][conf] 
      if (additionnal_libs) then
        for i in pairs(additionnal_libs) do                  
          links(additionnal_libs[i])
          if (DISPLAY_DEPENDENCIES) then        
              Output("ResolveDependency [" .. proj.name .. "] links       [" .. additionnal_libs[i] .. "] for " .. plat .. " " .. conf)
          end          
        end               
      end    
      
      local libname = other_proj.libname[plat][conf] 
      if (libname) then
        if (IsTable(libname)) then
          for i in pairs(libname) do
            links(libname[i])
            if (DISPLAY_DEPENDENCIES) then        
              Output("ResolveDependency [" .. proj.name .. "] links       [" .. libname[i] .. "] for " .. plat .. " " .. conf)
            end
          end
        else
          links(libname)
          if (DISPLAY_DEPENDENCIES) then        
            Output("ResolveDependency [" .. proj.name .. "] links       [" .. libname .. "] for " .. plat .. " " .. conf)
          end     
        end                                       
      end      
  
    end
               
    CopyResourceFiles(proj, other_proj, plat, conf, false) -- resources from dependancies cannot be visible
              
  end
   
  if (other_proj.proj_type == TYPE_RESOURCES) then -- for resources, only copy required data
    CopyResourceFiles(proj, other_proj, plat, conf, false) -- resources from dependancies cannot be visible    
  end
    	          
end

-- =============================================================================
-- Fully propagate dependencies
-- =============================================================================

function ResolveDependencyAndCopy(proj, plat, conf)
  if (proj.proj_type ~= TYPE_EXTERNAL_LIBRARY and proj.proj_type ~= TYPE_RESOURCES) then  -- external lib has not (and cannot be) resolved while it is not compiled    
    project(proj.name)    
    for j in pairs(proj.dependencies) do
      local other_proj = FindProject(proj.dependencies[j])
      if (other_proj) then    
        ResolveDependency(proj, other_proj, plat, conf)      
      end      
    end   
    CopyResourceFiles(proj, proj, plat, conf,   true) -- resources from executable himself must be visible       
  end           
end

for i in pairs(MYPROJECTS) do   
  local proj = MYPROJECTS[i]
  ResolveDependencyAndCopy(proj, x32, DEBUG)      
  ResolveDependencyAndCopy(proj, x64, DEBUG)
  ResolveDependencyAndCopy(proj, x32, RELEASE)
  ResolveDependencyAndCopy(proj, x64, RELEASE)
end  


-- =============================================================================
-- Generate documentation
-- =============================================================================

for i in pairs(MYPROJECTS) do   
  local proj = MYPROJECTS[i]
  if (proj.gendoxygen) then
  
    local resources_proj_name = GetDependantDocumentationProjName(proj.name)
      
    group(proj.group_name) -- same group than the library      
    project(resources_proj_name)
    kind("Makefile")  
    
    local build_command_str = "\"" .. DOXYGEN_SCRIPT .. "\" \"" .. proj.root_path .. "\" \"" .. proj.build_path .. "\" \"" .. proj.name .. "\""    
             
    configuration {DEBUG, x32}
    buildcommands (build_command_str)
    rebuildcommands (build_command_str)
    configuration {DEBUG, x64}
    buildcommands (build_command_str)
    rebuildcommands (build_command_str)
    configuration {RELEASE, x32}
    buildcommands (build_command_str)
    rebuildcommands (build_command_str)
    configuration {RELEASE, x64}
    buildcommands (build_command_str)
    rebuildcommands (build_command_str)
                 
    if (AUTO_DOCUMENTATION) then
      project(proj.name)      
      links(resources_proj_name)
    end  
  end
end

  


