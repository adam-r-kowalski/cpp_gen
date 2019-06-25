#include <boost/filesystem.hpp>
#include <boost/program_options.hpp>
#include <fmt/core.h>
#include <fmt/ostream.h>

namespace fs = boost::filesystem;
namespace po = boost::program_options;

auto cmake_lists_txt(const std::string &name) -> std::string {
  return fmt::format("cmake_minimum_required(VERSION 3.0)\n\n"
                     "set(CMAKE_EXPORT_COMPILE_COMMANDS 1)\n"
                     "set(CMAKE_CXX_COMPILER g++-9)\n\n"
                     "project({0})\n\n"
                     "add_executable({0} src/main.cc)\n\n"
                     "target_include_directories({0} PRIVATE include)\n\n"
                     "target_compile_features({0} PRIVATE cxx_std_17)\n\n"
                     "target_compile_options({0} PRIVATE -fconcepts)\n",
                     name);
}

auto build_sh() -> std::string {
  return "if [[ $# -eq 0 ]]; then\n"
         "    mkdir -p build\n"
         "    cd build\n"
         "    cmake -G Ninja ..\n"
         "else\n"
         "    mkdir -p build_$1\n"
         "    cd build_$1\n"
         "    cmake -G Ninja -D CMAKE_BUILD_TYPE=$1 ..\n"
         "fi\n";
}

auto run_sh(const std::string &name) -> std::string {
  return fmt::format("if [[ $# -eq 0 ]]; then\n"
                     "    cd build\n"
                     "else\n"
                     "    cd build_$1\n"
                     "fi\n"
                     "\n"
                     "cmake --build . -j 8\n"
                     "mv compile_commands.json ..\n"
                     "./{}\n",
                     name);
}

auto main_cc() -> std::string {
  return "#include <iostream>\n\n"
         "auto main() -> int {\n"
         "  std::cout << \"hello world\\n\";\n"
         "  return 0;\n"
         "}\n";
}

auto git_ignore() -> std::string { return "build*/"; }

auto create_project(const po::variables_map &vm) -> int {
  if (!vm.count("name")) {
    fmt::print("project name required\n");
    return 1;
  }

  const auto name = vm["name"].as<std::string>();
  const auto path = fs::path(vm["path"].as<std::string>()) / name;

  if (fs::exists(path)) {
    fmt::print("directory already exists\n");
    return 1;
  }

  fs::create_directory(path);
  fs::ofstream{path / "CMakeLists.txt"} << cmake_lists_txt(name);

  const auto permissions = fs::owner_all | fs::group_read | fs::others_read;

  fs::ofstream{path / "build.sh"} << build_sh();
  fs::permissions(path / "build.sh", permissions);

  fs::ofstream{path / "run.sh"} << run_sh(name);
  fs::permissions(path / "run.sh", permissions);

  fs::create_directory(path / "src");
  fs::ofstream{path / "src" / "main.cc"} << main_cc();

  fs::create_directory(path / "include");

  fs::ofstream{path / ".gitignore"} << git_ignore();

  return 0;
}

auto main(int ac, char **av) -> int {
  auto desc = po::options_description{"Allowed options"};
  desc.add_options()("help", "produce help message")(
      "name,n", po::value<std::string>(), "project name")(
      "path,p",
      po::value<std::string>()->default_value(fs::current_path().string()),
      "project path");

  auto p = po::positional_options_description{};
  p.add("name", -1);

  auto vm = po::variables_map{};
  po::store(po::command_line_parser(ac, av).options(desc).positional(p).run(),
            vm);
  po::notify(vm);

  if (vm.count("help")) {
    fmt::print("{}\n", desc);
    return 1;
  }

  return create_project(vm);
}
