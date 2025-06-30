#!/usr/bin/env python3

import os
import subprocess

from actions_utils import require_env_variable


def main():
    # check that we have all the required environment variables
    require_env_variable("PRIVATE_ZIP_PASSWORD")

    if "PRIVATE_ZIP_PASSWORD" in os.environ:
        private_zip_password = os.environ["PRIVATE_ZIP_PASSWORD"]
        subprocess.check_call(["unzip", "-P", private_zip_password, "private.zip"])


if __name__ == "__main__":
    main()
