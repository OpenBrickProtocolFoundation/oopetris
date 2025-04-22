const core = require('@actions/core')
const exec = require('@actions/exec')
const io = require('@actions/io')

/**
 * @callback ExecListenerCallback
 * @param {Buffer} data
 * @returns {void}
 */

/**
 * @typedef ExecListeners
 * @type {object}
 * @property {ExecListenerCallback} stdout
 */

/**
 * @typedef ExecOptions
 * @type {object}
 * @property {ExecListeners} listeners
 * @property {boolean} failOnStdErr
 * @property {boolean} ignoreReturnCode
 */

/**
 * @typedef AnnotationProperties
 * @type {object}
 * @property {string} title
 * @property {string} file
/**
 *
 * @param {string} executable
 * @param {string[]} arguments
 * @async
 * @returns {Promise<string[]>}
 */
async function execAndGetStdout(executable, args) {
	/** @type {string} */
	let output = ''

	/** @type {ExecOptions} */
	const options = {
		failOnStdErr: true,
		ignoreReturnCode: false,
	}

	options.listeners = {
		stdout: (data) => {
			output += data.toString()
		},
	}

	await io.which(executable, true)

	/** @type {number} */
	const exitCode = await exec.exec(executable, args, options)

	if (exitCode != 0) {
		throw new Error(`${executable} exited with exit code ${exitCode}`)
	}

	return output.split('\n')
}

/**
 * @param {boolean} onlyGitFiles
 * @async
 * @returns {Promise<string[]>}
 */
async function getMesonFiles(onlyGitFiles) {
	/** @type {string[]} */
	let files

	/** @type {string[]} */
	const mesonFiles = ['meson.build', 'meson.options', 'meson_options.txt']

	if (onlyGitFiles) {
		/** @type {string[]} */
		const gitFiles = mesonFiles.map((file) => `--exclude='${file}'`)

		files = await execAndGetStdout('git', [
			'ls-files',
			...gitFiles,
			'--ignored',
			'-c',
		])
	} else {
		/** @type {string[][]} */
		const findFiles = mesonFiles.map((file) => ['-name', file])

		/** @type {string[]} */
		const finalFindFiles = findFiles.reduce((acc, elem, index) => {
			if (index != 0) {
				acc.push('-o')
			}

			acc.push(...elem)

			return acc
		}, [])

		files = await execAndGetStdout('find', [
			'.',
			'(',
			...finalFindFiles,
			')',
		])
	}

	return files
}

/**
 *
 * @param {string} file
 * @param {string} formatFile
 * @async
 * @returns {Promise<boolean>}
 */
async function checkFile(file, formatFile) {
	/** @type {ExecOptions} */
	const options = {
		ignoreReturnCode: true,
	}

	/** @type {string[]} */
	const additionalArgs = formatFile == '' ? [] : ['-c', formatFile]

	/** @type {number} */
	const exitCode = await exec.exec(
		'meson',
		['format', '--check-only', ...additionalArgs, file],
		options
	)

	return exitCode == 0
}

/**
 * @async
 * @returns {Promise<void>}
 */
async function main() {
	try {
		/** @type {string} */
		const os = core.platform.platform

		if (os != 'linux') {
			throw new Error(
				`Action atm only supported on linux: but are on: ${os}`
			)
		}

		//WIP
		core.error(`file input is: ${core.getInput('format-file', { required: false })}`)
		core.error(`git-file input is: ${core.getInput('only-git-files', { required: false })}`)

		/** @type {string} */
		const formatFile = core.getInput('format-file', { required: false })

		/** @type {boolean} */
		const onlyGitFiles = core.getBooleanInput('only-git-files', {
			required: false,
		})

		/** @type {string[]} */
		const files = await getMesonFiles(onlyGitFiles)

		await io.which('meson', true)

		/** @type {string[]} */
		const notFormattedFiles = []

		core.startGroup('Check all files')

		//TODO: maybe parallelize this
		for (const file of files) {
			core.info(`Checking file: '${file}'`)

			/** @type {boolean} */
			const result = await checkFile(file, formatFile)

			core.info(
				result
					? 'File is formatted correctly'
					: 'File has formatting errors'
			)
			core.info('')

			if (!result) {
				notFormattedFiles.push(file)

				/** @type {AnnotationProperties} */
				const properties = {
					file,
					title: 'File not formatted correctly',
				}

				core.error('File not formatted correctly', properties)
			}
		}

		core.endGroup()

		if (notFormattedFiles.length == 0) {
			core.summary.clear()

			core.summary.addHeading('Result', 1)
			core.summary.addRaw(
				':white_check_mark: All files are correctly formatted',
				true
			)

			core.summary.write()
			return
		}

		core.summary.clear()

		core.summary.addList(
			notFormattedFiles.map((file) => `\`${file}\``),
			false
		)

		/** @type {string} */
		const fileList = core.summary.stringify()
		core.summary.clear()

		core.summary.addHeading('Result', 1)
		core.summary.addRaw(':x: Some files are not formatted correctly', true)

		core.summary.addDetails('Affected Files', fileList)
		core.summary.addSeparator()

		core.summary.addRaw(
			'To format the files run the following command',
			true
		)

		/** @type {string} */
		const additionalArgs = formatFile == '' ? [] : [`-c ${formatFile}`]

		/** @type {string} */
		const finalFileList = notFormattedFiles.join(' ')

		core.summary.addCodeBlock(
			`meson format ${additionalArgs} -i ${finalFileList}`,
			'bash'
		)

		core.summary.write()

		throw new Error('Soem files are not formatted correctly')
	} catch (error) {
		core.setFailed(error)
	}
}

main()
