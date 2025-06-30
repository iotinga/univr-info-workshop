import subprocess
import logging


def parse_version(version: str):
    if not version[0] == "v":
        raise ValueError("Tag should begin with `v` character")

    parts = list(map(int, version[1:].split(".")))
    if len(parts) not in (2, 3):
        raise ValueError("Tag format is not correct")

    major = parts[0]
    minor = parts[1]
    if len(parts) == 3:
        patch = parts[2]
    else:
        patch = 0

    return (major, minor, patch)


def get_latest_tag():
    output = subprocess.check_output(["git", "tag"])
    latest_tag = (0, 0, 0)
    for line in output.splitlines():
        line = line.decode("utf-8")

        try:
            tag_version = parse_version(line)
            if tag_version > latest_tag:
                latest_tag = tag_version
        except ValueError:
            logging.warning("Repository contains a malformed tag: %s", line)

    return latest_tag


def version_to_string(version):
    assert len(version) == 3
    return f"v{version[0]}.{version[1]}.{version[2]}"
