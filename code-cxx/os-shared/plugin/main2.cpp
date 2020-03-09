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

int main(int argc, char *argv[])
{
  bool error{false};
  void *module{nullptr};
  void *init{nullptr};
  const char *moduleName = "libplugin1.so";

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
