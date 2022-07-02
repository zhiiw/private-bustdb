//===----------------------------------------------------------------------===//
//
//                         BusTub
//
// update_executor.cpp
//
// Identification: src/execution/update_executor.cpp
//
// Copyright (c) 2015-20, Carnegie Mellon University Database Group
//
//===----------------------------------------------------------------------===//
#include <memory>

#include "execution/executors/update_executor.h"

namespace bustub {

UpdateExecutor::UpdateExecutor(ExecutorContext *exec_ctx, const UpdatePlanNode *plan,
                               std::unique_ptr<AbstractExecutor> &&child_executor)
    : AbstractExecutor(exec_ctx) ,plan_(plan),child_executor_{std::move(child_executor)}{
  auto oid = plan->TableOid();
  table_info_ = exec_ctx->GetCatalog()->GetTable(oid);

}

void UpdateExecutor::Init() {
  child_executor_->Init();
  table_indexes = this->exec_ctx_->GetCatalog()->GetTableIndexes(table_info_->name_);
}

bool UpdateExecutor::Next([[maybe_unused]] Tuple *tuple, RID *rid) {
  Tuple dummyTuple;
  RID emit_rid;

  if(!child_executor_->Next(&dummyTuple,&emit_rid)){
    return false;
  }

  Tuple to_be_ipdated;
  auto fetched = table_info_->table_->GetTuple(emit_rid,&to_be_ipdated,exec_ctx_->GetTransaction());
  if (!fetched) {
    return false;
  }
  Tuple updated_tuple = GenerateUpdatedTuple(to_be_ipdated);
  bool updated = table_info_->table_->UpdateTuple(updated_tuple,emit_rid,exec_ctx_->GetTransaction());
  if(updated){
    std::for_each(
        table_indexes.begin(),table_indexes.end(),
        [&to_be_ipdated,&updated_tuple,&emit_rid ,&table_info= table_info_, &ctx = exec_ctx_, this]
        (IndexInfo *indexInfo){
          indexInfo->index_->DeleteEntry(
              to_be_ipdated.KeyFromTuple(table_info_->schema_,indexInfo->key_schema_,indexInfo->index_->GetKeyAttrs()),
              emit_rid,exec_ctx_->GetTransaction()
              );
          indexInfo->index_->InsertEntry(
              updated_tuple.KeyFromTuple(table_info_->schema_,indexInfo->key_schema_,indexInfo->index_->GetKeyAttrs()),
              emit_rid,exec_ctx_->GetTransaction()
              );
          ctx->GetTransaction()->GetIndexWriteSet()->emplace_back(emit_rid, table_info->oid_, WType::UPDATE,
                                                                  updated_tuple, to_be_ipdated, indexInfo->index_oid_,
                                                                  ctx->GetCatalog());
        }

        );

  }
  return  updated;

}
}  // namespace bustub
