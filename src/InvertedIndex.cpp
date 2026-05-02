#include "InvertedIndex.h"

namespace lab5
{

void InvertedIndex::AddDocument(ParsedDocument parsed_doc)
{
    std::size_t doc_id = parsed_doc.doc.GetId();

    for (const std::string& word : parsed_doc.words)
    {
        index_[word][doc_id]++;
    }

    docs_.insert({doc_id, std::move(parsed_doc.doc)});
}

void InvertedIndex::RemoveDocument(std::size_t doc_id)
{
    if (docs_.erase(doc_id) == 0)
        return;

    for (auto it = index_.begin(); it != index_.end();)
    {
        it->second.erase(doc_id);

        if (it->second.empty())
        {
            it = index_.erase(it);
        }
        else
        {
            ++it;
        }
    }
}

std::unordered_map<std::size_t, std::size_t> InvertedIndex::Search(const std::string& word) const
{
    auto it = index_.find(word);
    if (it != index_.end())
    {
        return it->second;
    }
    return {};
}

const Document* InvertedIndex::GetDocument(std::size_t doc_id) const
{
    auto it = docs_.find(doc_id);
    if (it != docs_.end())
    {
        return &it->second;
    }
    return nullptr;
}

} // namespace lab5