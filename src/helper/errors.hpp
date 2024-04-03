

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
        GeneralError(const std::string& message, error::Severity severity);

        GeneralError(std::string&& message, error::Severity severity);

        [[nodiscard]] const std::string& message() const;
        [[nodiscard]] error::Severity severity() const;
    };

    struct FatalError : public GeneralError {
        FatalError(const std::string& message);

        FatalError(std::string&& message);
    };

    struct MajorError : public GeneralError {
        MajorError(const std::string& message);

        MajorError(std::string&& message);
    };

    struct MinorError : public GeneralError {
        MinorError(const std::string& message);

        MinorError(std::string&& message);
    };

    using InitializationError = FatalError;

} // namespace helper
