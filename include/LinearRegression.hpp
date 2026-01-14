#pragma once
#include "Column.hpp"
#include "Logger.hpp"
#include <expected>
#include <ranges>
#include <string>

class LinearRegression {
  std::vector<std::vector<double>> featureMatrix;
  std::vector<double> y;
  std::vector<double> weights;
  const double CONVERGENCE_THRESHOLD = 1e-6;

private:
  std::vector<double> toDouble(const Column &col) const;
  void buildMatrix(const std::initializer_list<Column> features);
  inline std::expected<double, std::string>
  squareError(const std::vector<double> &y,
              const std::vector<double> &yhat) const {
    if (y.size() != yhat.size()) {
      return std::unexpected("Vectors must be of the same length");
    }
    double error = 0.0;
    if (y.size() != yhat.size()) {
      return std::unexpected(
          "Vectors must be of the same length: " + std::to_string(y.size()) +
          " != " + std::to_string(yhat.size()));
    }
    for (auto [i, j] : std::views::zip(y, yhat)) {
      error += (i - j) * (i - j);
    }
    return error;
  }
  inline std::vector<double> predict(const std::vector<double> &w) {

    std::vector<double> yhat;
    yhat.reserve(featureMatrix.size());

    for (const auto &row : featureMatrix) {
      if (row.size() != w.size()) {
        throw std::runtime_error(
            "predict: row.size()=" + std::to_string(row.size()) +
            " but w.size()=" + std::to_string(w.size()));
      }
      double s = 0.0;
      for (size_t j = 0; j < w.size(); ++j) {
        s += row[j] * w[j];
      }
      yhat.emplace_back(s);
    }
    return yhat;
  }
  std::expected<std::vector<double>, std::string>
  gradientDescent(const std::vector<double> &y, double learningRate,
                  int epochs);

public:
  std::vector<double> fit(double learningRate = 0.01, int epochs = 1000);
  double predictSingle(const std::vector<double> &features);
  LinearRegression(std::initializer_list<Column> features, Column &dependant);
};
