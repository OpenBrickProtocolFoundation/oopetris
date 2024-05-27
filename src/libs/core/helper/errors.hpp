

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

        ~GeneralError();

        GeneralError(const GeneralError& error) noexcept;
        [[nodiscard]] GeneralError& operator=(const GeneralError& error) noexcept;

        GeneralError(GeneralError&& error) noexcept;
        [[nodiscard]] GeneralError& operator=(GeneralError&& error) noexcept;

        [[nodiscard]] const std::string& message() const;
        [[nodiscard]] error::Severity severity() const;

        [[nodiscard]] const char* what() const noexcept override;
    };

    struct FatalError : public GeneralError {
        explicit FatalError(const std::string& message) noexcept;

        explicit FatalError(std::string&& message) noexcept;
    };

    struct MajorError : public GeneralError {
        explicit MajorError(const std::string& message) noexcept;

        explicit MajorError(std::string&& message) noexcept;
    };

    struct MinorError : public GeneralError {
        explicit MinorError(const std::string& message) noexcept;

        explicit MinorError(std::string&& message) noexcept;
    };

    using InitializationError = FatalError;

} // namespace helper
