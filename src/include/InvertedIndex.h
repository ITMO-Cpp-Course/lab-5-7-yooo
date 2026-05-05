#pragma once
#include "DocumentBuilder.h"
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>
namespace lab5
{

class InvertedIndex
{
    using word_t = std::string;
    using doc_id_t = std::size_t;
    using count_t = std::size_t;

    std::unordered_map<doc_id_t, Document> docs_;
    std::unordered_map<word_t, std::unordered_map<doc_id_t, count_t>> index_;

  public:
    void AddDocument(ParsedDocument&& parsed_doc);

    void RemoveDocument(doc_id_t doc_id);

    std::unordered_map<doc_id_t, count_t> Search(const word_t& word) const;

    std::optional<std::reference_wrapper<const Document>> GetDocument(doc_id_t doc_id) const;
};

} // namespace lab5