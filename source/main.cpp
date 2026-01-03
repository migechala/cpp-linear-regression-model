#include "Logger.hpp"

int main() {
  Logger::open("log.txt");
  Logger::log() << "Starting..." << std::endl;
  //

  //
  Logger::log() << "Done" << std::endl;
  return 0;
}
