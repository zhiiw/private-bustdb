//===----------------------------------------------------------------------===//
//
//                         BusTub
//
// seq_scan_executor.cpp
//
// Identification: src/execution/seq_scan_executor.cpp
//
// Copyright (c) 2015-19, Carnegie Mellon University Database Group
//
//===----------------------------------------------------------------------===//
#include "execution/executors/seq_scan_executor.h"

namespace bustub {

SeqScanExecutor::SeqScanExecutor(ExecutorContext *exec_ctx, const SeqScanPlanNode *plan) : AbstractExecutor(exec_ctx),plan_(plan) {
  this->table_info_ = exec_ctx_->GetCatalog()->GetTable(plan->GetTableOid());
}

void SeqScanExecutor::Init() {
  table_iter = std::make_unique<TableIterator>(table_info_->table_->Begin(exec_ctx_->GetTransaction()));
}

bool SeqScanExecutor::Next(Tuple *tuple, RID *rid) {
  Tuple raw_tuple;
  do {
    if (*table_iter == table_info_->table_->End()) {
      return false;
    }

    raw_tuple = *(*table_iter);

    ++(*table_iter);
  } while (plan_->GetPredicate() != nullptr &&
           !plan_->GetPredicate()->Evaluate(&raw_tuple, &(table_info_->schema_)).GetAs<bool>());
  std::vector<Value> ee;
  std::transform(plan_->OutputSchema()->GetColumns().begin(),plan_->OutputSchema()->GetColumns().end(), std::back_inserter(ee), [](const Value &v) {
    return v;
  });
  *rid = raw_tuple.GetRid();
  *tuple ={ee,plan_->OutputSchema()};
}

}  // namespace bustub
