#! /usr/bin/python3.5

# A mydoxygen.conf file can be created with the command :
#
#  doxygen -g mydoxygen.conf
#
# You may then alter manually it 

import os, sys, shutil, string, subprocess


import msvcrt

if (len(sys.argv) <= 3): # argument 0 for the EXE name
  print("Usage : mydoxygen.py [SOURCE_DIR] [DST_DIR] [PROJ_NAME]")
else:
  src                  = sys.argv[1]  
  dst                  = sys.argv[2]
  project_name         = sys.argv[3]  
  general_script_path  = os.path.join(os.path.dirname(__file__), "mydoxygen.conf")
  special_script_path  = os.path.join(src, "doxygen", "mydoxygen.conf")
  
  print ("MYDOXYGEN [%s] = > [%s]" % (src, dst))
  
  try:
    gen_fp       = open(general_script_path)
    content      = gen_fp.read()
    
    if (os.path.isfile(special_script_path)): # test special conf file in "doxygen" directory
      spe_fp  = open(special_script_path)
      content = content + spe_fp.read()
      
      print(content)          

    content      = content.replace('$INPUT', src).replace('$OUTPUT', dst).replace('$PROJECT_NAME', project_name)
    doxygen_path = os.path.join(os.path.dirname(__file__), "doxygen.exe") 

    p = subprocess.Popen(doxygen_path + " -", stdin = subprocess.PIPE)
    p.communicate(content.encode('utf-8'))
                    
  except Exception as e:
  
    print("Exception : %s" % e, file = sys.stderr)  
    pass
