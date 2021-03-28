#!/usr/bin/env python2
#
#  Prepare a duk_config.h and combined/separate sources for compilation,
#  given user supplied config options, built-in metadata, Unicode tables, etc.
#
#  This is intended to be the main tool application build scripts would use
#  before their build step, so convenient, versions, Python compatibility,
#  etc all matter.
#
#  When obsoleting options, leave the option definitions behind (with
#  help=optparse.SUPPRESS_HELP) and give useful suggestions when obsolete
#  options are used.  This makes it easier for users to fix their build
#  scripts.
#

import logging
import sys
logging.basicConfig(level=logging.INFO, stream=sys.stdout, format='%(name)-21s %(levelname)-7s %(message)s')
logger = logging.getLogger('configure.py')
logger.setLevel(logging.INFO)

import os
import re
import shutil
import glob
import optparse
import tarfile
import json
import tempfile
import subprocess
import atexit

def import_warning(module, aptPackage, pipPackage):
    sys.stderr.write('\n')
    sys.stderr.write('*** NOTE: Could not "import %s".  Install it using e.g.:\n' % module)
    sys.stderr.write('\n')
    sys.stderr.write('    # Linux\n')
    sys.stderr.write('    $ sudo apt-get install %s\n' % aptPackage)
    sys.stderr.write('\n')
    sys.stderr.write('    # Windows\n')
    sys.stderr.write('    > pip install %s\n' % pipPackage)

try:
    import yaml
except ImportError:
    import_warning('yaml', 'python-yaml', 'PyYAML')
    sys.exit(1)

import genconfig

# Helpers

def exec_get_stdout(cmd, input=None, default=None, print_stdout=False):
    try:
        proc = subprocess.Popen(cmd, stdin=subprocess.PIPE, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
        ret = proc.communicate(input=input)
        if print_stdout:
            sys.stdout.write(ret[0])
            sys.stdout.flush()
        if proc.returncode != 0:
            sys.stdout.write(ret[1])  # print stderr on error
            sys.stdout.flush()
            if default is not None:
                logger.info('WARNING: command %r failed, return default' % cmd)
                return default
            raise Exception('command failed, return code %d: %r' % (proc.returncode, cmd))
        return ret[0]
    except:
        if default is not None:
            logger.info('WARNING: command %r failed, return default' % cmd)
            return default
        raise

def exec_print_stdout(cmd, input=None):
    ret = exec_get_stdout(cmd, input=input, print_stdout=True)

def mkdir(path):
    os.mkdir(path)

def copy_file(src, dst):
    with open(src, 'rb') as f_in:
        with open(dst, 'wb') as f_out:
            f_out.write(f_in.read())

def copy_files(filelist, srcdir, dstdir):
    for i in filelist:
        copy_file(os.path.join(srcdir, i), os.path.join(dstdir, i))

def copy_and_replace(src, dst, rules):
    # Read and write separately to allow in-place replacement
    keys = sorted(rules.keys())
    res = []
    with open(src, 'rb') as f_in:
        for line in f_in:
            for k in keys:
                line = line.replace(k, rules[k])
            res.append(line)
    with open(dst, 'wb') as f_out:
        f_out.write(''.join(res))

def copy_and_cquote(src, dst):
    with open(src, 'rb') as f_in:
        with open(dst, 'wb') as f_out:
            f_out.write('/*\n')
            for line in f_in:
                line = line.decode('utf-8')
                f_out.write(' *  ')
                for c in line:
                    if (ord(c) >= 0x20 and ord(c) <= 0x7e) or (c in '\x0a'):
                        f_out.write(c.encode('ascii'))
                    else:
                        f_out.write('\\u%04x' % ord(c))
            f_out.write(' */\n')

def read_file(src, strip_last_nl=False):
    with open(src, 'rb') as f:
        data = f.read()
        if len(data) > 0 and data[-1] == '\n':
            data = data[:-1]
        return data

def delete_matching_files(dirpath, cb):
    for fn in os.listdir(dirpath):
        if os.path.isfile(os.path.join(dirpath, fn)) and cb(fn):
            logger.debug('Deleting %r' % os.path.join(dirpath, fn))
            os.unlink(os.path.join(dirpath, fn))

def create_targz(dstfile, filelist):
    # https://docs.python.org/2/library/tarfile.html#examples

    def _add(tf, fn):  # recursive add
        logger.debug('Adding to tar: ' + fn)
        if os.path.isdir(fn):
            for i in sorted(os.listdir(fn)):
                _add(tf, os.path.join(fn, i))
        elif os.path.isfile(fn):
            tf.add(fn)
        else:
            raise Exception('invalid file: %r' % fn)

    with tarfile.open(dstfile, 'w:gz') as tf:
        for fn in filelist:
            _add(tf, fn)

def cstring(x):
    return '"' + x + '"'  # good enough for now

# DUK_VERSION is grepped from duktape.h.in: it is needed for the
# public API and we want to avoid defining it in two places.
def get_duk_version(apiheader_filename):
    r = re.compile(r'^#define\s+FIN_VERSION\s+(.*?)L?\s*$')
    with open(apiheader_filename, 'rb') as f:
        for line in f:
            m = r.match(line)
            if m is not None:
                fin_version = int(m.group(1))
                fin_major = fin_version / 10000
                fin_minor = (fin_version % 10000) / 100
                fin_patch = fin_version % 100
                fin_version_formatted = '%d.%d.%d' % (fin_major, fin_minor, fin_patch)
                return fin_version, fin_major, fin_minor, fin_patch, fin_version_formatted

    raise Exception('cannot figure out singlefin version')

# Option parsing

def main():
    parser = optparse.OptionParser(
        usage='Usage: %prog [options]',
        description='Prepare Duktape source files and a duk_config.h configuration header for compilation. ' + \
                    'Source files can be combined (amalgamated) or kept separate. ' + \
                    'See http://wiki.duktape.org/Configuring.html for examples.'
    )

    # Forced options from multiple sources are gathered into a shared list
    # so that the override order remains the same as on the command line.
    force_options_yaml = []
    def add_force_option_yaml(option, opt, value, parser):
        # XXX: check that YAML parses
        force_options_yaml.append(value)
    def add_force_option_file(option, opt, value, parser):
        # XXX: check that YAML parses
        with open(value, 'rb') as f:
            force_options_yaml.append(f.read())
    def add_force_option_define(option, opt, value, parser):
        tmp = value.split('=')
        if len(tmp) == 1:
            doc = { tmp[0]: True }
        elif len(tmp) == 2:
            doc = { tmp[0]: tmp[1] }
        else:
            raise Exception('invalid option value: %r' % value)
        force_options_yaml.append(yaml.safe_dump(doc))
    def add_force_option_undefine(option, opt, value, parser):
        tmp = value.split('=')
        if len(tmp) == 1:
            doc = { tmp[0]: False }
        else:
            raise Exception('invalid option value: %r' % value)
        force_options_yaml.append(yaml.safe_dump(doc))

    fixup_header_lines = []
    def add_fixup_header_line(option, opt, value, parser):
        fixup_header_lines.append(value)
    def add_fixup_header_file(option, opt, value, parser):
        with open(value, 'rb') as f:
            for line in f:
                if line[-1] == '\n':
                    line = line[:-1]
                fixup_header_lines.append(line)

    # Options for configure.py tool itself.
    parser.add_option('--source-directory', dest='source_directory', default=None, help='Directory with raw input sources (defaulted based on configure.py script path)')
    parser.add_option('--output-directory', dest='output_directory', default=None, help='Directory for output files (created automatically if it doesn\'t exist, reused if safe)')
    parser.add_option('--license-file', dest='license_file', default=None, help='Source for LICENSE.txt (defaulted based on configure.py script path)')
    parser.add_option('--authors-file', dest='authors_file', default=None, help='Source for AUTHORS.rst (defaulted based on configure.py script path)')
    parser.add_option('--git-commit', dest='git_commit', default=None, help='Force git commit hash')
    parser.add_option('--git-describe', dest='git_describe', default=None, help='Force git describe')
    parser.add_option('--git-branch', dest='git_branch', default=None, help='Force git branch name')
    parser.add_option('--fin-dist-meta', dest='fin_dist_meta', default=None, help='fin_dist_meta.json to read git commit etc info from')
    parser.add_option('--module', dest='module', default=None, help='Module to generate code for')

    # Options for combining sources.
    parser.add_option('--separate-sources', dest='separate_sources', action='store_true', default=False, help='Output separate sources instead of combined source (default is combined)')
    parser.add_option('--line-directives', dest='line_directives', action='store_true', default=False, help='Output #line directives in combined source (default is false)')

    # Options forwarded to genconfig.py.
    genconfig.add_genconfig_optparse_options(parser)

    # Log level options.
    parser.add_option('--quiet', dest='quiet', action='store_true', default=False, help='Suppress info messages (show warnings)')
    parser.add_option('--verbose', dest='verbose', action='store_true', default=False, help='Show verbose debug messages')

    (opts, args) = parser.parse_args()
    if len(args) > 0:
        raise Exception('unexpected arguments: %r' % args)

    #if opts.obsolete_builtin_metadata is not None:
    #    raise Exception('--user-builtin-metadata has been removed, use --builtin-file instead')

    # Log level.
    forward_loglevel = []
    if opts.quiet:
        logger.setLevel(logging.WARNING)
        forward_loglevel = [ '--quiet' ]
    elif opts.verbose:
        logger.setLevel(logging.DEBUG)
        forward_loglevel = [ '--verbose' ]

    # Figure out directories, git info, etc

    entry_cwd = os.getcwd()
    script_path = sys.path[0]  # http://stackoverflow.com/questions/4934806/how-can-i-find-scripts-directory-with-python

    def default_from_script_path(optname, orig, alternatives):
        if orig is not None:
            orig = os.path.abspath(orig)
            if os.path.exists(orig):
                logger.debug(optname + ' ' + orig)
                return orig
            else:
                raise Exception('invalid argument to ' + optname)
        for alt in alternatives:
            cand = os.path.abspath(os.path.join(script_path, '..', alt))
            if os.path.exists(cand):
                logger.debug('default ' + optname + ' to ' + cand)
                return cand
        raise Exception('no ' + optname + ' and cannot default based on script path')

    if opts.output_directory is None:
        raise Exception('missing --output-directory')
    opts.output_directory = os.path.abspath(opts.output_directory)
    outdir = opts.output_directory

    opts.source_directory = default_from_script_path('--source-directory', opts.source_directory, [ 'src-input' ])
    srcdir = opts.source_directory

    opts.config_metadata = default_from_script_path('--config-metadata', opts.config_metadata, [ 'config' ])

    #opts.license_file = default_from_script_path('--license-file', opts.license_file, [ 'LICENSE.txt' ])
    #license_file = opts.license_file

    #opts.authors_file = default_from_script_path('--authors-file', opts.authors_file, [ 'AUTHORS.rst' ])
    #authors_file = opts.authors_file

    fin_dist_meta = None
    if opts.fin_dist_meta is not None:
        with open(opts.fin_dist_meta, 'rb') as f:
            fin_dist_meta = json.loads(f.read())

    fin_version, fin_major, fin_minor, fin_patch, fin_version_formatted = \
        get_duk_version(os.path.join(srcdir, 'singlefin.h.in'))

    git_commit = None
    git_branch = None
    git_describe = None

    if fin_dist_meta is not None:
        git_commit = fin_dist_meta['git_commit']
        git_branch = fin_dist_meta['git_branch']
        git_describe = fin_dist_meta['git_describe']

    if opts.git_commit is not None:
        git_commit = opts.git_commit
    if opts.git_describe is not None:
        git_describe = opts.git_describe
    if opts.git_branch is not None:
        git_branch = opts.git_branch

    if git_commit is None:
        logger.debug('Git commit not specified, autodetect from current directory')
        git_commit = exec_get_stdout([ 'git', 'rev-parse', 'HEAD' ], default='external').strip()
    if git_describe is None:
        logger.debug('Git describe not specified, autodetect from current directory')
        git_describe = exec_get_stdout([ 'git', 'describe', '--always', '--dirty' ], default='external').strip()
    if git_branch is None:
        logger.debug('Git branch not specified, autodetect from current directory')
        git_branch = exec_get_stdout([ 'git', 'rev-parse', '--abbrev-ref', 'HEAD' ], default='external').strip()

    git_commit = str(git_commit)
    git_describe = str(git_describe)
    git_branch = str(git_branch)

    git_commit_cstring = cstring(git_commit)
    git_describe_cstring = cstring(git_describe)
    git_branch_cstring = cstring(git_branch)


    logger.info('Configuring Duktape version %s, commit %s, describe %s, branch %s' % \
                (fin_version_formatted, git_commit, git_describe, git_branch))
    logger.info('  - source input directory: ' + opts.source_directory)
    #logger.info('  - license file: ' + opts.license_file)
    #logger.info('  - authors file: ' + opts.authors_file)
    logger.info('  - config metadata directory: ' + opts.config_metadata)
    logger.info('  - output directory: ' + opts.output_directory)

    # Create output directory.  If the directory already exists, reuse it but
    # only when it's safe to do so, i.e. it contains only known output files.
    allow_outdir_reuse = True
    outdir_whitelist = [ 'fin_config.h', 'singlefin.c', 'singlefin.h', 'fin_source_meta.json' ]
    if os.path.exists(outdir):
        if not allow_outdir_reuse:
            raise Exception('configure target directory %s already exists, please delete it first' % repr(outdir))
        for fn in os.listdir(outdir):
            if fn == '.' or fn == '..' or (fn in outdir_whitelist and os.path.isfile(os.path.join(outdir, fn))):
                continue
            else:
                raise Exception('configure target directory %s already exists, cannot reuse because it contains unknown files such as %s' % (repr(outdir), repr(fn)))
        logger.info('Reusing output directory (already exists but contains only safe, known files)')
        for fn in outdir_whitelist:
            if os.path.isfile(os.path.join(outdir, fn)):
                os.unlink(os.path.join(outdir, fn))
    else:
        logger.debug('Output directory doesn\'t exist, create it')
        os.mkdir(outdir)

    # Temporary directory.
    tempdir = tempfile.mkdtemp(prefix='tmp-fin-prepare-')
    atexit.register(shutil.rmtree, tempdir)
    mkdir(os.path.join(tempdir, 'src'))
    logger.debug('Using temporary directory %r' % tempdir)

    # Separate sources are mostly copied as is at present.
    copy_files([
        'fin_internal.h',
        'fin_adc.h',
        'fin_cli.c',
        'fin_cli.h',
        'fin_critical.h',
        'fin_debug.h',
        'fin_debug_opt.h',
        'fin_dss.c',
        'fin_dss.h',
        'fin_file.c',
        'fin_gpio.h',
        'fin_http_client.c',
        'fin_http_client.h',
        'fin_hwrandom.c',
        'fin_hwrandom.h',
        'fin_i2c.h',
        'fin_list.h',
        'fin_malloc.c',
        'fin_net.c',
        'fin_net.h',
        'printf.c',
        'fin_retarget.c',
        'fin_sleep.h',
        'fin_spi.h',
        'fin_strings.c',
        'fin_stubs.c',
        'fin_time.c',
        'fin_uart.h' 
    ], srcdir, os.path.join(tempdir, 'src'))

    if opts.module:
        copy_files([
            'fin_adc.c',
            'fin_gpio.c',
            'fin_i2c.c',
            'fin_spi.c',
            'fin_uart.c',
        ], srcdir + '/target/'+ opts.module , os.path.join(tempdir, 'src' ))

    # Build temp versions of LICENSE.txt and AUTHORS.rst for embedding into
    # autogenerated C/H files.

    #copy_and_cquote(license_file, os.path.join(tempdir, 'LICENSE.txt.tmp'))
    #copy_and_cquote(authors_file, os.path.join(tempdir, 'AUTHORS.rst.tmp'))


    # Create a duk_config.h.
    # XXX: might be easier to invoke genconfig directly, but there are a few
    # options which currently conflict (output file, git commit info, etc).
    def forward_genconfig_options():
        res = []
        res += [ '--metadata', os.path.abspath(opts.config_metadata) ]  # rename option, --config-metadata => --metadata
        # if opts.platform is not None:
        #     res += [ '--platform', opts.platform ]
        # if opts.compiler is not None:
        #     res += [ '--compiler', opts.compiler ]
        # if opts.architecture is not None:
        #     res += [ '--architecture', opts.architecture ]
        # if opts.c99_types_only:
        #     res += [ '--c99-types-only' ]
        # if opts.dll:
        #     res += [ '--dll' ]
        # if opts.support_feature_options:
        #     res += [ '--support-feature-options' ]
        # if opts.emit_legacy_feature_check:
        #     res += [ '--emit-legacy-feature-check' ]
        # if opts.emit_config_sanity_check:
        #     res += [ '--emit-config-sanity-check' ]
        # if opts.omit_removed_config_options:
        #     res += [ '--omit-removed-config-options' ]
        # if opts.omit_deprecated_config_options:
        #     res += [ '--omit-deprecated-config-options' ]
        # if opts.omit_unused_config_options:
        #     res += [ '--omit-unused-config-options' ]
        # if opts.add_active_defines_macro:
        #     res += [ '--add-active-defines-macro' ]
        # if len(opts.force_options_yaml) > 0:
        #     # Use temporary files so that large option sets don't create
        #     # excessively large commands.
        #     for idx,i in enumerate(opts.force_options_yaml):
        #         tmpfn = os.path.join(tempdir, 'genconfig%d.yaml' % idx)
        #         with open(tmpfn, 'wb') as f:
        #             f.write(i)
        #         with open(tmpfn, 'rb') as f:
        #             logger.debug(f.read())
        #         res += [ '--option-file', tmpfn ]
        # for i in opts.fixup_header_lines:
        #     res += [ '--fixup-line', i ]
        # if not opts.sanity_strict:
        #     res += [ '--sanity-warning' ]
        # if opts.use_cpp_warning:
        #     res += [ '--use-cpp-warning' ]
        return res

    cmd = [
        sys.executable, os.path.join(script_path, 'genconfig.py'),
        '--output', os.path.join(tempdir, 'fin_config.h.tmp'),
        #'--output-active-options', os.path.join(tempdir, 'fin_config_active_options.json'),
        #'--git-commit', git_commit, '--git-describe', git_describe, '--git-branch', git_branch,
        #'--used-stridx-metadata', os.path.join(tempdir, 'fin_used_stridx_bidx_defs.json.tmp')
    ]
    cmd += forward_genconfig_options()
    cmd += [
        'duk-config-header'
    ] + forward_loglevel
    logger.debug(repr(cmd))
    exec_print_stdout(cmd)

    copy_file(os.path.join(tempdir, 'fin_config.h.tmp'), os.path.join(outdir, 'fin_config.h'))

    # Build duktape.h from parts, with some git-related replacements.
    # The only difference between single and separate file duktape.h
    # is the internal DUK_SINGLE_FILE define.
    #
    # Newline after 'i \':
    # http://stackoverflow.com/questions/25631989/sed-insert-line-command-osx

    copy_and_replace(os.path.join(srcdir, 'singlefin.h.in'), os.path.join(tempdir, 'singlefin.h'), {
        '@FIN_SINGLE_FILE@': '#define FIN_SINGLE_FILE',
   #     '@LICENSE_TXT@': read_file(os.path.join(tempdir, 'LICENSE.txt.tmp'), strip_last_nl=True),
   #     '@AUTHORS_RST@': read_file(os.path.join(tempdir, 'AUTHORS.rst.tmp'), strip_last_nl=True),
        '@FIN_VERSION_FORMATTED@': fin_version_formatted,
        '@GIT_COMMIT@': git_commit,
        '@GIT_COMMIT_CSTRING@': git_commit_cstring,
        '@GIT_DESCRIBE@': git_describe,
        '@GIT_DESCRIBE_CSTRING@': git_describe_cstring,
        '@GIT_BRANCH@': git_branch,
        '@GIT_BRANCH_CSTRING@': git_branch_cstring
    })

    if opts.separate_sources:
        # keep the line so line numbers match between the two variant headers
        copy_and_replace(os.path.join(tempdir, 'singlefin.h'), os.path.join(outdir, 'singlefin.h'), {
            '#define DUK_SINGLE_FILE': '#undef FIN_SINGLE_FILE'
        })
    else:
        copy_file(os.path.join(tempdir, 'singlefin.h'), os.path.join(outdir, 'singlefin.h'))

    
    # Create a combined source file, duktape.c, into a separate combined source
    # directory.  This allows user to just include "duktape.c", "duktape.h", and
    # "duk_config.h" into a project and maximizes inlining and size optimization
    # opportunities even with older compilers.  Because some projects include
    # these files into their repository, the result should be deterministic and
    # diffable.  Also, it must retain __FILE__/__LINE__ behavior through
    # preprocessor directives.  Whitespace and comments can be stripped as long
    # as the other requirements are met.  For some users it's preferable *not*
    # to use #line directives in the combined source, so a separate variant is
    # created for that, see: https://github.com/svaarala/duktape/pull/363.

    def create_source_prologue(license_file, authors_file):
        res = []

        # Because duktape.c/duktape.h/duk_config.h are often distributed or
        # included in project sources as is, add a license reminder and
        # Duktape version information to the duktape.c header (duktape.h
        # already contains them).

        fin_major = fin_version / 10000
        fin_minor = fin_version / 100 % 100
        fin_patch = fin_version % 100
        res.append('/*')
        res.append(' *  Single source autogenerated distributable for Duktape %d.%d.%d.' % (fin_major, fin_minor, fin_patch))
        res.append(' *')
        res.append(' *  Git commit %s (%s).' % (git_commit, git_describe))
        res.append(' *  Git branch %s.' % git_branch)
        res.append(' *')
        res.append(' *  See Duktape AUTHORS.rst and LICENSE.txt for copyright and')
        res.append(' *  licensing information.')
        res.append(' */')
        res.append('')

        # Add LICENSE.txt and AUTHORS.rst to combined source so that they're automatically
        # included and are up-to-date.

        res.append('/* LICENSE.txt */')
        if os.path.exists(license_file):
            with open(license_file, 'rb') as f:
                for line in f:
                    res.append(line.strip())
            res.append('')
            res.append('/* AUTHORS.rst */')
        if os.path.exists(authors_file):
            with open(authors_file, 'rb') as f:
                for line in f:
                    res.append(line.strip())

        return '\n'.join(res) + '\n'

    def select_combined_sources():
        # These files must appear before the alphabetically sorted
        # ones so that static variables get defined before they're
        # used.  We can't forward declare them because that would
        # cause C++ issues (see GH-63).  When changing, verify by
        # compiling with g++.
        handpick = [
            'fin_malloc.c',
            'fin_retarget.c',
            'fin_file.c'
        ]

        files = []
        for fn in handpick:
            files.append(fn)

        for fn in sorted(os.listdir(os.path.join(tempdir, 'src'))):
            f_ext = os.path.splitext(fn)[1]
            if f_ext not in [ '.c' ]:
                continue
            if fn in files:
                continue
            files.append(fn)

        res = map(lambda x: os.path.join(tempdir, 'src', x), files)
        logger.debug(repr(files))
        logger.debug(repr(res))
        return res

    if opts.separate_sources:
        for fn in os.listdir(os.path.join(tempdir, 'src')):
            copy_file(os.path.join(tempdir, 'src', fn), os.path.join(outdir, fn))
    else:
        with open(os.path.join(tempdir, 'prologue.tmp'), 'wb') as f:
            f.write(create_source_prologue(os.path.join(tempdir, 'LICENSE.txt.tmp'), os.path.join(tempdir, 'AUTHORS.rst.tmp')))

        cmd = [
            sys.executable,
            os.path.join(script_path, 'combine_src.py'),
            '--include-path', os.path.join(tempdir, 'src'),
            '--include-exclude', 'fin_config.h',  # don't inline
            '--include-exclude', 'singlefin.h',     # don't inline
            '--prologue', os.path.join(tempdir, 'prologue.tmp'),
            '--output-source', os.path.join(outdir, 'singlefin.c'),
            '--output-metadata', os.path.join(tempdir, 'combine_src_metadata.json')
        ]
        if opts.line_directives:
            cmd += [ '--line-directives' ]
        cmd += select_combined_sources()
        cmd += forward_loglevel
        exec_print_stdout(cmd)

    # Merge metadata files.

    doc = {
        'type': 'fin_source_meta',
        'comment': 'Metadata for prepared Duktape sources and configuration',
        'git_commit': git_commit,
        'git_branch': git_branch,
        'git_describe': git_describe,
        'fin_version': fin_version,
        'fin_version_string': fin_version_formatted
    }

    with open(os.path.join(tempdir, 'combine_src_metadata.json'), 'rb') as f:
        tmp = json.loads(f.read())
        for k in tmp.keys():
            doc[k] = tmp[k]

    with open(os.path.join(outdir, 'fin_source_meta.json'), 'wb') as f:
        f.write(json.dumps(doc, indent=4))

    logger.debug('Configure finished successfully')

if __name__ == '__main__':
    main()
