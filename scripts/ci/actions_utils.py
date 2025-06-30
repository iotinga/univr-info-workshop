import sys
import logging
import os


def is_github_actions():
    return "GITHUB_ACTIONS" in os.environ


def set_output(name: str, value):
    print(f"::set-output name={name}::{value}")


def start_group(title: str):
    print(f"::group::{title}")


def end_group():
    print("::endgroup::")


def set_env_variable(name: str, value: str):
    logging.info(f"EXPORT {name} = {value}")
    if os.environ.get(name, None) == name:
        return  # already set
    env_file = os.environ.get("GITHUB_ENV", None)
    if env_file is not None:
        with open(env_file, "a") as f:
            print(f"{name}={value}", file=f)
    else:
        logging.warn("GitHub env file not defined")

    os.environ[name] = value


def require_env_variable(name: str, export=False):
    if name not in os.environ:
        logging.error(f"Missing required environment variable: {name}")
        sys.exit(1)
    value = os.environ[name]
    logging.info(f"ENV[{name}] = {value} (export: {export})")
    if export:
        set_env_variable(name, value)


def ensure_ci():
    if not is_github_actions():
        raise RuntimeError("not running in GitHub actions")
