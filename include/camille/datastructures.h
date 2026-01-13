#ifndef CAMILLE_INCLUDE_CAMILLE_DATA_STRUCTURES_H_
#define CAMILLE_INCLUDE_CAMILLE_DATA_STRUCTURES_H_

#include <unordered_map>
#include <list>

namespace camille {
namespace datastructure {

class DataStructure {
 public:
  virtual ~DataStructure() = default;
};

template <typename KeyType, typename ValueType>
class LruCache : public DataStructure {
 public:
  using Items = std::list<std::pair<KeyType, ValueType>>;
  using It = Items::iterator;
  using Cache = std::unordered_map<KeyType, It>;

 public:
  explicit LruCache(size_t capacity)
      : capacity_(capacity) {}

  std::optional<ValueType> get(const KeyType& key) {
    auto it = cache_.find(key);
    if (it == cache_.end()) {
      return std::nullopt;
    }
    items_.splice(items_.begin(), items_, it->second);
    return it->second->first;
  }

  void put(KeyType key, ValueType value) {
    auto it = cache_.find(key);
    if (it != cache_.end()) {
      items_.splice(items_.begin(), items_, it->second->second);
      it->second->first = value;
      return;
    }
    if (items_.size() == capacity_) {
      KeyType key_to_delete = items_.back().first;
      items_.pop_back();
      cache_.erase(key_to_delete);
    }

    items_.emplace_front(key, value);
    cache_[key] = items_.begin();
  }

 private:
  size_t capacity_;
  Items items_;
  Cache cache_;
};

class PrefixTree : public DataStructure {};

};  // namespace datastructure
};  // namespace camille

#endif