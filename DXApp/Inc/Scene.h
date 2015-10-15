#pragma once
#include "pch.h"
#include "Model.h"
#include "VertexTypes.h"

using namespace DirectX;

namespace MAPS
{
    class Scene
    {
    public:
        static std::unique_ptr<Scene> CreateScene(
            ID3D11Device* d3d11Device,
            ID3D11DeviceContext* d3d11DevContext);
        ~Scene();

        XMMATRIX World() const;

        void LoadMeshFromSTLFile(const wchar_t* fileName, char mode = 't');

        void Draw(CXMMATRIX view, CXMMATRIX projection, bool wireframe = true);

    private:
        Scene(ID3D11Device* d3d11Device, ID3D11DeviceContext* d3d11DevContext);

        void ParseSTLFile(
            const wchar_t* fileName,
            char mode,
            std::vector<VertexPositionNormal>& vertices,
            std::vector<uint16_t>& indices);

        std::unique_ptr<Model> model_;
        XMFLOAT4X4 world_;

        ID3D11Device* d3d11Device_;
        ID3D11DeviceContext* d3d11DevContext_;
    };
};
