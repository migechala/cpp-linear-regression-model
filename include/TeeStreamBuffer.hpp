#pragma once
#include <cstddef>
#include <streambuf>

class TeeStreamBuf : public std::streambuf {
 public:
  TeeStreamBuf(std::streambuf* out1, std::streambuf* out2)
      : out1_(out1), out2_(out2) {
    if (!out1_ && !out2_) {
      throw std::runtime_error("Both streams cannot be nullptr");
    }
  }

 protected:
  /**
   * Overflow function for a single character
   * \param ch the character to to be written
   * \return either EOF or new pos
   */
  int overflow(int ch) override {
    if (ch == EOF) return !EOF;

    // forward to both buffers
    if (!out1_) {
      return out1_->sputc(static_cast<char>(ch));
    }
    if (!out2_) {
      return out1_->sputc(static_cast<char>(ch));
    }

    return (out1_->sputc(static_cast<char>(ch)) == EOF ||
            out2_->sputc(static_cast<char>(ch)) == EOF)
               ? EOF
               : ch;
  }

  /**
   * Tee stream into out1_ and out2_
   * \param s the text to be tee'd
   * \param n the streamsize to be streamed
   * \return the new streamsize which should be 0
   */
  std::streamsize xsputn(const char* s, std::streamsize n) override {
    std::streamsize r1, r2;
    if (out1_) {
      r1 = out1_->sputn(s, n);
    }
    if (out2_) {
      r2 = out2_->sputn(s, n);
    }
    return (r1 < n || r2 < n) ? 0 : n;  // both must succeed
  }

 private:
  std::streambuf* out1_;  // e.g. std::cout.rdbuf()
  std::streambuf* out2_;  // e.g. file_stream.rdbuf()
};
