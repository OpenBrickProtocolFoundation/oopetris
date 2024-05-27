import fs from "fs"
import path from "path"

const rootDir = path.join(__dirname, "..", "..")
const oopetris = require("node-gyp-build")(rootDir)

export type AdditionalInformation = Record<string, any>

export type InputEvent =
	| "RotateLeftPressed"
	| "RotateRightPressed"
	| "MoveLeftPressed"
	| "MoveRightPressed"
	| "MoveDownPressed"
	| "DropPressed"
	| "HoldPressed"
	| "RotateLeftReleased"
	| "RotateRightReleased"
	| "MoveLeftReleased"
	| "MoveRightReleased"
	| "MoveDownReleased"
	| "DropReleased"
	| "HoldReleased"

export type TetrionRecord = {
	event: InputEvent
	simulation_step_index: number
	tetrion_index: number
}

export type MinoPosition = {
	x: number
	y: number
}

export type TetrominoType = "I" | "J" | "L" | "O" | "S" | "T" | "Z"

export type Mino = {
	position: MinoPosition
	type: TetrominoType
}

export type TetrionSnapshot = {
	level: number
	lines_cleared: number
	mino_stack: Mino[]
	score: number
	simulation_step_index: number
	tetrion_index: number
}

export type TetrionHeader = {
	seed: number
	starting_level: number
}

export type RecordingInformation = {
	information: AdditionalInformation
	records: TetrionRecord[]
	snapshots: TetrionSnapshot[]
	tetrion_headers: TetrionHeader[]
	version: number
}

export type GridProperties = {
	height: number
	width: number
}

export type RecordingsProperties = {
	gridProperties: GridProperties
}

export class RecordingsUtility {
	static isRecordingFile(file: string): boolean {
		try {
			// this throws, when file is not an string or not there at all, just be safe for JS land
			return oopetris.isRecordingFile(file)
		} catch (_err) {
			return false
		}
	}

	static getInformation(file: string): null | RecordingInformation {
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

	static get properties(): GridProperties {
		return oopetris.properties
	}

	static get version(): string {
		return oopetris.version
	}
}
