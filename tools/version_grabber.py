#!/usr/bin/env python3

from dataclasses import dataclass
from typing import Callable, Literal, Optional, TypeVar
import yaml
import sys

A = TypeVar("A")


def parse_value(
    input: dict[str, str],
    key: str,
    *,
    default_value: Optional[A] = None,
    parser: Callable[[str], A] = str,
) -> A:
    value: str | None = input.get(key)
    if value is None:
        if default_value is None:
            raise ValueError(f"key '{key}' has to be present, but wasn't")

        return default_value

    return parser(value)


def required_to_be(value: str) -> Callable[[str], None]:
    def has_to_be(input: str) -> None:
        if value != input:
            raise ValueError(f"value has to be '{value}' but was '{input}'")
        return None

    return has_to_be


B = TypeVar("B")


def parse_list(
    parser: Callable[[str], B],
    *,
    separator: str = ",",
    trim_whitespace: bool = True,
    ignore_empty_cases: bool = True,
) -> Callable[[str], list[B]]:
    def func(input: str) -> list[B]:
        result: list[B] = []
        try:
            values = input.split(separator)
            for _value in values:
                value = _value.strip() if trim_whitespace else _value
                if ignore_empty_cases and value == "":
                    continue

                parsed_value = parser(value)
                result.append(parsed_value)
        except Exception as e:
            raise KeyError(f"not a list: {e}")

        return result

    return func


@dataclass
class PackageDescription:
    name: str
    version: str | Literal[""]
    depends: list[str]
    feature: str | Literal[""]
    default_features: list[str]

    @staticmethod
    def sorted(input: list["PackageDescription"]) -> list["PackageDescription"]:
        filtered_input: list[PackageDescription] = [
            inp for inp in input if inp.feature == ""
        ]

        for inp in input:
            if len(inp.default_features) != 0:
                dependencies: list[str] = inp.depends
                for feature in inp.default_features:
                    for sub_dependency in input:
                        if (
                            sub_dependency.name == inp.name
                            and sub_dependency.feature == feature
                        ):
                            dependencies.extend(sub_dependency.depends)

                filtered_input.append(
                    PackageDescription(inp.name, inp.version, dependencies, "", [])
                )

        wrong_dependencies = ["vcpkg-cmake", "vcpkg-cmake-config"]

        filtered_input_deps: list[PackageDescription] = []
        for inp in filtered_input:
            if inp.name not in wrong_dependencies:
                new_package = PackageDescription(
                    inp.name,
                    inp.version,
                    [
                        i
                        for i in inp.depends
                        if i not in wrong_dependencies and i != inp.name
                    ],
                    "",
                    [],
                )
                if new_package.name not in [i.name for i in filtered_input_deps]:
                    filtered_input_deps.append(new_package)

        key: Callable[[PackageDescription], int] = lambda a: len(a.depends)
        return sorted(filtered_input_deps, key=key)

    def __str__(self) -> str:
        components: list[str] = [self.name]
        if self.version != "":
            components.append(self.version)

        # a len of 0 has to be included, so that meson can parse it as empty list
        components.append(",".join(self.depends))

        return ":".join(components)


def parse_entry(raw_dict: dict[str, str]) -> PackageDescription:
    parse_value(raw_dict, "Status", parser=required_to_be("install ok installed"))
    name = parse_value(raw_dict, "Package")
    version = parse_value(raw_dict, "Version", default_value="")
    depends = parse_value(raw_dict, "Depends", parser=parse_list(str), default_value=[])

    feature = parse_value(raw_dict, "Feature", default_value="")
    default_features = parse_value(
        raw_dict, "Default-Features", default_value=[], parser=parse_list(str)
    )
    result: PackageDescription = PackageDescription(
        name, version, depends, feature, default_features
    )

    return result


with open(sys.argv[1], "r") as file:
    content = file.read()
    libraries = content.replace("\r", "").split("\n\n")
    parsed_libs = [parse_entry(yaml.safe_load(lib)) for lib in libraries if lib != ""]
    if len(sys.argv) > 2:
        name = sys.argv[2]
        for package in parsed_libs:
            if package.name == name:
                print(package.version)
                exit(0)

        print(f"Couldn't find {name}")
        exit(2)

    else:
        for package in PackageDescription.sorted(parsed_libs):
            print(package)
