#include "errors.hpp"

helper::GeneralError::GeneralError(const std::string& message, error::Severity severity) noexcept
    : m_message{ message },
      m_severity{ severity } { }

helper::GeneralError::GeneralError(std::string&& message, error::Severity severity) noexcept
    : m_message{ std::move(message) },
      m_severity{ severity } { }

helper::GeneralError::GeneralError(const GeneralError& error) noexcept
    : m_message{ error.message() },
      m_severity{ error.severity() } { }

[[nodiscard]] const std::string& helper::GeneralError::message() const {
    return m_message;
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
