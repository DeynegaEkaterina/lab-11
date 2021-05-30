// Copyright 2020 Your Name <your_email>

#include <builder.hpp>

namespace process = boost::process;

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

builder::~builder() {
  delete build_func;
  delete install_func;
  delete pack_func;
  delete wait_for_timeout;
}
