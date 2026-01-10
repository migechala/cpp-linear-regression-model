#include <expected>
#include <span>
#include <string>
#include <unordered_map>
#include <variant>
#include <vector>

enum LoadError {
  FileNotFound,
  InvalidCSV,
  MissingTarget,
  NonNumericValue,
  EmptyDataset
};
using Column = std::variant<std::vector<int>, std::vector<double>,
                            std::vector<std::string>>;
class LoadData {
  const int yearIndex = 6;
  const int monthIndex = 3;
  const int dayIndex = 0;
  int parseDate(const std::string &dateStr);

public:
  std::expected<std::unordered_map<std::string, Column>, LoadError>
  fromCSV(const std::string &filename,
          const std::span<std::string_view> targetColumns);
  LoadData() = default;
}; // class LoadData