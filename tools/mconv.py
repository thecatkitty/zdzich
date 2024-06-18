import re
import sys

from argparse import ArgumentParser, FileType
from io import TextIOWrapper
from typing import Generator


# Regular expressions for formatting specifiers
RE_WINFMT = re.compile(
    r"%(\d+)!([-\+# ]*\d*\.?\d*(hh|ll|I32|I64|[hjlLtwzI])?[cCdiouxXeEfFgGaAnpsSZ])!")
RE_LOWER_S = re.compile(r"(%\d+\$[-\+# ]*\d*\.?\d*)s")
RE_UPPER_S = re.compile(r"(%\d+\$[-\+# ]*\d*\.?\d*)S")
RE_POS = re.compile(r"%(\d+)\$")


# Parse an assignemnt into a key-value paur
def get_assignment(line: str) -> tuple[str, str]:
    key, value = line.rstrip().split("=", 1)
    if value.startswith("(") and value.endswith(")"):
        value = value[1:-1]

    return key, value


# Load all messages from a MC file
def get_messages(input: TextIOWrapper) -> Generator[tuple[int, int, str], None, None]:
    languages = dict()

    message_id = str()
    message_language = int()
    message_text = str()

    for line in input:
        if line.startswith((";", "\n")):
            continue

        if line.rstrip() == ".":
            yield message_id, message_language, message_text.rstrip("\n")
            message_text = str()
            continue

        if re.match(r"^\w+=", line):
            key, value = get_assignment(line)
            if key == "LanguageNames":
                lang_name, details = get_assignment(value)
                languages[lang_name] = int(details.split(":")[0], 0)
            if key == "MessageId":
                message_id = int(value, 0)
            if key == "Language":
                message_language = languages[value]
            continue

        message_text += line


# Convert MSVC positional specifiers to POSIX syntax
def to_posix_fmt(winfmt: str) -> str:
    posfmt = RE_WINFMT.sub(r"%\1$\2", winfmt)
    posfmt = RE_LOWER_S.sub(r"\1ls", posfmt)
    posfmt = RE_UPPER_S.sub(r"\1s", posfmt)
    return posfmt


# Strip positional specifiers
def to_ansi_fmt(posfmt: str) -> str:
    assert is_nano_compatible(posfmt)
    return RE_POS.sub("%", posfmt)


# Check if there's no position change
def is_nano_compatible(fmt: str) -> bool:
    for i, pos in enumerate(RE_POS.findall(fmt), 1):
        if i != int(pos):
            return False

    return True


# Escape backslashes, double quotes, new lines
def escape_string(string: str) -> str:
    ret = str(string)
    for old, new in [
        ("\\", "\\\\"),
        ("\"", "\\\""),
        ("\n", "\\n"),
    ]:
        ret = ret.replace(old, new)
    return ret


# Parse command line arguments
parser = ArgumentParser(description="Utility for converting Message Compiler text files into C language",
                        epilog="Copyright (c) 2024 Mateusz Karcz. MIT Licensed.")

parser.add_argument("input", type=FileType(
    encoding="utf-8-sig"), help="Message Compiler input text file")
parser.add_argument(
    "lcid", type=lambda x: int(x, 0), help="LCID of the language to be extracted")
parser.add_argument("-o", type=FileType("w", encoding="utf-8"), dest="output", default=sys.stdout,
                    help="output C file (stdout is the default)")

args = parser.parse_args()

# Create the declaration
print("const struct { unsigned id; const char *msg; }", file=args.output)
print(f"MESSAGES_{args.lcid:04X}[] = {{", file=args.output)

# Convert all messages filtered by LCID to C language
for id, lang, text in get_messages(args.input):
    if lang != args.lcid:
        continue

    converted = escape_string(to_ansi_fmt(to_posix_fmt(text)))
    print(f"{{0x{id:04X}, \"{converted}\"}},", file=args.output)

# Write the footer
print("{0, 0}", file=args.output)
print("};", file=args.output)
