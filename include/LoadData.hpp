/**
 * \author: Mikhail Chalakov
 * Email: mchalakov@wisc.edu
 * \date: 21/12/2025
 */
#pragma once
#include "Column.hpp"
#include <expected>
#include <span>
#include <string>
#include <unordered_map>
#include <variant>
#include <vector>
/**
 * Enum to depict all of the different types of errors in loading our csv
 */
enum LoadError {
  FileNotFound,
  InvalidCSV,
  MissingTarget,
  NonNumericValue,
  EmptyDataset
};

class LoadData {
  const int yearIndex = 6;
  const int monthIndex = 3;
  const int dayIndex = 0;
  const char delimiter = ',';
  int parseDate(const std::string &dateStr);
  std::vector<std::string> parseCSVLine(const std::string &line);

public:
  std::expected<std::unordered_map<std::string, Column>, LoadError>
  fromCSV(const std::string &filename,
          const std::span<std::string_view> targetColumns);
  LoadData() = default;
}; // class LoadData