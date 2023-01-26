#pragma once
#ifndef LAUNCHER_HEADER
#define LAUNCHER_HEADER

#include <json/reader.h>
#include <json/value.h>
#include <pybind11/pybind11.h>

namespace py = pybind11;

class LauncherInput
{
public:
	const char* mcdir;
	const char* version;
	const char* java_path;
	const char* maxMem;
	const char* minMem;
	const char* username;
	const char* height;
	const char* weigth;
};

void start(int argc, char* argv[]);
bool existFile(const char* file);
void setGame(::std::string& MC_args, const LauncherInput& inArgs, const Json::Value& json_value);
void setFabric(::std::string& MC_args, const LauncherInput& inArgs, const Json::Value& json_value);
void setForge(::std::string& MC_args, const LauncherInput& inArgs, const Json::Value& json_value);
void setOptifine(::std::string& MC_args, const LauncherInput& inArgs, const Json::Value& json_value);
bool ZIPUnpress(const char* src, const char* dst);
wchar_t* charToWchar(const char* src);
bool existVersion(const char* version, const char* mcdir);
void launcher(const LauncherInput& inArgs);
::std::string setJVM(const LauncherInput& inArgs, const Json::Value& json_value);
::std::string setArguements(const LauncherInput& inArgs, const Json::Value json_value);
::std::string replace(::std::string str, const ::std::string& from, const ::std::string& to);
::std::string domainToPath(const ::std::string& domain);
::std::vector<::std::string> split(const ::std::string& _str, const ::std::string& pattern);
void help();

#endif // !LAUNCHER_HEADER
