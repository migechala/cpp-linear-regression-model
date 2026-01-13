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
  std::vector<std::string_view> columns = {"AREA", "TIME OCC", "Crm Cd",
                                           "Vict Age"};
  LoadData dataLoader;

  auto result = dataLoader.fromCSV("assets/crime_LA.csv", columns);

  if (!result) {
    Logger::log() << "Error loading data: " << static_cast<int>(result.error())
                  << std::endl;
    return -1;
  }
  auto data = result.value();
  for (const auto &i : data) {
    std::cout << i.first << ": ";
    std::visit(Visitor{[](auto &x) {
                 for (int i = 0; i < 200; ++i)
                   std::cout << x[i] << ",";
               }},
               i.second);
    std::cout << "\n\n";
  }
  LinearRegression lr({data["AREA"]}, data["Vict Age"]);
  lr.fit(0.001, 1000);
  double prediction = lr.predictSingle({15.0});
  Logger::log() << "Prediction for AREA=15: Vict Age= " << prediction
                << std::endl;
  //
  Logger::log() << "Done" << std::endl;
  return 0;
}
