#include "LinearRegression.hpp"
#include <algorithm>
#include <utility>
std::vector<double> LinearRegression::toDouble(const Column &col) const {
  std::vector<double> out;
  std::visit(Visitor{
                 [&](const std::vector<double> &var) {
                   for (const double &i : var) {
                     if (std::isnan(i)) {
                       throw std::runtime_error("Found NaN value in variable " +
                                                std::to_string(i));
                       out.emplace_back(i);
                     }
                   }
                 },
                 [&](const std::vector<int> &var) {
                   for (const int &i : var) {
                     double d = static_cast<double>(i);
                     if (std::isnan(d)) {
                       throw std::runtime_error("Found NaN value in variable" +
                                                std::to_string(i));
                     }
                     out.emplace_back(d);
                   }
                 },
                 [&](const std::vector<std::string> &var) {
                   for (const std::string &i : var) {
                     out.emplace_back((double)0.00);
                   }
                 },
             },
             col);
  return out;
}

void LinearRegression::buildMatrix(
    const std::initializer_list<Column> features) {
  std::vector<std::vector<double>> cols;
  for (const Column &i : features) {
    cols.emplace_back(toDouble(i));
  }
  size_t numFeatures = cols.size();
  size_t numSamples = cols[0].size();
  featureMatrix.resize(numSamples, std::vector<double>(numFeatures));
  for (size_t s = 0; s < numSamples; ++s) {
    for (size_t f = 0; f < numFeatures; ++f) {
      featureMatrix[s][f] = cols[f][s];
    }
  }
}

std::expected<std::vector<double>, std::string>
LinearRegression::gradientDescent(const std::vector<double> &y,
                                  double learningRate, int epochs) {
  size_t n = featureMatrix.size();
  size_t m = featureMatrix[0].size();
  if (n == 0 || m == 0) {
    return std::unexpected("Feature matrix is empty");
  }
  std::vector<double> w(m, 0.00);
  for (int i = 0; i < epochs; ++i) {
    std::vector<double> yhat = predict(w);
    double error = squareError(y, yhat).value_or(0.0);
    if (error < CONVERGENCE_THRESHOLD) {
      break;
    }
    for (size_t j = 0; j < m; ++j) {
      double gradient = 0.0;
      for (size_t k = 0; k < n; ++k) {
        gradient += (yhat[k] - y[k]) * featureMatrix[k][j];
      }
      if (std::isnan(gradient)) {
        return std::unexpected("Gradient resulted in NaN at epoch " +
                               std::to_string(i) + ", feature " +
                               std::to_string(j));
      }
      w[j] -= learningRate * gradient / n;
    }
  }
  return w;
}

std::vector<double> LinearRegression::fit(double learningRate, int epochs) {
  auto start = std::chrono::high_resolution_clock::now();
  Logger::log() << "Training model..." << std::endl;
  auto result = gradientDescent(y, learningRate, epochs);
  if (result) {
    weights = result.value();
  } else {
    Logger::log() << "Error in gradient descent: " << result.error()
                  << std::endl;
  }
  auto end = std::chrono::high_resolution_clock::now();
  std::chrono::duration<double> duration = end - start;
  Logger::log() << "Model trained, training took: " << duration.count()
                << " seconds" << std::endl;
  return weights;
}

double LinearRegression::predictSingle(const std::vector<double> &features) {
  double result = 0.0;
  for (size_t i = 0; i < features.size(); ++i) {
    result += features[i] * weights[i];
    if (std::isnan(result)) {
      throw std::runtime_error("Prediction resulted in NaN: [" +
                               std::to_string(i) + "]");
    }
  }
  return result;
}

LinearRegression::LinearRegression(std::initializer_list<Column> features,
                                   Column &dependant) {
  buildMatrix(features);
  y = toDouble(dependant);
}
