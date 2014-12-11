#!/usr/bin/env python

# Test compiler against suite of LIFC programs.

import os, sys, re
from subprocess import Popen, PIPE
from getopt import getopt, GetoptError

def Usage():
    print '''
Usage: test_proj1.py [ --errors ] [ --keep ] [ --compiler=COMPILER ] DIR/BASE.lc ...
    Runs COMPILER (default ./lifcc) on each DIR/BASE.lc

    In the absence of the --errors option runs MARS on the output of COMPILER.
    Compares the output to file DIR/BASE.std, if that is present, and otherwise
    just checks that compiler exits normally.

    With the --errors option, checks that the compiler exits with a
    non-zero exit code and that stderr contains at least one error message.

    With the --keep option, intermediate files are kept after execution. Else
    they are discarded when the script finishes.

    Reports results and exits with 0 if there are no errors, and
    with 1 otherwise.'''

def Run(command, *args, **keys):
    if args:
        command = command % args
    proc = Popen (command, shell=True, stdin=PIPE, stdout=PIPE, stderr=PIPE)
    out, err = proc.communicate(keys.get ('stdin', ''))
    return proc.returncode, out, err

def Remove(file):
    try:
        os.unlink(file)
    except OSError:
        pass

def Contents(file):
    try: 
        f = open(file)
        r = f.read()
        f.close()
        return r
    except:
        return ""

def Compile(prog, dst):
    global Stdout, Stderr
    code, Stdout, Stderr = Run("%s %s > %s", compiler, prog, dst)
    return code == 0

def Memcheck(prog):
    global Stdout, Stderr
    code, Stdout, Stderr = Run("valgrind --tool=memcheck --leak-check=full --track-origins=yes -q %s %s", compiler, prog)
    return code == 0

def Execute(prog, inp):
    global Stdout, Stderr
    code, Stdout, Stderr = Run("mars %s | tail -n +3", prog, stdin=inp)
    return code == 0

def HasError(errout):
    return re.search(r'.+', errout)

try:
    opts, tests = getopt (sys.argv[1:], 'h',
                          ['help', 'keep', 'errors', 'compiler='])
except GetoptError:
    Usage()
    sys.exit(1)

compiler = './lifcc'
errors = False
keep = False

for opt, val in opts:
    if opt in ( '-h', '--help' ):
        Usage()
        sys.exit(0)
    elif opt == '--errors':
        errors = True
    elif opt == '--compiler':
        compiler = val
    elif opt == '--keep':
        keep = True

N = 0
OK = 0

for f in tests:
    N += 1
    dir, file = os.path.split(f)
    base, ext = os.path.splitext(file)
    print base + ".lc:",

    dst = os.path.join(dir, base + ".s")
    inp = os.path.join(dir, base + ".in")
    if errors:
        if Compile(f, dst):
            msg = "FAIL (wrong exit code)"
        elif HasError (Stderr):
            msg = "OK"
        else:
            msg = "FAIL (no error message)"
    else:
        std = Contents (os.path.join(dir, base + ".std"))
        if not Compile(f, dst):
            msg = "FAIL (wrong exit code)"
        elif Stderr:
            msg = "FAIL (error messages)"
        elif not Execute(dst, Contents(inp)):
            msg = "FAIL (execution error)"
        elif std and std != Stdout:
            msg = "FAIL (wrong output)"
        elif not Memcheck(f):
            msg = "FAIL (memory leaked)"
        else:
            msg = "OK"
    if not keep:
        Remove(dst)

    if msg == "OK":
        OK += 1
    print msg

print
print "Ran %d tests." % N,
if OK == N:
    print "All passed."
    sys.exit(0)
else:
    print "%d failed." % (N - OK)
    sys.exit(1)

