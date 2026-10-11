# thanks a lot for
# https://github.com/AlexeyDmitriev/inline_includes/blob/master/inline_includes

import sys
import os
import os.path

USED = set()
INCLUDE_PATHS = None
IN_BLOCK_COMMENT = False


def strip_comments(line):
    # drops // and /* */ comments, skipping string and char literals
    global IN_BLOCK_COMMENT
    res = []
    i = 0
    quote = None
    while i < len(line):
        c = line[i]
        if IN_BLOCK_COMMENT:
            if line.startswith("*/", i):
                IN_BLOCK_COMMENT = False
                i += 2
            else:
                i += 1
            continue
        if quote is not None:
            res.append(c)
            if c == "\\" and i + 1 < len(line):
                res.append(line[i + 1])
                i += 1
            elif c == quote:
                quote = None
            i += 1
            continue
        if line.startswith("//", i):
            break
        if line.startswith("/*", i):
            IN_BLOCK_COMMENT = True
            i += 2
            continue
        if c in "\"'":
            quote = c
        res.append(c)
        i += 1
    stripped = "".join(res).rstrip()
    if stripped == "" and line.strip() != "":
        return None  # the line was only a comment
    return stripped + "\n"


def include_target(file, path):
    for include_path in [os.path.dirname(file), *INCLUDE_PATHS]:
        candidate = os.path.join(include_path, path)
        if os.path.isfile(candidate):
            return candidate
    return None


def dfs(file):
    file = os.path.abspath(file)
    if file in USED:
        return
    USED.add(file)
    with open(file, "r") as f:
        for line in f:
            line = strip_comments(line)
            if line is None:
                continue
            if line.startswith("#include "):
                path = include_target(file, line[8:].strip('\n\r "<>'))
                if path is not None:
                    dfs(path)
                else:
                    if not line.startswith("#pragma once"):
                        print(line, end="")
            else:
                if not line.startswith("#pragma once"):
                    print(line, end="")


def main():
    global INCLUDE_PATHS
    INCLUDE_PATHS = sys.argv[:-1]
    dfs(sys.argv[-1])


if __name__ == "__main__":
    main()
