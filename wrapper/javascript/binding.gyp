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
            ],
            "conditions": [
                [
                    'OS == "mac"',
                    {"cflags_cc!": ["-std=c++23"], "cflags_cc+": ["-std=c++20"]},
                ]
            ],
            "defines": ["V8_DEPRECATION_WARNINGS=1"],
            "sources": ["src/cpp/wrapper.cpp"],
            "include_dirs": [
                "<!@(node -e \"require('nan')\")",
                "<!@(pkg-config oopetris-recordings --cflags-only-I | sed s/-I//g)",
            ],
            "libraries": [
                "<!@(pkg-config oopetris-recordings --libs-only-l)",
            ],
            "library_dirs": [
                "<!@(pkg-config oopetris-recordings --libs-only-L | sed s/-L//g)",
            ],
        }
    ],
}
