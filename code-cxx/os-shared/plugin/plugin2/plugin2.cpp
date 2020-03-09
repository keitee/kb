#include <iostream>
#include <renderer.h>

using namespace std;

static void plugin2_term(WstRenderer *renderer)
{
  (void)renderer;
  std::cout << "plugin2_term() called" << std::endl;
}

static void plugin2_update_scene(WstRenderer *renderer)
{
  (void)renderer;
  std::cout << "plugin2_update_scene() called" << std::endl;
}

static WstRenderSurface *plugin2_surface_create(WstRenderer *renderer)
{
  (void)renderer;
  std::cout << "plugin2_surface_create() called" << std::endl;
  return nullptr;
}

static void plugin2_surface_destroy(WstRenderer *renderer,
                                    WstRenderSurface *surf)
{
  (void)renderer;
  (void)surf;
  std::cout << "plugin2_surface_destroy() called" << std::endl;
}

static void plugin2_surface_set_visible(WstRenderer *renderer,
                                        WstRenderSurface *surface,
                                        bool visible)
{
  (void)renderer;
  (void)surface;
  (void)visible;
  std::cout << "plugin2_surface_set_visible() called" << std::endl;
}

static bool plugin2_surface_get_visible(WstRenderer *renderer,
                                        WstRenderSurface *surface,
                                        bool *visible)
{
  (void)renderer;
  (void)surface;
  (void)visible;
  std::cout << "plugin2_surface_get_visible() called" << std::endl;
  return true;
}

extern "C"
{
  int renderer_init(WstRenderer *renderer, int argc, char **argv)
  {
    int rc{};

    if (renderer)
    {
      // renderer->renderer= rendererEMB;

      renderer->renderTerm        = plugin2_term;
      renderer->updateScene       = plugin2_update_scene;
      renderer->surfaceCreate     = plugin2_surface_create;
      renderer->surfaceDestroy    = plugin2_surface_destroy;
      renderer->surfaceSetVisible = plugin2_surface_set_visible;
      renderer->surfaceGetVisible = plugin2_surface_get_visible;
    }
    else
    {
      rc = -1;
    }

  exit:

    return 0;
  }
}
