#include <criterion/criterion.h>

#include <oopetris/oopetris_wrapper.h>

Test(IsRecordingFile, NullptrAsArgument) {
    bool is_recordings_file = oopetris_is_recording_file(NULL);
    cr_assert(!is_recordings_file, "NULL is supported and doesn't crash");
}


Test(IsRecordingFile, NonExistentFile) {
    bool is_recordings_file = oopetris_is_recording_file("./NON_EXISTENT_FILE");
    cr_assert(!is_recordings_file, "Non existent recordings file is detected correctly");
}

Test(IsRecordingFile, IncorrectFile) {
    bool is_recordings_file = oopetris_is_recording_file("./incorrect.rec");
    cr_assert(!is_recordings_file, "Incorrect recordings file is detected correctly");
}

Test(IsRecordingFile, CorrectFile) {
    bool is_recordings_file = oopetris_is_recording_file("./correct.rec");
    cr_assert(is_recordings_file, "Correct recordings file is detected correctly");
}
