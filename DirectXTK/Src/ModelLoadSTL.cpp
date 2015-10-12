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
#include <string>
#include <vector>

using namespace DirectX;
using Microsoft::WRL::ComPtr;


void ParseSTLFile(const char* szFileName, std::vector<std::string>& buffer)
{
	// read in the contents of the stl file
	std::ifstream file(szFileName, std::ios::in);
	std::string line;

	if (file.is_open())
	{
		while (std::getline(file, line))
		{
			buffer.push_back(line);
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
	std::vector<std::string> lines;
	ParseSTLFile(szFileName, lines);
    return nullptr;
}
