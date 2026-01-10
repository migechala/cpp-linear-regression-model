#include "LoadData.hpp"
#include "Logger.hpp"
template <typename... Callable> struct Visitor : Callable... {
  using Callable::operator()...;
};
int main() {
  Logger::open("log.txt");
  Logger::log() << "Starting..." << std::endl;
  //
  std::vector<std::string_view> columns = {"AREA NAME", "Crm Cd Desc", "Age"};
  LoadData dataLoader;

  auto result = dataLoader.fromCSV("assets/crime_LA.csv", columns);

  if (!result) {
    Logger::log() << "Error loading data: " << static_cast<int>(result.error())
                  << std::endl;
    return -1;
  }
  auto data = result.value();
  int idx = 0;
  for (const auto &i : data) {
    if (idx == 10)
      break;
    std::cout << i.first << ": ";
    std::visit(Visitor{[](auto x) {
                 for (int i = 0; i < 10; ++i)
                   std::cout << x[i] << ",";
               }},
               i.second);
    std::cout << std::endl;
    idx++;
  }
  //
  Logger::log() << "Done" << std::endl;
  return 0;
}
