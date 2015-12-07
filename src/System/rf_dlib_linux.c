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
        tmpname = RFS(RF_STR_PF_FMT "/" RF_STR_PF_FMT".so",
                      RF_STR_PF_ARG(path),
                      RF_STR_PF_ARG(name));
    } else {
        tmpname = RFS(RF_STR_PF_FMT".so", RF_STR_PF_ARG(name));
    }
    ret->hndl = dlopen(rf_string_cstr_from_buff_or_die(tmpname), RTLD_LAZY);
    RFS_POP();

    if (!ret->hndl) {
        RF_ERROR("Failed to open dynamic library \""RF_STR_PF_FMT"\".\n%s",
                 RF_STR_PF_ARG(name), dlerror());
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
        RF_ERROR("dlclose() failed on library "RF_STR_PF_FMT".\n%s",
                 RF_STR_PF_ARG(&dl->name), dlerror());
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
            RF_ERROR("No symbol named \""RF_STR_PF_FMT"\" could be found in "
                     "library "RF_STR_PF_FMT".",
                     RF_STR_PF_ARG(name),
                     RF_STR_PF_ARG(&dl->name));
        } else {
            RF_ERROR("Failed to to load symbol \""RF_STR_PF_FMT"\" from library "
                     RF_STR_PF_FMT".\n%s",
                     RF_STR_PF_ARG(name),
                     RF_STR_PF_ARG(&dl->name),
                     err);
        }
    }
    return sym;
}
