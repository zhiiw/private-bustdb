//===----------------------------------------------------------------------===//
//
//                         BusTub
//
// lru_replacer.cpp
//
// Identification: src/buffer/lru_replacer.cpp
//
// Copyright (c) 2015-2019, Carnegie Mellon University Database Group
//
//===----------------------------------------------------------------------===//

#include "buffer/lru_replacer.h"

namespace bustub {

LRUReplacer::LRUReplacer(size_t num_pages) {
  this->num_pages = num_pages;  // capacity
  this->size = 0;
  first = new Node();
  end = new Node();
  first->next = end;
  end->prev = first;
  first->prev = nullptr;
  end->next = nullptr;
  end->val = -1;
  first->val = -2;

}

LRUReplacer::~LRUReplacer() {
  lru_latchs.lock();
  Node *temp = end;
  Node *newerOne;
  while (!temp->prev) {
    newerOne = temp->prev;
    delete temp;
    temp = newerOne;
  }
  lru_latchs.unlock();
}

bool LRUReplacer::Victim(frame_id_t *frame_id) {
  lru_latchs.lock();

  if (size == 0) {
    frame_id = nullptr;
    lru_latchs.unlock();
    return false;
  }
  Node *temp;
  temp = first->next;
  first->next = temp->next;
  temp->next->prev = first;
  frame_id_t ans = temp->val;
  *frame_id = ans;
  mapIn.erase(ans);
  size--;
  delete temp;
  lru_latchs.unlock();
  return true;
}

void LRUReplacer::Pin(frame_id_t frame_id) {
  lru_latchs.lock();
  Node *temp;
  Node *newOne;
  if (mapIn.count(frame_id) == 0) {
    lru_latchs.unlock();

    return;
  }
  temp = mapIn[frame_id];
  newOne = temp->next;        // end
  newOne->prev = temp->prev;  // end->prev=9
  temp->prev->next = newOne;  // 9->next=end
  size--;
  mapIn.erase(frame_id);
  delete temp;
  lru_latchs.unlock();
}

void LRUReplacer::Unpin(frame_id_t frame_id) {
  lru_latchs.lock();
  Node *newOne;
  Node *temp = new Node();

  if (mapIn.count(frame_id) != 0) {

    lru_latchs.unlock();

    return;
  }
  if (size < num_pages) {
    newOne = end->prev;
    newOne->next = temp;
    temp->next = end;
    temp->prev = newOne;
    temp->val = frame_id;
    end->prev = temp;
    size++;
    mapIn[frame_id] = temp;
    lru_latchs.unlock();

    return;
  }
  newOne = first->next;
  first->next = temp;
  temp->next = newOne;
  temp->prev = first;
  newOne->prev = temp;
  temp->val = frame_id;
  mapIn[frame_id] = temp;
  size++;
  lru_latchs.unlock();
}

size_t LRUReplacer::Size() { return size; }

}  // namespace bustub
