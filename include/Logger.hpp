/**
 * Author: Mikhail Chalakov
 * Email: mchalakov@wisc.edu
 * Date: 16/12/2025
 */
#pragma once

#include <fstream>
#include <iostream>
#include <string>

#include "TeeStreamBuffer.hpp"

enum LOG_VALUES { INFO, WARNING, ERROR };

/**
 * Logger class that works via singleton and by teeing into two (or one) buffers
 */
class Logger {
public:
  /**
   * Method to log, use << afterwards to stream into the logger
   * \return the instance of the logger
   */
  static Logger& log() {
    static Logger instance_;
    return instance_;
  }

  /**
   * Open a file to write log content within
   * \param path the file path desired
   */
  static void open(const std::string& path) { log().openImpl(path); }

  /**
   * Get the stream object
   * \return the stream object
   */
  std::ostream&              out() { return stream_; }
  template <class T> Logger& operator<<(const T& v) {
    if (!stream_ || !tee_) {
      tee_.reset(new TeeStreamBuf(std::cout.rdbuf(), nullptr));
      stream_.rdbuf(tee_.get());
    }
    stream_ << v;
    return *this;
  }
  Logger& operator<<(std::ostream& (*manip)(std::ostream&)) {
    stream_ << manip;
    return *this;
  }

private:
  Logger() : stream_(nullptr) {}

  /**
   * Open the file and tee the stream
   * \param path the file path for the ofstream
   */
  void openImpl(const std::string& path) {
    if (!file_.is_open()) {
      file_.open(path, std::ios::out | std::ios::app);
      if (!file_) throw std::runtime_error("Can't open log file");
      tee_.reset(new TeeStreamBuf(std::cout.rdbuf(), file_.rdbuf()));
      stream_.rdbuf(tee_.get());
    }
  }

  std::ofstream                 file_;
  std::unique_ptr<TeeStreamBuf> tee_;
  std::ostream                  stream_;
};