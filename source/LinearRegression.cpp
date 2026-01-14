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
                     }
                     out.emplace_back(i);
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
                     Logger::log()
                         << "Warning: Non-numeric value '" << i
                         << "' found in feature column. Replacing with 0.00."
                         << std::endl;
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
  cols.reserve(features.size());
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
  const size_t n = featureMatrix.size();
  if (n == 0) {
    return std::unexpected("Feature matrix is empty (no rows)");
  }

  const size_t m = featureMatrix[0].size();
  if (m == 0) {
    return std::unexpected("Feature matrix is empty (no columns)");
  }

  for (size_t r = 0; r < n; ++r) {
    if (featureMatrix[r].size() != m) {
      return std::unexpected("Feature matrix is not rectangular at row " +
                             std::to_string(r));
    }
  }

  if (y.size() != n) {
    return std::unexpected(
        "y size mismatch: y.size()=" + std::to_string(y.size()) +
        " but featureMatrix has n=" + std::to_string(n) + " rows");
  }

  if (!(std::isfinite(learningRate) && learningRate > 0.0)) {
    return std::unexpected("Invalid learningRate (must be finite and > 0)");
  }

  if (epochs <= 0) {
    return std::unexpected("Invalid epochs (must be > 0)");
  }

  std::vector<double> w(m, 0.0);

  for (int epoch = 0; epoch < epochs; ++epoch) {
    std::vector<double> yhat = predict(w);

    if (yhat.size() != n) {
      return std::unexpected(
          "predict(w) returned wrong size: " + std::to_string(yhat.size()) +
          " expected " + std::to_string(n));
    }

    auto errExp = squareError(y, yhat);
    if (!errExp) {
      return std::unexpected("squareError failed at epoch " +
                             std::to_string(epoch) + ": " + errExp.error());
    }

    const double error = *errExp;

    if (!std::isfinite(error)) {
      return std::unexpected("Error became non-finite at epoch " +
                             std::to_string(epoch));
    }

    if (error < CONVERGENCE_THRESHOLD) {
      break;
    }

    for (size_t j = 0; j < m; ++j) {
      double gradient = 0.0;
      for (size_t k = 0; k < n; ++k) {
        gradient += (yhat[k] - y[k]) * featureMatrix[k][j];
      }

      if (!std::isfinite(gradient)) {
        return std::unexpected("Gradient became non-finite at epoch " +
                               std::to_string(epoch) + ", feature " +
                               std::to_string(j));
      }

      w[j] -= learningRate * (gradient / static_cast<double>(n));

      if (!std::isfinite(w[j])) {
        return std::unexpected("Weight became non-finite at epoch " +
                               std::to_string(epoch) + ", feature " +
                               std::to_string(j));
      }
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
  if (features.size() != weights.size()) {
    throw std::runtime_error(
        "predictSingle size mismatch: features.size()=" +
        std::to_string(features.size()) +
        " weights.size()=" + std::to_string(weights.size()));
  }

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
