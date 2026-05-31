import sys

import numpy as np
from Cython.Build import cythonize
from setuptools import Extension, setup

extensions = [
    Extension(
        "airroute.py_kdtree",
        sources=["src/py_kdtree.pyx", "src/kdtree.c"],
        include_dirs=["src", np.get_include()],
        libraries=["m"] if sys.platform != "win32" else [],
    )
]

setup(
    name="airroute",
    version="0.0.1",
    python_requires=">=3.10",
    packages=["airroute"],
    install_requires=["numpy>=1.20"],
    extras_require={"dev": ["pytest>=7", "Cython"]},
    ext_modules=cythonize(extensions, language_level=3),
)
