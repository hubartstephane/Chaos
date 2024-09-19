#! /usr/bin/python3
# beware : zipfile.write(...) insert a file in the archive with the full
#          path given => that why we use a os.chdir() to avoid useless
#          hierarchy node

import os, sys, zipfile
from os import walk

if (len(sys.argv) > 2): # argument 0 for the EXE name
	src = sys.argv[1]
	dst = sys.argv[2]
	print ("MYZIP [%s] = > [%s]" % (src, dst))


old_dir = os.getcwd()

try:
	dirname = os.path.dirname(dst)
	os.makedirs(dirname, exist_ok = True)

	with zipfile.ZipFile(dst, 'w') as myzip:
		if os.path.isfile(src):
			d = os.path.dirname(src)
			os.chdir(d)
			src = os.path.basename(src)
			myzip.write(src)
			print ("Insert file : %s" % src)
		elif os.path.isdir(src):
			os.chdir(src)
			src = "."
			for root, dirs, files in os.walk(src):
				for n in files:
					myzip.write(os.path.join(root, n))
					print ("Insert file : %s" % os.path.join(root, n))
except Exception as e:
	print ("Failure : %s" % (e), file=sys.stderr)

	os.chdir(old_dir)
