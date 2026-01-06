#include "LoadData.hpp"

#include "Logger.hpp"
#include <algorithm>
#include <fstream>
#include <unordered_map>
int LoadData::parseDate(const std::string &dateStr) {
  int year = std::stoi(dateStr.substr(yearIndex, 4));
  int month = std::stoi(dateStr.substr(monthIndex, 2));
  int day = std::stoi(dateStr.substr(dayIndex, 2));
  return year * 10000 + month * 100 + day;
}

template <typename... Callable> struct Visitor : Callable... {
  using Callable::operator()...;
};

std::expected<std::unordered_map<std::string, Column>, LoadError>
LoadData::fromCSV(const std::string &filename,
                  const std::span<std::string_view> targetColumns) {

  const std::string path = std::string(BASE_PATH) + "/" + filename;
  Logger::log() << path << "\n";

  std::ifstream file(path);
  if (!file.is_open()) {
    return std::unexpected(LoadError::FileNotFound);
  }

  const char delimiter = ',';
  std::string line;

  std::unordered_map<std::string, Column> data;
  std::unordered_map<int, std::string> csvIndexToName;

  auto isInteger = [](const std::string &s) {
    if (s.empty())
      return false;
    size_t i = 0;
    if (s[i] == '+' || s[i] == '-') {
      if (++i >= s.size())
        return false;
    }
    for (; i < s.size(); ++i) {
      if (!std::isdigit(static_cast<unsigned char>(s[i])))
        return false;
    }
    return true;
  };

  auto isDouble = [](const std::string &s) {
    if (s.empty())
      return false;
    size_t i = 0;
    if (s[i] == '+' || s[i] == '-') {
      if (++i >= s.size())
        return false;
    }

    bool sawDigit = false;
    bool sawDot = false;

    for (; i < s.size(); ++i) {
      unsigned char c = static_cast<unsigned char>(s[i]);
      if (std::isdigit(c)) {
        sawDigit = true;
        continue;
      }
      if (s[i] == '.' && !sawDot) {
        sawDot = true;
        continue;
      }
      return false;
    }

    return sawDigit && sawDot;
  };

  int rowIndex = 0;
  while (std::getline(file, line)) {
    size_t start = 0;
    size_t end = line.find(delimiter);
    int currentColumn = 0;

    //
    while (end != std::string::npos) {
      const std::string entry = line.substr(start, end - start);
      if (rowIndex == 0) {
        auto it = std::find(targetColumns.begin(), targetColumns.end(), entry);
        if (it != targetColumns.end()) {
          csvIndexToName[std::distance(targetColumns.begin(), it)] = entry;
          data[entry];
        }
      } else if (csvIndexToName.contains(
                     currentColumn)) { // if a column of interest
        if (rowIndex == 1) {

          if (isInteger(entry)) {
            data[csvIndexToName[currentColumn]] = std::vector<int>{};
          } else if (isDouble(entry)) {
            data[csvIndexToName[currentColumn]] = std::vector<double>{};
          } else {
            data[csvIndexToName[currentColumn]] = std::vector<std::string>{};
          }
        }
        // push into column vec
        std::visit(Visitor{[entry](std::vector<double> &d) {
                             d.emplace_back(std::stod(entry));
                           },
                           [entry](std::vector<int> &i) {
                             i.emplace_back(std::stoi(entry));
                           },
                           [entry](std::vector<std::string> &s) {
                             s.emplace_back(entry);
                           }},
                   data[entry]);
      }
      start = end + 1;
      end = line.find(delimiter, start);
      currentColumn++;
    }

    // get last entry
    std::string entry = line.substr(start);

    // increment row index
    rowIndex++;
  }
  return data;
}
