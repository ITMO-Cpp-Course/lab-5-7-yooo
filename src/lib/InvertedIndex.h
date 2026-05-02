#pragma once
#include "DocumentBuilder.h"
#include <string>
#include <unordered_map>
#include <vector>

namespace lab5
{

class InvertedIndex
{
    std::unordered_map<std::size_t, Document> docs_;

    std::unordered_map<std::string, std::unordered_map<std::size_t, std::size_t>> index_;

  public:
    void AddDocument(ParsedDocument parsed_doc);

    void RemoveDocument(std::size_t doc_id);

    std::unordered_map<std::size_t, std::size_t> Search(const std::string& word) const;

    const Document* GetDocument(std::size_t doc_id) const;
};

} // namespace lab5