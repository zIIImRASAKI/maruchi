#include	"enemy.h"	
#include    "../system/CDirectInput.h"
#include	"../system/meshmanager.h"

#include	"../scene/CarDriveScene.h"

void enemy::init() 
{
	m_mesh = MeshManager::getMesh<CStaticMesh>("car001.x");
	m_shader = MeshManager::getShader<CShader>("car001.x");
	m_meshrenderer = MeshManager::getRenderer<CStaticMeshRenderer>("car001.x");
}

void enemy::update(uint64_t dt) {


}

void enemy::draw(uint64_t dt) {


	Matrix4x4 mtx = m_srt.GetMatrix();

	Renderer::SetWorldMatrix(&mtx);

	m_shader->SetGPU();
	m_meshrenderer->Draw();

}

void enemy::dispose() {

}
