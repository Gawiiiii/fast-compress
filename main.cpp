#include <iostream>
#include <fstream>
#include <filesystem>
#include <algorithm>
#include <random>
#include <memory>
#include <unordered_map>
#include "compress.h"
#include "util.h"

using namespace FastCompress;
using namespace util;

static constexpr size_t kPageSize = 4096;
static constexpr size_t kMegaByte = 0x01 << 20;

class MemPage {
  char bytes[kPageSize]{};

 public:
  MemPage() = default;

  ~MemPage() = default;

  MemPage(const MemPage& page) { memcpy(bytes, page.bytes, kPageSize); }

  MemPage& operator=(const MemPage& page) {
    memcpy(bytes, page.bytes, kPageSize);
    return *this;
  }
};

//compressor factory function: algorithm name -> corresponding compressor
std::unique_ptr<LosslessCompressor> createCompressor(const std::string& algorithm) {
  if (algorithm == "lz4hc") {
    return std::make_unique<LZ4HC>();
  } else if (algorithm == "lz4") {
    return std::make_unique<LZ4>();
  } else if (algorithm == "lzo") {
    return std::make_unique<LZO>();
  } else if (algorithm == "lzo-rle") {
    return std::make_unique<LZORLE>();
  } else if (algorithm == "zstd") {
    return std::make_unique<ZSTD>();
  } else if (algorithm == "842") {
    return std::make_unique<Deflate842>();
  } else {
    throw std::invalid_argument("Unknown compression algorithm: " + algorithm);
  }
}

int main(int argc, char* argv[]) {
  if(argc < 3) {
    std::cerr << "[USAGE]: file path, block size [n pages], number of iteration,"
                 " [page random shuffle, false by default]" << std::endl;
    exit(EXIT_FAILURE);
  }

  std::string path = std::string(argv[1]);
  size_t block_size = std::stoul(argv[2]) * kPageSize;
  size_t niteration = std::stoul(argv[3]);
  bool page_shuffle = false;
  page_shuffle = argc >=5 ? std::stoi(argv[4]) : false;//not use page shuffle as defualt
  std::string algorithm = argc >=6 ? argv[5] : "zstd";//use zstd as default

  PinningMap pin;
  pin.pinning_thread(0, 0, pthread_self());
  std::cout << "[INFO]: block size " << block_size / kPageSize << " pages, "
            << "number of iterations " << niteration << std::endl;
  std::ifstream fin(path);
  if(!fin.good()) {
    std::cerr << "[ERROR]: can't open " << path << std::endl;
    exit(EXIT_FAILURE);
  }
  size_t size = std::filesystem::file_size(path);
  size = size / block_size * block_size;
  void* origin = aligned_alloc(kPageSize, size);
  fin.read((char*) origin, size);
  std::cout << "[INFO]: file size " << size << ", number of blocks " << size / block_size << std::endl;

  if(page_shuffle) {
    std::mt19937 generator(std::random_device{}());
    std::shuffle((MemPage*) origin, (MemPage*) ((uintptr_t) origin + size - kPageSize), generator);
  }

//  LosslessCompressor* compressor = new ZSTD();
//  LosslessCompressor* compressor = new LZ4();

  //use factory function to choose commpressor based on input
  std::unique_ptr<LosslessCompressor> compressor = createCompressor(algorithm);

  size_t comp_block_size = block_size * 2;
  void* compressed = aligned_alloc(kPageSize, comp_block_size * size / block_size);
  size_t* compressed_size = (size_t*) calloc(size / block_size, sizeof(size_t));
  size_t nblock = size / block_size;
  size_t total_compressed = 0;
  Timer timer;
  timer.start();

  //double loop compression
  for(size_t i = 0; i < niteration; i++) {//first loop: compression level
    for(size_t bid = 0; bid < nblock; bid++) {//second loop: compress each block
      void* dst = (char*) compressed + bid * comp_block_size;
      void* src = (char*) origin + bid * block_size;
      size_t res = compressor->compress(dst, comp_block_size, src, block_size);
      total_compressed += res;
      compressed_size[bid] = res;
    }
  }
  long drt = timer.duration_us();

  double tpt = double(size * niteration) / kMegaByte / drt * 1000000ul;
  double ratio = double(size * niteration) / total_compressed;
  std::cout << "[INFO]: compression throughput " << tpt << " MiB/Second" << std::endl;
  std::cout << "[INFO]: compression ratio (original size / compressed size) " << ratio
            << ", compressed size / original size " << 1 / ratio << std::endl;

  timer.start();

  //double loop decompression
  for(size_t i = 0; i < niteration; i++) {
    for(size_t bid = 0; bid < nblock; bid++) {
      void* src = (char*) compressed + bid * comp_block_size;
      void* dst = (char*) origin + bid * block_size;
      compressor->decompress(dst, block_size, src, compressed_size[bid]);
    }
  }
  drt = timer.duration_us();
  tpt = double(size * niteration) / kMegaByte / drt * 1000000ul;
  std::cout << "[INFO]: decompression throughput " << tpt << " MiB/Second" << std::endl;

  free(compressed_size);
  free(compressed);
  free(origin);

  return 0;
}