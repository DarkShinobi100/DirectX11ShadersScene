#include "App1.h"

App1::App1()
{

}

void App1::init(HINSTANCE hinstance, HWND hwnd, int screenWidth, int screenHeight, Input *in, bool VSYNC, bool FULL_SCREEN)
{
	// Call super/parent init function (required!)
	BaseApplication::init(hinstance, hwnd, screenWidth, screenHeight, in, VSYNC, FULL_SCREEN);

	// Initalise scene variables.
		width = screenWidth;
		height = screenHeight;

	// Load textures.
		textureMgr->loadTexture(L"leaves", L"res/leaf.png");
		textureMgr->loadTexture(L"Orangeleaves", L"res/Orangeleaf.png");
		textureMgr->loadTexture(L"Whiteleaves", L"res/Whiteleaf.png");
		textureMgr->loadTexture(L"BWleaves", L"res/BWleaves.png");
		// https://commons.wikimedia.org/wiki/File:Water_texture_1380389_Nevit.jpg
		textureMgr->loadTexture(L"Water", L"res/water.jpg");
		textureMgr->loadTexture(L"BWWater", L"res/BWwater.jpg");
		//https://commons.wikimedia.org/wiki/File:Wood_planks_texture.jpg
		textureMgr->loadTexture(L"Wood", L"res/wood.jpg");
		textureMgr->loadTexture(L"BWWood", L"res/BWwood.jpg");
		textureMgr->loadTexture(L"tree", L"res/tree2Color.png");
		textureMgr->loadTexture(L"treeNorms", L"res/TreeNormalMap.png");
		textureMgr->loadTexture(L"building", L"res/building.jpg");
		textureMgr->loadTexture(L"buildingNorms", L"res/buildingNormalMap.png");
		textureMgr->loadTexture(L"building2", L"res/2room-building.jpg");
		textureMgr->loadTexture(L"building2Norms", L"res/2RoomBuildingNormalMap.png");

	//set-up Lights
	SetUpLights();

	//create shaders
		//create depth shaders
		depthShader = new DepthTextureShader(renderer->getDevice(), hwnd);
		depthManipulationShader = new DepthManipulationShader(renderer->getDevice(), hwnd);
		depthTesselationShader = new DepthTesselationShader(renderer->getDevice(), hwnd);
		depthGeometryShader = new DepthGeometryShader(renderer->getDevice(), hwnd);

		//shadows
		shadowTextureShader = new ShadowTexture(renderer->getDevice(), hwnd);
		shadowManipulationShader = new ShadowManipulation(renderer->getDevice(), hwnd);
		shadowTesselationShader = new ShadowTesselation(renderer->getDevice(), hwnd);
		geometryShader = new GeometryShader(renderer->getDevice(), hwnd);

		//post Processing 
		horizontalBlurShader = new HorizontalBlurShader(renderer->getDevice(), hwnd);
		verticalBlurShader = new VerticalBlurShader(renderer->getDevice(), hwnd);
		FXtextureShader = new FXTextureShader(renderer->getDevice(), hwnd);

	// Create Mesh objects
		RoadMesh = new UserMesh(renderer->getDevice(), renderer->getDeviceContext(),150,50);
		BridgeMesh = new UserMesh(renderer->getDevice(), renderer->getDeviceContext(), 200, 50);
		WaterMesh = new UserTesselationMesh(renderer->getDevice(), renderer->getDeviceContext(), 250,300);
		orthoMesh = new OrthoMesh(renderer->getDevice(), renderer->getDeviceContext(), screenWidth, screenHeight);
		smallOrthoMesh = new OrthoMesh(renderer->getDevice(), renderer->getDeviceContext(), screenWidth / 2, screenHeight / 2);
		CameraorthoMesh = new OrthoMesh(renderer->getDevice(), renderer->getDeviceContext(), screenWidth / 4, screenHeight / 4, screenWidth / 2.7, -screenHeight / 2.7);

		// Create Models
		for (int i = 0; i < 6; i++)
		{
			if (i < 2)
			{
				Buildings[i] = new AModel(renderer->getDevice(), "res/building.fbx");
			}
			if (i == 2)
			{
				Buildings[i] = new AModel(renderer->getDevice(), "res/double_room_building.obj");
			}
			Trees[i] = new AModel(renderer->getDevice(), "res/tree2.fbx");
			LeafyTrees[i] = new AModel(renderer->getDevice(), "res/tree2.fbx");
			RNGValues[i] = rand() % 180;
		}

	// Shadow maps
		shadowMap = new ShadowMap(renderer->getDevice(), shadowmapWidth, shadowmapHeight);
		for (int i = 0; i < 13; i++)
		{
			DepthMaps[i] = new ShadowMap(renderer->getDevice(), shadowmapWidth, shadowmapHeight);
		}
		shadowMapScene = new ShadowMap(renderer->getDevice(), shadowmapWidth, shadowmapHeight);


	//Initialise required render textures
		horizontalBlurTexture = new RenderTexture(renderer->getDevice(), screenWidth / 2, screenHeight / 2, SCREEN_NEAR, SCREEN_DEPTH);
		verticalBlurTexture = new RenderTexture(renderer->getDevice(), screenWidth / 2, screenHeight / 2, SCREEN_NEAR, SCREEN_DEPTH);
		downSampleTexture = new RenderTexture(renderer->getDevice(), screenWidth / 2, screenHeight / 2, SCREEN_NEAR, SCREEN_DEPTH);
		upSampleTexture = new RenderTexture(renderer->getDevice(), screenWidth, screenHeight, SCREEN_NEAR, SCREEN_DEPTH);
}

App1::~App1()
{
	// Run base application deconstructor
	BaseApplication::~BaseApplication();

	// Release the Direct3D object.
	
}

bool App1::frame()
{
	bool result;

	result = BaseApplication::frame();
	if (!result)
	{
		return false;
	}
	
	// Render the graphics.
	result = render();
	if (!result)
	{
		return false;
	}

	return true;
}

bool App1::render()
{
	// Perform depth pass
	for (int i = 0; i < 8; i++)
	{
		depthPass(i,i);
	}
	for (int i = 0; i < 8; i++)
	{
		shadowMaps[i] = DepthMaps[i]->getDepthMapSRV();
	}

	//Pointlight
	for (int i = 8; i < 13; i++)
	{
		//Xaxis
		if (i == 8)
		{
			light[7]->setDirection(1, 0, 0);
		}
		if (i == 9)
		{
			light[7]->setDirection(-1, 0, 0);
		}

		//Yaxis
		if (i == 10)
		{
			light[7]->setDirection(0, 1, 0);
		}
		if (i == 11)
		{
			light[7]->setDirection(0, -1, 0);
		}
		//Zaxis
		if (i == 12)
		{
			light[7]->setDirection(0, 0, 1);
		}
		if (i == 13)
		{
			light[7]->setDirection(0, 0, -1);
		}
		depthPass(7,i);
	}

	camera->update();
	//depth pass for scene
	depthPass(0, 14);
	for (int i = 8; i < 13; i++)
	{
		shadowMaps[i] = DepthMaps[i]->getDepthMapSRV();
	}
	FirstPass();

	if (!ToggleFX)
	{//no special effect
	 //firest pass will present scene to screen
	}
	else
	{//apply special effect as default
			
		 // Apply horizontal blur stage
		horizontalBlur();
		// Apply vertical blur to the horizontal blur stage
		verticalBlur();

		UpSampleTexture();
		FinalPass();
	}
	// Render GUI
	gui();

	// Present the rendered scene to the screen.
	renderer->endScene();

	return true;
}

void App1::depthPass(int LightNumber,int MapNumber)
{
	if(MapNumber==14)
	{ 
		// Set the render target to be the render to texture.
		shadowMapScene->BindDsvAndSetNullRenderTarget(renderer->getDeviceContext());
		// get the world, view, and projection matrices from the camera and d3d objects.
		XMMATRIX worldMatrix = renderer->getWorldMatrix();
		CameraViewMatrix = camera->getViewMatrix();
		CameraProjectionMatrix = XMMatrixPerspectiveFovLH(XMConvertToRadians(90), (width/height), Near, Far+1.0);


		worldMatrix = XMMatrixMultiply(XMMatrixTranslation(30.0f, 0.0f, -38.0f), XMMatrixRotationY(-0.40f));
		DepthManipulation(RoadMesh, worldMatrix, CameraViewMatrix, CameraProjectionMatrix);

		worldMatrix = XMMatrixMultiply(XMMatrixTranslation(97.5f, 1.0f, -178.0f), XMMatrixRotationY(-1.50f));
		DepthManipulation(BridgeMesh, worldMatrix, CameraViewMatrix, CameraProjectionMatrix);

		worldMatrix = XMMatrixTranslation(-70.0f, -4.0f, 0.0f);
		if (!ToggleWater)
		{
			DepthTesselation(worldMatrix, CameraViewMatrix, CameraProjectionMatrix);
		}

		// Render model
		SetupDepthModels(worldMatrix, CameraViewMatrix, CameraProjectionMatrix);

	
	}
	else 
	{
		// Set the render target to be the render to texture.
		DepthMaps[MapNumber]->BindDsvAndSetNullRenderTarget(renderer->getDeviceContext());

		// get the world, view, and projection matrices from the camera and d3d objects.
		light[LightNumber]->generateViewMatrix();
		lightViewMatrix[MapNumber] = light[LightNumber]->getViewMatrix();

		if (LightNumber == 6)
		{
			lightProjectionMatrix[MapNumber] = light[LightNumber]->getOrthoMatrix();

		}
		else
		{
			lightProjectionMatrix[MapNumber] = light[LightNumber]->getProjectionMatrix();
		}

		XMMATRIX worldMatrix = renderer->getWorldMatrix();
		worldMatrix = XMMatrixMultiply(XMMatrixTranslation(30.0f, 0.0f, -38.0f), XMMatrixRotationY(-0.40f));
		DepthManipulation(RoadMesh, worldMatrix, lightViewMatrix[MapNumber], lightProjectionMatrix[MapNumber]);

		worldMatrix = XMMatrixMultiply(XMMatrixTranslation(97.5f, 1.0f, -178.0f), XMMatrixRotationY(-1.50f));
		DepthManipulation(BridgeMesh, worldMatrix, lightViewMatrix[MapNumber], lightProjectionMatrix[MapNumber]);

		worldMatrix = XMMatrixTranslation(-70.0f, -4.0f, 0.0f);
		if (!ToggleWater)
		{
			DepthTesselation(worldMatrix, lightViewMatrix[MapNumber], lightProjectionMatrix[MapNumber]);
		}

		// Render model
		SetupDepthModels(worldMatrix, lightViewMatrix[MapNumber], lightProjectionMatrix[MapNumber]);
	}	
	// Set back buffer as render target and reset view port.
	renderer->setBackBufferRenderTarget();
	renderer->resetViewport();
}

void App1::FirstPass()
{
	if (!ToggleFX)
	{//no special effect
		// Clear the scene. (default blue colour)
		if (Seasons == 0)
		{
			renderer->beginScene(0.39f, 0.58f, 0.92f, 1.0f);
		}
		else if (Seasons == 1)
		{
			renderer->beginScene(0.59f, 0.50f, 0.50f, 1.0f);
		}
		else
		{
			renderer->beginScene(0.40f, 0.40f, 0.65f, 1.0f);
		}
	}
	else
	{//apply special effect
		// Set the render target to be the render to texture.
		// Clear the scene. (default blue colour)
		if (Seasons == 0)
		{
			downSampleTexture->setRenderTarget(renderer->getDeviceContext());
			downSampleTexture->clearRenderTarget(renderer->getDeviceContext(), 0.39f, 0.58f, 0.92f, 1.0f);
		}
		else if (Seasons == 1)
		{
			downSampleTexture->setRenderTarget(renderer->getDeviceContext());
			downSampleTexture->clearRenderTarget(renderer->getDeviceContext(), 0.59f, 0.50f, 0.50f, 1.0f);
		}
		else
		{
			downSampleTexture->setRenderTarget(renderer->getDeviceContext());
			downSampleTexture->clearRenderTarget(renderer->getDeviceContext(), 0.40f, 0.40f, 0.65f, 1.0f);
		}
	}
	// Get the world, view, projection, and ortho matrices from the camera and Direct3D objects.
	XMMATRIX worldMatrix = renderer->getWorldMatrix();
	XMMATRIX viewMatrix = camera->getViewMatrix();
	XMMATRIX projectionMatrix = renderer->getProjectionMatrix();	

	worldMatrix = XMMatrixMultiply(XMMatrixTranslation(30.0f, 0.0f, -38.0f), XMMatrixRotationY(-0.40f));
	ShadowVertexManipulation(RoadMesh, worldMatrix, viewMatrix, projectionMatrix, lightViewMatrix, lightProjectionMatrix);

	worldMatrix = XMMatrixMultiply(XMMatrixTranslation(97.5f, 1.0f, -178.0f), XMMatrixRotationY(-1.50f));
	ShadowVertexManipulation(BridgeMesh, worldMatrix, viewMatrix, projectionMatrix, lightViewMatrix, lightProjectionMatrix);

	worldMatrix = XMMatrixTranslation(-70.0f, -4.0f, 0.0f);
	if (!ToggleWater)
	{
		ShadowTesselatedMesh(worldMatrix, viewMatrix, projectionMatrix, lightViewMatrix, lightProjectionMatrix);
	}
	
	// Render model
	SetupShadowModels(worldMatrix, viewMatrix, projectionMatrix, lightViewMatrix, lightProjectionMatrix);

	if (!ToggleFX)
	{//no special effect
	}
	else
	{//apply special effect
	// Set back buffer as render target and reset view port.
		renderer->setBackBufferRenderTarget();
	}
}

void App1::horizontalBlur()
{
	DepthofFieldtexture[0] = downSampleTexture->getShaderResourceView();
	DepthofFieldtexture[1] = shadowMapScene->getDepthMapSRV();


	XMMATRIX worldMatrix, baseViewMatrix, orthoMatrix;

	float screenSizeX = (float)horizontalBlurTexture->getTextureWidth();
	horizontalBlurTexture->setRenderTarget(renderer->getDeviceContext());
	horizontalBlurTexture->clearRenderTarget(renderer->getDeviceContext(), 1.0f, 1.0f, 0.0f, 1.0f);

	worldMatrix = renderer->getWorldMatrix();
	baseViewMatrix = camera->getOrthoViewMatrix();
	orthoMatrix = horizontalBlurTexture->getOrthoMatrix();

	// Render for Horizontal Blur
	renderer->setZBuffer(false);
	smallOrthoMesh->sendData(renderer->getDeviceContext());
	horizontalBlurShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, baseViewMatrix, orthoMatrix, DepthofFieldtexture, screenSizeX);
	horizontalBlurShader->render(renderer->getDeviceContext(), smallOrthoMesh->getIndexCount());
	renderer->setZBuffer(true);

	// Reset the render target back to the original back buffer and not the render to texture anymore.
	renderer->setBackBufferRenderTarget();
}

void App1::verticalBlur()
{
	DepthofFieldtexture[0] = horizontalBlurTexture->getShaderResourceView();
	DepthofFieldtexture[1] = shadowMapScene->getDepthMapSRV();

	XMMATRIX worldMatrix, baseViewMatrix, orthoMatrix;

	float screenSizeY = (float)verticalBlurTexture->getTextureHeight();
	verticalBlurTexture->setRenderTarget(renderer->getDeviceContext());
	verticalBlurTexture->clearRenderTarget(renderer->getDeviceContext(), 0.0f, 1.0f, 1.0f, 1.0f);

	worldMatrix = renderer->getWorldMatrix();
	baseViewMatrix = camera->getOrthoViewMatrix();
	// Get the ortho matrix from the render to texture since texture has different dimensions being that it is smaller.
	orthoMatrix = verticalBlurTexture->getOrthoMatrix();

	// Render for Vertical Blur
	renderer->setZBuffer(false);
	smallOrthoMesh->sendData(renderer->getDeviceContext());
	verticalBlurShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, baseViewMatrix, orthoMatrix, DepthofFieldtexture, screenSizeY);
	verticalBlurShader->render(renderer->getDeviceContext(), smallOrthoMesh->getIndexCount());
	renderer->setZBuffer(true);

	// Reset the render target back to the original back buffer and not the render to texture anymore.
	renderer->setBackBufferRenderTarget();
}

void App1::UpSampleTexture()
{
	// Set the render target to be the render to texture and clear it
	upSampleTexture->setRenderTarget(renderer->getDeviceContext());
	upSampleTexture->clearRenderTarget(renderer->getDeviceContext(), 0.0f, 0.0f, 1.0f, 1.0f);

	XMMATRIX worldMatrix = renderer->getWorldMatrix();
	XMMATRIX orthoMatrix = renderer->getOrthoMatrix();  // ortho matrix for 2D rendering
	XMMATRIX orthoViewMatrix = camera->getOrthoViewMatrix();	// Default camera position for orthographic rendering

	orthoMesh->sendData(renderer->getDeviceContext());
	FXtextureShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, orthoViewMatrix, orthoMatrix, verticalBlurTexture->getShaderResourceView());
	FXtextureShader->render(renderer->getDeviceContext(), orthoMesh->getIndexCount());

	// Reset the render target back to the original back buffer and not the render to texture anymore.
	renderer->setBackBufferRenderTarget();
}

void App1::FinalPass()
{
	// Clear the scene. (default blue colour)
	if (Seasons == 0)
	{
		renderer->beginScene(0.39f, 0.58f, 0.92f, 1.0f);
	}
	else if (Seasons == 1)
	{
		renderer->beginScene(0.59f, 0.50f, 0.50f, 1.0f);
	}
	else
	{
		renderer->beginScene(0.40f, 0.40f, 0.65f, 1.0f);
	}

	// RENDER THE RENDER TEXTURE SCENE
	// Requires 2D rendering and an ortho mesh.
	renderer->setZBuffer(false);
	XMMATRIX worldMatrix = renderer->getWorldMatrix();
	XMMATRIX orthoMatrix = renderer->getOrthoMatrix();  // ortho matrix for 2D rendering
	XMMATRIX orthoViewMatrix = camera->getOrthoViewMatrix();	// Default camera position for orthographic rendering

	orthoMesh->sendData(renderer->getDeviceContext());
	FXtextureShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, orthoViewMatrix, orthoMatrix, upSampleTexture->getShaderResourceView());
	FXtextureShader->render(renderer->getDeviceContext(), orthoMesh->getIndexCount());

	CameraorthoMesh->sendData(renderer->getDeviceContext());
	FXtextureShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, orthoViewMatrix, orthoMatrix, DepthofFieldtexture[1]);
	FXtextureShader->render(renderer->getDeviceContext(), CameraorthoMesh->getIndexCount());

	renderer->setZBuffer(true);
}

void App1::gui()
{
	// Force turn off unnecessary shader stages.
	renderer->getDeviceContext()->GSSetShader(NULL, NULL, 0);
	renderer->getDeviceContext()->HSSetShader(NULL, NULL, 0);
	renderer->getDeviceContext()->DSSetShader(NULL, NULL, 0);

	// Build UI
	ImGui::Text("FPS: %.2f", timer->getFPS());
	ImGui::Checkbox("Wireframe mode", &wireframeToggle);

	//set tesselation variables
	ImGui::SliderFloat("tesselation Left:", &tesselationLeft, 1, 24);
	ImGui::SliderFloat("tesselation Right:", &tesselationRight, 1, 24);
	ImGui::SliderFloat("tesselation Bottom", &tesselationBottom, 1, 24);
	ImGui::SliderFloat("tesselation Inside", &tesselationInside, 1, 24);

	ImGui::SliderFloat("Light Direction X:", &Position.x, -100, 1000);
	ImGui::SliderFloat("Light Direction Y:", &Position.y, -100, 1000);
	ImGui::SliderFloat("Light Direction Z:", &Position.z, -100, 1000);


	ImGui::Checkbox("Toggle Normals", &ToggleNormals);
	ImGui::Checkbox("Turn Off Water?", &ToggleWater);
	ImGui::Checkbox("Turn Off Leaves?", &ToggleLeaves);
	ImGui::Checkbox("Turn On Effect?", &ToggleFX);

	ImGui::SliderInt("Choose Season", &Seasons,0,2);
	ChangeSeason();

	ImGui::SliderFloat("Near Plane (reccomended 4)", &Near, 1, 499);
	ImGui::SliderFloat("Far Plane (reccomended 499)", &Far, 1, 499);
	//package user data for later
	TesselationValues = { tesselationLeft,tesselationRight,tesselationBottom,tesselationInside };
	time = { time.x,0.0f,0.0f,0.0f };
	light[6]->setDirection(Position.x +1.0, Position.y, Position.z);

	// Render UI
	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
}

void App1::ChangeSeason()
{
	for (int i = 0; i < 6; i++)
	{
		if (Seasons == 0)
		{
			light[i]->setDiffuseColour(0.5f, 0.5f, 0.5f, 1.0f);
		}
		else if (Seasons == 1)
		{
			light[i]->setDiffuseColour(0.85f, 0.35f, 0.35f, 1.0f);
		}
		else
		{
			light[i]->setDiffuseColour(0.25f, 0.25f, 0.85f, 1.0f);
		}
	}
	if (Seasons == 0)
	{
		Leaftexture = textureMgr->getTexture(L"leaves");
	}
	else if (Seasons == 1)
	{
		Leaftexture = textureMgr->getTexture(L"Orangeleaves");
	}
	else
	{
		Leaftexture = textureMgr->getTexture(L"Whiteleaves");
	}
}

void App1::SetUpLights()
{
	// Configure point light.
	for (int i = 0; i < 6; i++)
	{
		light[i] = new Light();
		light[i]->setAmbientColour(0.05f, 0.05f, 0.05f, 1.0f);
		light[i]->setDiffuseColour(0.5f, 0.5f, 0.5f, 1.0f);
		light[i]->setDirection(0.0f, -1.0f, 0.0f);
		light[i]->generateProjectionMatrix(2.f, 200.f);
		light[i]->generateViewMatrix();
	}
	//set position
	light[0]->setPosition(73.984f, 8.13f, 7.317f);
	light[1]->setPosition(39.837f, 9.756f, -1.1626f);
	light[2]->setPosition(25.2f, 12.195f, 25.2f);
	light[3]->setPosition(61.789f, 9.756f, 43.0f);
	light[4]->setPosition(4.065f, 13.0f, 74.4f);
	light[5]->setPosition(38.211f, 9.756f, 87.8f);

	// Configure directional light.
	light[6] = new Light();
	light[6]->setAmbientColour(0.05f, 0.05f, 0.05f, 1.0f);
	light[6]->setDiffuseColour(0.20f, 0.20f, 0.20f, 1.0f);
	light[6]->setPosition(0.06f, 50.0f, 0.0f);
	light[6]->setDirection(-0.7f, -0.7f, 0.0f);
	light[6]->generateOrthoMatrix((float)sceneWidth, (float)sceneHeight, 0.1f, 1000.0f);
	light[6]->generateViewMatrix();

	// Configure point light. //make spot light later
	light[7] = new Light();
	light[7]->setAmbientColour(0.05f, 0.05f, 0.05f, 1.0f);
	light[7]->setDiffuseColour(0.0f, 0.0f, 1.00f, 1.0f);
	light[7]->setPosition(88.6f, 28.455f, 110.15f);
	light[7]->setDirection(0.0f, -1.0f, 0.0f);
	light[7]->generateProjectionMatrix(0.5f, 35.f);
	light[7]->generateViewMatrix();
}

void App1::DepthTesselation(XMMATRIX worldMatrix, XMMATRIX viewMatrix, XMMATRIX projectionMatrix)
{
	XMFLOAT4 cameraPosition = { camera->getPosition().x,camera->getPosition().y,camera->getPosition().z, 1.0f };
	time.x += timer->getTime();
	// Send geometry data, set shader parameters, render object with shader
	WaterMesh->sendData(renderer->getDeviceContext(), D3D11_PRIMITIVE_TOPOLOGY_4_CONTROL_POINT_PATCHLIST);
	depthTesselationShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, viewMatrix, projectionMatrix, TesselationValues, textureMgr->getTexture(L"BWWater"),
		time, cameraPosition, textureMgr->getTexture(L"BWWater"));
	depthTesselationShader->render(renderer->getDeviceContext(), WaterMesh->getIndexCount());
}

void App1::DepthManipulation(UserMesh* Mesh, XMMATRIX worldMatrix, XMMATRIX viewMatrix, XMMATRIX projectionMatrix)
{
	Mesh->sendData(renderer->getDeviceContext());
	depthManipulationShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, viewMatrix, projectionMatrix,textureMgr->getTexture(L"BWWood"), textureMgr->getTexture(L"BWWater"));
	depthManipulationShader->render(renderer->getDeviceContext(), Mesh->getIndexCount());
}

void App1::SetupDepthModels(XMMATRIX worldMatrix, XMMATRIX viewMatrix, XMMATRIX projectionMatrix)
{
	worldMatrix = XMMatrixTranslation(162.0f, 10.0f, 750.0f);
	XMMATRIX scaleMatrix = XMMatrixScaling(0.35f, 0.35f, 0.35f);
	worldMatrix = XMMatrixMultiply(worldMatrix, scaleMatrix);
	Buildings[0]->sendData(renderer->getDeviceContext());
	DepthModels(Buildings[0], worldMatrix, viewMatrix, projectionMatrix, textureMgr->getTexture(L"building"), textureMgr->getTexture(L"buildingNorms"));

	worldMatrix = XMMatrixTranslation(-81.0, 10.0f, 750.0f);
	scaleMatrix = XMMatrixScaling(0.35f, 0.35f, 0.35f);
	worldMatrix = XMMatrixMultiply(worldMatrix, scaleMatrix);
	Buildings[1]->sendData(renderer->getDeviceContext());
	DepthModels(Buildings[1], worldMatrix, viewMatrix, projectionMatrix, textureMgr->getTexture(L"building"), textureMgr->getTexture(L"buildingNorms"));

	worldMatrix = XMMatrixTranslation(894.0f, 10.0f, 1138.0f);
	scaleMatrix = XMMatrixScaling(0.1f, 0.1f, 0.1f);
	worldMatrix = XMMatrixMultiply(worldMatrix, scaleMatrix);
	Buildings[2]->sendData(renderer->getDeviceContext());
	DepthModels(Buildings[2], worldMatrix, viewMatrix, projectionMatrix, textureMgr->getTexture(L"building2"), textureMgr->getTexture(L"building2Norms"));

	scaleMatrix = XMMatrixScaling(0.025f, 0.025f, 0.025f);
	worldMatrix = XMMatrixMultiply(worldMatrix, scaleMatrix);
	Trees[0]->sendData(renderer->getDeviceContext());
	DepthModels(Trees[0], worldMatrix, viewMatrix, projectionMatrix, textureMgr->getTexture(L"tree"), textureMgr->getTexture(L"treeNorms"));
	LeafyTrees[0]->sendData(renderer->getDeviceContext());
	DepthRenderGeometry(LeafyTrees[0], worldMatrix, viewMatrix, projectionMatrix,RNGValues[0]);

	worldMatrix = XMMatrixTranslation(1463.0f, 10.0f, 0.0f);

	scaleMatrix = XMMatrixScaling(0.025f, 0.025f, 0.025f);
	worldMatrix = XMMatrixMultiply(worldMatrix, scaleMatrix);
	Trees[1]->sendData(renderer->getDeviceContext());
	DepthModels(Trees[1], worldMatrix, viewMatrix, projectionMatrix, textureMgr->getTexture(L"tree"), textureMgr->getTexture(L"treeNorms"));
	LeafyTrees[1]->sendData(renderer->getDeviceContext());
	DepthRenderGeometry(LeafyTrees[1], worldMatrix, viewMatrix, projectionMatrix, RNGValues[1]);

	worldMatrix = XMMatrixTranslation(162.0f, 10.0f, 3070.0f);
	scaleMatrix = XMMatrixScaling(0.025f, 0.025f, 0.025f);
	worldMatrix = XMMatrixMultiply(worldMatrix, scaleMatrix);
	Trees[2]->sendData(renderer->getDeviceContext());
	DepthModels(Trees[2], worldMatrix, viewMatrix, projectionMatrix, textureMgr->getTexture(L"tree"), textureMgr->getTexture(L"treeNorms"));
	LeafyTrees[2]->sendData(renderer->getDeviceContext());
	DepthRenderGeometry(LeafyTrees[2], worldMatrix, viewMatrix, projectionMatrix, RNGValues[2]);


	worldMatrix = XMMatrixTranslation(2845.0f, 10.0f, 371.0f);
	scaleMatrix = XMMatrixScaling(0.025f, 0.025f, 0.025f);
	worldMatrix = XMMatrixMultiply(worldMatrix, scaleMatrix);
	Trees[3]->sendData(renderer->getDeviceContext());
	DepthModels(Trees[3], worldMatrix, viewMatrix, projectionMatrix, textureMgr->getTexture(L"tree"), textureMgr->getTexture(L"treeNorms"));
	LeafyTrees[3]->sendData(renderer->getDeviceContext());
	DepthRenderGeometry(LeafyTrees[3], worldMatrix, viewMatrix, projectionMatrix, RNGValues[3]);

	worldMatrix = XMMatrixTranslation(2357.0f, 10.0f, 1659.0f);
	scaleMatrix = XMMatrixScaling(0.025f, 0.025f, 0.025f);
	worldMatrix = XMMatrixMultiply(worldMatrix, scaleMatrix);
	Trees[4]->sendData(renderer->getDeviceContext());
	DepthModels(Trees[4], worldMatrix, viewMatrix, projectionMatrix, textureMgr->getTexture(L"tree"), textureMgr->getTexture(L"treeNorms"));
	LeafyTrees[4]->sendData(renderer->getDeviceContext());
	DepthRenderGeometry(LeafyTrees[4], worldMatrix, viewMatrix, projectionMatrix, RNGValues[4]);

	worldMatrix = XMMatrixTranslation(1626.0f, 10.0f, 3252.0f);
	scaleMatrix = XMMatrixScaling(0.025f, 0.025f, 0.025f);
	worldMatrix = XMMatrixMultiply(worldMatrix, scaleMatrix);
	Trees[5]->sendData(renderer->getDeviceContext());
	DepthModels(Trees[5], worldMatrix, viewMatrix, projectionMatrix, textureMgr->getTexture(L"tree"), textureMgr->getTexture(L"treeNorms"));
	LeafyTrees[5]->sendData(renderer->getDeviceContext());
	DepthRenderGeometry(LeafyTrees[5], worldMatrix, viewMatrix, projectionMatrix, RNGValues[5]);
}

void App1::DepthModels(AModel* Model, XMMATRIX worldMatrix, XMMATRIX viewMatrix, XMMATRIX projectionMatrix, ID3D11ShaderResourceView* texture, ID3D11ShaderResourceView* Normaltexture)
{
	Model->sendData(renderer->getDeviceContext());
	depthShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, viewMatrix, projectionMatrix, Normaltexture);
	depthShader->render(renderer->getDeviceContext(), Model->getIndexCount());
}

void App1::DepthRenderGeometry(AModel* Model, XMMATRIX worldMatrix, XMMATRIX viewMatrix, XMMATRIX projectionMatrix, float RNG)
{
	if (!ToggleLeaves)
	{
		// Send geometry data, set shader parameters, render object with shader
		Model->sendData(renderer->getDeviceContext(), D3D11_PRIMITIVE_TOPOLOGY_3_CONTROL_POINT_PATCHLIST);
		depthGeometryShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, viewMatrix, projectionMatrix, textureMgr->getTexture(L"leaves"), RNG);
		depthGeometryShader->render(renderer->getDeviceContext(), Model->getIndexCount());
	}
}

void App1::ShadowTesselatedMesh(XMMATRIX worldMatrix, XMMATRIX viewMatrix, XMMATRIX projectionMatrix, XMMATRIX lightViewMatrix[13], XMMATRIX lightProjectionMatrix[13])
{
	XMFLOAT4 cameraPosition = { camera->getPosition().x,camera->getPosition().y,camera->getPosition().z, 1.0f };
	time.x += timer->getTime();
	// Send geometry data, set shader parameters, render object with shader
	WaterMesh->sendData(renderer->getDeviceContext(), D3D11_PRIMITIVE_TOPOLOGY_4_CONTROL_POINT_PATCHLIST);
	shadowTesselationShader->setAttenuation(1.0f, 0.125f, 0.0f, 0.0f);
	shadowTesselationShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, viewMatrix, projectionMatrix, lightViewMatrix, lightProjectionMatrix,  TesselationValues, textureMgr->getTexture(L"Water"), textureMgr->getTexture(L"BWWater"), shadowMaps,
		light, time, cameraPosition, ToggleNormals);
	shadowTesselationShader->render(renderer->getDeviceContext(), WaterMesh->getIndexCount());

}

void App1::ShadowVertexManipulation(UserMesh* Mesh, XMMATRIX worldMatrix, XMMATRIX viewMatrix, XMMATRIX projectionMatrix, XMMATRIX lightViewMatrix[13], XMMATRIX lightProjectionMatrix[13])
{
	Mesh->sendData(renderer->getDeviceContext());
	shadowManipulationShader->setAttenuation(1.0f, 0.125f, 0.0f, 0.0f);
	shadowManipulationShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, viewMatrix, projectionMatrix, lightViewMatrix, lightProjectionMatrix, textureMgr->getTexture(L"Wood"), textureMgr->getTexture(L"BWWood"), shadowMaps,
		light, ToggleNormals, timer->getTime());
	shadowManipulationShader->render(renderer->getDeviceContext(), Mesh->getIndexCount());
}

void App1::SetupShadowModels(XMMATRIX worldMatrix, XMMATRIX viewMatrix, XMMATRIX projectionMatrix, XMMATRIX lightViewMatrix[13], XMMATRIX lightProjectionMatrix[13])
{
	worldMatrix = XMMatrixTranslation(162.0f, 10.0f, 750.0f);
	XMMATRIX scaleMatrix = XMMatrixScaling(0.35f, 0.35f, 0.35f);
	worldMatrix = XMMatrixMultiply(worldMatrix, scaleMatrix);
	Buildings[0]->sendData(renderer->getDeviceContext());
	ShadowModels(Buildings[0], worldMatrix, viewMatrix, projectionMatrix, lightViewMatrix, lightProjectionMatrix, textureMgr->getTexture(L"building"), shadowMaps);

	worldMatrix = XMMatrixTranslation(-81.0, 10.0f, 750.0f);
	scaleMatrix = XMMatrixScaling(0.35f, 0.35f, 0.35f);
	worldMatrix = XMMatrixMultiply(worldMatrix, scaleMatrix);
	Buildings[1]->sendData(renderer->getDeviceContext());
	ShadowModels(Buildings[1], worldMatrix, viewMatrix, projectionMatrix, lightViewMatrix, lightProjectionMatrix, textureMgr->getTexture(L"building"), shadowMaps);

	worldMatrix = XMMatrixTranslation(894.0f, 10.0f, 1138.0f);
	scaleMatrix = XMMatrixScaling(0.1f, 0.1f, 0.1f);
	worldMatrix = XMMatrixMultiply(worldMatrix, scaleMatrix);
	Buildings[2]->sendData(renderer->getDeviceContext());
	ShadowModels(Buildings[2], worldMatrix, viewMatrix, projectionMatrix, lightViewMatrix, lightProjectionMatrix, textureMgr->getTexture(L"building2"), shadowMaps);

	scaleMatrix = XMMatrixScaling(0.025f, 0.025f, 0.025f);
	worldMatrix = XMMatrixMultiply(worldMatrix, scaleMatrix);
	Trees[0]->sendData(renderer->getDeviceContext());
	ShadowModels(Trees[0], worldMatrix, viewMatrix, projectionMatrix, lightViewMatrix, lightProjectionMatrix, textureMgr->getTexture(L"tree"), shadowMaps);
	LeafyTrees[0]->sendData(renderer->getDeviceContext());
	ShadowRenderGeometry(LeafyTrees[0], worldMatrix, viewMatrix, projectionMatrix, lightViewMatrix, lightProjectionMatrix, shadowMaps,RNGValues[0]);

	worldMatrix = XMMatrixTranslation(1463.0f, 10.0f, 0.0f);

	scaleMatrix = XMMatrixScaling(0.025f, 0.025f, 0.025f);
	worldMatrix = XMMatrixMultiply(worldMatrix, scaleMatrix);
	Trees[1]->sendData(renderer->getDeviceContext());
	ShadowModels(Trees[1], worldMatrix, viewMatrix, projectionMatrix, lightViewMatrix, lightProjectionMatrix,textureMgr->getTexture(L"tree"), shadowMaps);
	LeafyTrees[1]->sendData(renderer->getDeviceContext());
	ShadowRenderGeometry(LeafyTrees[1], worldMatrix, viewMatrix, projectionMatrix, lightViewMatrix, lightProjectionMatrix, shadowMaps, RNGValues[1]);

	worldMatrix = XMMatrixTranslation(162.0f, 10.0f, 3070.0f);
	scaleMatrix = XMMatrixScaling(0.025f, 0.025f, 0.025f);
	worldMatrix = XMMatrixMultiply(worldMatrix, scaleMatrix);
	Trees[2]->sendData(renderer->getDeviceContext());
	ShadowModels(Trees[2], worldMatrix, viewMatrix, projectionMatrix, lightViewMatrix, lightProjectionMatrix, textureMgr->getTexture(L"tree"), shadowMaps);
	LeafyTrees[2]->sendData(renderer->getDeviceContext());
	ShadowRenderGeometry(LeafyTrees[2], worldMatrix, viewMatrix, projectionMatrix, lightViewMatrix, lightProjectionMatrix, shadowMaps, RNGValues[2]);


	worldMatrix = XMMatrixTranslation(2845.0f, 10.0f, 371.0f);
	scaleMatrix = XMMatrixScaling(0.025f, 0.025f, 0.025f);
	worldMatrix = XMMatrixMultiply(worldMatrix, scaleMatrix);
	Trees[3]->sendData(renderer->getDeviceContext());
	ShadowModels(Trees[3], worldMatrix, viewMatrix, projectionMatrix, lightViewMatrix, lightProjectionMatrix, textureMgr->getTexture(L"tree"), shadowMaps);
	LeafyTrees[3]->sendData(renderer->getDeviceContext());
	ShadowRenderGeometry(LeafyTrees[3], worldMatrix, viewMatrix, projectionMatrix, lightViewMatrix, lightProjectionMatrix, shadowMaps, RNGValues[3]);

	worldMatrix = XMMatrixTranslation(2357.0f, 10.0f, 1659.0f);
	scaleMatrix = XMMatrixScaling(0.025f, 0.025f, 0.025f);
	worldMatrix = XMMatrixMultiply(worldMatrix, scaleMatrix);
	Trees[4]->sendData(renderer->getDeviceContext());
	ShadowModels(Trees[4], worldMatrix, viewMatrix, projectionMatrix, lightViewMatrix, lightProjectionMatrix,textureMgr->getTexture(L"tree"), shadowMaps);
	LeafyTrees[4]->sendData(renderer->getDeviceContext());
	ShadowRenderGeometry(LeafyTrees[4], worldMatrix, viewMatrix, projectionMatrix, lightViewMatrix, lightProjectionMatrix, shadowMaps, RNGValues[4]);

	worldMatrix = XMMatrixTranslation(1626.0f, 10.0f, 3252.0f);
	scaleMatrix = XMMatrixScaling(0.025f, 0.025f, 0.025f);
	worldMatrix = XMMatrixMultiply(worldMatrix, scaleMatrix);
	Trees[5]->sendData(renderer->getDeviceContext());
	ShadowModels(Trees[5], worldMatrix, viewMatrix, projectionMatrix, lightViewMatrix, lightProjectionMatrix, textureMgr->getTexture(L"tree"), shadowMaps);
	LeafyTrees[5]->sendData(renderer->getDeviceContext());
	ShadowRenderGeometry(LeafyTrees[5], worldMatrix, viewMatrix, projectionMatrix, lightViewMatrix, lightProjectionMatrix, shadowMaps, RNGValues[5]);
}

void App1::ShadowModels(AModel* Model, XMMATRIX worldMatrix, XMMATRIX viewMatrix, XMMATRIX projectionMatrix, XMMATRIX lightViewMatrix[13], XMMATRIX lightProjectionMatrix[13], ID3D11ShaderResourceView* texture, ID3D11ShaderResourceView* shadowtexture[13])
{
	Model->sendData(renderer->getDeviceContext());
	shadowTextureShader->setAttenuation(1.0f, 0.125f, 0.0f, 0.0f);
	shadowTextureShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, viewMatrix, projectionMatrix, lightViewMatrix, lightProjectionMatrix, texture, shadowtexture,light,ToggleNormals, timer->getTime());
	shadowTextureShader->render(renderer->getDeviceContext(), Model->getIndexCount());
}

void App1::ShadowRenderGeometry(AModel* Model, XMMATRIX worldMatrix, XMMATRIX viewMatrix, XMMATRIX projectionMatrix, XMMATRIX lightViewMatrix[13], XMMATRIX lightProjectionMatrix[13], ID3D11ShaderResourceView* shadowtexture[13], float RNG)
{
	if (!ToggleLeaves)
	{
		// Send geometry data, set shader parameters, render object with shader
		renderer->setAlphaBlending(true);
		Model->sendData(renderer->getDeviceContext(), D3D11_PRIMITIVE_TOPOLOGY_3_CONTROL_POINT_PATCHLIST);
		geometryShader->setAttenuation(1.0f, 0.125f, 0.0f, 0.0f);
		geometryShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, viewMatrix, projectionMatrix, lightViewMatrix, lightProjectionMatrix, Leaftexture, shadowtexture, light, ToggleNormals, RNG, textureMgr->getTexture(L"BWleaves"), timer->getTime());
		geometryShader->render(renderer->getDeviceContext(), Model->getIndexCount());
		renderer->setAlphaBlending(false);
	}
}