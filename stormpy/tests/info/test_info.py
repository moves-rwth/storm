import stormpy
from stormpy.info import info

class TestInfo:
    def test_version(self):
        s = info.Version.short
        s = info.Version.long
        s = info.Version.build_info
