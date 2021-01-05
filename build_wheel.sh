#!/bin/bash

python setup.py clean --all
python setup.py bdist_wheel

python3 -m twine upload --repository popsicle dist/*.whl
