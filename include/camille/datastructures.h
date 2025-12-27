#ifndef CAMILLE_INCLUDE_CAMILLE_DATA_STRUCTURES_H_
#define CAMILLE_INCLUDE_CAMILLE_DATA_STRUCTURES_H_

namespace camille {
namespace datastructure {

class Datastructure {
 public:
  virtual ~Datastructure() = default;
};

class DynamicBuffer : public Datastructure {
 public:
 private:
};

};  // namespace datastructure
};  // namespace camille

#endif

/**
// A safer way to handle growing data
std::vector<std::byte> dynamic_buffer;
dynamic_buffer.reserve(4096); // Start with 4KB

while (headers_not_complete) {
    std::byte temp_chunk[1024];
    auto result = stream->Read(temp_chunk, 1024);

    // Append to our growing buffer
    dynamic_buffer.insert(dynamic_buffer.end(), temp_chunk, temp_chunk + result);

    // Security Check: Don't let the buffer grow to 1GB!
    if (dynamic_buffer.size() > MAX_ALLOWED_HEADER_SIZE) {
        throw std::runtime_error("Request Header Too Large");
    }
}
 */