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

bool py_existFile(py::object filename)
{
    ::std::string _filename = filename.cast<::std::string>();
    return existFile(_filename.c_str());
}

bool py_ZipUnpress(py::object src, py::object dst)
{
    ::std::string _src = src.cast<::std::string>();
    ::std::string _dst = dst.cast<::std::string>();
    return ZipUnpress(_src.c_str(), _dst.c_str());
}

PYBIND11_MODULE(MinecraftLauncher, m) {
    m.doc() = "pybind11 Minecraft Luncher plugin"; // optional module docstring

    py::class_<LauncherInput>(m, "LauncherInput")
        .def(py::init<>());

    m.def("start", &py_start, "start a Minecraft version", py::arg("argv"));
    m.def("existFile", &py_existFile, "start a Minecraft version", py::arg("filename"));
    m.def("ZipUnpress", &py_ZipUnpress, "unpress zip file to path", py::arg("src"), py::arg("dst"));
    m.def("help", &help);
    m.def("launcher", &launcher, "start a Minecraft version", py::arg("input"));
}