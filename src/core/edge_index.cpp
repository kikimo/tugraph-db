/* Copyright (c) 2022 AntGroup. All Rights Reserved. */

#include "core/edge_index.h"
#include "core/transaction.h"

namespace lgraph {
EdgeIndexIterator::EdgeIndexIterator(EdgeIndex* idx, Transaction* txn,
                                     KvTable& table, const Value& key_start,
                                     const Value& key_end, VertexId vid,
                                     VertexId vid2, LabelId lid, EdgeId eid,
                                     bool unique)
    : IteratorBase(txn),
      index_(idx),
      it_(txn->GetTxn(), table,
          unique ? key_start : _detail::PatchKeyWithEUid(key_start, vid, vid2, lid, eid),
          true),
      key_end_(unique ? Value::MakeCopy(key_end) :
                        _detail::PatchKeyWithEUid(key_end, -1, -1, -1, -1)),
      iv_(),
      valid_(false),
      pos_(0),
      unique_(unique) {
    if (!it_.IsValid() || KeyOutOfRange()) {
        return;
    }
    LoadContentFromIt();
}

EdgeIndexIterator::EdgeIndexIterator(EdgeIndex* idx, KvTransaction* txn,
                                     KvTable& table, const Value& key_start,
                                     const Value& key_end, VertexId vid,
                                     VertexId vid2, LabelId lid, EdgeId eid, bool unique)
    : IteratorBase(nullptr),
      index_(idx),
      it_(*txn, table,
          unique ? key_start : _detail::PatchKeyWithEUid(key_start, vid, vid2, lid, eid),
          true),
      key_end_(unique ? Value::MakeCopy(key_end) :
                      _detail::PatchKeyWithEUid(key_end, -1, -1, -1, -1)),
      iv_(),
      valid_(false),
      pos_(0),
      unique_(unique) {
    if (!it_.IsValid() || KeyOutOfRange()) {
        return;
    }
    LoadContentFromIt();
}

EdgeIndexIterator::EdgeIndexIterator(EdgeIndexIterator&& rhs)
    : IteratorBase(std::move(rhs)),
      index_(rhs.index_),
      it_(std::move(rhs.it_)),
      key_end_(std::move(rhs.key_end_)),
      curr_key_(std::move(rhs.curr_key_)),
      iv_(std::move(rhs.iv_)),
      valid_(rhs.valid_),
      pos_(rhs.pos_),
      src_vid_(rhs.src_vid_),
      dst_vid_(rhs.dst_vid_),
      lid_(rhs.lid_),
      eid_(rhs.eid_),
      unique_(rhs.unique_) {
    rhs.valid_ = false;
}

void EdgeIndexIterator::CloseImpl() {
    it_.Close();
    valid_ = false;
}
}  // namespace lgraph
