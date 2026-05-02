#include "Document.h"

namespace lab5
{

Document::Document(std::size_t id, std::string name, std::string content)
    : Id(id), Name(std::move(name)), Content(std::move(content))
{
}

std::size_t Document::GetId() const
{
    return Id;
}

const std::string& Document::GetName() const
{
    return Name;
}

const std::string& Document::GetContent() const
{
    return Content;
}

} // namespace lab5