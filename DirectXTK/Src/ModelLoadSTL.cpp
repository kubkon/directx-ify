//--------------------------------------------------------------------------------------
// File: ModelLoadSTL.cpp
//
// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
// PARTICULAR PURPOSE.
//
// Copyright (c) Jakub Konka. All rights reserved.
//
//--------------------------------------------------------------------------------------

#include "pch.h"
#include "Model.h"

#include "Effects.h"
#include "VertexTypes.h"

#include "DirectXHelpers.h"
#include "PlatformHelpers.h"
#include "BinaryReader.h"

#include <iostream>
#include <fstream>
#include <istream>
#include <vector>

#include <string>
#include "boost/algorithm/string.hpp"
#include "boost/lexical_cast.hpp"

using namespace DirectX;
using Microsoft::WRL::ComPtr;

void ParseSTLFile(const char* szFileName, std::vector<float>& vertices)
{
	// read in the contents of the stl file
	std::ifstream file(szFileName, std::ios::in);
	std::string line;
	std::vector<std::string> splitV;

	if (file.is_open())
	{
		while (std::getline(file, line))
		{
			if (boost::find_first(line, "vertex"))
			{
				boost::trim(line);
				boost::split(
					splitV,
					line,
					boost::is_any_of(" "),
					boost::token_compress_on);
				for (std::vector<std::string>::size_type i = 1; i != splitV.size(); i++)
				{
					vertices.push_back(boost::lexical_cast<float>(splitV[i]));
				}
			}
		}
		file.close();
	}
}

//--------------------------------------------------------------------------------------
// Shared VB input element description
static INIT_ONCE g_InitOnce = INIT_ONCE_STATIC_INIT;
static std::shared_ptr<std::vector<D3D11_INPUT_ELEMENT_DESC>> g_vbdecl;

static BOOL CALLBACK InitializeDecl(PINIT_ONCE initOnce, PVOID Parameter, PVOID *lpContext)
{
    UNREFERENCED_PARAMETER(initOnce);
    UNREFERENCED_PARAMETER(Parameter);
    UNREFERENCED_PARAMETER(lpContext);

    g_vbdecl = std::make_shared<std::vector<D3D11_INPUT_ELEMENT_DESC>>(VertexPositionColor::InputElements,
                   VertexPositionColor::InputElements + VertexPositionColor::InputElementCount);

    return TRUE;
}

//--------------------------------------------------------------------------------------
_Use_decl_annotations_
std::unique_ptr<Model> DirectX::Model::CreateFromSTL(ID3D11Device* d3dDevice, const char* szFileName,
                                                     std::shared_ptr<IEffect> ieffect, bool ccw, bool pmalpha)
{
	//// parse STL file
	//std::vector<VertexPositionColor> vertices;
	//ParseSTLFile(szFileName, vertices);

	//// create vertex and index buffers
 //   size_t vertSize = sizeof(VertexPositionColor) * vertices.size;
	//auto numVertices = 0;
	//auto verts = &vertices[0];

 //   size_t indexSize = sizeof(uint16_t) * header->numIndices;
	//auto numIndices = 0;
 //   auto indices = reinterpret_cast<const uint16_t*>( meshData + sizeof(VBO::header_t) + vertSize );

 //   // Create vertex buffer
 //   ComPtr<ID3D11Buffer> vb;
 //   {
 //       D3D11_BUFFER_DESC desc = { 0 };
 //       desc.Usage = D3D11_USAGE_DEFAULT;
 //       desc.ByteWidth = static_cast<UINT>(vertSize);
 //       desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

 //       D3D11_SUBRESOURCE_DATA initData = { 0 };
 //       initData.pSysMem = verts;

 //       ThrowIfFailed(
 //           d3dDevice->CreateBuffer(&desc, &initData, vb.GetAddressOf())
 //           );

 //       SetDebugObjectName(vb.Get(), "ModelSTL");
 //   }

 //   // Create index buffer
 //   ComPtr<ID3D11Buffer> ib;
 //   {
 //       D3D11_BUFFER_DESC desc = { 0 };
 //       desc.Usage = D3D11_USAGE_DEFAULT;
 //       desc.ByteWidth = static_cast<UINT>(indexSize);
 //       desc.BindFlags = D3D11_BIND_INDEX_BUFFER;

 //       D3D11_SUBRESOURCE_DATA initData = { 0 };
 //       initData.pSysMem = indices;

 //       ThrowIfFailed(
 //           d3dDevice->CreateBuffer(&desc, &initData, ib.GetAddressOf())
 //           );

 //       SetDebugObjectName(ib.Get(), "ModelSTL");
 //   }

 //   // Create input layout and effect
 //   if (!ieffect)
 //   {
 //       auto effect = std::make_shared<BasicEffect>(d3dDevice);
 //       effect->EnableDefaultLighting();
 //       effect->SetLightingEnabled(true);

 //       ieffect = effect;
 //   }

 //   ComPtr<ID3D11InputLayout> il;
 //   {
 //       void const* shaderByteCode;
 //       size_t byteCodeLength;

 //       ieffect->GetVertexShaderBytecode(&shaderByteCode, &byteCodeLength);

 //       ThrowIfFailed(
 //           d3dDevice->CreateInputLayout(VertexPositionNormalTexture::InputElements,
 //           VertexPositionNormalTexture::InputElementCount,
 //           shaderByteCode, byteCodeLength,
 //           il.GetAddressOf()));

 //       SetDebugObjectName(il.Get(), "ModelVBO");
 //   }

 //   auto part = new ModelMeshPart();
 //   part->indexCount = numIndices;
 //   part->startIndex = 0;
 //   part->vertexStride = static_cast<UINT>( sizeof(VertexPositionColor) );
 //   part->inputLayout = il;
 //   part->indexBuffer = ib;
 //   part->vertexBuffer = vb;
 //   part->effect = ieffect;
 //   part->vbDecl = g_vbdecl;

 //   auto mesh = std::make_shared<ModelMesh>();
 //   mesh->ccw = ccw;
 //   mesh->pmalpha = pmalpha;
 //   BoundingSphere::CreateFromPoints(mesh->boundingSphere, numVertices, &verts->position, sizeof(VertexPositionColor));
 //   BoundingBox::CreateFromPoints(mesh->boundingBox, numVertices, &verts->position, sizeof(VertexPositionColor));
 //   mesh->meshParts.emplace_back(part);

 //   std::unique_ptr<Model> model(new Model());
 //   model->meshes.emplace_back(mesh);
 //
 //   return model;

    return nullptr;
}
