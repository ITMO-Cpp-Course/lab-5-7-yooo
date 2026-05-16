#include "IndexStore.hpp"

namespace lab5
{

Result<void> IndexStore::AddDocument(ParsedDocument&& doc)
{
    auto tx_res = BeginTransaction();
    if (!tx_res)
        return std::unexpected(tx_res.error());

    auto add_res = tx_res->AddDocument(std::move(doc));
    if (!add_res)
        return add_res;

    return tx_res->Commit();
}

Result<void> IndexStore::RemoveDocument(std::size_t doc_id)
{
    auto tx_res = BeginTransaction();
    if (!tx_res)
        return std::unexpected(tx_res.error());

    auto rm_res = tx_res->RemoveDocument(doc_id);
    if (!rm_res)
        return rm_res;

    return tx_res->Commit();
}

Result<std::unordered_map<std::size_t, std::size_t>> IndexStore::Search(const std::string& word) const
{
    return index_.Search(word);
}

Result<std::reference_wrapper<const Document>> IndexStore::GetDocument(std::size_t doc_id) const
{
    auto opt = index_.GetDocument(doc_id);
    if (opt.has_value())
    {
        return *opt;
    }
    return std::unexpected(Error{ErrorCode::DocumentNotFound, "Document not found in store"});
}

Result<UpdateTransaction> IndexStore::BeginTransaction()
{
    return UpdateTransaction(*this);
}

} // namespace lab5