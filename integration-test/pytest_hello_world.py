from typing import Callable

import pytest
from pytest_embedded_idf.dut import IdfDut


@pytest.mark.esp32
@pytest.mark.generic
def test_hello_world(
    dut: IdfDut, log_minimum_free_heap_size: Callable[..., None]
) -> None:
    dut.expect("Hello world From ESP32!")
    log_minimum_free_heap_size()


@pytest.mark.linux
@pytest.mark.host_test
def test_hello_world_linux(dut: IdfDut) -> None:
    dut.expect("Hello world From EMULATOR!")
