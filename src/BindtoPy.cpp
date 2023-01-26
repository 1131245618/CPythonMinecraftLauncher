#include <pybind11/pybind11.h>
#include "Launcher.h"
#include <iostream>
namespace py = pybind11;

char* copyCString(const char* c_str)
{
    size_t len = std::strlen(c_str) + 1;
    char* res_str = new char[len];
    for (int i = 0; i < len; ++i)
    {
        res_str[i] = c_str[i];
    }
    return res_str;
}

void py_start(py::list argv)
{
    int argc = py::len(argv);
    char** c_argv = new char* [argc];
    for (int i = 0; i < argc; ++i)
    {
        c_argv[i] = copyCString(argv[i].cast<std::string>().c_str());
        std::cout << c_argv[i] << std::endl;
    }

    start(argc, c_argv);

    delete[] c_argv;
}

PYBIND11_MODULE(MinecraftLauncher, m) {
    m.doc() = "pybind11 Minecraft Luncher plugin"; // optional module docstring

    m.def("start", &py_start, "start a Minecraft version", py::arg("argv"));
    //m.def("existFile", &existFile, "start a Minecraft version");
    //m.def("ZIPUnpress", &ZIPUnpress, "start a Minecraft version");
    //m.def("existVersion", &existVersion, "start a Minecraft version");
    m.def("launcher", &launcher, "start a Minecraft version", py::arg("input"));
    //m.def("setJVM", &setJVM, "start a Minecraft version");
}