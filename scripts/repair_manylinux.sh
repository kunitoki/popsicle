#!/bin/bash

auditwheel repair --plat manylinux2010_x86_64 -L /data/ --strip dist/popsicle-0.0.6-cp36-cp36m-linux_x86_64.whl
