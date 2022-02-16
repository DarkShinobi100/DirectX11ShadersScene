// plane mesh
// Quad mesh made of many quads.
#include "UserMesh.h"

// Initialise buffer and load texture.
UserMesh::UserMesh(ID3D11Device* device, ID3D11DeviceContext* deviceContext, int x, int y, int lresolution)
{
	resolution = lresolution;
	XValue = x;
	YValue = y;
	initBuffers(device);
}

// Release resources.
UserMesh::~UserMesh()
{
	// Run parent deconstructor
	BaseMesh::~BaseMesh();
}

// Generate plane (including texture coordinates and normals).
void UserMesh::initBuffers(ID3D11Device* device)
{
	VertexType* vertices;
	unsigned long* indices;
	int index, i, j;
	float positionX, positionZ, u, v, incrementx, incrementy;
	D3D11_BUFFER_DESC vertexBufferDesc, indexBufferDesc;
	D3D11_SUBRESOURCE_DATA vertexData, indexData;

	// Calculate the number of vertices in the terrain mesh.
	vertexCount = (XValue - 1) * (YValue - 1) * 6;


	indexCount = vertexCount;
	vertices = new VertexType[vertexCount];
	indices = new unsigned long[indexCount];


	index = 0;
	// UV coords.
	u = 0;
	v = 0;
	incrementx = 1.0f / XValue;
	incrementy = 1.0f / YValue;

	for (j = 0; j < (XValue - 1); j++)
	{
		for (i = 0; i < (YValue - 1); i++)
		{
			// Upper left.
			positionX = (float)i;
			positionZ = (float)(j);

			vertices[index].position = XMFLOAT3(positionX, 0.0f, positionZ);
			vertices[index].texture = XMFLOAT2(u, v);
			vertices[index].normal = XMFLOAT3(0.0, 1.0, 0.0);
			indices[index] = index;
			index++;

			// Upper right.
			positionX = (float)(i + 1);
			positionZ = (float)(j + 1);

			vertices[index].position = XMFLOAT3(positionX, 0.0f, positionZ);
			vertices[index].texture = XMFLOAT2(u + incrementy, v + incrementx);
			vertices[index].normal = XMFLOAT3(0.0, 1.0, 0.0);
			indices[index] = index;
			index++;


			// lower left
			positionX = (float)(i);
			positionZ = (float)(j + 1);


			vertices[index].position = XMFLOAT3(positionX, 0.0f, positionZ);
			vertices[index].texture = XMFLOAT2(u, v + incrementx);
			vertices[index].normal = XMFLOAT3(0.0, 1.0, 0.0);
			indices[index] = index;
			index++;

			// Upper left
			positionX = (float)(i);
			positionZ = (float)(j);

			vertices[index].position = XMFLOAT3(positionX, 0.0f, positionZ);
			vertices[index].texture = XMFLOAT2(u, v);
			vertices[index].normal = XMFLOAT3(0.0, 1.0, 0.0);
			indices[index] = index;
			index++;

			// Bottom right
			positionX = (float)(i + 1);
			positionZ = (float)(j);

			vertices[index].position = XMFLOAT3(positionX, 0.0f, positionZ);
			vertices[index].texture = XMFLOAT2(u + incrementy, v);
			vertices[index].normal = XMFLOAT3(0.0, 1.0, 0.0);
			indices[index] = index;
			index++;

			// Upper right.
			positionX = (float)(i + 1);
			positionZ = (float)(j + 1);

			vertices[index].position = XMFLOAT3(positionX, 0.0f, positionZ);
			vertices[index].texture = XMFLOAT2(u + incrementy, v + incrementx);
			vertices[index].normal = XMFLOAT3(0.0, 1.0, 0.0);
			indices[index] = index;
			index++;

			u += incrementy;

		}

		u = 0;
		v += incrementx;
	}

	// Set up the description of the static vertex buffer.
	vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexBufferDesc.ByteWidth = sizeof(VertexType) * vertexCount;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = 0;
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;
	// Give the subresource structure a pointer to the vertex data.
	vertexData.pSysMem = vertices;
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;
	// Now create the vertex buffer.
	device->CreateBuffer(&vertexBufferDesc, &vertexData, &vertexBuffer);

	// Set up the description of the static index buffer.
	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth = sizeof(unsigned long) * indexCount;
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;
	indexBufferDesc.StructureByteStride = 0;
	// Give the subresource structure a pointer to the index data.
	indexData.pSysMem = indices;
	indexData.SysMemPitch = 0;
	indexData.SysMemSlicePitch = 0;
	// Create the index buffer.
	device->CreateBuffer(&indexBufferDesc, &indexData, &indexBuffer);

	// Release the arrays now that the buffers have been created and loaded.
	delete[] vertices;
	vertices = 0;
	delete[] indices;
	indices = 0;
}
