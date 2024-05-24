
#pragma once

#include "checksum_helper.hpp"
#include "helper.hpp"
#include "helper/expected.hpp"
#include "helper/optional.hpp"
#include "helper/types.hpp"

#include <fmt/format.h>
#include <fmt/ranges.h>
#include <iostream>
#include <istream>
#include <string>
#include <unordered_map>
#include <utility>
#include <variant>
#include <vector>

namespace recorder {

    struct InformationValue {
    private:
        std::variant<std::string, float, double, bool, u8, i8, u32, i32, u64, i64, std::vector<InformationValue>>
                m_value;

        enum class ValueType : u8 { String = 0, Float, Double, Bool, U8, I8, U32, I32, U64, I64, Vector };
        constexpr static u32 max_recursion_depth = 15;

    public:
        template<typename T>
        explicit InformationValue(T value) : m_value{ std::move(value) } {
            //
        }


        template<typename T>
        [[nodiscard]] bool is() const {
            const T* retrieved = std::get_if<T>(&m_value);

            return retrieved != nullptr;
        }

        template<typename T>
        [[nodiscard]] T& as() {
            if (!is<T>()) {
                throw std::runtime_error(fmt::format("Bad cast: can't cast to type '{}' !", typeid(T).name()));
            }

            return std::get<T>(m_value);
        }

        template<typename T>
        [[nodiscard]] const T& as() const {
            if (!is<T>()) {
                throw std::runtime_error(fmt::format("Bad cast: can't cast to type '{}' !", typeid(T).name()));
            }

            return std::get<T>(m_value);
        }


        [[nodiscard]] std::string to_string(u32 recursion_depth = 0) const;

        template<typename T>
        [[nodiscard]] bool operator==(const T& other) const { // NOLINT(misc-no-recursion)

            const T* retrieved = std::get_if<T>(&m_value);

            if (retrieved == nullptr) {
                return false;
            }

            return *retrieved == other;
        }

        [[nodiscard]] bool operator==(const InformationValue& other) const { // NOLINT(misc-no-recursion)
            return std::visit(
                    helper::overloaded{
                            [this](const std::string& value) { return *this == value; },
                            [this](const float& value) { return *this == value; },
                            [this](const double& value) { return *this == value; },
                            [this](const bool& value) { return *this == value; },
                            [this](const u8& value) { return *this == value; },
                            [this](const i8& value) { return *this == value; },
                            [this](const u32& value) { return *this == value; },
                            [this](const i32& value) { return *this == value; },
                            [this](const u64& value) { return *this == value; },
                            [this](const i64& value) { return *this == value; },
                            [this](const std::vector<InformationValue>& value) { // NOLINT(misc-no-recursion)
                                if (not this->is<std::vector<InformationValue>>()) {
                                    return false;
                                }

                                const auto& other = this->as<std::vector<InformationValue>>();

                                if (other.size() != value.size()) {
                                    return false;
                                }

                                for (decltype(other.size()) i = 0; i < other.size(); ++i) {
                                    if (other.at(i) != value.at(i)) {
                                        return false;
                                    }
                                }

                                return true;
                            } },
                    other.m_value
            );
        }

        static helper::expected<std::pair<std::string, InformationValue>, std::string> read_from_istream(
                std::istream& istream
        );

        [[nodiscard]] helper::expected<std::vector<char>, std::string> to_bytes(u32 recursion_depth = 0) const;

        [[nodiscard]] static std::vector<char> string_to_bytes(const std::string& value);

    private:
        static helper::expected<std::string, std::string> read_string_from_istream(std::istream& istream);

        static helper::expected<InformationValue, std::string>
        read_value_from_istream(std::istream& istream, u32 recursion_depth = 0);
    };


    struct AdditionalInformation {
    private:
        static constexpr u32 magic_start_byte = 0xABCDEF01;
        std::unordered_map<std::string, InformationValue> m_values{};

        AdditionalInformation(std::unordered_map<std::string, InformationValue>&& values);

    public:
        explicit AdditionalInformation();

        static helper::expected<AdditionalInformation, std::string> from_istream(std::istream& istream);


        void add_value(const std::string& key, const InformationValue& value, bool overwrite = false);

        template<typename T>
        void add(const std::string& key, const T& raw_value, bool overwrite = false) {
            const auto value = InformationValue{ raw_value };
            add_value(key, value, overwrite);
        }

        [[nodiscard]] helper::optional<InformationValue> get(const std::string& key) const;

        [[nodiscard]] bool has(const std::string& key) const;

        template<typename T>
        [[nodiscard]] helper::optional<T> get_if(const std::string& key) const {

            if (not has(key)) {
                return helper::nullopt;
            }

            const auto& value = m_values.at(key);

            if (not value.is<T>()) {
                return helper::nullopt;
            }

            return value.as<T>();
        }

        [[nodiscard]] helper::expected<std::vector<char>, std::string> to_bytes() const;

        [[nodiscard]] helper::expected<Sha256Stream::Checksum, std::string> get_checksum() const;
    };


} // namespace recorder
