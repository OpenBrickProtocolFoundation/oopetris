

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
    bool i;
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


export type AdditionalInformation = Record<string, any>

        export type InputEvent =
                | "RotateLeftPressed" | "RotateRightPressed" | "MoveLeftPressed" | "MoveRightPressed"
                | "MoveDownPressed" | "DropPressed" | "HoldPressed" | "RotateLeftReleased" | "RotateRightReleased"
                | "MoveLeftReleased" | "MoveRightReleased" | "MoveDownReleased" | "DropReleased"
                | "HoldReleased"

                export type TetrionRecord = { event: InputEvent simulation_step_index: number tetrion_index: number }

export type MinoPosition = { x: number y: number }

export type TetrominoType = "I" | "J" | "L" | "O" | "S" | "T"
                            | "Z"

                            export type Mino = { position: MinoPosition type: TetrominoType }

export type TetrionSnapshot = {
    level: number lines_cleared: number mino_stack: Mino[] score: number simulation_step_index: number
    tetrion_index: number
}

export type TetrionHeader = { seed: number starting_level: number }

export type RecordingInformation = {
    information: AdditionalInformation records: TetrionRecord[] snapshots: TetrionSnapshot[]
    tetrion_headers: TetrionHeader[] version: number
}

export type GridProperties = { height: number width: number }

export type RecordingsProperties = { gridProperties: GridProperties }

export class RecordingsUtility {
    static isRecordingFile(file : string) : boolean {
        try {
            // this throws, when file is not an string or not there at all, just be safe for JS land
            return oopetris.isRecordingFile(file)
        } catch (_err) {
            return false
        }
    }

    static getInformation(file : string) : null | RecordingInformation {
        if (!fs.existsSync(file)) {
            return null
        }

        try {
            if (!RecordingsUtility.isRecordingFile(file)) {
                return null
            }

            // this throws, when file is not an string, not there at all, or some other error regarding serialization from c++ land to JS land occurs,  just be safe for JS land
            return oopetris.getInformation(file)
        } catch (_err) {
            return null
        }
    }

    static get properties() : GridProperties {
        return oopetris.properties
    }

    static get version() : string {
        return oopetris.version
    }
}
