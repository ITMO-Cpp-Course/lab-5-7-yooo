#include "IndexStore.hpp"

namespace lab5
{
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