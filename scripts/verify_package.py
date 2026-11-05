"""Reject source-only or wrong-architecture packages before publication."""
import json, pathlib, struct, sys, zipfile
path = pathlib.Path(sys.argv[1])
with zipfile.ZipFile(path) as z:
    metadata = json.loads(z.read('mod.json'))
    expected = json.loads(pathlib.Path('mod.json').read_text())
    assert metadata['id'] == expected['id'] and metadata['version'] == expected['version']
    dll = z.read('dim5l.practice_sync.dll')
    assert dll[:2] == b'MZ'
    pe = struct.unpack_from('<I', dll, 0x3c)[0]
    assert dll[pe:pe+4] == b'PE\0\0' and struct.unpack_from('<H', dll, pe+4)[0] == 0x8664
    so = z.read('dim5l.practice_sync.android64.so')
    assert so[:6] == b'\x7fELF\x02\x01' and struct.unpack_from('<H', so, 18)[0] == 183
    assert len(dll)>100000 and len(so)>100000
print('Verified Windows x64 + Android ARM64 binaries:', path)
