@ECHO OFF

REM Go to the directory where the batch is
PUSHD "%~dp0

REM Search target in arguments (beware case sensitive)
SET TARGET=vs2019

REM Search the target
IF "%1" == "" GOTO CONTINUE
SET TARGET=%1
:CONTINUE

SET TOOLS_PATH=build_tools

SET PREMAKE_PATH1=%TOOLS_PATH%
SET PREMAKE_PATH2=C:\Program Files\Premake
SET PREMAKE_PATH3=C:\Program Files (x86)\Premake

REM Start generation
IF EXIST "%PREMAKE_PATH1%\premake5.exe" (
SET PREMAKE_PATH="%PREMAKE_PATH1%"
) ELSE IF EXIST "%PREMAKE_PATH2%\premake5.exe" (
SET PREMAKE_PATH="%PREMAKE_PATH2%"
) ELSE IF EXIST "%PREMAKE_PATH3%\premake5.exe" (
SET PREMAKE_PATH="%PREMAKE_PATH3%""
) ELSE (
ECHO ERROR : Premake5.exe not found !
GOTO CONTINUE
)
ECHO PREMAKE PATH : %PREMAKE_PATH%\premake5.exe

%PREMAKE_PATH%\premake5.exe %TARGET%  --no-auto-doc

REM the characterset(...) instruction in premake does not work with "NotSet" (VS2015 force unicode)
REM we are forced to directly patch the .vcxproj directly the XML

REM %TOOLS_PATH%\PatchProjects.py ..\premaked_solutions\%TARGET%

REM go back to initial directory
:END
POPD

REM PAUSE