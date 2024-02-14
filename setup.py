from glob import glob
import platform
from setuptools import setup, Extension
from setuptools.command.build_ext import build_ext
from setuptools.command.build_clib import build_clib
import sys
import pybind11

__version__ = '0.0.0'


libevent = ('eventManager', {'sources': ['cpp/RAT/events/eventManager.cpp'],
                             'include_dirs': ['cpp/RAT/events/']})


ext_modules = [
    Extension(
        'rat.rat_core',
        sources=['cpp/rat.cpp', *glob('cpp/RAT/*.c*')],
        include_dirs=[
            # Path to pybind11 headers
            pybind11.get_include(),
            pybind11.get_include(True),
            'cpp/RAT/',
        ],
        language='c++'
    ),
]


# check whether compiler supports a flag
def has_flag(compiler, flagname):
    import tempfile
    from distutils.errors import CompileError
    with tempfile.NamedTemporaryFile('w', suffix='.cpp') as f:
        f.write('int main (int argc, char **argv) { return 0; }')
        try:
            compiler.compile([f.name], extra_postargs=[flagname])
        except CompileError:
            return False
    return True


def get_shared_object_name(lib_name):
    if platform.system() == 'Windows':
        return f'{lib_name}.dll'
    elif platform.system() == 'Darwin':
        return f'{lib_name}.dylib'
    else:
        return f'{lib_name}.so'


class BuildExt(build_ext):
    """A custom build extension for adding compiler-specific options."""
    c_opts = {
        'msvc': ['/EHsc'],
        'unix': ['-fopenmp'],
    }
    l_opts = {
        'msvc': [],
        'unix': ['-fopenmp'],
    }

    if sys.platform == 'darwin':
        darwin_opts = ['-stdlib=libc++', '-mmacosx-version-min=10.7']
        c_opts['unix'] += darwin_opts
        l_opts['unix'] += darwin_opts

    def build_extensions(self):
        ct = self.compiler.compiler_type
        opts = self.c_opts.get(ct, [])
        link_opts = self.l_opts.get(ct, [])
        if ct == 'unix':
            if '-Wstrict-prototypes' in self.compiler.compiler_so:
                self.compiler.compiler_so.remove('-Wstrict-prototypes')

            opts.append('-DVERSION_INFO="%s"' % self.distribution.get_version())
            opts.append('-std=c++11')
            if has_flag(self.compiler, '-fvisibility=hidden'):
                opts.append('-fvisibility=hidden')
        elif ct == 'msvc':
            opts.append('/DVERSION_INFO=\\"%s\\"' % self.distribution.get_version())
        for ext in self.extensions:
            ext.extra_compile_args = opts
            ext.extra_link_args = link_opts
        build_ext.build_extensions(self)


class BuildClib(build_clib):
    def initialize_options(self):
        super().initialize_options()
        build_py = self.get_finalized_command('build_py')
        self.build_clib =  build_py.get_package_dir("rat")

    def build_libraries(self, libraries):
        # bug in distutils: flag not valid for c++
        flag = '-Wstrict-prototypes'
        if (hasattr(self.compiler, 'compiler_so')
                and flag in self.compiler.compiler_so):
            self.compiler.compiler_so.remove(flag)

        compiler_type = self.compiler.compiler_type
        if compiler_type == 'msvc':
            compile_args = ['/std:c++11', '/EHsc', '/LD']
        else:
            compile_args = ['-std=c++11', '-fPIC']

        for (lib_name, build_info) in libraries:
            build_info['cflags'] = compile_args
            macros = build_info.get('macros')
            include_dirs = build_info.get('include_dirs')
            cflags = build_info.get('cflags')
            sources = list(build_info.get('sources'))
            objects = self.compiler.compile(
                            sources,
                            output_dir=self.build_temp,
                            macros=macros,
                            include_dirs=include_dirs,
                            extra_postargs=cflags,
                            debug=self.debug
                            )
            language = self.compiler.detect_language(sources)
            self.compiler.link_shared_object(
                objects,                     
                get_shared_object_name(lib_name),
                output_dir=self.build_clib, 
                target_lang=language
                )

        super().build_libraries(libraries)


setup(
    name='rat',
    version=__version__,
    author='demo',
    author_email='demo@gmail.com',
    url='https://github.com/RascalSoftware/python-RAT',
    description='Python extension for the Reflectivity Analysis Toolbox (RAT)',
    long_description='',
    packages=['rat'],
    package_data={'rat': [get_shared_object_name(libevent[0])]},
    libraries=[libevent],
    ext_modules=ext_modules,
    install_requires=['pybind11>=2.4', 'numpy'],
    python_requires='>=3.9',
    # setup_requires=['pybind11>=2.4'],
    cmdclass={'build_ext': BuildExt, 'build_clib': BuildClib},
    zip_safe=False,
)
