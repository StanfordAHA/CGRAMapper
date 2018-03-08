import delegator
import os
os.environ["MANTLE"] = "coreir"
import magma as m
from magma.testing import check_files_equal
import mantle
from mantle.coreir.arith import DefineAdd, DefineSub
from magma.testing.newfunction import testvectors as function_test
from magma.simulator.python_simulator import testvectors as simulator_test

primitives = ["DefineAdd", "DefineSub"]

def pytest_generate_tests(metafunc):
    if 'primitive' in metafunc.fixturenames:
        metafunc.parametrize("primitive", primitives)

def test_primitive(primitive):
    prim = getattr(mantle.coreir, primitive)
    width = 16
    m.compile(f"build/test_{primitive}16", prim(width), output="coreir")
    assert check_files_equal(__file__,
            f"build/test_{primitive}16.json", f"gold/test_{primitive}16.json")

    dir_path = os.path.dirname(os.path.realpath(__file__))
    assert not delegator.run(f"../../bin/cgra-mapper build/test_{primitive}16.json build/test_{primitive}16_mapped.json", cwd=dir_path).return_code
    assert check_files_equal(__file__,
            f"build/test_{primitive}16_mapped.json", f"gold/test_{primitive}16_mapped.json")
