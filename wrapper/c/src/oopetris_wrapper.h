

#pragma once

#define STBDS_NO_SHORT_NAMES
#include "./thirdparty/stb_ds.h"


#ifdef __cplusplus
extern "C" {
#else
#endif
#include <stdbool.h>
#include <stdint.h>

/**
 * @brief this determines if a file (may be nonexistent) is a recording file, it is not guaranteed, tat a consecutive call to oopetris_get_recording_information never fails, if this returns true, but it's highly unliekely, and this is faster, as it doesn't do as much work
 * 
 * @param file_path The FilePath of the potential recording file
 * @return bool
 */
bool oopetris_is_recording_file(const char* file_path);

// opaque type
typedef struct OOPetrisAdditionalInformationImpl OOPetrisAdditionalInformation;


const char** oopetris_additional_information_get_keys(OOPetrisAdditionalInformation* information);


void oopetris_additional_information_keys_free(const char*** keys);

//TODO: make functions to introspect this


typedef enum : uint8_t {
    OOPetrisInputEvent_RotateLeftPressed = 0,
    OOPetrisInputEvent_RotateRightPressed,
    OOPetrisInputEvent_MoveLeftPressed,
    OOPetrisInputEvent_MoveRightPressed,
    OOPetrisInputEvent_MoveDownPressed,
    OOPetrisInputEvent_DropPressed,
    OOPetrisInputEvent_HoldPressed,
    OOPetrisInputEvent_RotateLeftReleased,
    OOPetrisInputEvent_RotateRightReleased,
    OOPetrisInputEvent_MoveLeftReleased,
    OOPetrisInputEvent_MoveRightReleased,
    OOPetrisInputEvent_MoveDownReleased,
    OOPetrisInputEvent_DropReleased,
    OOPetrisInputEvent_HoldReleased,
} OOPetrisInputEvent;

typedef struct {
    uint64_t simulation_step_index;
    OOPetrisInputEvent event;
    uint8_t tetrion_index;
} OOPetrisTetrionRecord;

typedef struct {
    uint8_t x;
    uint8_t y;
} OOpetrisMinoPosition;

typedef enum : uint8_t {
    OOPetrisTetrominoType_I = 0,
    OOPetrisTetrominoType_J,
    OOPetrisTetrominoType_L,
    OOPetrisTetrominoType_O,
    OOPetrisTetrominoType_S,
    OOPetrisTetrominoType_T,
    OOPetrisTetrominoType_Z,
} OOPetrisTetrominoType;

typedef struct {
    OOpetrisMinoPosition position;
    OOPetrisTetrominoType type;
} OOPetrisMino;

typedef struct {
    uint32_t level;
    OOPetrisMino* mino_stack;
    uint64_t score;
    uint64_t simulation_step_index;
    uint32_t lines_cleared;
    uint8_t tetrion_index;
} OOpetrisTetrionSnapshot;

typedef struct {
    uint64_t seed;
    uint32_t starting_level;
} OOPetrisTetrionHeader;


/**
 * @brief non opaque type, fields can be safely accessed, except they are opaque structs again
 * The values that are pointers are dynamic array, as defined by stb_ds.h, see tests for usage example, an exception is the opaque type OOPetrisAdditionalInformation, you have to use some helper functions, to get access to it, it is a object / dict with dynamic types
 * 
 */
typedef struct {
    OOPetrisAdditionalInformation* information; //NOT AN ARRAY
    OOPetrisTetrionRecord* records;
    OOpetrisTetrionSnapshot* snapshots;
    OOPetrisTetrionHeader* tetrion_headers;
    uint32_t version;
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
} OOPetrisGridProperties;

OOPetrisGridProperties* oopetris_get_grid_properties();

void oopetris_free_grid_properties(OOPetrisGridProperties** properties);


#ifdef __cplusplus
}
#endif
