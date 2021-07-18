//===----------------------------------------------------------------------===//
//
//                         BusTub
//
// buffer_pool_manager.cpp
//
// Identification: src/buffer/buffer_pool_manager.cpp
//
// Copyright (c) 2015-2019, Carnegie Mellon University Database Group
//
//===----------------------------------------------------------------------===//

#include "buffer/buffer_pool_manager.h"

#include <list>
#include <unordered_map>
#include "common/logger.h"
namespace bustub {

BufferPoolManager::BufferPoolManager(size_t pool_size, DiskManager *disk_manager, LogManager *log_manager)
    : pool_size_(pool_size), disk_manager_(disk_manager), log_manager_(log_manager) {
  // We allocate a consecutive memory space for the buffer pool.
  pages_ = new Page[pool_size_];
  replacer_ = new LRUReplacer(pool_size);

  // Initially, every page is in the free list.
  for (size_t i = 0; i < pool_size_; ++i) {
    free_list_.emplace_back(static_cast<int>(i));
  }
}

BufferPoolManager::~BufferPoolManager() {
  delete[] pages_;
  delete replacer_;
}

Page *BufferPoolManager::FetchPageImpl(page_id_t page_id) {
  // 1.     Search the page table for the requested page (P).
  // 1.1    If P exists, pin it and return it immediately.
  // 1.2    If P does not exist, find a replacement page (R) from either the free list or the replacer.
  //        Note that pages are always found from the free list first.
  // 2.     If R is dirty, write it back to the disk.
  // 3.     Delete R from the page table and insert P.
  // 4.     Update P's metadata, read in the page content from disk, and then return a pointer to P.
  int r = 0; //the old frame id of the old page
  int p = 0;//the new frame id of the page.

  int oldPage=-1;

  if (page_table_.count(page_id) == 0) {
    if (free_list_.size() > 0) {
      p = free_list_.front();  // this is frame. use this frame to store the page.p is requested frame just store it in pages_[p]
      free_list_.pop_front();
    }
    else {
      if (replacer_->Size()==0){
        return nullptr;
      }
      replacer_->Victim(&r);  // get the frame id.
      for (auto it : page_table_) {
        if (it.second == r) {
          oldPage = it.first;//get the page id
          break;
        }
      }
      //r is frame_id
      if (pages_[r].IsDirty()) {//frame
        char * test = new char [PAGE_SIZE];
        test=pages_[r].GetData();
        disk_manager_->WritePage(oldPage, test);  // write to the r(eplacement page ) and store the page
      }

      // replacer_->Unpin(r);//maybe? not sure
      page_table_.erase(oldPage);
      p=r;
      // TODO: let a new var to define the page
    }
  } else {
    int frame = page_table_[page_id];
    replacer_->Pin(frame);
    return &pages_[frame];
  }
  page_table_[page_id] = p;  // page id to frame id.

  pages_[p].is_dirty_ = false;
  pages_[p].pin_count_++;
  pages_[p].page_id_ = page_id;
  replacer_->Pin(p);
  char * ee = new char [PAGE_SIZE];
  disk_manager_->ReadPage(page_id,ee);

  strcpy(pages_[p].GetData(), ee);

  return &pages_[p];
}
//manage pin page
bool BufferPoolManager::UnpinPageImpl(page_id_t page_id, bool is_dirty) {
  if (page_table_.count(page_id) == 0) {
    return false;
  }
  int frame = page_table_[page_id];

  if (pages_[frame].pin_count_==0){

    return true;
  }else if (pages_[frame].pin_count_<0){
    return false;
  }
  pages_[frame].pin_count_=0;
  pages_[frame].is_dirty_ = is_dirty;
  replacer_->Unpin(frame);
  return true;
}

bool BufferPoolManager::FlushPageImpl(page_id_t page_id) {
  // Make sure you call DiskManager::WritePage!
  if (page_table_.count(page_id) == 0) {
    return false;
  }
  int frame = page_table_[page_id];
  pages_[frame].is_dirty_ = false;
  pages_[frame].pin_count_ = 0;
  disk_manager_->WritePage(page_id, pages_[frame].GetData());
  return true;
}

Page *BufferPoolManager::NewPageImpl(page_id_t *page_id) {
  if (replacer_->Size() == 0 && free_list_.size() == 0) {
    return nullptr;
  }
  int frame = 0;
  if (free_list_.size() > 0) {
    frame = free_list_.front();  // this is frame id
    free_list_.pop_front();
  } else {

    replacer_->Victim(&frame);
    int pageTemp;
    for (auto i : page_table_) {
      if (i.second == frame) {
        pageTemp = i.first;
        page_table_.erase(pageTemp);  // delete the first page id to contain the current page id
        break;
      }
    }
    if (pages_[frame].IsDirty()) {//frame
      char * test = new char [PAGE_SIZE];
      test=pages_[frame].GetData();

      disk_manager_->WritePage(pageTemp, test);  // write to the r(eplacement page ) and store the page
    }
    page_table_.erase(pageTemp);
  }
  int page = disk_manager_->AllocatePage();
  page_table_[page] = frame;
  pages_[frame].page_id_ = page;
  pages_[frame].pin_count_ = 1;
  pages_[frame].is_dirty_= false;

  *page_id = page;
  // 0.   Make sure you call DiskManager::AllocatePage!
  // 1.   If all the pages in the buffer pool are pinned, return nullptr.
  // 2.   Pick a victim page P from either the free list or the replacer. Always pick from the free list first.
  // 3.   Update P's metadata, zero out memory and add P to the page table.
  // 4.   Set the page ID output parameter. Return a pointer to P.
  return &pages_[frame];
}

bool BufferPoolManager::DeletePageImpl(page_id_t page_id) {
  if (page_table_.count(page_id) == 0) return true;
  auto frame = page_table_[page_id];
  if (pages_[frame].pin_count_ > 0) {
    return false;
  }
  free_list_.emplace_back(frame);
  page_table_.erase(page_id);
  disk_manager_->DeallocatePage(page_id);
  pages_[frame].is_dirty_ = false;
  pages_[frame].page_id_ = INVALID_PAGE_ID;
  // 0.   Make sure you call DiskManager::DeallocatePage!
  // 1.   Search the page table for the requested page (P).
  // 1.   If P does not exist, return true.
  // 2.   If P exists, but has a non-zero pin-count, return false. Someone is using the page.
  // 3.   Otherwise, P can be deleted. Remove P from the page table, reset its metadata and return it to the free list.
  return true;
}

void BufferPoolManager::FlushAllPagesImpl() {
  for (auto i : page_table_) {
    disk_manager_->WritePage(i.first, pages_[i.second].GetData());
    pages_[i.second].is_dirty_ = false;
  }
  // You can do it!
}

}  // namespace bustub
