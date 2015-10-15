#include "pch.h"
#include "Scene.h"
#include "CommonStates.h"
#include "Effects.h"

using namespace DirectX;
using Microsoft::WRL::ComPtr;
using namespace MAPS;


Scene::Scene(ID3D11Device* d3d11Device, ID3D11DeviceContext* d3d11DevContext)
{
    d3d11Device_ = d3d11Device;
    d3d11DevContext_ = d3d11DevContext;
}

Scene::~Scene()
{
    d3d11Device_ = nullptr;
    d3d11DevContext_ = nullptr;
}

std::unique_ptr<Scene> Scene::CreateScene(
    ID3D11Device* d3d11Device,
    ID3D11DeviceContext* d3d11DevContext)
{
    std::unique_ptr<Scene> scene(new Scene(d3d11Device, d3d11DevContext));
    return scene;
}

XMMATRIX Scene::World() const
{
    return XMLoadFloat4x4(&world_);
}

void Scene::LoadMeshFromSTLFile(const wchar_t* fileName, char mode)
{
    // parse STL file
    std::vector<VertexPositionNormal> vertsV;
    std::vector<uint16_t> indicesV;
    ParseSTLFile(fileName, mode, vertsV, indicesV);

    // world matrix
    XMMATRIX scale, rotate, world;
    scale = XMMatrixScaling(1.0f, 1.0f, 1.0f);
    rotate = XMMatrixRotationRollPitchYaw(0.0f, 0.0f, 0.0f);

    // adjust for RH model
    world = scale * rotate;
    world.r[2] *= -1.0f;

    XMStoreFloat4x4(&world_, world);

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
            d3d11Device_->CreateBuffer(&desc, &initData, vb.GetAddressOf())
            );
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
            d3d11Device_->CreateBuffer(&desc, &initData, ib.GetAddressOf())
            );
    }

    // Create input layout and effect
    auto effect = std::make_shared<BasicEffect>(d3d11Device_);
    effect->EnableDefaultLighting();
    effect->SetLightingEnabled(true);

    ComPtr<ID3D11InputLayout> il;
    {
        void const* shaderByteCode;
        size_t byteCodeLength;

        effect->GetVertexShaderBytecode(&shaderByteCode, &byteCodeLength);

        ThrowIfFailed(
            d3d11Device_->CreateInputLayout(VertexPositionNormal::InputElements,
            VertexPositionNormal::InputElementCount,
            shaderByteCode, byteCodeLength,
            il.GetAddressOf()));
    }

    auto part = new ModelMeshPart();
    part->indexCount = numIndices;
    part->startIndex = 0;
    part->vertexStride = static_cast<UINT>( sizeof(VertexPositionNormal) );
    part->inputLayout = il;
    part->indexBuffer = ib;
    part->vertexBuffer = vb;
    part->effect = effect;
    part->vbDecl = std::make_shared<std::vector<D3D11_INPUT_ELEMENT_DESC>>(VertexPositionNormal::InputElements,
                   VertexPositionNormal::InputElements + VertexPositionNormal::InputElementCount);

    auto mesh = std::make_shared<ModelMesh>();
    mesh->ccw = false;
    mesh->pmalpha = false;
    BoundingSphere::CreateFromPoints(mesh->boundingSphere, numVertices, &verts->position, sizeof(VertexPositionNormal));
    BoundingBox::CreateFromPoints(mesh->boundingBox, numVertices, &verts->position, sizeof(VertexPositionNormal));
    mesh->meshParts.emplace_back(part);

    model_ = std::make_shared<Model>();
    model_->meshes.emplace_back(mesh);
}

void Scene::Draw(CXMMATRIX view, CXMMATRIX projection, bool wireframe)
{
    CommonStates states(d3d11Device_);
    model_->Draw(
        d3d11DevContext_,
        states,
        World(),
        view,
        projection,
        wireframe);
}

void Scene::ParseSTLFile(
    const wchar_t* fileName,
    char mode,
    std::vector<VertexPositionNormal>& vertices,
    std::vector<uint16_t>& indices)
{
    // read in the contents of the stl file
    std::ifstream file(fileName, std::ios::in);
    std::string line;
    std::string searchPhrases[2] = { "normal", "vertex" };

    // temp storage
    VertexPositionNormal rhTriangle[3] = {};
    uint16_t counter = 0;
    uint16_t index = 0;
    XMVECTOR pos, normal;

    auto parseStringToXMVector = [](std::string line)
    {
        std::vector<std::string> splitV;
        boost::trim(line);
        boost::split(
            splitV,
            line,
            boost::is_any_of(" "),
            boost::token_compress_on);

        return XMVectorSet(
            boost::lexical_cast<float>(splitV[0]),
            boost::lexical_cast<float>(splitV[1]),
            boost::lexical_cast<float>(splitV[2]),
            0.0f);
    };

    if (file.is_open())
    {
        while (std::getline(file, line))
        {
            if (boost::find_first(line, searchPhrases[0]))
            {
                line.erase(0, searchPhrases[0].size() + line.find(searchPhrases[0]));
                normal = parseStringToXMVector(line);
            }

            if (boost::find_first(line, searchPhrases[1]))
            {
                line.erase(0, searchPhrases[1].size() + line.find(searchPhrases[1]));
                pos = parseStringToXMVector(line);

                // create Vertex
                rhTriangle[counter++] = VertexPositionNormal(pos, normal);
                indices.push_back(index++);
            }

            if (counter > 2)
            {
                vertices.push_back(rhTriangle[0]);
                vertices.push_back(rhTriangle[2]);
                vertices.push_back(rhTriangle[1]);
                counter = 0;
            }
        }

        file.close();
    }
}
