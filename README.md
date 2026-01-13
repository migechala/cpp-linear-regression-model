# Basic Linear Regression Model
Included in this repo is an example csv that is downloaded from a third party website. Be mindful that the example csv downloaded is 255 Mb so you might prefer to use your own csv, just adjust the features in `main.cpp`.
```bash
git clone https://github.com/migechala/cpp-linear-regression-model.git
cd cpp-linear-regression-model
chmod +x download_sample_csv.sh
./download_sample_csv
cmake -S . -B build && cmake --build build
./build/linear_regression
```