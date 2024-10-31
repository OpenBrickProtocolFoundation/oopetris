

#pragma once

#include "./types.hpp"
#include "./windows.hpp"

#include <core/helper/types.hpp>

#include <exception>
#include <string>

namespace helper {

    namespace error {
        enum class Severity : u8 { Fatal, Major, Minor };
    }

    struct GeneralError : std::exception {
    private:
        std::string m_message;
        error::Severity m_severity;

    public:
        OOPETRIS_CORE_EXPORTED GeneralError(const std::string& message, error::Severity severity) noexcept;

        OOPETRIS_CORE_EXPORTED GeneralError(std::string&& message, error::Severity severity) noexcept;

        OOPETRIS_CORE_EXPORTED ~GeneralError();

        OOPETRIS_CORE_EXPORTED GeneralError(const GeneralError& error) noexcept;
        [[nodiscard]] GeneralError& operator=(const GeneralError& error) noexcept;

        OOPETRIS_CORE_EXPORTED GeneralError(GeneralError&& error) noexcept;
        OOPETRIS_CORE_EXPORTED [[nodiscard]] GeneralError& operator=(GeneralError&& error) noexcept;

        OOPETRIS_CORE_EXPORTED [[nodiscard]] const std::string& message() const;
        OOPETRIS_CORE_EXPORTED [[nodiscard]] error::Severity severity() const;

        OOPETRIS_CORE_EXPORTED [[nodiscard]] const char* what() const noexcept override;
    };

    struct FatalError : public GeneralError {
        OOPETRIS_CORE_EXPORTED explicit FatalError(const std::string& message) noexcept;

        OOPETRIS_CORE_EXPORTED explicit FatalError(std::string&& message) noexcept;
    };

    struct MajorError : public GeneralError {
        OOPETRIS_CORE_EXPORTED explicit MajorError(const std::string& message) noexcept;

        OOPETRIS_CORE_EXPORTED explicit MajorError(std::string&& message) noexcept;
    };

    struct MinorError : public GeneralError {
        OOPETRIS_CORE_EXPORTED explicit MinorError(const std::string& message) noexcept;

        OOPETRIS_CORE_EXPORTED explicit MinorError(std::string&& message) noexcept;
    };

    using InitializationError = FatalError;

} // namespace helper
