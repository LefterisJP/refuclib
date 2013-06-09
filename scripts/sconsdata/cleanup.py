import os

types_list = ["I", "UI"]


def search_file_type(main_list, root, type_d):
    l1 = [os.path.join(dirpath, f)
          for dirpath, dirnames, files in os.walk(
                  os.path.join(root, "include")
          )
          for f in files if f.endswith(type_d+".h")]    
    l2 = [os.path.join(dirpath, f)
          for dirpath, dirnames, files in os.walk(
                  os.path.join(root, "include")
          )
          for f in files if f.endswith(type_d+"_decl.h")]   
    l3 = [os.path.join(dirpath, f)
          for dirpath, dirnames, files in os.walk(
                  os.path.join(root, "src")
          )
          for f in files if f.endswith(type_d+".c")]
    l = l1 + l2 + l3
    main_list.extend(l)

def clean_generated_files(root):
    files_list = []
    #find all the generated files
    for type_d in types_list:
        search_file_type(files_list, root, type_d)
    #also find all include files that end with _extra.h
    l = [os.path.join(dirpath, f)
          for dirpath, dirnames, files in os.walk(
                  os.path.join(root, "include")
          )
          for f in files if f.endswith("_extra.h")]   
    files_list.extend(l)
    #and now delete them
    for f in files_list:
        print("Deleting generated file \"{}\"".format(f))
        os.remove(f)
