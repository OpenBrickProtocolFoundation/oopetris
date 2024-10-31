

#pragma once

#include <core/game/mino_stack.hpp>
#include <core/helper/magic_enum_wrapper.hpp>
#include <core/helper/parse_json.hpp>

#include "./additional_information.hpp"
#include "./recording.hpp"
#include "./recording_reader.hpp"
#include "./tetrion_snapshot.hpp"

namespace nlohmann {
    template<>
    struct adl_serializer<recorder::InformationValue> {
        static recorder::InformationValue from_json(const json& /* obj */) {
            //TODO(Totto): Implement
            throw std::runtime_error{ "NOT IMPLEMENTED" };
        }

        static void to_json(json& obj, const recorder::InformationValue& information) { // NOLINT(misc-no-recursion)
            std::visit(
                    helper::Overloaded{
                            [&obj](const std::string& value) { obj = value; },
                            [&obj](const float& value) { obj = value; }, [&obj](const double& value) { obj = value; },
                            [&obj](const bool& value) { obj = value; }, [&obj](const u8& value) { obj = value; },
                            [&obj](const i8& value) { obj = value; }, [&obj](const u32& value) { obj = value; },
                            [&obj](const i32& value) { obj = value; }, [&obj](const u64& value) { obj = value; },
                            [&obj](const i64& value) { obj = value; },
                            [&obj](const std::vector<recorder::InformationValue>& value) { // NOLINT(misc-no-recursion)
                                obj = value;
                            } },
                    information.underlying()
            );
        }
    };

    template<>
    struct adl_serializer<recorder::AdditionalInformation> {
        static recorder::AdditionalInformation from_json(const json& /* obj */) {
            //TODO(Totto): Implement
            throw std::runtime_error{ "NOT IMPLEMENTED" };
        }

        static void to_json(json& obj, const recorder::AdditionalInformation& information) {

            obj = nlohmann::json::object();

            for (const auto& [key, value] : information) {

                json value_json;
                nlohmann::adl_serializer<recorder::InformationValue>::to_json(value_json, value);
                obj[key] = value_json;
            }
        }
    };

    template<>
    struct adl_serializer<recorder::TetrionHeader> {
        static recorder::TetrionHeader from_json(const json& /* obj */) {
            //TODO(Totto): Implement
            throw std::runtime_error{ "NOT IMPLEMENTED" };
        }

        static void to_json(json& obj, const recorder::TetrionHeader& tetrion_header) {
            obj = nlohmann::json::object({
                    {           "seed",           tetrion_header.seed },
                    { "starting_level", tetrion_header.starting_level }
            });
        }
    };

    template<>
    struct adl_serializer<InputEvent> {
        static InputEvent from_json(const json& /* obj */) {
            //TODO(Totto): Implement
            throw std::runtime_error{ "NOT IMPLEMENTED" };
        }

        static void to_json(json& obj, const InputEvent& event) {

            obj = magic_enum::enum_name<InputEvent>(event);
        }
    };

    template<>
    struct adl_serializer<recorder::Record> {
        static recorder::Record from_json(const json& /* obj */) {
            //TODO(Totto): Implement
            throw std::runtime_error{ "NOT IMPLEMENTED" };
        }

        static void to_json(json& obj, const recorder::Record& record) {

            obj = nlohmann::json::object({
                    {         "tetrion_index",         record.tetrion_index },
                    { "simulation_step_index", record.simulation_step_index },
                    {                 "event",                 record.event }
            });
        }
    };


    template<typename T>
    struct adl_serializer<shapes::AbstractPoint<T>> {
        static shapes::AbstractPoint<T> from_json(const json& /* obj */) {
            //TODO(Totto): Implement
            throw std::runtime_error{ "NOT IMPLEMENTED" };
        }

        static void to_json(json& obj, const shapes::AbstractPoint<T>& point) {
            obj = nlohmann::json::object({
                    { "x", point.x },
                    { "y", point.y }
            });
        }
    };


    template<>
    struct adl_serializer<helper::TetrominoType> {
        static helper::TetrominoType from_json(const json& /* obj */) {
            //TODO(Totto): Implement
            throw std::runtime_error{ "NOT IMPLEMENTED" };
        }

        static void to_json(json& obj, const helper::TetrominoType& type) {
            obj = magic_enum::enum_name<helper::TetrominoType>(type);
        }
    };


    template<>
    struct adl_serializer<Mino> {
        static Mino from_json(const json& /* obj */) {
            //TODO(Totto): Implement
            throw std::runtime_error{ "NOT IMPLEMENTED" };
        }

        static void to_json(json& obj, const Mino& mino) {
            obj = nlohmann::json::object({
                    { "position", mino.position() },
                    {     "type",     mino.type() }
            });
        }
    };


    template<>
    struct adl_serializer<TetrionSnapshot> {
        static TetrionSnapshot from_json(const json& /* obj */) {
            //TODO(Totto): Implement
            throw std::runtime_error{ "NOT IMPLEMENTED" };
        }

        static void to_json(json& obj, const TetrionSnapshot& snapshot) {

            json mino_stack_json;
            nlohmann::adl_serializer<std::vector<Mino>>::to_json(mino_stack_json, snapshot.mino_stack().minos());


            obj = nlohmann::json::object({
                    {         "tetrion_index",         snapshot.tetrion_index() },
                    {                 "level",                 snapshot.level() },
                    {                 "score",                 snapshot.score() },
                    {         "lines_cleared",         snapshot.lines_cleared() },
                    { "simulation_step_index", snapshot.simulation_step_index() },
                    {            "mino_stack",                  mino_stack_json }
            });
        }
    };


    template<>
    struct adl_serializer<recorder::RecordingReader> {
        static recorder::RecordingReader from_json(const json& /* obj */) {
            //TODO(Totto): Implement
            throw std::runtime_error{ "NOT IMPLEMENTED" };
        }

        static void to_json(json& obj, const recorder::RecordingReader& recording_reader) {

            json information_json;
            nlohmann::adl_serializer<recorder::AdditionalInformation>::to_json(
                    information_json, recording_reader.information()
            );

            json tetrion_headers_json;
            nlohmann::adl_serializer<std::vector<recorder::TetrionHeader>>::to_json(
                    tetrion_headers_json, recording_reader.tetrion_headers()
            );

            json records_json;
            nlohmann::adl_serializer<std::vector<recorder::Record>>::to_json(records_json, recording_reader.records());

            json snapshots_json;
            nlohmann::adl_serializer<std::vector<std::vector<TetrionSnapshot>>>::to_json(
                    snapshots_json, recording_reader.snapshots()
            );

            obj = nlohmann::json::object({
                    {         "version", recorder::Recording::current_supported_version_number },
                    {     "information",                                      information_json },
                    { "tetrion_headers",                                  tetrion_headers_json },
                    {         "records",                                          records_json },
                    {       "snapshots",                                        snapshots_json },
            });
        }
    };
} // namespace nlohmann
