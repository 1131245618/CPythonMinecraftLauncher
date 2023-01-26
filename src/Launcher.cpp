#include <iostream>
#include <fstream>
#include "zip_util/unzip.h"
#include <format>
#include "Launcher.h"

struct LauncherInput inArgs;

//int main(int argc, char* argv[])
//{
//	start(argc - 2, argv + 2);
//}

void start(int argc, char* argv[])
{

	if (argc == 0)
		help();

	argc > 0 ? inArgs.mcdir = argv[0] : inArgs.mcdir = "%AppData%Roaming\\.minecraft";
	argc > 1 ? inArgs.version = argv[1] : inArgs.version = "1.18.2";
	argc > 2 ? inArgs.java_path = argv[2] : inArgs.java_path = "%JAVA_HOME%\\bin\\java.exe";
	argc > 3 ? inArgs.maxMem = argv[3] : inArgs.maxMem = "1024m";
	argc > 4 ? inArgs.minMem = argv[4] : inArgs.maxMem = "256m";
	argc > 5 ? inArgs.username = argv[5] : inArgs.username = "Steve";
	argc > 6 ? inArgs.height = argv[6] : inArgs.height = "480";
	argc > 7 ? inArgs.weigth = argv[7] : inArgs.height = "854";

	if (!(existVersion(inArgs.version, inArgs.mcdir)))
	{
		::std::cerr << "Version does not exist\n";
		abort();
	}

	 launcher(inArgs);

}

void launcher(const LauncherInput& inArgs)
{
	::std::string json_file_name = ::std::format("{0}\\versions\\{1}\\{1}.json", inArgs.mcdir, inArgs.version);
	::std::ifstream json_file;
	json_file.open(json_file_name.c_str(), std::ios::binary);
	if (!json_file)
	{
		::std::cerr << "Missing file(" << json_file_name << ")\n";
		abort();
	}
	Json::Reader json_reader;
	Json::Value json_value;

	if (!json_reader.parse(json_file, json_value))
	{
		::std::cout << json_reader.getFormattedErrorMessages() << ::std::endl;
		::std::cout << "failed to prase version json\n";
		abort();
	}

	json_file.close();

	//::std::cout << json_value["id"].asCString() << ::std::endl;

	for (Json::Value libs : json_value["libraries"])
	{
		if(libs["downloads"]["classifiers"].isString())
			for (Json::Value native : libs["downloads"])
			{
				if (native.asString().compare("artifact") == 0)
				{
					::std::string dest_path, source_path;
					dest_path = ::std::format("{0}\\versions\\{1}\\{1}-natives", inArgs.mcdir, inArgs.version);
					source_path = ::std::format("{0}\\libraries\\{1}", inArgs.mcdir, native["path"].asCString());
					ZipUnpress(source_path.c_str(), dest_path.c_str());
				}
				else if (native.asString().compare("classifiers") == 0)
				{
					for (Json::Value lib : native)
					{
						::std::string dest_path, source_path;
						dest_path = ::std::format("{0}\\versions\\{1}", inArgs.mcdir, native["path"].asCString());
						source_path = ::std::format("{0}\\libraries\\{1}", inArgs.mcdir, lib["path"].asCString());
						ZipUnpress(source_path.c_str(), dest_path.c_str());
					}
				}
			}
	}

	::std::string JVMSetting = setJVM(inArgs, json_value);
	::std::string MCSetting = setArguements(inArgs, json_value);

	::std::string cmd = JVMSetting + " " + MCSetting;

	std::ofstream bat("run.bat", std::ios::out);
	bat << "@ECHO OFF\n";
	bat << ::std::format("cd /{0} {1}\\versions\\{2}\n", inArgs.mcdir[0], inArgs.mcdir, inArgs.version);
	bat << cmd << "\n";
	bat << "pause\n";
	bat.close();
	system("run");

}

::std::string setJVM(const LauncherInput& inArgs, const Json::Value& json_value)
{
	::std::string resualt = ::std::format("\
\"{0}\"\
 -Dfile.encoding=GB18030\
 -Dminecraft.client.jar=\".minecraft\\versions\\{2}\\{2}.jar\"\
 -XX:+UnlockExperimentalVMOptions\
 -XX:+UseG1GC\
 -XX:G1NewSizePercent=20\
 -XX:G1ReservePercent=20\
 -XX:MaxGCPauseMillis=50\
 -XX:G1HeapRegionSize=16m\
 -XX:-UseAdaptiveSizePolicy\
 -XX:-OmitStackTraceInFastThrow\
 -XX:-DontCompileHugeMethods\
 -Xmx{3} -Xmn{4}\
 -Dfml.ignoreInvalidMinecraftCertificates=true\
 -Dfml.ignorePatchDiscrepancies=true\
 -Djava.rmi.server.useCodebaseOnly=true\
 -Dcom.sun.jndi.rmi.object.trustURLCodebase=false\
 -Dcom.sun.jndi.cosnaming.object.trustURLCodebase=false\
 -Dlog4j2.formatMsgNoLookups=true\
 -Dlog4j.configurationFile=\"{1}\\versions\\{2}\\log4j2.xml\"\
 -XX:HeapDumpPath=MojangTricksIntelDriversForPerformance_javaw.exe_minecraft.exe.heapdump\
 -Djava.library.path=\"{1}\\versions\\{2}\\natives-windows-x86_64\"\
 -Dminecraft.launcher.brand=CppMCL\
 -Dminecraft.launcher.version=1.0.0\
 -cp\
", inArgs.java_path, inArgs.mcdir, inArgs.version, inArgs.maxMem, inArgs.minMem);

	::std::string class_path("");
	class_path += "\"";
	for (Json::Value libs : json_value["libraries"])
	{
		if (libs["downloads"]["classifiers"].isObject())
		{
			if (libs["downloads"]["artifact"]["path"].isString())
			{
				::std::string lib_path = ::std::format("{0}\\libraries\\{1}", inArgs.mcdir, libs["downloads"]["artifact"]["path"].asCString());
				if (existFile(lib_path.c_str()))
					class_path += lib_path + ";";
			}
		}
	}

	for (Json::Value patch : json_value["patches"])
	{
		if (patch["libraries"].isArray())
		{
			for (Json::Value patch_lib : patch["libraries"])
			{
				::std::string patch_lib_name = ::std::format("{0}\\libraries\\{1}.jar", inArgs.mcdir, domainToPath(patch_lib["name"].asString()));
				//if (class_path.find(fabric_lib_name, 0) == -1)
				//if (existFile(patch_lib_name.c_str()))
				class_path += patch_lib_name + ";";
			}
		}
	}

	class_path += ::std::format("{0}\\versions\\{1}\\{1}.jar", inArgs.mcdir, inArgs.version);
	class_path += "\"";
	class_path = replace(class_path, "/", "\\");

	resualt += ::std::format(" {0} -Dlog4j.formatMsgNoLookups=true", class_path);

	return resualt;
}


::std::string setArguements(const LauncherInput& inArgs, const Json::Value json_value)
{
	::std::string MC_args = json_value["mainClass"].asString();
	MC_args += " ";

	for (Json::Value patch : json_value["patches"])
	{
		if (!patch.isObject())
			continue;

		if (patch["minecraftArguments"].isString())
		{
			MC_args += patch["minecraftArguments"].asString();
			break;
		}

		for (Json::Value arg : patch["arguments"])
		{
			if (arg.compare(patch["arguments"]["jvm"]) == 0)
				continue;
			if (arg.isString())
			{
				MC_args += arg.asString() + " ";
			}
			else if (arg.isArray())
			{
				for (Json::Value sub_arg : arg)
				{
					if (sub_arg.isString())
					{
						MC_args += sub_arg.asString() + " ";
					}
					else if (sub_arg.isObject())
					{
						for (Json::Value a : sub_arg["value"])
						{
							MC_args += a.asString() + " ";
						}
					}
				}
			}
		}
	}

	setGame(MC_args, inArgs, json_value);
	setFabric(MC_args, inArgs, json_value);
	setForge(MC_args, inArgs, json_value);
	setOptifine(MC_args, inArgs, json_value);

	return MC_args;

}

void setGame(::std::string& MC_args, const LauncherInput& inArgs, const Json::Value& json_value)
{
	MC_args = replace(MC_args, "${auth_player_name}", inArgs.username);
	MC_args = replace(MC_args, "${version_name}", inArgs.version);
	MC_args = replace(MC_args, "${game_directory}", ::std::format("{0}\\versions\\{1}", inArgs.mcdir, inArgs.version));
	MC_args = replace(MC_args, "${assets_root}", ::std::format("{0}\\assets", inArgs.mcdir));
	MC_args = replace(MC_args, "${assets_index_name}", json_value["assetIndex"]["id"].asString());
	MC_args = replace(MC_args, "${auth_uuid}", "{}");
	MC_args = replace(MC_args, "${auth_access_token}", "{}");
	MC_args = replace(MC_args, "${clientid}", inArgs.version);
	MC_args = replace(MC_args, "${auth_xuid}", "{}");
	MC_args = replace(MC_args, "${user_type}", "Legacy");
	MC_args = replace(MC_args, "${clientid}", inArgs.version);
	MC_args = replace(MC_args, "${version_type}", "CppMCL-1.0.0");
	MC_args = replace(MC_args, "${resolution_width}", inArgs.weigth);
	MC_args = replace(MC_args, "${resolution_height}", inArgs.height);
	MC_args = replace(MC_args, "--demo ", "");
}

void setFabric(::std::string& MC_args, const LauncherInput& inArgs, const Json::Value& json_value)
{

}

void setForge(::std::string& MC_args, const LauncherInput& inArgs, const Json::Value& json_value)
{
}

void setOptifine(::std::string& MC_args, const LauncherInput& inArgs, const Json::Value& json_value)
{

}

bool ZipUnpress(const char* src, const char* dst)
{
	if (!existFile(src))
		::std::cerr << "[WARN] source file(" << src << ") doesn't exist\n";
		return false;
	HZIP zip = OpenZip(charToWchar(src), NULL);
	SetUnzipBaseDir(zip, charToWchar(dst));
	ZIPENTRY ze;
	GetZipItem(zip, -1, &ze);
	int numitems = ze.index;
	for (int i = 0; i < numitems; i++)
	{
		GetZipItem(zip, i, &ze);
		UnzipItem(zip, i, ze.name);
	}
	CloseZip(zip);
	return true;
}

wchar_t* charToWchar(const char* src)
{
	size_t len = std::strlen(src);
	wchar_t* dst = new wchar_t[len];
	mbstowcs_s(nullptr, dst, len, src, _TRUNCATE);
	return dst;
}

bool existVersion(const char* version, const char* mcdir)
{
	struct stat buffer;
	::std::string name = ::std::format("{0}\\versions\\{1}\\{1}.json", mcdir, version);
	::std::cout << name << ::std::endl;
	return (stat(name.c_str(), &buffer) == 0);
}

bool existFile(const char* file)
{
	struct stat buffer;
	return (stat(file, &buffer) == 0);
}

::std::string replace(::std::string str, const ::std::string& from, const ::std::string& to)
{
	size_t start_pos = 0;
	while ((start_pos = str.find(from, start_pos)) != ::std::string::npos)
	{
		str.replace(start_pos, from.length(), to);
		start_pos += to.length();
	}
	return str;
}

::std::string domainToPath(const ::std::string& domain)
{
	::std::string path("");
	::std::vector<::std::string> splited = split(domain, ":");
	::std::vector<::std::string> splitedDomin = split(splited[0], ".");
	for (::std::string str1 : splitedDomin)
	{
		path += str1 + "\\";
	}
	for (int i = 1;i < splited.size(); ++i)
	{
		path += splited[i] + "\\";
	}
	for (int i = 1; i < splited.size() - 1; ++i)
	{
		path += splited[i] + "-";
	}
	path += splited[splited.size()-1];
	return path;
}

::std::vector<::std::string> split(const ::std::string& _str, const ::std::string& pattern)
{
	::std::string str = _str + pattern;
	::std::string::size_type pos;
	::std::vector<::std::string> result;
	int size = str.size();
	for (int i = 0; i < size; ++i)
	{
		pos = str.find(pattern, i);
		if (pos < size)
		{
			::std::string s = str.substr(i, pos - i);
			result.push_back(s);
			i = pos + pattern.size() - 1;
		}
	}
	return result;
}

void help()
{
	::std::cout << R"(
the 1st argument: Minecraft directory;
the 2nd argument: Minecraft version;
the 3rd argument: Java path;
the 4th argument: max memory of JVM;
the 5th argument: min memory of JVM;
the 6th argument: player name;
the 7th argument: window height;
the 8th argument: window weigth;
)" << std::endl;
}
