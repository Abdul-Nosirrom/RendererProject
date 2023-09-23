#include "RomanceException.h"
#include <sstream>

RomanceException::RomanceException(int line, const char* file) noexcept
    : m_line(line), m_file(file) 
{}

const char* RomanceException::what() const
{
    std::ostringstream oss;
    oss << GetType() << std::endl
        << GetOriginString();
    m_whatBuffer = oss.str();
    // Have to return the char* through our buffer, if we return oss.c_str(), it's lifetime ends after this scope
    // so it'll be garbage ptr upon return
    return m_whatBuffer.c_str();
}

const char* RomanceException::GetType() const noexcept
{
    return "Romance Exception";
}

int RomanceException::GetLine() const noexcept
{
    return m_line;
}

const std::string& RomanceException::GetFile() const noexcept
{
    return m_file;
}

std::string RomanceException::GetOriginString() const noexcept
{
    std::ostringstream oss;
    oss << "[File] " << m_file << std::endl
        << "[Line] " << m_line;
    return oss.str();
}
