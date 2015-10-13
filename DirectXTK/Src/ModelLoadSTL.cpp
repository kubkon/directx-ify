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


//--------------------------------------------------------------------------------------
// Shared VB input element description
static INIT_ONCE g_InitOnce = INIT_ONCE_STATIC_INIT;
static std::shared_ptr<std::vector<D3D11_INPUT_ELEMENT_DESC>> g_vbdecl;

static BOOL CALLBACK InitializeDecl(PINIT_ONCE initOnce, PVOID Parameter, PVOID *lpContext)
{
    UNREFERENCED_PARAMETER(initOnce);
    UNREFERENCED_PARAMETER(Parameter);
    UNREFERENCED_PARAMETER(lpContext);

    g_vbdecl = std::make_shared<std::vector<D3D11_INPUT_ELEMENT_DESC>>(VertexPositionNormal::InputElements,
                   VertexPositionNormal::InputElements + VertexPositionNormal::InputElementCount);

    return TRUE;
}

void ParseSTLFile(
	const char* szFileName,
	std::vector<VertexPositionNormal>& vertices,
	std::vector<uint16_t>& indices)
{
	// read in the contents of the stl file
	std::ifstream file(szFileName, std::ios::in);
	std::string line;
	std::vector<std::string> splitV;
	VertexPositionNormal vertex;
	uint16_t index = 0;
	uint16_t rhIndices[3];

	if (file.is_open())
	{
		while (std::getline(file, line))
		{
			if (boost::find_first(line, "normal"))
			{
				boost::trim(line);
				boost::split(
					splitV,
					line,
					boost::is_any_of(" "),
					boost::token_compress_on);
				vertex.normal.x = boost::lexical_cast<float>(splitV[2]);
				vertex.normal.y = boost::lexical_cast<float>(splitV[3]);
				vertex.normal.z = boost::lexical_cast<float>(splitV[4]);

				std::getline(file, line);

				for (int i = 0; i < 3; i++)
				{
					std::getline(file, line);
					boost::trim(line);
					boost::split(
						splitV,
						line,
						boost::is_any_of(" "),
						boost::token_compress_on);
					vertex.position.x = boost::lexical_cast<float>(splitV[1]);
					vertex.position.y = boost::lexical_cast<float>(splitV[2]);
					vertex.position.z = boost::lexical_cast<float>(splitV[3]);
					vertices.push_back(vertex);
					rhIndices[i] = index;
					index++;
				}

				indices.push_back(rhIndices[0]);
				indices.push_back(rhIndices[2]);
				indices.push_back(rhIndices[1]);
			}
		}
		file.close();
	}
}

//--------------------------------------------------------------------------------------
_Use_decl_annotations_
std::unique_ptr<Model> DirectX::Model::CreateFromSTL(ID3D11Device* d3dDevice, const char* szFileName,
                                                     std::shared_ptr<IEffect> ieffect, bool ccw, bool pmalpha)
{
	// parse STL file
	std::vector<VertexPositionNormal> vertsV;
	std::vector<uint16_t> indicesV;
	ParseSTLFile(szFileName, vertsV, indicesV);

	// create vertex and index buffers
	auto numVertices = vertsV.size();
	size_t vertSize = sizeof(VertexPositionNormal) * numVertices;
	auto verts = &vertsV[0];

	auto numIndices = indicesV.size();
	size_t indexSize = sizeof(uint16_t) * numIndices;
	auto indices = &indicesV[0];

    // Create vertex buffer
    ComPtr<ID3D11Buffer> vb;
    {
        D3D11_BUFFER_DESC desc = { 0 };
        desc.Usage = D3D11_USAGE_DEFAULT;
        desc.ByteWidth = static_cast<UINT>(vertSize);
        desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

        D3D11_SUBRESOURCE_DATA initData = { 0 };
        initData.pSysMem = verts;

        ThrowIfFailed(
            d3dDevice->CreateBuffer(&desc, &initData, vb.GetAddressOf())
            );

        SetDebugObjectName(vb.Get(), "ModelSTL");
    }

    // Create index buffer
    ComPtr<ID3D11Buffer> ib;
    {
        D3D11_BUFFER_DESC desc = { 0 };
        desc.Usage = D3D11_USAGE_DEFAULT;
        desc.ByteWidth = static_cast<UINT>(indexSize);
        desc.BindFlags = D3D11_BIND_INDEX_BUFFER;

        D3D11_SUBRESOURCE_DATA initData = { 0 };
        initData.pSysMem = indices;

        ThrowIfFailed(
            d3dDevice->CreateBuffer(&desc, &initData, ib.GetAddressOf())
            );

        SetDebugObjectName(ib.Get(), "ModelSTL");
    }

    // Create input layout and effect
    if (!ieffect)
    {
        auto effect = std::make_shared<BasicEffect>(d3dDevice);
        effect->EnableDefaultLighting();
        effect->SetLightingEnabled(true);

        ieffect = effect;
    }

    ComPtr<ID3D11InputLayout> il;
    {
        void const* shaderByteCode;
        size_t byteCodeLength;

        ieffect->GetVertexShaderBytecode(&shaderByteCode, &byteCodeLength);

        ThrowIfFailed(
            d3dDevice->CreateInputLayout(VertexPositionNormal::InputElements,
            VertexPositionNormal::InputElementCount,
            shaderByteCode, byteCodeLength,
            il.GetAddressOf()));

        SetDebugObjectName(il.Get(), "ModelSTL");
    }

    auto part = new ModelMeshPart();
    part->indexCount = numIndices;
    part->startIndex = 0;
    part->vertexStride = static_cast<UINT>( sizeof(VertexPositionNormal) );
    part->inputLayout = il;
    part->indexBuffer = ib;
    part->vertexBuffer = vb;
    part->effect = ieffect;
    part->vbDecl = g_vbdecl;

    auto mesh = std::make_shared<ModelMesh>();
    mesh->ccw = ccw;
    mesh->pmalpha = pmalpha;
    BoundingSphere::CreateFromPoints(mesh->boundingSphere, numVertices, &verts->position, sizeof(VertexPositionNormal));
    BoundingBox::CreateFromPoints(mesh->boundingBox, numVertices, &verts->position, sizeof(VertexPositionNormal));
    mesh->meshParts.emplace_back(part);

    std::unique_ptr<Model> model(new Model());
    model->meshes.emplace_back(mesh);
 
    return model;
}

