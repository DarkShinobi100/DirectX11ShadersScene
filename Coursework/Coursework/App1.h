// Application.h
#ifndef _APP1_H
#define _APP1_H

// Includes
#include "DXF.h"	// include dxframework

//Meshes
	#include "UserMesh.h"
	#include "UserTesselationMesh.h"

//DepthPass
	#include "DepthManipulationShader.h"
	#include "DepthTesselationShader.h"
	#include "DepthTextureShader.h"
	#include "DepthGeometryShader.h"

//Shadows
	#include "ShadowManipulation.h"
	#include "ShadowTesselation.h"
	#include "ShadowTexture.h"
	#include "GeometryShader.h"

//PostProcessing
	#include "HorizontalBlurShader.h"
	#include "VerticalBlurShader.h"
	#include "FXTextureShader.h"

class App1 : public BaseApplication
{
public:

	App1();
	~App1();
	void init(HINSTANCE hinstance, HWND hwnd, int screenWidth, int screenHeight, Input* in, bool VSYNC, bool FULL_SCREEN);

	bool frame();

protected:
	bool render();
	void depthPass(int LightNumber, int MapNumber);
	void FirstPass();

	//post processing
		void verticalBlur();
		void horizontalBlur();
		void UpSampleTexture();
		void FinalPass();

	void gui();

private:
	void SetUpLights();
	void ChangeSeason();
	//Depth pass Data gathering
		void DepthTesselation(XMMATRIX worldMatrix, XMMATRIX viewMatrix, XMMATRIX projectionMatrix);
		void DepthManipulation(UserMesh* Mesh, XMMATRIX worldMatrix, XMMATRIX viewMatrix, XMMATRIX projectionMatrix);
		void SetupDepthModels(XMMATRIX worldMatrix, XMMATRIX viewMatrix, XMMATRIX projectionMatrix);
		void DepthModels(AModel* Model, XMMATRIX worldMatrix, XMMATRIX viewMatrix, XMMATRIX projectionMatrix, ID3D11ShaderResourceView* texture, ID3D11ShaderResourceView* Normaltexture);
		void DepthRenderGeometry(AModel* Model, XMMATRIX worldMatrix, XMMATRIX viewMatrix, XMMATRIX projectionMatrix, float RNG);

	//Shadows shader
		void ShadowTesselatedMesh(XMMATRIX worldMatrix, XMMATRIX viewMatrix, XMMATRIX projectionMatrix, XMMATRIX lightViewMatrix[13], XMMATRIX lightProjectionMatrix[13]);
		void ShadowVertexManipulation(UserMesh* Mesh, XMMATRIX worldMatrix, XMMATRIX viewMatrix, XMMATRIX projectionMatrix, XMMATRIX lightViewMatrix[13], XMMATRIX lightProjectionMatrix[13]);
		void SetupShadowModels(XMMATRIX worldMatrix, XMMATRIX viewMatrix, XMMATRIX projectionMatrix, XMMATRIX lightViewMatrix[13], XMMATRIX lightProjectionMatrix[13]);
		void ShadowModels(AModel* Model, XMMATRIX worldMatrix, XMMATRIX viewMatrix, XMMATRIX projectionMatrix, XMMATRIX lightViewMatrix[13], XMMATRIX lightProjectionMatrix[13], ID3D11ShaderResourceView* texture, ID3D11ShaderResourceView* shadowtexture[13]);
		void ShadowRenderGeometry(AModel* Model, XMMATRIX worldMatrix, XMMATRIX viewMatrix, XMMATRIX projectionMatrix, XMMATRIX lightViewMatrix[13], XMMATRIX lightProjectionMatrix[13],ID3D11ShaderResourceView* shadowtexture[13],float RNG);

	//Shaders
		//depth Pass
			DepthTextureShader* depthShader;
			DepthManipulationShader* depthManipulationShader;
			DepthTesselationShader* depthTesselationShader;
			DepthGeometryShader* depthGeometryShader;

	//shadows
		ShadowTexture* shadowTextureShader;
		ShadowManipulation* shadowManipulationShader;
		ShadowTesselation* shadowTesselationShader;
		GeometryShader* geometryShader;

	//post Processing
		FXTextureShader* FXtextureShader;
		VerticalBlurShader* verticalBlurShader;
		HorizontalBlurShader* horizontalBlurShader;

	//Variables
		XMFLOAT4 time;
		Light* light[8];
		ID3D11ShaderResourceView* Leaftexture;
		float RNGValues[6];
		float width;
		float height;

	//Meshes
		UserMesh* RoadMesh;
		UserMesh* BridgeMesh;
		UserTesselationMesh* WaterMesh;
		//https://sketchfab.com/3d-models/tree-1d26dcc1af0c4a06a95c459b7ca4cf6f
		AModel* Trees[6];
		AModel* LeafyTrees[6];
		//https://sketchfab.com/3d-models/medieval-building-833d2e67e8f44a7a921229c5e0e6b72b
		//https://sketchfab.com/3d-models/scifi-building-1220360fcebc4aa0874f1ca6bcb4cb30
		AModel* Buildings[3];

	//Shadows
		ShadowMap* shadowMap;
		ShadowMap* DepthMaps[13];
		ID3D11ShaderResourceView* shadowMaps[13];
		XMMATRIX lightViewMatrix[13];
		XMMATRIX lightProjectionMatrix[13];

	// Variables for defining shadow map
		int shadowmapWidth = 1024;
		int shadowmapHeight = 1024;
		int sceneWidth = 100;
		int sceneHeight = 100;

	//imGui float values
		float tesselationLeft = 1.0f;
		float tesselationRight = 1.0f;
		float tesselationBottom = 1.0f;
		float tesselationInside = 1.0f;
		XMFLOAT3 Position;
		XMFLOAT4 TesselationValues;
		bool ToggleNormals = false;
		bool ToggleLeaves = false;
		bool ToggleWater = false;
		bool ToggleFX = false;
		int Normals =0;
		int Seasons = 0;
		float Near = 1.0f;
		float Far = 1.0f;

	//post Processing
		ShadowMap* shadowMapScene;
		XMMATRIX CameraViewMatrix;
		XMMATRIX CameraProjectionMatrix;
		OrthoMesh* orthoMesh;
		OrthoMesh* smallOrthoMesh;
		OrthoMesh* CameraorthoMesh;
		RenderTexture* horizontalBlurTexture;
		RenderTexture* verticalBlurTexture;
		RenderTexture* downSampleTexture;
		RenderTexture* upSampleTexture;
		ID3D11ShaderResourceView* DepthofFieldtexture[2];
};

#endif