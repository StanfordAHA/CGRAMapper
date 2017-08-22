import pytest

def pytest_addoption(parser):
    parser.addoption("--libs", nargs='+', help='coreir libs to load', default=())
    parser.addoption("--files", nargs='+', help='coreir files to load', default=())

@pytest.fixture
def libs(request):
    return request.config.getoption("--libs")

@pytest.fixture
def files(request):
    return request.config.getoption("--files")
