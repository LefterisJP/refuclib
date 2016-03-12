#include <System/rf_dlib.h>
#include <String/rf_str_core.h>
#include <String/rf_str_conversion.h>
#include <Utils/memory.h>

#include <dlfcn.h>

struct rf_dlib {
    struct RFstring name;
    void *hndl;
};

struct rf_dlib *rf_dlib_create(const struct RFstring *path, const struct RFstring *name)
{
    struct rf_dlib *ret;
    RF_MALLOC(ret, sizeof(*ret), return NULL);
    if (!rf_string_copy_in(&ret->name, name)) {
        RF_ERROR("Failed to initialize dynamic library's name string");
        return NULL;
    }
    RFS_PUSH();
    struct RFstring *tmpname;
    if (path) {
        tmpname = RFS(RFS_PF "/" RFS_PF".so", RFS_PA(path), RFS_PA(name));
    } else {
        tmpname = RFS(RFS_PF".so", RFS_PA(name));
    }
    ret->hndl = dlopen(rf_string_cstr_from_buff_or_die(tmpname), RTLD_LAZY);
    RFS_POP();

    if (!ret->hndl) {
        RF_ERROR(
            "Failed to open dynamic library \""RFS_PF"\".\n%s",
            RFS_PA(name),
            dlerror()
        );
        rf_string_deinit(&ret->name);
        free(ret);
        ret = NULL;
    }
    return ret;
}

bool rf_dlib_destroy(struct rf_dlib *dl)
{
    bool ret = true;
    if (0 != dlclose(dl->hndl)) {
        RF_ERROR("dlclose() failed on library "RFS_PF".\n%s",
                 RFS_PA(&dl->name), dlerror());
        ret = false;
    }
    rf_string_deinit(&dl->name);
    free(dl);
    return ret;
}

void *rf_dlib_symbol(struct rf_dlib *dl, const struct RFstring *name)
{
    dlerror(); // clear last error
    RFS_PUSH();
    void *sym = dlsym(dl->hndl, rf_string_cstr_from_buff_or_die(name));
    RFS_POP();

    if (!sym) {
        char *err = dlerror();
        if (!err) {
            RF_ERROR("No symbol named \""RFS_PF"\" could be found in "
                     "library "RFS_PF".",
                     RFS_PA(name),
                     RFS_PA(&dl->name));
        } else {
            RF_ERROR("Failed to to load symbol \""RFS_PF"\" from library "
                     RFS_PF".\n%s",
                     RFS_PA(name),
                     RFS_PA(&dl->name),
                     err);
        }
    }
    return sym;
}
