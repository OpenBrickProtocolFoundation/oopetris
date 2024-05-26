import { RecordingsUtility } from "../src/ts/index"
import { expect } from "@jest/globals"
import path from "path"
import fs from "fs"

function fail(reason = "fail was called in a test."): never {
	throw new Error(reason)
}

global.fail = fail

function getFilePath(name: string): string {
	return path.join(__dirname, `files`, name)
}

describe("isRecordingFile: works as expected", () => {
	it("should return false for non existent file", async () => {
		const file = getFilePath("NON-EXISTENT.rec")

		expect(fs.existsSync(file)).toBe(false)

		const result = RecordingsUtility.isRecordingFile(file)
		expect(result).toBe(false)
	})

	it("should return false for incorrect file", async () => {
		const file = getFilePath("incorrect.rec")

		expect(fs.existsSync(file)).toBe(true)

		const result = RecordingsUtility.isRecordingFile(file)
		expect(result).toBe(false)
	})

	it("should return true for correct file", async () => {
		const file = getFilePath("correct.rec")

		expect(fs.existsSync(file)).toBe(true)

		const result = RecordingsUtility.isRecordingFile(file)
		expect(result).toBe(true)
	})
})

describe("getInformation: works as expected", () => {
	it("should return null for non existent file", async () => {
		const file = getFilePath("NON-EXISTENT.rec")

		expect(fs.existsSync(file)).toBe(false)

		const result = RecordingsUtility.getInformation(file)
		expect(result).toBe(null)
	})

	it("should return null for incorrect file", async () => {
		const file = getFilePath("incorrect.rec")

		expect(fs.existsSync(file)).toBe(true)

		const result = RecordingsUtility.getInformation(file)
		expect(result).toBe(null)
	})

	it("should return an object for correct file", async () => {
		const file = getFilePath("correct.rec")

		expect(fs.existsSync(file)).toBe(true)

		const result = RecordingsUtility.getInformation(file)
		expect(result).not.toBe(null)
	})

	it("should return correct object for correct file", async () => {
		const file = getFilePath("correct.rec")
		expect(fs.existsSync(file)).toBe(true)

		const serializedFile = getFilePath("correct.serialized.json")
		expect(fs.existsSync(serializedFile)).toBe(true)

		const correctResult = JSON.parse(
			fs.readFileSync(serializedFile).toString()
		)

		const result = RecordingsUtility.getInformation(file)
		expect(result).toMatchObject(correctResult)
	})
})
