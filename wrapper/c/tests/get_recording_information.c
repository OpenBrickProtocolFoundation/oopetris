#include <criterion/criterion.h>

#include <criterion/new/assert.h>
#include <oopetris/oopetris_wrapper.h>

Test(GetRecordingInformation, NullptrAsArgument) {
    OOPetrisRecordingReturnValue* return_value = oopetris_get_recording_information(NULL);
    cr_assert(return_value != NULL, "NULL is supported and returns an error");


    const bool is_error = oopetris_is_error(return_value);

    cr_assert(is_error, "Non existent recordings file is error");

    const char* error = oopetris_get_error(return_value);

    char* dynamic_error = malloc(strlen(error) + 1);
    strcpy(dynamic_error, error);

    cr_assert(eq(str, dynamic_error, "FilePath is NULL"));
    free(dynamic_error);
}


Test(GetRecordingInformation, NonExistentFile) {
    OOPetrisRecordingReturnValue* return_value = oopetris_get_recording_information("./NON_EXISTENT_FILE");
    cr_assert(return_value != NULL, "Non existent recordings file is detected correctly");

    const bool is_error = oopetris_is_error(return_value);

    cr_assert(is_error, "Non existent recordings file is error");

    const char* error = oopetris_get_error(return_value);

    char* dynamic_error = malloc(strlen(error) + 1);
    strcpy(dynamic_error, error);

    cr_assert(eq(str, dynamic_error, "File './NON_EXISTENT_FILE' doesn't exist!"));
    free(dynamic_error);
}

Test(GetRecordingInformation, IncorrectFile) {

    OOPetrisRecordingReturnValue* return_value = oopetris_get_recording_information("./incorrect.rec");
    cr_assert(return_value != NULL, "Incorrect recordings file is detected correctly");

    const bool is_error = oopetris_is_error(return_value);

    cr_assert(is_error, "Incorrect recordings file is error");

    const char* error = oopetris_get_error(return_value);

    char* dynamic_error = malloc(strlen(error) + 1);
    strcpy(dynamic_error, error);

    cr_assert(
            eq(str, dynamic_error,
               "An error occurred during parsing of the recording file './incorrect.rec': magic file bytes are not "
               "correct, this is either an old format or no recording at all")
    );
    free(dynamic_error);
}


Test(GetRecordingInformation, CorrectFile) {

    OOPetrisRecordingReturnValue* return_value = oopetris_get_recording_information("./correct.rec");
    cr_assert(return_value != NULL, "Correct recordings file is detected correctly");

    const bool is_not_error = !oopetris_is_error(return_value);

    const char* maybe_error = is_not_error ? "<NO ERROR>" : oopetris_get_error(return_value);

    cr_assert(is_not_error, "Correct recordings file isn't an error: %s", maybe_error);
}
