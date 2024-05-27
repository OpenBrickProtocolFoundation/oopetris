#include "./errors.hpp"

helper::GeneralError::GeneralError(const std::string& message, error::Severity severity) noexcept
    : m_message{ message },
      m_severity{ severity } { }

helper::GeneralError::GeneralError(std::string&& message, error::Severity severity) noexcept
    : m_message{ std::move(message) },
      m_severity{ severity } { }

helper::GeneralError::~GeneralError() = default;

helper::GeneralError::GeneralError(const GeneralError& error) noexcept = default;
[[nodiscard]] helper::GeneralError& helper::GeneralError::operator=(const GeneralError& error) noexcept = default;

helper::GeneralError::GeneralError(GeneralError&& error) noexcept = default;
[[nodiscard]] helper::GeneralError& helper::GeneralError::operator=(GeneralError&& error) noexcept = default;


[[nodiscard]] const std::string& helper::GeneralError::message() const {
    return m_message;
}

[[nodiscard]] const char* helper::GeneralError::what() const noexcept {
    return m_message.c_str();
}


[[nodiscard]] helper::error::Severity helper::GeneralError::severity() const {
    return m_severity;
}

helper::FatalError::FatalError(const std::string& message) noexcept
    : GeneralError{ message, error::Severity::Fatal } { }

helper::FatalError::FatalError(std::string&& message) noexcept
    : GeneralError{ std::move(message), error::Severity::Fatal } { }

helper::MajorError::MajorError(const std::string& message) noexcept
    : GeneralError{ message, error::Severity::Major } { }

helper::MajorError::MajorError(std::string&& message) noexcept
    : GeneralError{ std::move(message), error::Severity::Major } { }

helper::MinorError::MinorError(const std::string& message) noexcept
    : GeneralError{ message, error::Severity::Minor } { }

helper::MinorError::MinorError(std::string&& message) noexcept
    : GeneralError{ std::move(message), error::Severity::Minor } { }
