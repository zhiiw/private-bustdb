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
  first=new Node();
  end=new Node();
  first->next = end;
  end->prev = first;
  first->prev = NULL;
  end->next = NULL;
}

LRUReplacer::~LRUReplacer() {
  Node *temp = end;
  Node *newerOne;
  while (temp->prev == NULL) {
    newerOne = temp->prev;
    delete temp;
    temp = newerOne;
  }
}

bool LRUReplacer::Victim(frame_id_t *frame_id) {
  if (size == 0) {
    return false;
  }
  Node *temp=new Node();
  temp = first->next;
  first->next=temp->next;
  temp->next->prev=first;
  frame_id_t ans = temp->val;
  *frame_id = ans;
  mapIn.erase(ans);
  size--;
  delete temp;
  return true;
}

void LRUReplacer::Pin(frame_id_t frame_id) {
  Node *temp ;
  Node *newOne;
  if (mapIn.count(frame_id)==0) return;
  temp=mapIn[frame_id];
  newOne=temp->next;
  newOne->prev=temp->prev;
  temp->prev->next=newOne;
  size--;

  mapIn.erase(frame_id);
  delete temp;
  return;
}

void LRUReplacer::Unpin(frame_id_t frame_id) {
  Node* newOne;
  Node *temp = new Node();

  if (mapIn.count(frame_id) != 0) {
    newOne = mapIn[frame_id];
    newOne->prev->next = newOne->next;
    newOne->next->prev = newOne->prev;
    temp = first->next;
    first->next = newOne;
    temp->prev = newOne;
    return;
  }
  if (size<num_pages){
    newOne = end->prev;
    newOne->next=temp;
    temp->next=end;
    temp->prev=newOne;
    temp->val=frame_id;
    end->prev=temp;
    size++;
    mapIn[frame_id]=temp;
    return;
  }
  newOne = first->next;
  first->next=temp;
  temp->next=newOne;
  temp->prev=first;
  newOne->prev=temp;
  temp->val=frame_id;
  mapIn[frame_id]=temp;
  size++;
  return;
}

size_t LRUReplacer::Size() { return size; }

}  // namespace bustub
