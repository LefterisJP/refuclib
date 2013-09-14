import os

from code_gen import json_file_name


def search_file_type(main_list, root, type_d):
    l1 = [os.path.join(dirpath, f)
          for dirpath, dirnames, files in os.walk(
                  os.path.join(root, "include")
          )
          for f in files if f.endswith("_" + type_d + ".h")]
    l2 = [os.path.join(dirpath, f)
          for dirpath, dirnames, files in os.walk(
                  os.path.join(root, "include")
          )
          for f in files if f.endswith("_" + type_d + "_decl.h")]
    l3 = [os.path.join(dirpath, f)
          for dirpath, dirnames, files in os.walk(
                  os.path.join(root, "src")
          )
          for f in files if f.endswith("_" + type_d+".c")]
    l = l1 + l2 + l3
    main_list.extend(l)


def clean_generated_files(root, c):
    """
       Is called when "-c" option is passed to SCons.
       Cleans all the generated files.
       root -- The root directory of the library
       c - A codegen object, containing the type dictionary
    """
    # TODO: Find a way to delete the specializations for objects
    # outside of the library
    files_list = []
    #find all the generated files
    for type_d in c.type_dict:
        search_file_type(files_list, root, type_d)
    #also find all include files that end with _extra.h
    l = [os.path.join(dirpath, f)
          for dirpath, dirnames, files in os.walk(
                  os.path.join(root, "include")
          )
          for f in files if f.endswith("_extra.h")]
    files_list.extend(l)
    #also delete all .c files under the Generated directory
    l = [os.path.join(dirpath, f)
          for dirpath, dirnames, files in os.walk(
                  os.path.join(root, "src", "Generated")
          )
          for f in files if f.endswith(".c")]
    files_list.extend(l)
    #also delete the temporary json data (if existing)
    json_path = os.path.join(root, json_file_name)
    if os.path.isfile(json_path):
        files_list.append(json_path)
    #and now delete them
    for f in files_list:
        print("Deleting generated file \"{}\"".format(f))
        os.remove(f)
