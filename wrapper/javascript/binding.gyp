# type: ignore
{
    "targets": [
        {
            "target_name": "oopetris",
            "cflags_cc": [
                "-std=c++23",
                "-Wall",
                "-Wextra",
                "-Wno-unused-parameter",
                "-O3",
                "-Werror",
                "-Wpedantic",
                "-fexceptions",
                "-frtti",
                "-Wno-cast-function-type",  # since nan.h -> node.h has some warnings regarding that
                "<!@(pkg-config oopetris-recordings --cflags-only-other)",
                "-static",  # statically link this, as we do on windows ( it's the default as per node-gyp )
            ],
            "conditions": [
                [
                    'OS == "mac"',
                    {
                        "xcode_settings": {
                            "OTHER_CFLAGS+": [
                                "-std=c++23",
                                "-Wall",
                                "-Wextra",
                                "-Wno-unused-parameter",
                                "-O3",
                                "-Werror",
                                "-Wpedantic",
                                "-fexceptions",
                                "-frtti",
                                "-Wno-cast-function-type",  # since nan.h -> node.h has some warnings regarding that
                                "<!@(pkg-config oopetris-recordings --cflags-only-other)",
                                "-static",  # statically link this, as we do on windows ( it's the default as per node-gyp )
                                "-I/usr/local/include",  # this is and error with magic_enum in mac, but this small hack fixes it :)
                            ]
                        }
                    },
                ],
                [
                    'OS != "win"',
                    {
                        "libraries": [
                            "<!@(pkg-config oopetris-recordings --libs-only-l)",
                        ],
                    },
                ],
                [
                    'OS == "win"',
                    {
                        "include_dirs+": [
                            "<!@(node -e \"console.log(require('path').join(require.resolve('nan'),'..').replaceAll('\\\\','/'))\")",
                        ]
                    },
                ],
            ],
            "defines": ["V8_DEPRECATION_WARNINGS=1"],
            "sources": ["src/cpp/wrapper.cpp"],
            "include_dirs": [
                "<!@(node -e \"require('nan')\")",
                "<!@(pkg-config oopetris-recordings --cflags-only-I | sed s/-I//g)",
            ],
            "library_dirs": [
                "<!@(pkg-config oopetris-recordings --libs-only-L | sed s/-L//g)",
            ],
            "msvs_settings": {  ## settinsg cflags_cc doesn't really work onw windows, so using this
                "VCCLCompilerTool": {
                    "AdditionalOptions": [
                        "/std:c++latest",
                        "/W4",
                        "/EHsc",
                        "/O2",
                        "/wd4100",  # since nan.h -> node.h has some warnings regarding that
                        "<!@(pkg-config oopetris-recordings --cflags)",
                    ]
                },
                "VCLinkerTool": {
                    "AdditionalDependencies": [
                        "<!@(pkg-config oopetris-recordings --libs-only-l | sed s/-l/lib/g |sed 's/\s/.a /g')",  # adjust to the default setting, namely lib<name>.a via some sed magic
                    ],
                },
            },
        }
    ],
}
