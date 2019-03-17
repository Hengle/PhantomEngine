// #include "GraphicsManager.h"
// #ifdef OS_MACOS
// 	#include "CocoaMetalApplication.h"
// #endif
// #ifdef OS_WINDOWS
// 	#include "WindowsApplication.h"
// #endif
// #include "OpenGL/OpenGLGraphicsManager.h"

#include "AssetLoadManager.h"
#include "InputManager.h"
#include "Image.h"
#include "BMPParser.h"
#include "utility.hpp"
#include "OpenGEX.h"


namespace Phantom {
	InputManager*    g_pInputManager    = static_cast<InputManager*>(new InputManager);
}

