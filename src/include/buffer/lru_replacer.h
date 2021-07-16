//===----------------------------------------------------------------------===//
//
//                         BusTub
//
// lru_replacer.h
//
// Identification: src/include/buffer/lru_replacer.h
//
// Copyright (c) 2015-2019, Carnegie Mellon University Database Group
//
//===----------------------------------------------------------------------===//

#pragma once

#include <mutex>  // NOLINT
#include <unordered_map>
#include "buffer/replacer.h"
#include "common/config.h"
namespace bustub {

/**
 * LRUReplacer implements -+the lru replacement policy, which approximates the Least Recently Used policy.
 */
struct Node {
  Node *next;
  Node *prev;
  frame_id_t val;
};
class LRUReplacer : public Replacer {
 public:
  /**
   * Create a new LRUReplacer.
   * @param num_pages the maximum number of pages the LRUReplacer will be required to store
   */
  explicit LRUReplacer(size_t num_pages);

  /**
   * Destroys the LRUReplacer.
   */
  ~LRUReplacer() override;

  bool Victim(frame_id_t *frame_id) override;

  void Pin(frame_id_t frame_id) override;

  void Unpin(frame_id_t frame_id) override;

  size_t Size() override;

 private:
  int num_pages;
  int size;
  Node *first;
  Node *end;
  std::unordered_map<frame_id_t,Node *> mapIn;

  // TODO(student): implement me!
};

}  // namespace bustub
