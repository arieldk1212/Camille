#ifndef CAMILLE_INCLUDE_CAMILLE_DATA_STRUCTURES_H_
#define CAMILLE_INCLUDE_CAMILLE_DATA_STRUCTURES_H_

#include <memory>
#include <unordered_map>

namespace camille {
namespace datastructure {

class DataStructure {
 public:
  virtual ~DataStructure() = default;
};

template <typename KeyType, typename ValueType>
class LruCache : public DataStructure {
 public:
  class LruNode {
   public:
    LruNode() = default;
    LruNode(KeyType key, ValueType value)
        : key(key),
          value(value) {}
    ~LruNode() = default;

    KeyType key{KeyType()};
    ValueType value{ValueType()};
    std::unique_ptr<LruNode> prev{nullptr};
    std::unique_ptr<LruNode> next{nullptr};
  };

  using UniqueNode = std::unique_ptr<LruNode>;

 public:
  explicit LruCache(size_t capacity)
      : ds_capacity_(capacity),
        head(std::make_unique<LruNode>()),
        tail(std::make_unique<LruNode>()) {}

 private:
  void RemoveNode(UniqueNode node) {
    node->next->prev = node->prev;
    node->prev->next = node->next;
  }

  void AddToHead(UniqueNode node) {
    node->next = head->next;
    node->prev = head;
    head->prev->next = node;
    head->next = node;
  }

  UniqueNode RemoveTail() {
    UniqueNode node = std::move(tail->prev);
    RemoveNode(node);
    return node;
  }

 private:
  size_t ds_size_{0};
  size_t ds_capacity_;
  std::unique_ptr<LruNode> head;
  std::unique_ptr<LruNode> tail;
  std::unordered_map<KeyType, UniqueNode> cache_;
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