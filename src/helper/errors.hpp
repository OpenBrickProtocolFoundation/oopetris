

#pragma once

#include <exception>
#include <string>

namespace helper {

    namespace error {
        enum class Severity { Fatal, Major, Minor };
    }

    struct GeneralError : std::exception {
    private:
        std::string m_message;
        error::Severity m_severity;

    public:
        GeneralError(const std::string& message, error::Severity severity) noexcept;
        GeneralError(std::string&& message, error::Severity severity) noexcept;
        GeneralError(const GeneralError& error) noexcept;

        [[nodiscard]] const std::string& message() const;
        [[nodiscard]] error::Severity severity() const;
    };

    struct FatalError : public GeneralError {
        FatalError(const std::string& message) noexcept;

        FatalError(std::string&& message) noexcept;
    };

    struct MajorError : public GeneralError {
        MajorError(const std::string& message) noexcept;

        MajorError(std::string&& message) noexcept;
    };

    struct MinorError : public GeneralError {
        MinorError(const std::string& message) noexcept;

        MinorError(std::string&& message) noexcept;
    };

    using InitializationError = FatalError;

} // namespace helper
