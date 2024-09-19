#! /usr/bin/python3
# XXX : patchprojects.py is used to patch visual studio projects
#       replace 
#         <characterset>unicode</characterset>      
#       with 
#         <characterset>notset</characterset>

import os, sys, shutil,re
from tempfile import mkstemp
from shutil import move
from os import remove, close

def PatchFile(file_path, pattern, subst):
  # create temp file
  fh, abs_path = mkstemp()
  with open(abs_path, 'w') as new_file:
    with open(file_path) as old_file:
      for line in old_file:
        new_file.write(re.sub(pattern, subst, line, 0, re.IGNORECASE))      
  close(fh)
  # remove original file
  remove(file_path)
  # move new file
  move(abs_path, file_path)
    
def PatchProjects(target):
  try:
    if os.path.isdir(target):
      files = os.listdir(target)
      for f in files:
        PatchProjects(os.path.join(target, f)) 
    elif os.path.isfile(target):
      if re.match(".*\.vcxproj$", target, re.IGNORECASE):
        PatchFile(target, "<CharacterSet>Unicode</CharacterSet>", "<CharacterSet>NotSet</CharacterSet>")
        print("PatchFile [%s]" % target)    
      pass      
  except Exception as e:
    print ("Failure : %s" % (e))

if (len(sys.argv) > 1): # argument 0 for the EXE name
  target = sys.argv[1]  
  PatchProjects(target)  
