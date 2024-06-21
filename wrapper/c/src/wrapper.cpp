

#include "oopetris_wrapper.h"


#if defined(__GNUC__)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wmissing-field-initializers"
#endif

#define STB_DS_IMPLEMENTATION
#define STBDS_NO_SHORT_NAMES
#define STBDS_SIPHASH_2_4
#include "./thirdparty/stb_ds.h"


#if defined(__GNUC__)
#pragma GCC diagnostic pop
#endif


#include <core/core.hpp>
#include <cstring>
#include <ctime>
#include <recordings/recordings.hpp>


bool oopetris_is_recording_file(const char* file_path) {

    if (file_path == nullptr) {
        return false;
    }

    if (not std::filesystem::exists(file_path)) {
        return false;
    }

    auto parsed = recorder::RecordingReader::from_path(file_path);

    return parsed.has_value();
}


struct OOPetrisRecordingReturnValueImpl {
    bool is_error;
    union {
        OOPetrisRecordingInformation* information;
        char* error;
    } value;
};


static OOPetrisRecordingReturnValue*
construct_error_from_cstr_impl(OOPetrisRecordingReturnValue* return_value, const char* value, size_t length) {

    auto* alloced_str = static_cast<char*>(malloc(length + 1));

    if (alloced_str == nullptr) {
        free(return_value);
        return nullptr;
    }

    strcpy(alloced_str, value);

    return_value->is_error = true;
    return_value->value.error = alloced_str;

    return return_value;
}

static OOPetrisRecordingReturnValue*
construct_error_from_cstr(OOPetrisRecordingReturnValue* return_value, const char* value) {
    return construct_error_from_cstr_impl(return_value, value, strlen(value));
}

static OOPetrisRecordingReturnValue*
construct_error_from_string(OOPetrisRecordingReturnValue* return_value, const std::string& value) {
    return construct_error_from_cstr_impl(return_value, value.c_str(), value.size());
}


struct OOPetrisAdditionalInformationFieldImpl {
    OOPetrisAdditionalInformationType type;
    union {
        char* string;
        float float_v;
        double double_v;
        bool bool_v;
        uint8_t u8;
        int8_t i8;
        uint32_t u32;
        int32_t i32;
        uint64_t u64;
        int64_t i64;
        OOPetrisAdditionalInformationField** vector;
    } value;
};

struct OOPetrisAdditionalInformationImpl {
    char* key;
    OOPetrisAdditionalInformationField* value;
};

static void vector_of_value_free(OOPetrisAdditionalInformationField*** vector, size_t until_index);

static OOPetrisAdditionalInformationField* information_value_to_c(const recorder::InformationValue& value) {


    auto* return_value =
            static_cast<OOPetrisAdditionalInformationField*>(malloc(sizeof(OOPetrisAdditionalInformationField)));


    if (return_value == nullptr) {
        free(return_value);
        return nullptr;
    }

    return std::visit(
            helper::overloaded{ [return_value](const std::string& value) -> OOPetrisAdditionalInformationField* {
                                   auto* string = static_cast<char*>(malloc(value.size() + 1));

                                   if (string == nullptr) {
                                       free(string);
                                       free(return_value);
                                       return nullptr;
                                   }

                                   strcpy(string, value.c_str());

                                   return_value->type = OOPetrisAdditionalInformationType_String;
                                   return_value->value.string = string;
                                   return return_value;
                               },
                                [return_value](const float& value) -> OOPetrisAdditionalInformationField* {
                                    return_value->type = OOPetrisAdditionalInformationType_Float;
                                    return_value->value.float_v = value;
                                    return return_value;
                                },
                                [return_value](const double& value) -> OOPetrisAdditionalInformationField* {
                                    return_value->type = OOPetrisAdditionalInformationType_Double;
                                    return_value->value.double_v = value;
                                    return return_value;
                                },
                                [return_value](const bool& value) -> OOPetrisAdditionalInformationField* {
                                    return_value->type = OOPetrisAdditionalInformationType_Bool;
                                    return_value->value.bool_v = value;
                                    return return_value;
                                },
                                [return_value](const u8& value) {
                                    return_value->type = OOPetrisAdditionalInformationType_U8;
                                    return_value->value.u8 = value;
                                    return return_value;
                                },
                                [return_value](const i8& value) -> OOPetrisAdditionalInformationField* {
                                    return_value->type = OOPetrisAdditionalInformationType_I8;
                                    return_value->value.i8 = value;
                                    return return_value;
                                },
                                [return_value](const u32& value) {
                                    return_value->type = OOPetrisAdditionalInformationType_U32;
                                    return_value->value.u32 = value;
                                    return return_value;
                                },
                                [return_value](const i32& value) -> OOPetrisAdditionalInformationField* {
                                    return_value->type = OOPetrisAdditionalInformationType_I32;
                                    return_value->value.i32 = value;
                                    return return_value;
                                },
                                [return_value](const u64& value) {
                                    return_value->type = OOPetrisAdditionalInformationType_U64;
                                    return_value->value.u64 = value;
                                    return return_value;
                                },
                                [return_value](const i64& value) -> OOPetrisAdditionalInformationField* {
                                    return_value->type = OOPetrisAdditionalInformationType_Float;
                                    return_value->value.i64 = value;
                                    return return_value;
                                },
                                [return_value](const std::vector<recorder::InformationValue>& value
                                ) -> OOPetrisAdditionalInformationField* {
                                    OOPetrisAdditionalInformationField** fields = NULL;


                                    stbds_arrsetlen(fields, value.size());

                                    for (std::size_t i = 0; i < value.size(); ++i) {
                                        auto* result = information_value_to_c(value.at(i));
                                        if (result == nullptr) {
                                            vector_of_value_free(&fields, i);
                                            free(return_value);
                                            return nullptr;
                                        }
                                        fields[i] = result;
                                    }

                                    return_value->type = OOPetrisAdditionalInformationType_Vector;
                                    return_value->value.vector = fields;

                                    return return_value;
                                } },
            value.underlying()
    );
}

static void free_additional_value_field(OOPetrisAdditionalInformationField** field) {

    auto* orig_field = *field;

    if (orig_field->type == OOPetrisAdditionalInformationType_String) {
        free(orig_field->value.string);
    } else if (orig_field->type == OOPetrisAdditionalInformationType_Vector) {
        auto* vector = orig_field->value.vector;
        vector_of_value_free(&vector, stbds_arrlenu(vector));
    }


    free(*field);
    *field = NULL;
}


static void vector_of_value_free(OOPetrisAdditionalInformationField*** vector, size_t until_index) {


    for (size_t i = 0; i < until_index; ++i) {
        free_additional_value_field(&(*vector)[i]);
    }


    stbds_arrfree(*vector);
    *vector = NULL;
}


static OOPetrisAdditionalInformation* recording_reader_get_additional_information(
        const recorder::AdditionalInformation& information
) {

    stbds_rand_seed(time(NULL));

    OOPetrisAdditionalInformation* result = NULL;

    //NOTE: We use stbds_sh_new_strdup, so keys are automatically copied (malloced) and freed and we don't need to manage those
    stbds_sh_new_strdup(result);


    for (const auto& [key, value] : information) {

        auto* insert_value = information_value_to_c(value);

        stbds_shput(result, key.c_str(), insert_value);
    }

    return result;
}

const char** oopetris_additional_information_get_keys(OOPetrisAdditionalInformation* information) {

    const char** result_arr = NULL;

    const auto length = stbds_shlen(information);

    stbds_arrsetlen(result_arr, length);

    for (int i = 0; i < length; ++i) {

        result_arr[i] = information[i].key;
    }


    return result_arr;
}

void oopetris_additional_information_keys_free(const char*** keys) {
    stbds_arrfree(*keys);
    *keys = NULL;
}

const OOPetrisAdditionalInformationField*
oopetris_additional_information_get_field(OOPetrisAdditionalInformation* information, const char* key) {

    const auto index = stbds_shgeti(information, key);

    if (index < 0) {
        return nullptr;
    }

    return information[index].value;
}


OOPetrisAdditionalInformationType oopetris_additional_information_field_get_type(
        const OOPetrisAdditionalInformationField* const field
) {
    return field->type;
}

const char* oopetris_additional_information_field_get_string(const OOPetrisAdditionalInformationField* const field) {
    assert(oopetris_additional_information_field_get_type(field) == OOPetrisAdditionalInformationType_String);
    return field->value.string;
}

float oopetris_additional_information_field_get_float(const OOPetrisAdditionalInformationField* const field) {
    assert(oopetris_additional_information_field_get_type(field) == OOPetrisAdditionalInformationType_Float);
    return field->value.float_v;
}

double oopetris_additional_information_field_get_double(const OOPetrisAdditionalInformationField* const field) {
    assert(oopetris_additional_information_field_get_type(field) == OOPetrisAdditionalInformationType_Double);
    return field->value.double_v;
}


bool oopetris_additional_information_field_get_bool(const OOPetrisAdditionalInformationField* const field) {
    assert(oopetris_additional_information_field_get_type(field) == OOPetrisAdditionalInformationType_Bool);
    return field->value.bool_v;
}


uint8_t oopetris_additional_information_field_get_u8(const OOPetrisAdditionalInformationField* const field) {
    assert(oopetris_additional_information_field_get_type(field) == OOPetrisAdditionalInformationType_U8);
    return field->value.u8;
}


int8_t oopetris_additional_information_field_get_i8(const OOPetrisAdditionalInformationField* const field) {
    assert(oopetris_additional_information_field_get_type(field) == OOPetrisAdditionalInformationType_I8);
    return field->value.i8;
}


uint32_t oopetris_additional_information_field_get_u32(const OOPetrisAdditionalInformationField* const field) {
    assert(oopetris_additional_information_field_get_type(field) == OOPetrisAdditionalInformationType_U32);
    return field->value.u32;
}


int32_t oopetris_additional_information_field_get_i32(const OOPetrisAdditionalInformationField* const field) {
    assert(oopetris_additional_information_field_get_type(field) == OOPetrisAdditionalInformationType_I32);
    return field->value.i32;
}


uint64_t oopetris_additional_information_field_get_u64(const OOPetrisAdditionalInformationField* const field) {
    assert(oopetris_additional_information_field_get_type(field) == OOPetrisAdditionalInformationType_U64);
    return field->value.u64;
}


int64_t oopetris_additional_information_field_get_i64(const OOPetrisAdditionalInformationField* const field) {
    assert(oopetris_additional_information_field_get_type(field) == OOPetrisAdditionalInformationType_I64);
    return field->value.i64;
}


const OOPetrisAdditionalInformationField* const* oopetris_additional_information_field_get_vector(
        const OOPetrisAdditionalInformationField* const field
) {
    assert(oopetris_additional_information_field_get_type(field) == OOPetrisAdditionalInformationType_Vector);
    return field->value.vector;
}


static void oopetris_additional_information_free(OOPetrisAdditionalInformation** information) {


    for (int i = 0; i < stbds_shlen(*information); ++i) {

        free_additional_value_field(&((*information)[i].value));
    }

    stbds_shfree(*information);
    *information = NULL;
}

static OOPetrisTetrionRecord record_to_c(const recorder::Record& record) {
    return OOPetrisTetrionRecord{ .simulation_step_index = record.simulation_step_index,
                                  .event = static_cast<OOPetrisInputEvent>(record.event),
                                  .tetrion_index = record.tetrion_index };
}

static OOPetrisMino mino_to_c(const Mino& mino) {
    auto orig_pos = mino.position();

    auto position = OOpetrisMinoPosition{ .x = orig_pos.x, .y = orig_pos.y };

    return OOPetrisMino{ .position = position, .type = static_cast<OOPetrisTetrominoType>(mino.type()) };
}

static OOpetrisTetrionSnapshot snapshot_to_c(const TetrionSnapshot& snapshot) {


    // convert mino_stack

    auto minos = snapshot.mino_stack().minos();

    OOPetrisMino* mino_stack = NULL;
    stbds_arrsetlen(mino_stack, minos.size());

    for (std::size_t i = 0; i < minos.size(); ++i) {
        mino_stack[i] = mino_to_c(minos.at(i));
    }


    return OOpetrisTetrionSnapshot{ .level = snapshot.level(),
                                    .mino_stack = mino_stack,
                                    .score = snapshot.score(),
                                    .simulation_step_index = snapshot.simulation_step_index(),
                                    .lines_cleared = snapshot.lines_cleared(),
                                    .tetrion_index = snapshot.tetrion_index() };
}

static OOPetrisTetrionHeader tetrion_header_to_c(const recorder::TetrionHeader& header) {


    return OOPetrisTetrionHeader{ .seed = header.seed, .starting_level = header.starting_level };
}


static OOPetrisRecordingInformation* recording_reader_to_c(const recorder::RecordingReader& value) {

    auto* return_value = static_cast<OOPetrisRecordingInformation*>(malloc(sizeof(OOPetrisRecordingInformation)));


    if (return_value == nullptr) {
        free(return_value);
        return nullptr;
    }

    return_value->version = recorder::Recording::current_supported_version_number;

    return_value->information = recording_reader_get_additional_information(value.information());
    if (return_value->information == nullptr) {
        free(return_value);
        return nullptr;
    }

    // convert records

    auto records = value.records();

    return_value->records = NULL;
    stbds_arrsetlen(return_value->records, records.size());

    for (std::size_t i = 0; i < records.size(); ++i) {
        return_value->records[i] = record_to_c(records.at(i));
    }

    // convert snapshots

    auto snapshots = value.snapshots();

    return_value->snapshots = NULL;
    stbds_arrsetlen(return_value->snapshots, snapshots.size());

    for (std::size_t i = 0; i < snapshots.size(); ++i) {
        return_value->snapshots[i] = snapshot_to_c(snapshots.at(i));
    }

    // convert tetrion_headers

    auto tetrion_headers = value.tetrion_headers();

    return_value->tetrion_headers = NULL;
    stbds_arrsetlen(return_value->tetrion_headers, tetrion_headers.size());

    for (std::size_t i = 0; i < tetrion_headers.size(); ++i) {
        return_value->tetrion_headers[i] = tetrion_header_to_c(tetrion_headers.at(i));
    }

    return return_value;
}


OOPetrisRecordingReturnValue* oopetris_get_recording_information(const char* file_path) {

    auto* return_value = static_cast<OOPetrisRecordingReturnValue*>(malloc(sizeof(OOPetrisRecordingReturnValue)));

    if (return_value == nullptr) {
        free(return_value);
        return nullptr;
    }

    if (file_path == nullptr) {
        return construct_error_from_cstr(return_value, "FilePath is NULL");
    }


    if (not std::filesystem::exists(file_path)) {
        std::string error = "File '";
        error += file_path;
        error += "' doesn't exist!";

        return construct_error_from_string(return_value, error);
    }

    auto parsed = recorder::RecordingReader::from_path(file_path);

    if (not parsed.has_value()) {
        std::string error = "An error occurred during parsing of the recording file '";
        error += file_path;
        error += "': ";
        error += parsed.error();

        return construct_error_from_string(return_value, error);
    }

    const auto recording_reader = std::move(parsed.value());

    OOPetrisRecordingInformation* information = recording_reader_to_c(recording_reader);

    if (information == nullptr) {
        return construct_error_from_cstr(return_value, "Failure in internal conversion");
    }

    return_value->is_error = false;
    return_value->value.information = information;
    return return_value;
}

bool oopetris_is_error(OOPetrisRecordingReturnValue* value) {
    return value->is_error;
}

const char* oopetris_get_error(OOPetrisRecordingReturnValue* value) {
    return value->value.error;
}

OOPetrisRecordingInformation* oopetris_get_information(OOPetrisRecordingReturnValue* value) {
    return value->value.information;
}

void oopetris_free_recording_information(OOPetrisRecordingInformation** information) {

    oopetris_additional_information_free(&((*information)->information));

    stbds_arrfree((*information)->records);


    for (std::size_t i = 0; i < stbds_arrlenu((*information)->snapshots); ++i) {
        const auto* mino_stack = (*information)->snapshots[i].mino_stack;
        stbds_arrfree(mino_stack);
    }

    stbds_arrfree((*information)->snapshots);

    stbds_arrfree((*information)->tetrion_headers);

    free(*information);
    *information = nullptr;
}

void oopetris_free_recording_value_only(OOPetrisRecordingReturnValue** information) {
    free(*information);
    *information = nullptr;
}

void oopetris_free_recording_value_whole(OOPetrisRecordingReturnValue** information) {
    if (oopetris_is_error(*information)) {
        free((*information)->value.error);
    } else {
        oopetris_free_recording_information(&((*information)->value.information));
    }

    oopetris_free_recording_value_only(information);
}

const char* oopetris_get_lib_version(void) {
    return utils::version();
}

OOPetrisGridProperties* oopetris_get_grid_properties(void) {
    auto* properties = static_cast<OOPetrisGridProperties*>(malloc(sizeof(OOPetrisGridProperties)));
    if (properties == nullptr) {
        return nullptr;
    }

    properties->height = grid::height_in_tiles;
    properties->width = grid::width_in_tiles;

    return properties;
}


void oopetris_free_grid_properties(OOPetrisGridProperties** properties) {
    free(*properties);
    *properties = nullptr;
}
