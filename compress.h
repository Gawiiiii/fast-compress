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
#include <vector>
#include <zstd.h>
#include <lz4.h>
#include <lz4hc.h>
#include <lzo/lzo1x.h>
#include <zlib.h>

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


class LZ4HC : public LosslessCompressor {
public:
  LZ4HC() = default;
  ~LZ4HC()  override = default;
  int comp_level_ = 1;

  size_t compress(void* dst, size_t dst_len, void* src, size_t src_len) override {
    int compressed = LZ4_compress_HC((char*)src, (char*)dst, src_len, dst_len, comp_level_);
    if(compressed == 0) {
      std::cout << "[ERROR]: LZ4 compression error!" << std::endl;
      exit(EXIT_FAILURE);
    }
    return compressed;
  }

  size_t decompress(void* dst, size_t dst_len, void* src, size_t src_len) override {
    int decompressed = LZ4_decompress_safe((char*)src, (char *)dst, src_len, dst_len);
    if(decompressed < 0) {
      std::cout << "[ERROR]: LZ4 decompression error!" << std::endl;
      exit(EXIT_FAILURE);
    }
    return decompressed;
  }
};

class LZ4 : public LosslessCompressor {
public:
  LZ4() = default;
  ~LZ4() override = default;

  size_t compress(void* dst, size_t dst_len, void* src, size_t src_len) override {
    int compressed = LZ4_compress_default((char*)src, (char*)dst, src_len, dst_len);
    if(compressed == 0) {
      std::cout << "[ERROR]: LZ4 compression error!" << std::endl;
      exit(EXIT_FAILURE);
    }
    return compressed;
  }

  size_t decompress(void* dst, size_t dst_len, void* src, size_t src_len) override {
    int decompressed = LZ4_decompress_safe((char*)src, (char *)dst, src_len, dst_len);
    if(decompressed < 0) {
      std::cout << "[ERROR]: LZ4 decompression error!" << std::endl;
      exit(EXIT_FAILURE);
    }
    return decompressed;
  }
};

class LZO : public LosslessCompressor {
public:
  LZO() {
    if(lzo_init() != LZO_E_OK) {
      std::cout <<"[ERROR]: LZO initialization failed!" << std::endl;
      exit(EXIT_FAILURE);
    }
  }

  ~LZO() override = default;

  size_t compress(void* dst, size_t dst_len, void* src, size_t src_len) override {
    lzo_uint compressed_size;
    int result = lzo1x_1_11_compress(
      (const unsigned char*)src, src_len, 
      (unsigned char*)dst, &compressed_size, 
      wrkmem_);
    if(result != LZO_E_OK) {
      std::cout << "[ERROR]: LZO compression error!" << std::endl;
      exit(EXIT_FAILURE);
    }
    return compressed_size;
  }

  size_t decompress(void* dst, size_t dst_len, void* src, size_t src_len) override {
    lzo_uint decompressed_size;
    int res = lzo1x_decompress((const unsigned char*)src, src_len, (unsigned char*)dst, &decompressed_size, nullptr);
    if(res != LZO_E_OK) {
      std::cout << "[ERROR]: LZO decompression error!" << std::endl;
      exit(EXIT_FAILURE);
    }
    return decompressed_size;
  }

  static lzo_uint8_t wrkmem_[LZO1X_1_MEM_COMPRESS];
};

lzo_uint8_t LZO::wrkmem_[LZO1X_1_MEM_COMPRESS];

class LZORLE : public LosslessCompressor {
public:
  LZORLE() = default;
  ~LZORLE() override = default;
  static lzo_uint8_t wrkmem_[LZO1X_1_MEM_COMPRESS];

  size_t compress(void* dst, size_t dst_len, void* src, size_t src_len) override {
    //step1:lzo compression
    lzo_uint compressed_size = dst_len;
    int res = lzo1x_1_11_compress(
      (const unsigned char*)src, 
      (lzo_uint)src_len,
      (unsigned char*)dst, 
      &compressed_size, 
      wrkmem_);
    if(res != LZO_E_OK) {
      std::cout << "[ERROR]: LZO compression error!" <<std::endl;
      exit(EXIT_FAILURE);
    }

    //step2:rle compression
    std::vector<unsigned char> rle_compressed_data;
    rleCompress((unsigned char*)dst, compressed_size, rle_compressed_data);

    //step3:copy rle-compressed data to dst buffer
    if(rle_compressed_data.size() > dst_len) {
      std::cout << "[ERROR]: RLE compressed data exceeds destination buffer!" << std::endl;
      exit(EXIT_FAILURE);
    }
    std::copy(rle_compressed_data.begin(), rle_compressed_data.end(), (unsigned char*)dst);

    return rle_compressed_data.size();
  }

  size_t decompress(void* dst, size_t dst_len, void* src, size_t src_len) override {
    //step1:rle decompression
    std::vector<unsigned char> rle_decompressed_data;
    rleDecompress((unsigned char*)src, src_len, rle_decompressed_data);

    //step2:lzo decompression
    lzo_uint decompressed_size = dst_len;
    int res = lzo1x_decompress(
      (const unsigned char*)rle_decompressed_data.data(),
      (lzo_uint)rle_decompressed_data.size(), //注意：这里要用rle解压后的src长度 而不是src_len
      (unsigned char*)dst,
      &decompressed_size, 
      nullptr);
    if (res != LZO_E_OK) {
      std::cout << "[ERROR]: LZO decompression error!" << std::endl;
      exit(EXIT_FAILURE);
    }

    return decompressed_size;
  }

private:
  //rle compression func
  void rleCompress(const unsigned char* input, size_t input_len, std::vector<unsigned char>& output) {
    size_t i = 0;
    while(i < input_len) {
      unsigned char current_byte = input[i];
      size_t run_length = 1;
      //find out repeat rate of current byte
      while(i + 1 < input_len && input[i+ 1] == current_byte && run_length < 255) {
        i++;
        run_length++;
      }
      //write rle data
      output.push_back(current_byte);//byte
      output.push_back((unsigned char)run_length);//repeat rate
      i++;
    }
  }

  //rle decompression func
  void rleDecompress(const unsigned char* input, size_t input_len, std::vector<unsigned char>& output) {
    size_t i = 0;
    while(i < input_len) {
      unsigned char current_byte = input[i++];
      unsigned char run_length = input[i++];
      for (size_t j = 0; j < run_length; j++) {
        output.push_back(current_byte);
      }

    }
  }
};

lzo_uint8_t LZORLE::wrkmem_[LZO1X_1_MEM_COMPRESS];

//compress()用于已知缓冲区大小 一次性解压
//注意：在当前类作用域下，没加作用域的compress会解析成自己这个虚函数->无限递归 栈溢出
//使用::compress()明确调用zlib的全局函数 或直接用compress2() [uncompress()同理]
//compress2()用于不知道缓冲区大小 可以动态调整
//compressBound()根据待压缩数据估算压缩后最大可能大小 帮助创建足够大缓冲区
class Deflate842 : public LosslessCompressor {
public:
  Deflate842() = default;
  ~Deflate842() override = default;

  size_t compress(void* dst, size_t dst_len, void* src, size_t src_len) override {
    uLongf compressed_size = dst_len;
    int res = ::compress((Bytef*)dst, &compressed_size, (const Bytef*)src, src_len);
    if(res != Z_OK) {
      std::cout << "[ERROR]: Deflate compression error!" << std::endl;
      exit(EXIT_FAILURE);
    }
    return compressed_size;
  }

  size_t decompress(void* dst, size_t dst_len, void* src, size_t src_len) override {
    uLongf decompressed_size = dst_len;
    int res = ::uncompress((Bytef*)dst, &decompressed_size, (Bytef*)src, src_len);
    if(res != Z_OK) {
      std::cout << "[ERROR]: Deflate decompression error!" << std::endl;
      exit(EXIT_FAILURE);
    }
    return decompressed_size;
  }
};

}

#endif //FASTCOMPRESS_COMPRESS_H