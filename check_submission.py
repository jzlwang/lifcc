#!/usr/bin/python

import os
import argparse
import string
import sys
import re

from subprocess import call
from subprocess import check_output
from subprocess import check_call
from subprocess import CalledProcessError
from subprocess import Popen, PIPE


def run(command, *args, **keys):
    if args:
        command = command % args
    proc = Popen (command, shell=True, stdin=PIPE, stdout=PIPE, stderr=PIPE)
    out, err = proc.communicate(keys.get ('stdin', ''))
    return proc.returncode, out, err


def is_in_git():
	'''Returns true if the working directory is under version control by git.'''
	start = os.getcwd();
	while True:
		if os.path.isdir('.git'):
			os.chdir(start);
			return True
		elif (os.getcwd() == '/'):
			os.chdir(start);
			return False
		else:
			os.chdir("..");

parser = argparse.ArgumentParser(description='Sanity check project 1 submission.');
parser.add_argument('--tmp', dest='tmp_dir', metavar='TMP_PATH', default='tmp', 
					help='The path to a directory which will be used for temporary unpacking of' 
					+ ' data. This directory should not exist before this script is run, and'
					+ ' should be specified relative to the current working directory. Default is tmp.');
parser.add_argument('--account', dest='acct', metavar="ACCOUNT",
					help='The name of your account. Defaults to $USER.')

args = parser.parse_args()
subm = 'proj1';
tmp_dir = args.tmp_dir
fbf = 'proj1.marker'
acct = args.acct


#validate that tmp_dir is an available path.
if (os.path.exists(tmp_dir)):
    sys.exit(tmp_dir + ' already exists. You can specify a different temporary directory for me to use with the --tmp option.');
os.mkdir(tmp_dir);

#The student whose repository we're pulling
if not acct:
	stud = check_output("echo -n $USER", shell=True)[-2:]
else:
	stud = acct[-2:]

#Path to the student's repository
git_path = 'git@github.com:ucberkeley-cs61c/cs61c-%s.git' % stud

start_dir = os.getcwd();

os.chdir(tmp_dir);
#check that we're not about to get ourselves into some git-in-git troubles.
if is_in_git():
	os.chdir(start_dir);
	call(["rm", "-rf", tmp_dir]);
	sys.exit(tmp_dir + ' is already under version control. Pulling into it would result in git-in-git issues. You can specify a '
			 + 'different temporary directory for me to use with the --tmp option.');

out = None
err = None
code = 0

#pull the assignment
run("git init");
code, out, err = run("git pull %s %s", git_path, subm);
if code:
	print "Could not find the tag [%s] in the repository [%s]" % (subm, git_path)
	os.chdir(start_dir);
	call(["rm", "-rf", tmp_dir]);
	sys.exit();

#Search for the marker file.
dirs = check_output(["find . -name " + fbf + " -printf \'%h\\n\'"], shell=True).split()
if len(dirs) == 0:
	print 'Could not find ' + fbf + '. ' + fbf + ' should only exist in exactly place inside your repository.'
	sys.exit()
elif len(dirs) > 1:
	print 'Found ' + fbf + ' in multiple locations. ' + fbf + ' should only exist in one place inside your repository.'
#pull time of commit information.
os.chdir(dirs[0]);

try:
	time_stamp = check_output("git log -n 1 --format=fuller | grep CommitDate", shell=True);
	print "Submission date %s" % time_stamp[time_stamp.find(':'):]
	if os.path.exists("Makefile"):
		print "Running your test suite."
		print
		call("make check", shell=True);
		
	else:
		print "No Makefile found."
except CalledProcessError:
	print "No submission found."

os.chdir(start_dir)
call(["rm", "-rf", tmp_dir]);
