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

std::vector<std::string> LoadData::parseCSVLine(const std::string &line) {
  std::vector<std::string> out;
  out.reserve(32);

  std::string cell;
  cell.reserve(64);

  bool inQuotes = false;

  for (size_t i = 0; i < line.size(); ++i) {
    char c = line[i];

    if (inQuotes) {
      if (c == '"') {
        if (i + 1 < line.size() && line[i + 1] == '"') {
          cell.push_back('"');
          ++i;
        } else {
          inQuotes = false;
        }
      } else {
        cell.push_back(c);
      }
    } else {
      if (c == '"') {
        inQuotes = true;
      } else if (c == delimiter) {
        out.push_back(std::move(cell));
        cell.clear();
      } else {
        cell.push_back(c);
      }
    }
  }

  out.push_back(std::move(cell));
  return out;
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
    int currentColumn = 0;

    auto processCell = [&](const std::string &entry) {
      if (rowIndex == 0) {
        auto it = std::find(targetColumns.begin(), targetColumns.end(),
                            std::string_view(entry));
        if (it != targetColumns.end()) {
          csvIndexToName[currentColumn] = entry;
          data.try_emplace(entry);
        }
        return;
      }

      auto it = csvIndexToName.find(currentColumn);
      if (it == csvIndexToName.end()) {
        return;
      }

      const std::string &colName = it->second;

      if (rowIndex == 1) {
        if (isInteger(entry)) {
          data[colName] = std::vector<int>{};
        } else if (isDouble(entry)) {
          data[colName] = std::vector<double>{};
        } else {
          data[colName] = std::vector<std::string>{};
        }
      }
      std::visit(
          Visitor{
              [&](std::vector<double> &d) { d.emplace_back(std::stod(entry)); },
              [&](std::vector<int> &i) { i.emplace_back(std::stoi(entry)); },
              [&](std::vector<std::string> &s) { s.emplace_back(entry); },
          },
          data[colName]);
    };

    auto cells = parseCSVLine(line);

    for (currentColumn = 0; currentColumn < static_cast<int>(cells.size());
         ++currentColumn) {
      processCell(cells[currentColumn]);
    }
    rowIndex++;
  }
  return data;
}
