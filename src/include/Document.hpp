#pragma once
#include <cstddef> // ╨┤╨╗╤Å std::size_t
#include <string>

namespace lab5
{

class Document
{
    std::size_t Id = 0;
    std::string Name;
    std::string Content;

  public:
    Document(std::size_t id, std::string name, std::string content);

    Document(const Document& other) = delete;
    Document& operator=(const Document& other) = delete;

    Document(Document&& other) noexcept = default;
    Document& operator=(Document&& other) noexcept = default;
    std::size_t GetId() const;
    const std::string& GetName() const;
    const std::string& GetContent() const;
};

} // namespace lab5
