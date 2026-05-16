#pragma once

#include "InvertedIndex.hpp"
#include "Result.hpp"
#include "UpdateTransaction.hpp"
#include <functional>
#include <string>
#include <unordered_map>

namespace lab5
{

class IndexStore
{
    InvertedIndex index_;
    friend class UpdateTransaction;

  public:
    IndexStore() = default;

    Result<void> AddDocument(ParsedDocument&& doc);
    Result<void> RemoveDocument(std::size_t doc_id);
    Result<std::unordered_map<std::size_t, std::size_t>> Search(const std::string& word) const;
    Result<std::reference_wrapper<const Document>> GetDocument(std::size_t doc_id) const;

    Result<UpdateTransaction> BeginTransaction();
};

} // namespace lab5