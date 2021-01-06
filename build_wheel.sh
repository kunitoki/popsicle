#!/bin/bash

python setup.py clean --all
python setup.py bdist_wheel

python3 -m twine upload --repository testpopsicle dist/*.whl
