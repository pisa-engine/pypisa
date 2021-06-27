import pypisa


def test_pypisa(tmpdir):
    pypisa.index("test/clueweb1k.plaintext", str(tmpdir), "plaintext", 100)
    pypisa.compress(str(tmpdir), "block_simdbp", "bm25", 128, False)
