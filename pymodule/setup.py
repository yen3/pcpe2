# setup.py
from distutils.core import setup, Extension

setup(name="pcpe2",
      version="1.0",
      py_modules = ['pcpe2.py'],
      ext_modules = [
            Extension("_pcpe2",
                      ["src/pymodule.cpp",
                       "src/esort.cpp",
                       "src/hash_table.cpp",
                       "src/main.cpp",
                       "src/max_comsubseq.cpp",
                       "src/pcpe_util.cpp"],
                      include_dirs = ["inc"],
                      define_macros = [('__USE_PCPE2_PYTHON_MODULE__', '1')],)
        ]
      )
