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
const oopetris = require("bindings")("oopetris")

describe("isRecordingFile", () => {
	it("should throw an error, when no first argument was given", async () => {
		try {
			oopetris.isRecordingFile()

			fail("it should not reach here")
		} catch (e) {
			expect((e as any).toString()).toEqual(
				"TypeError: Wrong number of arguments"
			)
		}
	})

	it("should throw an error, when the first argument is not a string", async () => {
		try {
			oopetris.isRecordingFile(1)

			fail("it should not reach here")
		} catch (e) {
			expect((e as any).toString()).toEqual(
				"TypeError: First argument must be string"
			)
		}
	})

	it("should return false, when the file doesn't exist", async () => {
		const file = getFilePath("NON-EXISTENT.rec")
		expect(fs.existsSync(file)).toBe(false)

		const result = oopetris.isRecordingFile(file)
		expect(result).toBe(false)
	})
})

describe("getInformation", () => {
	it("should throw an error, when no first argument was given", async () => {
		try {
			oopetris.getInformation()

			fail("it should not reach here")
		} catch (e) {
			expect((e as any).toString()).toEqual(
				"TypeError: Wrong number of arguments"
			)
		}
	})

	it("should throw an error, when the first argument is not a string", async () => {
		try {
			oopetris.getInformation(1)

			fail("it should not reach here")
		} catch (e) {
			expect((e as any).toString()).toEqual(
				"TypeError: First argument must be string"
			)
		}
	})

	it("should throw an error, when the file doesn't exist", async () => {
		const file = getFilePath("NON-EXISTENT.rec")
		expect(fs.existsSync(file)).toBe(false)

		try {
			oopetris.getInformation(file)

			fail("it should not reach here")
		} catch (e) {
			expect((e as any).toString()).toEqual(
				`Error: File '${file}' doesn't exist!`
			)
		}
	})
})

describe("exported properties", () => {
	it("should only have known properties", async () => {
		const expectedKeys = [
			"isRecordingFile",
			"getInformation",
			"version",
			"properties",
			"path",
		]

		const keys = Object.keys(oopetris)
		expect(keys).toStrictEqual(expectedKeys)
	})

	it("should have the expected properties", async () => {
		const expectedProperties: Record<string, any> = {
			isRecordingFile: () => {},
			getInformation: () => {},
			version: "0.5.6",
			properties: { gridProperties: { height: 20, width: 10 } },
		}

		for (const key in expectedProperties) {
			const value = expectedProperties[key]
			const rawValue = oopetris[key]

			if (typeof value === "string") {
				expect(value).toBe(rawValue)
			} else if (typeof value === "function") {
				expect(rawValue).toStrictEqual(expect.any(Function))
			} else {
				expect(value).toMatchObject(rawValue)
			}
		}
	})
})
