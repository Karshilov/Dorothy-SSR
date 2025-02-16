#include "Const/Header.h"
#include "Basic/Application.h"

#if BX_PLATFORM_IOS

#include "SDL_syswm.h"
#include "SDL.h"

#import <QuartzCore/CAMetalLayer.h>

NS_DOROTHY_BEGIN
void Application::updateWindowSize()
{
	CGRect bounds = [UIScreen mainScreen].bounds;
	CGFloat scale = [UIScreen mainScreen].scale;
	_bufferWidth = bounds.size.width * scale;
	_bufferHeight = bounds.size.height * scale;
	SDL_GetWindowSize(_sdlWindow, &_winWidth, &_winHeight);
	SDL_DisplayMode displayMode{SDL_PIXELFORMAT_UNKNOWN, 0, 0, 0, 0};
	SDL_GetWindowDisplayMode(_sdlWindow, &displayMode);
	if (displayMode.refresh_rate > 0)
	{
		_maxFPS = displayMode.refresh_rate;
	}
	_visualWidth = _winWidth;
	_visualHeight = _winHeight;
}

void Application::setupSdlWindow()
{
	SDL_SysWMinfo wmi;
	SDL_VERSION(&wmi.version);
	SDL_GetWindowWMInfo(_sdlWindow, &wmi);

	CALayer* layer = wmi.info.uikit.window.rootViewController.view.layer;
	CAMetalLayer* displayLayer = [[CAMetalLayer alloc] init];
	
	CGRect bounds = [UIScreen mainScreen].bounds;
	CGFloat scale = [UIScreen mainScreen].scale;
	displayLayer.contentsScale = scale;
	displayLayer.frame = bounds;
	[layer addSublayer:displayLayer];
	[layer layoutSublayers];

	bgfx::PlatformData pd;
	pd.ndt = NULL;
	pd.nwh = (__bridge void*)displayLayer;
	pd.context = NULL;
	pd.backBuffer = NULL;
	pd.backBufferDS = NULL;
	bgfx::setPlatformData(pd);
	updateWindowSize();
}
NS_DOROTHY_END

#endif // BX_PLATFORM_IOS
