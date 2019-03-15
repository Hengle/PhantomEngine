#pragma once
#include "SceneOjbectTypeDef.h"
#include "SceneBaseObject.h"
#include "SceneObjectMesh.h"

namespace Phantom {
    class SceneObjectGeometry : public SceneBaseObject
    {
        protected:
            std::vector<std::shared_ptr<SceneObjectMesh>> m_Mesh;
			bool        m_bVisible;
			bool        m_bShadow;
			bool        m_bMotionBlur;
            float       m_CollisionParameters[10];

        public:
            SceneObjectGeometry(void) : SceneBaseObject(SceneObjectType::kSceneObjectTypeGeometry) {}

			void SetVisibility(bool visible) { m_bVisible = visible; }
			const bool Visible() { return m_bVisible; }
			void SetIfCastShadow(bool shadow) { m_bShadow = shadow; }
			const bool CastShadow() { return m_bShadow; }
			void SetIfMotionBlur(bool motion_blur) { m_bMotionBlur = motion_blur; }
			const bool MotionBlur() { return m_bMotionBlur; };
            void SetCollisionParameters(const float* param, int32_t count)
            {
                assert(count > 0 && count < 10);
                memcpy(m_CollisionParameters, param, sizeof(float) * count);
            }
            const float* CollisionParameters() const { return m_CollisionParameters; }

            void AddMesh(std::shared_ptr<SceneObjectMesh>& mesh) { m_Mesh.push_back(std::move(mesh)); }
            const std::weak_ptr<SceneObjectMesh> GetMesh() { return (m_Mesh.empty()? nullptr : m_Mesh[0]); }
            const std::weak_ptr<SceneObjectMesh> GetMeshLOD(size_t lod) { return (lod < m_Mesh.size()? m_Mesh[lod] : nullptr); }

        friend std::ostream& operator<<(std::ostream& out, const SceneObjectGeometry& obj);
    };
}