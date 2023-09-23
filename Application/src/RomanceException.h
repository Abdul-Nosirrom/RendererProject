#pragma once
#include <exception>
#include <string>

/// @brief  Base exception class which will output line number and file name of where exception was thrown
class RomanceException : public std::exception
{
public:
    RomanceException(int line, const char* file) noexcept;
    /// @brief  std::exception virtual override
    const char* what() const override;
    /// @brief  Custom type of exception for base classes to override and specify
    virtual const char* GetType() const noexcept;
    int GetLine() const noexcept;
    const std::string& GetFile() const noexcept;
    /// @brief  Formatted exception type with line and file, returns where this exception was thrown from
    std::string GetOriginString() const noexcept;
private:
    int m_line;
    std::string m_file;
protected:
    /// @brief  Mutable as this buffer will be filled out in 'what()' which is const
    mutable std::string m_whatBuffer;
};
