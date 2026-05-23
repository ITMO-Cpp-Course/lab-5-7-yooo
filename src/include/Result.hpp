#pragma once
#include <expected>
#include <string>

namespace lab5
{

enum class ErrorCode
{
    DocumentNotFound,
    DocumentAlreadyExists,
    TransactionFailed,
    InvalidArgument
};

struct Error
{
    ErrorCode code;
    std::string message;
};

template <typename T> using Result = std::expected<T, Error>;

} // namespace lab5