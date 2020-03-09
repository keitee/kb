#ifndef RENDER_H
#define RENDER_H

typedef struct _WstRenderer WstRenderer;
typedef struct _WstRenderSurface WstRenderSurface;

// init function
typedef int (*WSTMethodRenderInit)( WstRenderer *renderer, int argc, char **argv);

typedef void (*WSTMethodRenderTerm)( WstRenderer *renderer );
typedef void (*WSTMethodUpdateScene)( WstRenderer *renderer );
typedef WstRenderSurface* (*WSTMethodSurfaceCreate)( WstRenderer *renderer );
typedef void (*WSTMethodSurfaceDestroy)( WstRenderer *renderer, WstRenderSurface *surf );
typedef void (*WSTMethodSurfaceSetVisible)( WstRenderer *renderer, WstRenderSurface *surface, bool visible );
typedef bool (*WSTMethodSurfaceGetVisible)( WstRenderer *renderer, WstRenderSurface *surface, bool *visible );

typedef struct _WstRenderer
{
   int outputWidth;
   int outputHeight;
   
   WSTMethodRenderTerm renderTerm;
   WSTMethodUpdateScene updateScene;
   WSTMethodSurfaceCreate surfaceCreate;
   WSTMethodSurfaceDestroy surfaceDestroy;
   WSTMethodSurfaceSetVisible surfaceSetVisible;
   WSTMethodSurfaceGetVisible surfaceGetVisible;
} WstRenderer;

typedef struct _WstRenderSurface
{
   int outputWidth;
   int outputHeight;
} WstRenderSurface;

#define RENDERER_MODULE_INIT "renderer_init"

#endif // RENDER_H
