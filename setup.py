from glob import glob
from pathlib import Path
import platform
import sys
import pybind11
from setuptools import setup, Extension
from setuptools.command.build_ext import build_ext
from setuptools.command.build_clib import build_clib


__version__ = '0.0.0'


libevent = ('eventManager', {'sources': ['cpp/RAT/events/eventManager.cpp'],
                             'include_dirs': ['cpp/RAT/events/']})


ext_modules = [
    Extension(
        'RAT.rat_core',
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
    from setuptools.errors import CompileError
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
        'unix': ['-fopenmp', '-std=c++11'],
    }
    l_opts = {
        'msvc': [],
        'unix': ['-fopenmp'],
    }

    if sys.platform == 'darwin':
        darwin_opts = ['-stdlib=libc++', '-mmacosx-version-min=10.9']
        c_opts['unix'] = [*darwin_opts, '-fopenmp']
        l_opts['unix'] = [*darwin_opts, '-lomp']

    def build_extensions(self):
        ct = self.compiler.compiler_type
        opts = self.c_opts.get(ct, [])
        link_opts = self.l_opts.get(ct, [])
        if ct == 'unix':
            if '-Wstrict-prototypes' in self.compiler.compiler_so:
                self.compiler.compiler_so.remove('-Wstrict-prototypes')

            opts.append('-DVERSION_INFO="%s"' % self.distribution.get_version())
            if has_flag(self.compiler, '-fvisibility=hidden'):
                opts.append('-fvisibility=hidden')
        elif ct == 'msvc':
            opts.append('/DVERSION_INFO=\\"%s\\"' % self.distribution.get_version())
        for ext in self.extensions:
            ext.extra_compile_args = opts
            ext.extra_link_args = link_opts
        build_ext.build_extensions(self)
    
    def run(self):
        super().run()
        build_py = self.get_finalized_command('build_py')
        package_dir = f'{build_py.build_lib}/RAT/'
        for p in Path(package_dir).glob("**/*"):
            if p.suffix in {".exp", ".a", ".lib"}:
                p.unlink() 
                
        if self.inplace:
            obj_name = get_shared_object_name(libevent[0])
            src = f'{build_py.build_lib}/RAT/{obj_name}'
            dest = f'{build_py.get_package_dir("RAT")}/{obj_name}'
            build_py.copy_file(src, dest)


class BuildClib(build_clib):
    def initialize_options(self):
        super().initialize_options()
        build_py = self.get_finalized_command('build_py')
        self.build_clib = f'{build_py.build_lib}/RAT'

    def build_libraries(self, libraries):
        # bug in distutils: flag not valid for c++
        flag = '-Wstrict-prototypes'
        if (hasattr(self.compiler, 'compiler_so')
                and flag in self.compiler.compiler_so):
            self.compiler.compiler_so.remove(flag)

        compiler_type = self.compiler.compiler_type
        if compiler_type == 'msvc':
            compile_args = ['/EHsc', '/LD']
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
    name = 'RAT',
    version = __version__,
    author = '',
    author_email = '',
    url = 'https://github.com/RascalSoftware/python-RAT',
    description = 'Python extension for the Reflectivity Analysis Toolbox (RAT)',
    packages = ['RAT'],
    include_package_data = True,
    package_data = {'': [get_shared_object_name(libevent[0])]},
    cmdclass = {'build_clib': BuildClib, 'build_ext': BuildExt},
    libraries = [libevent],
    ext_modules = ext_modules,
    python_requires = '>=3.9',
    install_requires = ['numpy >= 1.20', 'prettytable >= 3.9.0', 'pydantic >= 2.4.2'],
    extras_require = {':python_version < "3.11"': ['StrEnum >= 0.4.15'],
                      'Dev': ['pytest>=7.4.0', 'pytest-cov>=4.1.0'],
                      'Matlab_latest': ['matlabengine'],
                      'Matlab_2023b': ['matlabengine == 23.2.1'],
                      'Matlab_2023a': ['matlabengine == 9.14.3'],
                      'Matlab-2022b': ['matlabengine == 9.13.9'],
                      'Matlab_2022a': ['matlabengine == 9.12.19'],
                      'Matlab_2021b': ['matlabengine == 9.11.21'],
                      'Matlab_2021a': ['matlabengine == 9.10.3']},
    zip_safe=False,
)
