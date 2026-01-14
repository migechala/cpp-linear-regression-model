/**
 * \author: Mikhail Chalakov
 * Email: mchalakov@wisc.edu
 * \date: 16/12/2025
 */

#include "Column.hpp"
#include "LinearRegression.hpp"
#include "LoadData.hpp"
#include "Logger.hpp"

int main() {
  Logger::open("log.txt");
  Logger::log() << "Starting..." << std::endl;
  //
  std::vector<std::string_view> columns = {"Seattle", "Vancouver", "Jerusalem"};
  LoadData dataLoader;

  auto result = dataLoader.fromCSV("assets/temperature.csv", columns);

  if (!result) {
    Logger::log() << "Error loading data: " << static_cast<int>(result.error())
                  << std::endl;
    return -1;
  }
  auto data = result.value();
  Logger::log() << "\n";

  for (const auto &i : data) {
    std::visit(Visitor{[&i](auto &x) {
                 Logger::log() << i.first << ": " << x.size() << "\n";
               }},
               i.second);
  }
  Logger::log() << "\n";

  LinearRegression lr({data["Seattle"], data["Vancouver"]}, data["Jerusalem"]);

  lr.fit(0.00001, 1000);
  double prediction = lr.predictSingle({281.8, 284.63});
  Logger::log() << "Prediction for Seattle=281.8, Vancouver=284.63: Jerusalem= "
                << prediction << std::endl;
  Logger::log() << "Done" << std::endl;
  return 0;
}
