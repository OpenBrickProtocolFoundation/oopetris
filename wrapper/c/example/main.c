
#include <oopetris/oopetris_wrapper.h>
#include <stdio.h>
#include <stdlib.h>

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

    //TODO: do something with information

    oopetris_free_recording_information(&information);

    return EXIT_SUCCESS;
}
