from pathlib import Path

import os
import json
import glob
import argparse
import platform
import sys
import typing


# Если нужно добавить текущую папку для поиска имортов
# script_dir_path = Path(os.path.dirname(os.path.realpath(__file__)))
# sys.path.append(os.path.dirname(os.path.dirname(os.path.realpath(__file__))))


class c_str:

    COMPILE_COMMAND_JSON = "compile_commands.json"
    ARGUMENTS = "arguments"
    DIRECTORY = "directory"
    FILE = "file"

    # json
    OBJECTS = "objects"
    KIND = "kind"
    JSON_FILE = "jsonFile"
    CODEMODEL = "codemodel"
    TOOLCHAINS = "toolchains"
    COMPILER = "compiler"
    NAME = "name"

    IMPLICIT = "implicit"
    INCLUDE_DIRECTORIES = "includeDirectories"
    CONFIGURATIONS = "configurations"
    TARGETS = "targets"
    SOURCES = "sources"
    COMPILE_GROUPS = "compileGroups"

    LANGUAGE = "language"
    LANGUAGE_STANDARD = "languageStandard"
    STANDARD = "standard"
    INCLUDES = "includes"
    DEFINES = "defines"
    DEFINE = "define"
    PATH = "path"
    PATHS = "paths"
    BUILD = "build"


parser = argparse.ArgumentParser("Генератор compile_commands.json")
parser.add_argument("--srcDir", type=str, help="Путь до директории с исходниками проекта")
parser.add_argument("--inputFile", type=str, help="Путь до файла, для которого следует сформировать compile_commands.json")
parser.add_argument("--targetName", type=str, help="требуется передать имя библиотеки, чтобы определить target")
parser.add_argument("--buildDir", type=str, help="Путь до сборочной директории")
parser.add_argument("--compile_commands_path", type=str, help="Полный путь к будущему compile_commands.json")


def printJson(j):  # DELME
    print(json.dumps(j, indent=2))


class CommandLine:
    def __init__(self) -> None:
        self.all_args = parser.parse_args()

        if (not self.all_args.srcDir):
            print("‼️ Не указана папка к исходникам")  # FIXME english for Windows
            exit(1)
        self.srcDir = Path(self.all_args.srcDir)
        print(f" srcDir:      {self.srcDir}")

        if (not self.all_args.inputFile):
            print("‼️ Не указан файл для разбора")  # FIXME english for Windows
            exit(2)
        self.inputFile = Path(self.all_args.inputFile)
        print(f" inputFile:   {self.inputFile}")

        if (not self.all_args.buildDir):
            print("‼️ Не указана сборочная директория")  # FIXME english for Windows
            exit(3)
        self.buildDir = Path(self.all_args.buildDir)
        print(f" buildDir:    {self.buildDir}")

        if (not self.all_args.targetName):
            print("‼️ Не указано имя библиотеки для определения target")  # FIXME english for Windows
            exit(3)
        self.targetName = self.all_args.targetName
        print(f" targetName:  {self.targetName}")

        if (not self.all_args.compile_commands_path):
            print("‼️ Не указан путь к compile_commands")  # FIXME english for Windows
            exit(9)
        self.compile_commands_path = Path(self.all_args.compile_commands_path)
        print(f" ccPath:      {self.compile_commands_path}")


def get_json_param(j, param, exception_is_none=False):
    if not param in j:
        if exception_is_none:
            raise
        return None
    return j[param]


def get_data_index_file(path_to_dir):

    index_files = glob.glob(os.path.join(path_to_dir, "index-*.json"))

    if not index_files:
        print("CMake API index file not found.")
        exit(4)

    # TODO Должен быть первым и единственным.
    #   Однако, возможны случаи, когда их несколько.
    #   В этом случае, нужно взять наибольший по времени.
    #   https://cmake.org/cmake/help/v3.31/manual/cmake-file-api.7.html#v1-reply-index-file
    with open(index_files[0], "r") as f:
        index_data = json.load(f)
        return index_data


class BaseJsonLoad:
    """ Базовый класс получения инфы из json файла """

    def __init__(self, path: str) -> None:
        with open(path, mode="r") as f:
            self.data = json.load(f)


class Toolchain(BaseJsonLoad):
    """ Парсинг файла toolchains"""

    class Compiler_info:
        def __init__(self) -> None:
            self.path = ""  # Путь до компилятора
            self.includeDirectories = []  # Путь до системных include path

    def __init__(self, path) -> None:
        super().__init__(path)

        self.compiler_info = self.Compiler_info()

        self.collect()

    def collect(self):
        """ Получить информацию по компилятору """

        for toolchain in get_json_param(self.data, c_str.TOOLCHAINS, True):  # type: ignore

            language = get_json_param(toolchain, c_str.LANGUAGE)

            if (language == None) or (language != "CXX"):
                continue

            compiler = get_json_param(toolchain, c_str.COMPILER)
            if (compiler == None):
                continue

            # TODO в идеале бы проверить еще на то, является ли это компилятор clang'ом
            if platform.system() == "Linux":
                compiler_path = get_json_param(compiler, c_str.PATH)
                if (compiler_path == None):
                    continue
                self.compiler_info.path = compiler_path
            else:
                self.compiler_info.path = "clang.exe"

            compiler_implicit = get_json_param(compiler, c_str.IMPLICIT)
            if compiler_implicit == None:
                break

            include_directories = get_json_param(compiler_implicit, c_str.INCLUDE_DIRECTORIES)
            if include_directories == None:
                break
            self.compiler_info.includeDirectories = include_directories

            break  # Мы уже заполнили, что искали

    def get_arguments(self):
        """ Заполнить arguments для compile_commands.json """

        args = []
        if len(self.compiler_info.path) == 0:
            raise Exception("Unable to detect compiler in use for CXX")
        args.append(self.compiler_info.path)

        for include_directory in self.compiler_info.includeDirectories:
            args.append(f"-I{include_directory}")

        return args


class Target_info:
    def __init__(self) -> None:
        self.cxxStandard = None
        self.includes = []
        self.defines = []


class Target(BaseJsonLoad):

    def __init__(self, path) -> None:
        super().__init__(path)

        self.target_info = Target_info()

        self._compile_group: typing.Any

    def collect(self):

        compile_groups = get_json_param(self.data, c_str.COMPILE_GROUPS, True)

        len_compile_groups = len(compile_groups)  # type: ignore
        if len_compile_groups < 1:
            raise Exception("Unable to find compileGroups")

        if len_compile_groups > 1:
            print(" WARNING More than one compileGroups was found while generating compile_commands")
        self._compile_group = compile_groups[0]  # type: ignore | Еще ни разу не попадалось больше единицы и не очень понятно, от чего зависит, если будет несколько

        self.get_standard()
        self.get_includes()
        self.get_defines()

        return self.target_info

    def get_standard(self):

        lang_standard = get_json_param(self._compile_group, c_str.LANGUAGE_STANDARD)
        if lang_standard == None:
            return

        standard = get_json_param(lang_standard, c_str.STANDARD)
        if standard == None:
            return

        self.target_info.cxxStandard = standard

    def get_includes(self):
        includes = get_json_param(self._compile_group, c_str.INCLUDES)
        if includes == None:
            return

        for include in includes:
            include_path = get_json_param(include, c_str.PATH)
            if include_path == None:
                continue
            self.target_info.includes.append(include_path)

    def get_defines(self):
        defines = get_json_param(self._compile_group, c_str.DEFINES)
        if defines == None:
            return

        for define in defines:
            self.target_info.defines.append(define[c_str.DEFINE])

        for define in defines:
            define_name = get_json_param(define, c_str.DEFINE)
            if define_name == None:
                continue
            self.target_info.defines.append(define_name)


class Codemodel(BaseJsonLoad):
    def __init__(self, path) -> None:
        super().__init__(path)

        self.collect()
        self.result: Target_info

    def collect(self):
        configurations = self.data[c_str.CONFIGURATIONS]
        len_configs = len(configurations)
        if len_configs < 1:
            raise Exception("Unable to find configuration")

        if len_configs > 1:
            print(" WARNING More than one configuration was found while generating compile_commands")
        configuration = configurations[0]  # FIXME В одной сборочной директории, кажется, не должно быть больше одной конфигурации

        targets = configuration[c_str.TARGETS]

        for target in targets:
            target_name = get_json_param(target, c_str.NAME)
            if (target_name == None) or (target_name != cLine.targetName):
                continue

            target_json_file = get_json_param(target, c_str.JSON_FILE, True)
            t = Target(os.path.join(API_REPLY_DIR, target_json_file))  # type: ignore
            self.result = t.collect()
            break

    def get_arguments(self):
        args = []

        if self.result.cxxStandard != None:
            args.append(f"-std=c++{self.result.cxxStandard}")

        for include in self.result.includes:
            args.append(f"-I{include}")

        for define in self.result.defines:
            args.append(f"-D{define}")

        return args


# === START SCRIPT ===

cLine = CommandLine()

# Папка до всех json от CMake File Api
API_REPLY_DIR = cLine.buildDir / ".cmake/api/v1/reply"

objs = get_json_param(get_data_index_file(API_REPLY_DIR), c_str.OBJECTS)
if objs == None:
    exit(10)
kind_to_json = {obj["kind"]: obj["jsonFile"] for obj in objs}

TOOLCHAIN_FILEPATH = os.path.join(API_REPLY_DIR, kind_to_json[c_str.TOOLCHAINS])
CODEMODEL_FILEPATH = os.path.join(API_REPLY_DIR, kind_to_json[c_str.CODEMODEL])

arguments = []  # Раздел аргументов в compile_commands.json

# Аргументы, заполняемые из CMake
arguments = arguments + Toolchain(TOOLCHAIN_FILEPATH).get_arguments()
arguments = arguments + Codemodel(CODEMODEL_FILEPATH).get_arguments()


# Дополнительные аргументы, заполняемые не из CMake
if platform.system() == "Linux":
    arguments.append("-fPIC")
arguments.append("-c")


targetPaltform = "x86_64-pc-linux-gnu" if platform.system() == "Linux" else "x86_64-pc-windows-msvc"
arguments.append(f"--target={targetPaltform}")
arguments.append("-x")
arguments.append("c++")

fake_path = f"{cLine.inputFile}.cxx"
arguments.append(fake_path)


compile_command = {
    c_str.ARGUMENTS: arguments,
    c_str.DIRECTORY: str(cLine.compile_commands_path.parent),
    c_str.FILE: fake_path,
}
compile_commands = [compile_command]


printJson(compile_commands)

# Создаем директорию до формирования compile_commands.json
cLine.compile_commands_path.parent.mkdir(parents=True, exist_ok=True)

# Пишем сформированный compile_commands.json
with open(cLine.compile_commands_path, "w") as f:
    json.dump(compile_commands, f, indent=2)

print("compile_commands.json generated.")
