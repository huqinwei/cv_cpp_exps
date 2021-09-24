#include "fun1.h"
#include <functional>
#include <vector>
#include <iostream>
#include <fstream>
namespace {

typedef std::function<int(int, char**)> command_func_t;

int ShowHelp(
    const std::vector<std::pair<std::string, command_func_t>>& commands) {
  using namespace AI3D;

  std::cout <<                    "%s -- AI3D"           << std::endl
            << std::endl;

  std::cout << "Usage:" << std::endl;
  std::cout << "  ManyFun [command] [options]" << std::endl << std::endl;

 

  std::cout << "Example usage:" << std::endl;
  std::cout << "  ManyFun help [ -h, --help ]" << std::endl;
  std::cout << "  ManyFun fun_1" << std::endl;
  std::cout << "  ManyFun gui " << std::endl;//-h [ --help ]
  
  std::cout << "  ..." << std::endl << std::endl;

  std::cout << "Available commands:" << std::endl;
  std::cout << "  help" << std::endl;
  for (const auto& command : commands) {
    std::cout << "  " << command.first << std::endl;
  }
  std::cout << std::endl;

  return EXIT_SUCCESS;
}

}  // namespace

int main(int argc, char** argv) {
  using namespace AI3D;


  std::vector<std::pair<std::string, command_func_t>> commands;
  commands.emplace_back("fun_1", &Fun1);
  commands.emplace_back("gui", &Fun2);
  
  if (argc == 1) {
    return ShowHelp(commands);
  }

  const std::string command = argv[1];
  if (command == "help" || command == "-h" || command == "--help") {
    return ShowHelp(commands);
  } else {
    command_func_t matched_command_func = nullptr;
    for (const auto& command_func : commands) {
      if (command == command_func.first) {
        matched_command_func = command_func.second;
        break;
      }
    }
    if (matched_command_func == nullptr) {
      std::cerr << 
                       "ERROR: Command " << command.c_str() << " not recognized. To list the "
                       "available commands, run `ManyFun help`."  
                     
                << std::endl;
      return EXIT_FAILURE;
    } else {
      int command_argc = argc - 1;
      char** command_argv = &argv[1];
      command_argv[0] = argv[0];
      return matched_command_func(command_argc, command_argv);
    }
  }

  return ShowHelp(commands);
}
