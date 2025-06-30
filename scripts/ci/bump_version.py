#!/usr/bin/env python3

import logging
import os

from actions_utils import set_env_variable, require_env_variable
from git_utils import get_latest_tag, version_to_string


def main():
    # check that we have all the required environment variables
    require_env_variable("GITHUB_TOKEN")

    if "VERSION" in os.environ:
        version = os.environ["VERSION"]
        logging.info(f"Current tag: {version}")
    else:
        # version increase algorithm:
        # - list all git tags, sorted by value
        # - get latest tag
        # - if no tag found
        #   - fail
        # - if latest tag is vX.Y.Z
        #   - next version is vX.Y.(Z+1)
        # - else
        #   - next version is vX.Y.0
        last_tag = get_latest_tag()
        current_tag = (last_tag[0], last_tag[1], last_tag[2] + 1)
        version = version_to_string(current_tag)
        print(f"Bump tag to: {version}")

    # export environment variables
    set_env_variable("VERSION", version)


if __name__ == "__main__":
    main()
