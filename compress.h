/*
 * Copyright (c) 2025, Chen Yuan <yuan.chen@whu.edu.cn>
 *
 * All rights reserved. No warranty, explicit or implicit, provided.
 */

#ifndef FASTCOMPRESS_COMPRESS_H
#define FASTCOMPRESS_COMPRESS_H

#include <cstdint>
#include <cstddef>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <zstd.h>
#include <lz4frame.h>

namespace FastCompress {

/**
 * @brief Abstract Class for Lossless Compression Algorithm
 * */
class LosslessCompressor {
 public:
  LosslessCompressor() = default;

  virtual ~LosslessCompressor() = default;

  /**
   * @brief compress content in src into already allocated dst
   * @param dst already allocated buffer
   * @param dst_len the length of destination buffer
   * @param src the source content to compress
   * @param src_len the length of source content
   * @return compressed size written into dst
   * */
  virtual size_t compress(void* dst, size_t dst_len, void* src, size_t src_len) = 0;

  /**
   * @brief decompress content in src into already allocated dst
   * @param dst already allocated buffer
   * @param dst_len the length of destination buffer
   * @param src the source content to decompress
   * @param src_len the exact length of source content
   * @return decompressed size written into dst
   * */
  virtual size_t decompress(void* dst, size_t dst_len, void* src, size_t src_len) = 0;
};


class ZSTD : public LosslessCompressor {
  int comp_level_;

 public:
  ZSTD() : comp_level_(ZSTD_fast) {}

  explicit ZSTD(int comp_level) : comp_level_(comp_level) {
    if(comp_level < ZSTD_minCLevel() || comp_level > ZSTD_maxCLevel()) {
      std::cout << "[ERROR]: invalid zstd compression level!" << std::endl;
      exit(EXIT_FAILURE);
    }
  }

  ~ZSTD() override = default;

  size_t compress(void* dst, size_t dst_len, void* src, size_t src_len) override {
    size_t compressed = ZSTD_compress(dst, dst_len, src, src_len, comp_level_);
    if(ZSTD_isError(compressed)) {
      std::cout << "[ERROR]: zstd compression error!" << std::endl;
      exit(EXIT_FAILURE);
    }
    return compressed;
  }

  size_t decompress(void* dst, size_t dst_len, void* src, size_t src_len) override {
    size_t decompressed = ZSTD_decompress(dst, dst_len, src, src_len);
    if(ZSTD_isError(decompressed)) {
      std::cout << "[ERROR]: zstd decompression error!" << std::endl;
      exit(EXIT_FAILURE);
    }
    return decompressed;
  }
};


class LZ4 : public LosslessCompressor {
  LZ4F_preferences_t preferences;
  LZ4F_decompressOptions_t dopt;
  LZ4F_dctx* dctx;

 public:
  LZ4() : dctx(nullptr) {
    memset(&preferences, 0, sizeof(LZ4F_preferences_t));
    memset(&dopt, 0, sizeof(LZ4F_decompressOptions_t));
    size_t res = LZ4F_createDecompressionContext(&dctx, LZ4F_VERSION);
    if(LZ4F_isError(res)) {
      std::cout << "[ERROR]: lz4 create decompression context error!" << std::endl;
      exit(EXIT_FAILURE);
    }
  }

  ~LZ4() {
    size_t res = LZ4F_freeDecompressionContext(dctx);
    if(LZ4F_isError(res)) {
      std::cout << "[ERROR]: lz4 free decompression context error!" << std::endl;
      exit(EXIT_FAILURE);
    }
  }

  size_t compress(void* dst, size_t dst_len, void* src, size_t src_len) override {
    size_t compressed = LZ4F_compressFrame(dst, dst_len, src, src_len, &preferences);
    if(LZ4F_isError(compressed)) {
      std::cout << "[ERROR]: lz4 compression error!" << std::endl;
      exit(EXIT_FAILURE);
    }
    return compressed;
  }

  size_t decompress(void* dst, size_t dst_len, void* src, size_t src_len) override {
    size_t res = LZ4F_decompress(dctx, dst, &dst_len, src, &src_len, &dopt);
    if(LZ4F_isError(res)) {
      std::cout << "[ERROR]: lz4 decompression error!" << std::endl;
      exit(EXIT_FAILURE);
    }
    return dst_len;
  }
};

}


#endif //FASTCOMPRESS_COMPRESS_H