#pragma once
#include "IRuntimeModule.h"
#include "mat4.h"
#include "vector.h"
#include "camera.h"
#include "GfxStruct.h"


namespace Phantom {
	class GraphicsManager: implements IRuntimeModule
	{
	public:
		virtual ~GraphicsManager() {};

		virtual int Init();
		virtual void Shutdown();

		virtual void Tick();
		virtual void Clear() = 0;
		virtual void Draw() = 0;

		virtual void resize(int32_t width, int32_t height) {};

		bool Inited = false;
	private:
		void CalculateCameraMatrix();

		void UpdateConstants();

		virtual void bindShader() = 0;

		virtual void SetPerFrameConstants(const ContextPerFrame& context) {}
		virtual void SetPerFrameLight(const Light & light) {}
		virtual void SetPerBatchConstants(const std::vector<std::shared_ptr<ContextPerDrawBatch>>& batches) {}
	protected:
		Frame  m_Frame;
	};
	extern GraphicsManager* g_pGraphicsManager;
}
