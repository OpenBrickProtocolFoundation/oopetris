import type { JestConfigWithTsJest } from "ts-jest"

const jestConfig: JestConfigWithTsJest = {
    preset: "ts-jest",
    testEnvironment: "node",
    testTimeout: 5000,
}

export default jestConfig
