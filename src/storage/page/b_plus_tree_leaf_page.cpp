//===----------------------------------------------------------------------===//
//
//                         CMU-DB Project (15-445/645)
//                         ***DO NO SHARE PUBLICLY***
//
// Identification: src/page/b_plus_tree_leaf_page.cpp
//
// Copyright (c) 2018, Carnegie Mellon University Database Group
//
//===----------------------------------------------------------------------===//

#include <sstream>

#include "common/exception.h"
#include "common/rid.h"
#include "storage/page/b_plus_tree_leaf_page.h"

namespace bustub {

/*****************************************************************************
 * HELPER METHODS AND UTILITIES
 *****************************************************************************/

/**
 * Init method after creating a new leaf page
 * Including set page type, set current size to zero, set page id/parent id, set
 * next page id and set max size
 */
INDEX_TEMPLATE_ARGUMENTS
void B_PLUS_TREE_LEAF_PAGE_TYPE::Init(page_id_t page_id, page_id_t parent_id, int max_size) {
    this->SetMaxSize(max_size);
    this->SetPageId(page_id);
    this->SetParentPageId(parent_id);
    this->SetPageType(IndexPageType::LEAF_PAGE);
    this->SetSize(0);
}

/**
 * Helper methods to set/get next page id
 */
INDEX_TEMPLATE_ARGUMENTS
page_id_t B_PLUS_TREE_LEAF_PAGE_TYPE::GetNextPageId() const {
  //maybe the page 's next page but how can i find it ,in my memory we can get it by pointer
  return next_page_id_;//yeah like 1111this as a pointer

}

INDEX_TEMPLATE_ARGUMENTS
void B_PLUS_TREE_LEAF_PAGE_TYPE::SetNextPageId(page_id_t next_page_id) {
  this->next_page_id_=next_page_id;
}

/**
 * Helper method to find the first index i so that array[i].first >= key
 * NOTE: This method is only used when generating index iterator
 */
INDEX_TEMPLATE_ARGUMENTS
int B_PLUS_TREE_LEAF_PAGE_TYPE::KeyIndex(const KeyType &key, const KeyComparator &comparator) const {
  assert(GetSize() >= 0);
  // use binary search
  int l = 0, r = GetSize() - 1, mid;
  while (l <= r) {
    mid = (r - l) / 2 + l;
    if (comparator(array[mid].first, key) < 0) {
      l = mid + 1;
    } else {
      r = mid - 1;
    }
  }
  return r + 1;
}

/*
 * Helper method to find and return the key associated with input "index"(a.k.a
 * array offset)
 */
INDEX_TEMPLATE_ARGUMENTS
KeyType B_PLUS_TREE_LEAF_PAGE_TYPE::KeyAt(int index) const {
  // replace with your own code

  return array[index].first;
}

/*
 * Helper method to find and return the key & value pair associated with input
 * "index"(a.k.a array offset)
 */
INDEX_TEMPLATE_ARGUMENTS
const MappingType &B_PLUS_TREE_LEAF_PAGE_TYPE::GetItem(int index) {
  // replace with your own code
  return array[index];
}

/*****************************************************************************
 * INSERTION
 *****************************************************************************/
/*
 * Insert key & value pair into leaf page ordered by key
 * @return  page size after insertion
 */
INDEX_TEMPLATE_ARGUMENTS
int B_PLUS_TREE_LEAF_PAGE_TYPE::Insert(const KeyType &key, const ValueType &value, const KeyComparator &comparator) {
  //this is the core function
  int size= this->GetSize();
  std::pair<KeyType,ValueType> temp;
  std::pair<KeyType,ValueType> ee;
  ee.first=key;
  ee.second=value;
  if (comparator(key,array[0].first)<0){//if key is smaller than all
    //rearrange array

    temp=array[0];
    array[0]=ee;
    for (int i = 1; i < size; ++i) {
      ee=temp;
      temp=array[i];
      array[i]=ee;
    }
  }else {
    for (int i = 0; i < size; ++i) {
      if (comparator(key, array[i].first)<0) {
        // should i manage to re arrange its position?yalei yalei ie seems it need to re arrage it
        for (int j = i; j < size; ++j) {
          if (j != i) {
            ee = temp;
          }
          temp = array[j];
          array[j] = ee;
        }
        array[size] = temp;
        break;
        // this should have another logic
      }
    }
  }
  /*
  std::pair<KeyType,ValueType> temp;
  std::pair<KeyType,ValueType> temp;
  ee.first=key;
  ee.second=value;
  if (size==this->GetMaxSize()){
    //split but i can't so...
  }
  else{
    bool flag=false;
    for (int i = 0; i < size; ++i) {

      if (array[i].second>ee.second){
        flag= true;
        //should i manage to re arrange its position?yalei yalei ie seems it need to re arrage it
        for (int j = i; j < size; ++j) {
          if (j!=i){
            ee=temp;
          }
          temp=array[j];
          array[j]=ee;
        }
        array[size]=temp;
        break;
        //this should have another logic
      }
    }
    if (!flag){
      array[size]=ee;
    }
  }*/
  size++;
  this->SetSize(size);
  return size;

}

/*****************************************************************************
 * SPLIT
 *****************************************************************************/
/*
 * Remove half of key & value pairs from this page to "recipient" page
 */
INDEX_TEMPLATE_ARGUMENTS
void B_PLUS_TREE_LEAF_PAGE_TYPE::MoveHalfTo(BPlusTreeLeafPage *recipient) {
  //the things is to ensure which is bigger I will set the recepient second
  recipient->SetNextPageId(this->GetNextPageId());
  this->SetNextPageId(recipient->GetPageId());
  int max=this->GetMaxSize();
  if (this->GetMaxSize()%2==0){
    this->SetSize(max/2);
    int j=0;
    for (int i = max/2; i < max; ++i) {
      recipient->array[j]= this->array[i];
      j++;
    }
  }else{
    this->SetSize(max/2+1);
    int j=0;
    for (int i = max/2+1; i < max; ++i) {
      recipient->array[j]= this->array[i];
      j++;
    }
  }
}

/*
 * Copy starting from items, and copy {size} number of elements into me.
 */
INDEX_TEMPLATE_ARGUMENTS
void B_PLUS_TREE_LEAF_PAGE_TYPE::CopyNFrom(MappingType *items, int size) {
  for (int i = 0; i < size; ++i) {
    this->array[i]=items[i];
  }
}

/*****************************************************************************
 * LOOKUP
 *****************************************************************************/
/*
 * For the given key, check to see whether it exists in the leaf page. If it
 * does, then store its corresponding value in input "value" and return true.
 * If the key does not exist, then return false
 */
INDEX_TEMPLATE_ARGUMENTS
bool B_PLUS_TREE_LEAF_PAGE_TYPE::Lookup(const KeyType &key, ValueType *value, const KeyComparator &comparator) const {
  for (int i = 0; i < this->GetSize(); ++i) {
    if (comparator(this->array[i].first,key)==0){
      return true;
    }
  }
  return false;
}

/*****************************************************************************
 * REMOVE
 *****************************************************************************/
/*
 * First look through leaf page to see whether delete key exist or not. If
 * exist, perform deletion, otherwise return immediately.
 * NOTE: store key&value pair continuously after deletion
 * @return   page size after deletion
 */
INDEX_TEMPLATE_ARGUMENTS
int B_PLUS_TREE_LEAF_PAGE_TYPE::RemoveAndDeleteRecord(const KeyType &key, const KeyComparator &comparator) {
  int size=this->GetSize();
  for (int i = 0; i < this->GetSize(); ++i) {
    if (comparator(this->array[i].first,key)==0){
      for (int j = i+1; j < this->GetSize()-1; ++j) {
        array[j-1]=array[j];
      }
      this->SetSize(size-1);
      return size-1;
    }
  }
  return this->GetSize();
}

/*****************************************************************************
 * MERGE
 *****************************************************************************/
/*
 * Remove all of key & value pairs from this page to "recipient" page. Don't forget
 * to update the next_page id in the sibling page
 */
INDEX_TEMPLATE_ARGUMENTS
void B_PLUS_TREE_LEAF_PAGE_TYPE::MoveAllTo(BPlusTreeLeafPage *recipient) {
  for (int i = 0; i < this->GetSize(); ++i) {
    recipient->array[i]=this->array[i];//use smart pointer
  }
  //TODO: delete all
  this->SetSize(0);
}

/*****************************************************************************
 * REDISTRIBUTE
 *****************************************************************************/
/*
 * Remove the first key & value pair from this page to "recipient" page.
 */
INDEX_TEMPLATE_ARGUMENTS
void B_PLUS_TREE_LEAF_PAGE_TYPE::MoveFirstToEndOf(BPlusTreeLeafPage *recipient) {
  recipient->array[this->GetSize()]=this->array[0];
  for (int j = 1; j < this->GetSize(); ++j) {
    array[j-1]=array[j];
  }
}

/*
 * Copy the item into the end of my item list. (Append item to my array)
 */
INDEX_TEMPLATE_ARGUMENTS
void B_PLUS_TREE_LEAF_PAGE_TYPE::CopyLastFrom(const MappingType &item) {
  this->array[this->GetSize()]=item;
}

/*
 * Remove the last key & value pair from this page to "recipient" page.
 */
INDEX_TEMPLATE_ARGUMENTS
void B_PLUS_TREE_LEAF_PAGE_TYPE::MoveLastToFrontOf(BPlusTreeLeafPage *recipient) {
  //TODO : bujizhezuo

  int size= this->GetSize();
  std::pair<KeyType,ValueType> temp;
  std::pair<KeyType,ValueType> ee;
  ee=this->array[this->GetSize()-1];
  //bool flag= false;
  //rearrange array
  temp=array[0];
  array[0]=ee;
  for (int i = 1; i < size; ++i) {
    ee=temp;
    temp=array[i];
    array[i]=ee;

  }
  this->SetSize(this->GetSize()+1);
}

/*
 * Insert item at the front of my items. Move items accordingly.
 */
INDEX_TEMPLATE_ARGUMENTS
void B_PLUS_TREE_LEAF_PAGE_TYPE::CopyFirstFrom(const MappingType &item) {
  int size= this->GetSize();
  std::pair<KeyType,ValueType> temp;
  std::pair<KeyType,ValueType> ee;
  ee=this->array[this->GetSize()-1];
  //bool flag= false;
  //rearrange array
  temp=array[0];
  array[0]=item;
  for (int i = 1; i < size; ++i) {
    ee=temp;
    temp=array[i];
    array[i]=ee;
  }
}

template class BPlusTreeLeafPage<GenericKey<4>, RID, GenericComparator<4>>;
template class BPlusTreeLeafPage<GenericKey<8>, RID, GenericComparator<8>>;
template class BPlusTreeLeafPage<GenericKey<16>, RID, GenericComparator<16>>;
template class BPlusTreeLeafPage<GenericKey<32>, RID, GenericComparator<32>>;
template class BPlusTreeLeafPage<GenericKey<64>, RID, GenericComparator<64>>;
}  // namespace bustub
