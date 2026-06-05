import sys

import numpy as np
from Cython.Build import cythonize
from setuptools import Extension, setup

extensions = [
    Extension(
        "myknn.py_kdtree",
        sources=["src/py_kdtree.pyx", "src/kdtree.c"],
        include_dirs=["src", np.get_include()],
        libraries=["m"] if sys.platform != "win32" else [],
    )
]

setup(
    name="myknn",
    version="0.0.4",
    python_requires=">=3.10",
    packages=["myknn"],
    install_requires=["numpy>=1.20"],
    extras_require={"dev": ["pytest>=7", "Cython", "scikit-learn>=1.0"]},
    ext_modules=cythonize(extensions, language_level=3),
)
