// Copyright 2020 Your Name <your_email>

#include <builder.hpp>

namespace process = boost::process;
namespace ini = boost::process::initializers;

builder::builder()
    :   build_func(nullptr)
    , install_func(nullptr)
    , pack_func(nullptr)
    , wait_for_timeout(nullptr)
    , timeout_flag(false)
    , processes_completed(false)
    , process_failed(false)
    , current_process(nullptr)
{
  cmake_path = process::search_path("cmake");
}

builder::builder(const size_t& ms_timeout)
    :   build_func(nullptr)
    , install_func(nullptr)
    , pack_func(nullptr)
    , wait_for_timeout(nullptr)
    , timeout_flag(false)
    , processes_completed(false)
    , process_failed(false)
    , current_process(nullptr)
{
  cmake_path = process::search_path("cmake");
  std::thread([ms_timeout, this]{
    for (size_t i = 0; i < ms_timeout; i+=20) {
      std::this_thread::sleep_for(std::chrono::milliseconds(20));
      if (processes_completed) break;
    }
    timeout_flag = true;
    process::terminate(*current_process);
  }).detach();
}

void builder::set_build(const std::string &_config) {
  config = _config; //Release or debug
  build_func = new std::function<void()>([this]{
    if (timeout_flag || process_failed) return; // если вышло время или failed
    std::cout << "-----BUILD" << std::endl;
    std::vector<std::string> args;
    args.emplace_back(cmake_path);
    args.emplace_back("-H.");
    args.emplace_back("-B_builds");
    args.emplace_back("-DCMAKE_INSTALL_PREFIX=_install");
    args.emplace_back("-DCMAKE_BUILD_TYPE=" + config);
    current_process = std::make_unique<process::child>(
        process::execute(ini::throw_on_error(), ini::set_args(args),
                         ini::inherit_env()));
    try {
      int result = process::wait_for_exit(*current_process);
      if (result != 0) process_failed = true;
    } catch (...) {
      std::cout << "Build terminated: time expired" << std::endl;
      process_failed = true;
      return;
    }
    if (!process_failed){
      args.clear();
      args.emplace_back(cmake_path);
      args.emplace_back("--build");
      args.emplace_back("_builds");
      if (timeout_flag) return;
      current_process = nullptr;
      current_process = std::make_unique<process::child>(
          process::execute(ini::throw_on_error(), ini::set_args(args),
                           ini::inherit_env()));
      try {
        int result = process::wait_for_exit(*current_process);
        if (result != 0) process_failed = true;
      } catch (...) {
        std::cout << "Build terminated: time expired" << std::endl;
        process_failed = true;
        return;
      }
      if (!process_failed){
        std::cout << "Build ended successfully" << std::endl;
        return;
      }
    }
    std::cout << "Build failed" << std::endl;
    process_failed = true;
  });
}

builder::~builder() {
  delete build_func;
  delete install_func;
  delete pack_func;
  delete wait_for_timeout;
}
