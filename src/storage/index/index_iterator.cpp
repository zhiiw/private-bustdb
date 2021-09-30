/**
 * index_iterator.cpp
 */
#include <cassert>

#include "storage/index/index_iterator.h"

namespace bustub {

/*
 * NOTE: you can change the destructor/constructor method here
 * set your own input parameters
 */
INDEX_TEMPLATE_ARGUMENTS
INDEXITERATOR_TYPE::IndexIterator(BufferPoolManager *bpm,BPlusTreeLeafPage<KeyType, ValueType, KeyComparator> *leaf,int index) {
  this->index= index;
  this->bpm=bpm;
  this->leaf=leaf;
  //itr=ee.begin();
}

INDEX_TEMPLATE_ARGUMENTS
INDEXITERATOR_TYPE::~IndexIterator() {

}

INDEX_TEMPLATE_ARGUMENTS
bool INDEXITERATOR_TYPE::isEnd() {
  if (this->leaf== nullptr|| this->index== this->leaf->GetMaxSize()){
    return true;
  }
  return false;
}

INDEX_TEMPLATE_ARGUMENTS
const MappingType &INDEXITERATOR_TYPE::operator*() {
    return leaf->GetItem(this->index);
}

INDEX_TEMPLATE_ARGUMENTS
INDEXITERATOR_TYPE &INDEXITERATOR_TYPE::operator++() {
  this->index++;
  if (this->index>=this->leaf->GetSize()){
    auto next_id = leaf->GetNextPageId();
    auto next_page = reinterpret_cast<BPlusTreeLeafPage<KeyType, ValueType, KeyComparator> *>(this->bpm->FetchPage(next_id)->GetData());
    bpm->UnpinPage(leaf->GetPageId(), false);
    this->leaf=next_page;
  }
  return *this;
}

template class IndexIterator<GenericKey<4>, RID, GenericComparator<4>>;

template class IndexIterator<GenericKey<8>, RID, GenericComparator<8>>;

template class IndexIterator<GenericKey<16>, RID, GenericComparator<16>>;

template class IndexIterator<GenericKey<32>, RID, GenericComparator<32>>;

template class IndexIterator<GenericKey<64>, RID, GenericComparator<64>>;

}  // namespace bustub
