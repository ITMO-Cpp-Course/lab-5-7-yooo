#pragma once
#include "Document.hpp"
#include <string>
#include <vector>

namespace lab5
{

struct ParsedDocument
{
    Document doc;
    std::vector<std::string> words;
};

class DocumentBuilder
{
  public:
    ParsedDocument Build(std::size_t id, std::string name, std::string content) const;

  private:
    std::vector<std::string> ParseText(const std::string& text) const;
};

} // namespace lab5