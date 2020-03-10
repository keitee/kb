#include <dlfcn.h>
#include <renderer.h>
#include <stdio.h>
#include <stdlib.h>

// LD_LIBRARY_PATH=plugin1 ./test_plugin
// module (libplugin1.so) loaded and intialized
// plugin1_term() called
// plugin1_update_scene() called
// plugin1_surface_create() called
// plugin1_surface_destroy() called
// plugin1_surface_set_visible() called
// plugin1_surface_get_visible() called

// copy so files in the build so that can use LD_LIBRARY_PATH=.
//
// LD_LIBRARY_PATH=. ./test_plugin
// failed to load env
// module (libplugin1.so) loaded and intialized
// plugin1_term() called
// plugin1_update_scene() called
// plugin1_surface_create() called
// plugin1_surface_destroy() called
// plugin1_surface_set_visible() called
// plugin1_surface_get_visible() called
//
// LD_LIBRARY_PATH=. MODULE=libplugin1.so ./test_plugin
// module (libplugin1.so) loaded and intialized
// plugin1_term() called
// plugin1_update_scene() called
// plugin1_surface_create() called
// plugin1_surface_destroy() called
// plugin1_surface_set_visible() called
// plugin1_surface_get_visible() called
//
// LD_LIBRARY_PATH=. MODULE=libplugin2.so ./test_plugin
// module (libplugin2.so) loaded and intialized
// plugin2_term() called
// plugin2_update_scene() called
// plugin2_surface_create() called
// plugin2_surface_destroy() called
// plugin2_surface_set_visible() called
// plugin2_surface_get_visible() called

// readelf --syms --use-dynamic libplugin1.so
// 
// Symbol table of `.gnu.hash' for image:
//   Num Buc:    Value          Size   Type   Bind Vis      Ndx Name
//    12   0: 0000000000000a03   135 FUNC    GLOBAL DEFAULT  12 renderer_init
//    13   0: 0000000000201048     0 NOTYPE  GLOBAL DEFAULT  23 _end
//    14   0: 0000000000201040     0 NOTYPE  GLOBAL DEFAULT  22 _edata
//    15   1: 0000000000201040     0 NOTYPE  GLOBAL DEFAULT  23 __bss_start
//    16   1: 0000000000000738     0 FUNC    GLOBAL DEFAULT   9 _init
//    17   2: 0000000000000ae8     0 FUNC    GLOBAL DEFAULT  13 _fini

int main(int argc, char *argv[])
{
  bool error{false};
  void *module{nullptr};
  void *init{nullptr};

  const char *moduleNameDefault = "libplugin1.so";

  const char *moduleName = getenv("MODULE");
  if (!moduleName)
  {
    printf("failed to load env\n");
    moduleName = moduleNameDefault;
  }

  WstRenderer *renderer = (WstRenderer *)calloc(1, sizeof(WstRenderer));
  if (renderer)
  {
    int rc{};

    module = dlopen(moduleName, RTLD_NOW);
    if (!module)
    {
      printf("failed to load module (%s)\n", moduleName);
      printf("  detail: %s\n", dlerror());
      error = true;
      goto exit;
    }

    dlerror();

    // init function name and use macro
    init = dlsym(module, RENDERER_MODULE_INIT);
    if (!init)
    {
      printf("failed to find module (%s) method (%s)\n",
             moduleName,
             RENDERER_MODULE_INIT);
      printf("  detail: %s\n", dlerror());
      error = true;
      goto exit;
    }

    renderer->outputWidth  = 1024;
    renderer->outputHeight = 768;

    rc = ((WSTMethodRenderInit)init)(renderer, argc, argv);
    if (rc)
    {
      printf("module (%s) init failed: %d\n", moduleName, rc);
      error = true;
      goto exit;
    }

    printf("module (%s) loaded and intialized\n", moduleName);

    // TODO
    // calls functions from plubin so
    renderer->renderTerm(renderer);
    renderer->updateScene(renderer);
    renderer->surfaceCreate(renderer);
    renderer->surfaceDestroy(renderer, nullptr);
    renderer->surfaceSetVisible(renderer, nullptr, true);
    renderer->surfaceGetVisible(renderer, nullptr, nullptr);
  }

exit:

  if (error)
  {
    if (renderer)
    {
      free(renderer);
    }
    if (module)
    {
      dlclose(module);
    }
  }

  return EXIT_SUCCESS;
}
