// Copyright 2020 Your Name <your_email>

#include <builder.hpp>


namespace po = boost::program_options;


void Builder::build(int argc, char* argv[]){

  po::options_description desc("Allowed options");
  desc.add_options()
      ("help", "produce help message")
      ("config", po::value<std::string>(),
       "указываем конфигурацию сборки (по умолчанию Debug)")
      ("install",
       "добавляем этап установки (в директорию _install)")
      ("pack",
       "добавляем этап упаковки (в архив формата tar.gz)")
      ("timeout <count>", po::value<int>(),
       "указываем время ожидания (в секундах)");

  po::variables_map vm;
  po::store(po::parse_command_line(argc,argv, desc), vm);
  vm.notify();

  if(vm.count("help")){
    std::cout << desc << std::endl;
  }
}


