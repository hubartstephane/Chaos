#! /usr/bin/python3
import os, sys, shutil

def MyClean(src):
  print ("MYCLEAN [%s]" % src)
  try:
    if (os.path.isfile(src)):
      os.remove(src)    
    else:    
      shutil.rmtree(src)
  except Exception as e:
    print ("Failure : %s" % (e))    

if (len(sys.argv) > 1): # argument 0 for the EXE name
  src = sys.argv[1]  
  MyClean(src)  
