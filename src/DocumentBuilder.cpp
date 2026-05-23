#include "DocumentBuilder.hpp"
#include <cctype>

namespace lab5
{

ParsedDocument DocumentBuilder::Build(std::size_t id, std::string name, std::string content) const
{
    std::vector<std::string> words = ParseText(content);

    return {Document(id, std::move(name), std::move(content)), std::move(words)};
}

std::vector<std::string> DocumentBuilder::ParseText(const std::string& text) const
{
    std::vector<std::string> words;
    std::string current_word;

    for (char c : text)
    {
        if (std::isalpha(static_cast<unsigned char>(c)))
        {
            current_word += std::tolower(static_cast<unsigned char>(c));
        }
        else if (!current_word.empty())
        {
            words.push_back(current_word);
            current_word.clear();
        }
    }
    if (!current_word.empty())
    {
        words.push_back(current_word);
    }
    return words;
}

} // namespace lab5