# XXX : why such a useless executable that make   xcopy only ?
#       because path in commands send from premake replace \ by / (or the inverse)
#       that makes trouble on windows

# XXX : the name of this executable is important
#       if it was called 'copy.py', 
#       the shutil.copy(...) function would call system SHELL function 'copy' 
#       that would be interpreted by bash as 'copy.py'
#       That why there this file is called MYcopy.py       

import os, sys, shutil

def MyCopy(src, dst):
  print ("MYCOPY [%s] = > [%s]" % (src, dst))
  try:
    # create copy if source is a FILE   
    if os.path.isfile(src):
      if os.path.isfile(dst):
        os.remove(dst)
      target_dir = os.path.dirname(dst)                            
      if not os.path.isdir(target_dir):
        os.makedirs(target_dir)                                
      shutil.copy(src, dst)
    # create copy if source is a DIRECTORY      
    elif os.path.isdir(src):
      files = os.listdir(src)
      for f in files:
        MyCopy(os.path.join(src, f), os.path.join(dst, f))      
    pass      
  except Exception as e:
    print ("Failure : %s" % (e))

if (len(sys.argv) > 2): # argument 0 for the EXE name
  src = sys.argv[1]  
  dst = sys.argv[2]
  MyCopy(src, dst)  
