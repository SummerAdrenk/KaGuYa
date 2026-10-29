set(CMAKE_SYSTEM_NAME Windows)
set(CMAKE_SYSTEM_PROCESSOR x86)

set(KAGUYA_MSVC_VERSION "14.44" CACHE STRING "MSVC STL/CRT/ATL version used by clang-cl")
set(KAGUYA_LLVM_ROOT "" CACHE PATH "Optional LLVM installation root")
set(KAGUYA_VS_ROOT "" CACHE PATH "Optional Visual Studio installation with v143/ATL")

if(KAGUYA_LLVM_ROOT)
  find_program(_kaguya_clang clang-cl PATHS "${KAGUYA_LLVM_ROOT}/bin" NO_DEFAULT_PATH REQUIRED)
else()
  find_program(_kaguya_clang clang-cl REQUIRED)
endif()
get_filename_component(_kaguya_llvm_bin "${_kaguya_clang}" DIRECTORY)
find_program(_kaguya_lld lld-link PATHS "${_kaguya_llvm_bin}" NO_DEFAULT_PATH REQUIRED)
find_program(_kaguya_lib llvm-lib PATHS "${_kaguya_llvm_bin}" NO_DEFAULT_PATH REQUIRED)
find_program(KAGUYA_CLANGD clangd PATHS "${_kaguya_llvm_bin}" NO_DEFAULT_PATH REQUIRED)

if(NOT KAGUYA_VS_ROOT)
  find_program(_kaguya_vswhere vswhere
    PATHS "$ENV{ProgramFiles\(x86\)}/Microsoft Visual Studio/Installer" REQUIRED)
  execute_process(COMMAND "${_kaguya_vswhere}" -latest -version "[17.0,18.0)"
    -products * -property installationPath
    OUTPUT_VARIABLE _kaguya_vs OUTPUT_STRIP_TRAILING_WHITESPACE
    COMMAND_ERROR_IS_FATAL ANY)
  if(NOT _kaguya_vs)
    message(FATAL_ERROR "Set KAGUYA_VS_ROOT to an installation containing MSVC ${KAGUYA_MSVC_VERSION} and ATL.")
  endif()
  file(TO_CMAKE_PATH "${_kaguya_vs}" _kaguya_vs)
  set(KAGUYA_VS_ROOT "${_kaguya_vs}" CACHE PATH "" FORCE)
endif()
get_filename_component(_kaguya_env_script
  "${CMAKE_CURRENT_LIST_DIR}/../msvc_environment.cmd" ABSOLUTE)
set(_kaguya_env_stamp "${KAGUYA_VS_ROOT}|${KAGUYA_MSVC_VERSION}|x86")
if(NOT "$ENV{KAGUYA_LLVM_MSVC_ENV}" STREQUAL "${_kaguya_env_stamp}")
  execute_process(
    COMMAND "$ENV{COMSPEC}" /d /c "${_kaguya_env_script}" "${KAGUYA_VS_ROOT}" "${KAGUYA_MSVC_VERSION}"
    OUTPUT_VARIABLE _kaguya_environment ERROR_VARIABLE _kaguya_environment_error
    RESULT_VARIABLE _kaguya_environment_result)
  if(NOT _kaguya_environment_result EQUAL 0)
    message(FATAL_ERROR "MSVC x86 environment failed: ${_kaguya_environment_error}")
  endif()
  foreach(_key INCLUDE LIB LIBPATH VCToolsInstallDir WindowsSdkDir WindowsSDKVersion)
    string(REGEX MATCH "(^|\n)${_key}=([^\r\n]*)" _match "${_kaguya_environment}")
    if(_match)
      set(ENV{${_key}} "${CMAKE_MATCH_2}")
    endif()
  endforeach()
  string(REGEX MATCH "(^|\n)[Pp][Aa][Tt][Hh]=([^\r\n]*)" _path_match "${_kaguya_environment}")
  if(_path_match)
    set(ENV{PATH} "${CMAKE_MATCH_2}")
  endif()
  # try_compile会再次加载工具链，不能反复追加vcvars导致PATH超过cmd限制。
  set(ENV{KAGUYA_LLVM_MSVC_ENV} "${_kaguya_env_stamp}")
endif()
if(NOT EXISTS "$ENV{VCToolsInstallDir}/atlmfc/include/atlbase.h")
  message(FATAL_ERROR "The selected MSVC installation is missing ATL.")
endif()

set(CMAKE_C_COMPILER "${_kaguya_clang}" CACHE FILEPATH "" FORCE)
set(CMAKE_CXX_COMPILER "${_kaguya_clang}" CACHE FILEPATH "" FORCE)
set(CMAKE_C_COMPILER_TARGET i686-pc-windows-msvc)
set(CMAKE_CXX_COMPILER_TARGET i686-pc-windows-msvc)
set(CMAKE_LINKER "${_kaguya_lld}" CACHE FILEPATH "" FORCE)
set(CMAKE_LINKER_TYPE LLD CACHE STRING "" FORCE)
set(CMAKE_AR "${_kaguya_lib}" CACHE FILEPATH "" FORCE)
set(CMAKE_C_COMPILER_AR "${_kaguya_lib}" CACHE FILEPATH "" FORCE)
set(CMAKE_CXX_COMPILER_AR "${_kaguya_lib}" CACHE FILEPATH "" FORCE)
file(TO_CMAKE_PATH "$ENV{WindowsSdkDir}/bin/$ENV{WindowsSDKVersion}/x64" _kaguya_sdk_bin)
find_program(CMAKE_RC_COMPILER rc PATHS "${_kaguya_sdk_bin}" NO_DEFAULT_PATH REQUIRED)
find_program(CMAKE_MT mt PATHS "${_kaguya_sdk_bin}" NO_DEFAULT_PATH REQUIRED)
find_program(CMAKE_MAKE_PROGRAM ninja REQUIRED)
set(KAGUYA_MSBUILD "${KAGUYA_VS_ROOT}/MSBuild/Current/Bin/MSBuild.exe"
  CACHE FILEPATH "Read-only project metadata evaluator")

# 配置后从普通终端/CLion运行Ninja也要可用，系统include和LIBPATH显式进入命令。
file(TO_CMAKE_PATH "$ENV{INCLUDE}" _kaguya_system_include)
set(CMAKE_C_STANDARD_INCLUDE_DIRECTORIES "${_kaguya_system_include}" CACHE STRING "" FORCE)
set(CMAKE_CXX_STANDARD_INCLUDE_DIRECTORIES "${_kaguya_system_include}" CACHE STRING "" FORCE)
set(CMAKE_RC_STANDARD_INCLUDE_DIRECTORIES "${_kaguya_system_include}" CACHE STRING "" FORCE)
file(TO_CMAKE_PATH "$ENV{LIB}" _kaguya_system_libraries)
set(_kaguya_link_search "")
foreach(_directory IN LISTS _kaguya_system_libraries)
  string(APPEND _kaguya_link_search " /LIBPATH:\"${_directory}\"")
endforeach()
set(CMAKE_EXE_LINKER_FLAGS_INIT "${_kaguya_link_search}")
set(CMAKE_SHARED_LINKER_FLAGS_INIT "${_kaguya_link_search}")
set(CMAKE_MODULE_LINKER_FLAGS_INIT "${_kaguya_link_search}")

set(CMAKE_C_FLAGS_INIT "/utf-8 /clang:-fms-compatibility-version=19.44")
set(CMAKE_CXX_FLAGS_INIT "/utf-8 /clang:-fms-compatibility-version=19.44")
set(CMAKE_TRY_COMPILE_PLATFORM_VARIABLES
  KAGUYA_VS_ROOT KAGUYA_LLVM_ROOT KAGUYA_MSVC_VERSION)
