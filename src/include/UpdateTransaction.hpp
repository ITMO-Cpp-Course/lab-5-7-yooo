#pragma once

#include "DocumentBuilder.hpp"
#include "Result.hpp"
#include <unordered_map>
#include <unordered_set>

namespace lab5
{

class IndexStore;

class UpdateTransaction
{
    IndexStore* store_;
    std::unordered_map<std::size_t, ParsedDocument> pending_adds_;
    std::unordered_set<std::size_t> pending_removes_;
    bool committed_{false};

  public:
    explicit UpdateTransaction(IndexStore& store);

    UpdateTransaction(const UpdateTransaction&) = delete;
    UpdateTransaction& operator=(const UpdateTransaction&) = delete;

    UpdateTransaction(UpdateTransaction&& other) noexcept;
    UpdateTransaction& operator=(UpdateTransaction&& other) noexcept;

    ~UpdateTransaction();

    Result<void> AddDocument(ParsedDocument&& doc);
    Result<void> RemoveDocument(std::size_t doc_id);

    Result<void> Commit();
};

} // namespace lab5