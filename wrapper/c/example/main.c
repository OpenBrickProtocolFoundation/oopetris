
#include "oopetris_wrapper.h"
#include <assert.h>
#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "oopetris_wrapper.h"


// usage of a "typeof c++ operator in C, it's not the same, but it can do some stuff, first seen
// this in VO Eidp (Einführung in die Programmierung - PS 11)", _Generic makes this possible :)

void print_bool(bool val) {
    printf("%s", val ? "true" : "false");
}


void print_u8(uint8_t x) {
    printf("%" PRIu8, x);
}

void print_i8(int8_t x) {
    printf("%" PRIi8, x);
}

void print_u32(uint32_t x) {
    printf("%" PRIu32, x);
}

void print_i32(int32_t x) {
    printf("%" PRIi32, x);
}

void print_u64(uint64_t x) {
    printf("%" PRIu64, x);
}

void print_i64(int64_t x) {
    printf("%" PRIi64, x);
}

void print_float(float x) {
    printf("%f", x);
}

void print_double(double x) {
    printf("%f", x);
}

void print_string(const char* x) {
    printf("%s", x);
}


void print_field(const OOPetrisAdditionalInformationField* const field);

void print_vector(const OOPetrisAdditionalInformationField* const* const field) {

    printf("\n");
    for (size_t i = 0; i < stbds_arrlenu(field); ++i) {
        print_field(field[i]);
        printf("\n");
    }
}

// clang-format off
#define PRINT_VALUE(val) \
	_Generic((val),\
        _Bool    : print_bool,\
        uint8_t  : print_u8, \
        int8_t   : print_i8,\
        uint32_t : print_u32,\
        int32_t  : print_i32,\
        uint64_t : print_u64,\
        int64_t  : print_i64,\
        float    : print_float,\
        double   : print_double,\
        const char* : print_string,\
        const OOPetrisAdditionalInformationField* const* : print_vector)(val)
// clang-format on


void print_field(const OOPetrisAdditionalInformationField* const field) {
    switch (oopetris_additional_information_field_get_type(field)) {
        case OOPetrisAdditionalInformationType_String:
            PRINT_VALUE(oopetris_additional_information_field_get_string(field));
            return;
        case OOPetrisAdditionalInformationType_Float:
            PRINT_VALUE(oopetris_additional_information_field_get_float(field));
            return;
        case OOPetrisAdditionalInformationType_Double:
            PRINT_VALUE(oopetris_additional_information_field_get_double(field));
            return;
        case OOPetrisAdditionalInformationType_Bool:
            PRINT_VALUE(oopetris_additional_information_field_get_bool(field));
            return;
        case OOPetrisAdditionalInformationType_U8:
            PRINT_VALUE(oopetris_additional_information_field_get_u8(field));
            return;
        case OOPetrisAdditionalInformationType_I8:
            PRINT_VALUE(oopetris_additional_information_field_get_i8(field));
            return;
        case OOPetrisAdditionalInformationType_U32:
            PRINT_VALUE(oopetris_additional_information_field_get_u32(field));
            return;
        case OOPetrisAdditionalInformationType_I32:
            PRINT_VALUE(oopetris_additional_information_field_get_i32(field));
            return;
        case OOPetrisAdditionalInformationType_U64:
            PRINT_VALUE(oopetris_additional_information_field_get_u64(field));
            return;
        case OOPetrisAdditionalInformationType_I64:
            PRINT_VALUE(oopetris_additional_information_field_get_i64(field));
            return;
        case OOPetrisAdditionalInformationType_Vector:
            PRINT_VALUE(oopetris_additional_information_field_get_vector(field));
            return;

        default:
            assert(false && "UNREACHABLE");
            return;
    }
}

void print_tetrion_header(const OOPetrisTetrionHeader* const header) {

    printf("seed: %" PRIu64 "\tstarting_level: %" PRIu32 "\n", header->seed, header->starting_level);
}

const char* input_event_string(OOPetrisInputEvent event) {
    switch (event) {
        case OOPetrisInputEvent_RotateLeftPressed:
            return "RotateLeftPressed";
        case OOPetrisInputEvent_RotateRightPressed:
            return "RotateRightPressed";
        case OOPetrisInputEvent_MoveLeftPressed:
            return "MoveLeftPressed";
        case OOPetrisInputEvent_MoveRightPressed:
            return "MoveRightPressed";
        case OOPetrisInputEvent_MoveDownPressed:
            return "MoveDownPressed";
        case OOPetrisInputEvent_DropPressed:
            return "DropPressed";
        case OOPetrisInputEvent_HoldPressed:
            return "HoldPressed";
        case OOPetrisInputEvent_RotateLeftReleased:
            return "RotateLeftReleased";
        case OOPetrisInputEvent_RotateRightReleased:
            return "RotateRightReleased";
        case OOPetrisInputEvent_MoveLeftReleased:
            return "MoveLeftReleased";
        case OOPetrisInputEvent_MoveRightReleased:
            return "MoveRightReleased";
        case OOPetrisInputEvent_MoveDownReleased:
            return "MoveDownReleased";
        case OOPetrisInputEvent_DropReleased:
            return "DropReleased";
        case OOPetrisInputEvent_HoldReleased:
            return "HoldReleased";
        default:
            assert(false && "UNREACHABLE");
            return "<ERROR>";
    }
}

void print_record(const OOPetrisTetrionRecord* const record) {
    printf("simulation_step_index: %" PRIu64 "\ttetrion_index: %" PRIu8 "\tevent: %s\n", record->simulation_step_index,
           record->tetrion_index, input_event_string(record->event));
}

void print_mino_stack(const OOPetrisMino* const stack) {

    OOPetrisGridProperties* properties = oopetris_get_grid_properties();

    if (properties == NULL) {
        return;
    }

    const size_t buffer_size = properties->height * properties->width;

    char* buffer = malloc(buffer_size);

    if (buffer == NULL) {
        return;
    }

    memset(buffer, '.', buffer_size);

    for (size_t i = 0; i < stbds_arrlenu(stack); ++i) {
        const OOpetrisMinoPosition position = stack[i].position;
        buffer[position.x + (position.y * properties->width)] = '#';
    }


    for (size_t y = 0; y < properties->height; ++y) {
        int result = write(STDOUT_FILENO, buffer + (y * properties->width), properties->width);
        if (result < 0) {
            free(buffer);
            return;
        }
        printf("\n");
    }

    free(buffer);
    oopetris_free_grid_properties(&properties);
}

void print_snapshot(const OOpetrisTetrionSnapshot* const snapshot) {
    printf("\tsimulation_step_index: %" PRIu64 "\ttetrion_index: %" PRIu8 "\n", snapshot->simulation_step_index,
           snapshot->tetrion_index);

    printf("\tlevel: %" PRIu32 "\tscore: %" PRIu64 "\tlines_cleared: %" PRIu32 "\n", snapshot->level, snapshot->score,
           snapshot->lines_cleared);

    print_mino_stack(snapshot->mino_stack);
}


void print_recording_information(const OOPetrisRecordingInformation* const information) {

    printf("Version: %d\n\n", information->version);
    printf("Additional Information:\n");

    const char** keys = oopetris_additional_information_get_keys(information->information);

    for (size_t i = 0; i < stbds_arrlenu(keys); ++i) {
        printf("\t%s: ", keys[i]);
        print_field(oopetris_additional_information_get_field(information->information, keys[i]));
        printf("\n");
    }
    printf("\n");

    oopetris_additional_information_keys_free(&keys);

    printf("Headers:\n");

    for (size_t i = 0; i < stbds_arrlenu(information->tetrion_headers); ++i) {
        printf("\t");
        print_tetrion_header(&(information->tetrion_headers[i]));
    }
    printf("\n");

    printf("Records:\n");

    for (size_t i = 0; i < stbds_arrlenu(information->records); ++i) {
        printf("\t");
        print_record(&(information->records[i]));
    }
    printf("\n");

    printf("Snapshots:\n");

    for (size_t i = 0; i < stbds_arrlenu(information->snapshots); ++i) {
        print_snapshot(&(information->snapshots[i]));
    }
    printf("\n");
}

int main(int argc, char** argv) {


    if (argc != 2) {
        fprintf(stderr, "usage: %s <file>\n", argv[0]);
        return EXIT_FAILURE;
    }

    const char* file = argv[1];

    const bool is_recordings_file = oopetris_is_recording_file(file);

    if (!is_recordings_file) {
        fprintf(stderr, "Input file is no recordings file!\n");
        return EXIT_FAILURE;
    }

    OOPetrisRecordingReturnValue* return_value = oopetris_get_recording_information(file);

    const bool is_error = oopetris_is_error(return_value);

    if (is_error) {
        const char* error = oopetris_get_error(return_value);
        fprintf(stderr, "An error occured: %s\n", error);
        oopetris_free_recording_value_whole(&return_value);
        return EXIT_FAILURE;
    }

    OOPetrisRecordingInformation* information = oopetris_get_information(return_value);

    oopetris_free_recording_value_only(&return_value);

    print_recording_information(information);

    oopetris_free_recording_information(&information);

    return EXIT_SUCCESS;
}
