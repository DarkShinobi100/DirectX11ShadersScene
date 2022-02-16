/**
* \class User Mesh
*
* \Manipulatable plane mesh object
*
* Inherits from Base Mesh, Builds a simple plane with texture coordinates and normals.
* Provided resolution values deteremines the subdivisions of the plane.
* Builds a plane from unit quads.
*
*/


#ifndef _USERMESH_H_
#define _USERMESH_H_

#include "BaseMesh.h"

class UserMesh : public BaseMesh
{

public:
	/** \brief Initialises and builds a plane mesh
	*
	* Can specify resolution of plane, this deteremines how many subdivisions of the plane.
	* @param device is the renderer device
	* @param device context is the renderer device context
	* @param resolution is a int for subdivision of the plane. The number of unit quad on each axis. Default is 100.
	*/
	UserMesh(ID3D11Device* device, ID3D11DeviceContext* deviceContext,int x, int y, int resolution = 100);
	~UserMesh();

	void sendData(ID3D11DeviceContext* deviceContext, D3D_PRIMITIVE_TOPOLOGY top = D3D11_PRIMITIVE_TOPOLOGY_3_CONTROL_POINT_PATCHLIST) override;

protected:
	int resolution;
	void initBuffers(ID3D11Device* device);
	int YValue;
	int XValue;
	int ZValue;
};

#endif