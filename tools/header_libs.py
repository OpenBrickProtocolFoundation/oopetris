#!/usr/bin/env python3

import json
import sys

with open(sys.argv[1]) as file:
    ## see schema https://raw.githubusercontent.com/microsoft/vcpkg-tool/main/docs/vcpkg.schema.json
    vcpkg_content = json.load(file)
    for dependency in vcpkg_content["dependencies"]:
        if isinstance(dependency, str):
            print(dependency)
        elif isinstance(dependency, dict):
            print(dependency["name"])
        else:
            exit(2)
