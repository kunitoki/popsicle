wheel:
    python -m build --wheel -n && just install

install:
    pip install --force-reinstall dist/popsicle-*.whl

uninstall:
    pip uninstall -y popsicle

test *TEST_OPTS:
    pytest -s {{TEST_OPTS}}
