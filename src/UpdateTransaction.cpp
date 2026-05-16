#include "UpdateTransaction.hpp"
#include "IndexStore.hpp"

namespace lab5
{

UpdateTransaction::UpdateTransaction(IndexStore& store) : store_(&store) {}

UpdateTransaction::UpdateTransaction(UpdateTransaction&& other) noexcept
    : store_(other.store_), pending_adds_(std::move(other.pending_adds_)),
      pending_removes_(std::move(other.pending_removes_)), committed_(other.committed_)
{
    other.committed_ = true;
}

UpdateTransaction& UpdateTransaction::operator=(UpdateTransaction&& other) noexcept
{
    if (this != &other)
    {
        store_ = other.store_;
        pending_adds_ = std::move(other.pending_adds_);
        pending_removes_ = std::move(other.pending_removes_);
        committed_ = other.committed_;
        other.committed_ = true;
    }
    return *this;
}

UpdateTransaction::~UpdateTransaction() {}

Result<void> UpdateTransaction::AddDocument(ParsedDocument&& doc)
{
    if (committed_)
        return std::unexpected(Error{ErrorCode::TransactionFailed, "Transaction already committed"});

    std::size_t doc_id = doc.doc.GetId();

    bool exists_in_store = store_->index_.GetDocument(doc_id).has_value();
    bool pending_remove = (pending_removes_.find(doc_id) != pending_removes_.end());
    bool pending_add = (pending_adds_.find(doc_id) != pending_adds_.end());

    if (pending_add || (exists_in_store && !pending_remove))
    {
        return std::unexpected(Error{ErrorCode::DocumentAlreadyExists, "Document already exists"});
    }

    pending_adds_.emplace(doc_id, std::move(doc));
    return {};
}

Result<void> UpdateTransaction::RemoveDocument(std::size_t doc_id)
{
    if (committed_)
        return std::unexpected(Error{ErrorCode::TransactionFailed, "Transaction already committed"});

    bool exists_in_store = store_->index_.GetDocument(doc_id).has_value();
    bool pending_remove = (pending_removes_.find(doc_id) != pending_removes_.end());
    bool pending_add = (pending_adds_.find(doc_id) != pending_adds_.end());

    if (pending_remove || (!exists_in_store && !pending_add))
    {
        return std::unexpected(Error{ErrorCode::DocumentNotFound, "Document not found"});
    }

    if (pending_add)
    {
        pending_adds_.erase(doc_id);
    }

    if (exists_in_store)
    {
        pending_removes_.insert(doc_id);
    }

    return {};
}

Result<void> UpdateTransaction::Commit()
{
    if (committed_)
        return std::unexpected(Error{ErrorCode::TransactionFailed, "Transaction already committed"});

    for (std::size_t id : pending_removes_)
    {
        store_->index_.RemoveDocument(id);
    }

    for (auto& [id, doc] : pending_adds_)
    {
        store_->index_.AddDocument(std::move(doc));
    }

    committed_ = true;
    return {};
}

} // namespace lab5