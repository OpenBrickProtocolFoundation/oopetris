

#pragma once


#ifdef __cplusplus
extern "C" {
#else
#endif
#include <stdbool.h>
#include <stdint.h>

/**
 * @brief this determines if a file (may be nonexistent) is a recording file, it is not guaranteed, tat a consecutive call to oopetris_get_recording_information never fails, if rthis returns true, but it's highly unliekely, and this is faster, as it doesn't do as much work
 * 
 * @param file_path The FilePath of the potential recording file
 * @return bool
 */
bool oopetris_is_recording_file(const char* file_path);

// non opaque type, fields can be safely accessed, excpet the are opaque structs again
typedef struct {
    bool i; //TODO
    int z;
} OOPetrisRecordingInformation;


// opaque type
typedef struct OOPetrisRecordingReturnValueImpl OOPetrisRecordingReturnValue;


/**
 * @brief Retrieve the Recording information of a file, returns an opaque struct pointer, that may be null and you have to use oopetris_is_error and similar functions to retrieve the internal value
 * 
 * @param file_path The FilePath of the recording file
 * @return OOPetrisRecordingReturnValue* 
 */
OOPetrisRecordingReturnValue* oopetris_get_recording_information(const char* file_path);

/**
 * @brief Determine if the given OOPetrisRecordingReturnValue* is an error or not, after calling this you can call either oopetris_get_error or oopetris_get_information. This has to be freed later, but pay attention, which free function you use, read more in the description of oopetris_free_recording_value
 * 
 * @param value The return value of oopetris_get_recording_information 
 * @return bool 
 */
bool oopetris_is_error(OOPetrisRecordingReturnValue* value);

const char* oopetris_get_error(OOPetrisRecordingReturnValue* value);

OOPetrisRecordingInformation* oopetris_get_information(OOPetrisRecordingReturnValue* value);

void oopetris_free_recording_information(OOPetrisRecordingInformation** information);

void oopetris_free_recording_value_only(OOPetrisRecordingReturnValue** information);

void oopetris_free_recording_value_whole(OOPetrisRecordingReturnValue** information);

const char* oopetris_get_lib_version();

typedef struct {
    uint32_t height;
    uint32_t width;
} GridProperties;

GridProperties* oopetris_get_grid_properties();

void oopetris_free_grid_properties(GridProperties** properties);


#ifdef __cplusplus
}
#endif
