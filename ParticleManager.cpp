/*********************************************************************
 * \file   ParticleManager.cpp
 * \brief  
 * 
 * \author Harukichimaru
 * \date   December 2024
 * \note   
 *********************************************************************/
#include "ParticleManager.h"
#include "Logger.h"
using namespace Logger;


///=============================================================================
///						シングルトンインスタンス
ParticleManager* ParticleManager::GetInstance() {
	if(instance_ == nullptr) {
		instance_ = new ParticleManager;
	}
	return instance_;
}

///=============================================================================
///						初期化
void ParticleManager::Initialize(DirectXCore* dxCore, const std::string& textureDirectoryPath, SrvSetup* srvSetup) {
	//---------------------------------------
	// 引数でdxManagerを受取
	dxCore_ = dxCore;
	
	//---------------------------------------
	// SrvSetupの設定
	srvSetup_ = srvSetup;

    //---------------------------------------
	// ランダムエンジンの初期化
	randomEngine_.seed(std::random_device()( ));

    //---------------------------------------
    // グラフィックスパイプラインの生成
    CreateGraphicsPipeline();

    //---------------------------------------
	// 頂点データの初期化(座標など)
    // 頂点リソースの作成
    CreateVertexData();

    // 頂点バッファビュー(VBV)の作成
    // 頂点リソースに頂点データを書き込む
	CreateVertexBufferView();
}

///=============================================================================
///						更新
void ParticleManager::Update() {
}

///=============================================================================
///						描画
void ParticleManager::Draw() {
}


///=============================================================================
///						静的メンバ関数
///--------------------------------------------------------------
///						  パーティクルグループの作成
void ParticleManager::CreatePathcleGroup(const std::string& groupName, const std::string& textureFilePath) {
    // 登録済みの名前からチェックしてassert
    assert(particleGroups_.find(groupName) == particleGroups_.end());

    // 新たな空のパーティクルグループを作成して、コンテナに登録
    ParticleGroup newGroup;
    particleGroups_[groupName] = newGroup;

    // 新たなパーティクルグループの初期化
    // マテリアルデータにテクスチャファイルパスを設定
    particleGroups_[groupName].materialFilePath = textureFilePath;

    // テクスチャを読み込む(事前の読み込みも可能)
    DirectX::ScratchImage image = DirectXCore::LoadTexture(textureFilePath);
    Microsoft::WRL::ComPtr<ID3D12Resource> texture = dxCore_->CreateTextureResource(image.GetMetadata());
    dxCore_->UploadTextureData(texture, image);

    // マテリアルデータにテクスチャのSRVインデックスを記録
    particleGroups_[groupName].instancingSrvIndex = srvSetup_->Allocate();
    srvSetup_->CreateSRVforTexture2D(particleGroups_[groupName].instancingSrvIndex, texture.Get(), image.GetMetadata().format, image.GetMetadata().mipLevels);

    // インスタンシング用のリソースを生成
    particleGroups_[groupName].instancingResource = dxCore_->CreateBufferResource(sizeof(ParticleForGPU) * kNumMaxInstance_);

    // インスタンシング用のSRVを確保してSRVインデックスを記録
    particleGroups_[groupName].instancingSrvIndex = srvSetup_->Allocate();
    srvSetup_->CreateSRVStructuredBuffer(particleGroups_[groupName].instancingSrvIndex, particleGroups_[groupName].instancingResource.Get(), DXGI_FORMAT_UNKNOWN, sizeof(ParticleForGPU));

    // SRV生成(StructureBuffer用)
    particleGroups_[groupName].instancingDataPtr = nullptr;
    particleGroups_[groupName].instancingResource->Map(0, nullptr, reinterpret_cast<void**>(&particleGroups_[groupName].instancingDataPtr));
}

///--------------------------------------------------------------
///						 ルートシグネチャーの作成
void ParticleManager::CreateRootSignature() {
    /// ===RootSignature作成=== ///
    D3D12_ROOT_SIGNATURE_DESC descriptionRootSignature{};
    descriptionRootSignature.Flags =
        D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

    /// ===DescriptorRangeの設定=== ///
	D3D12_DESCRIPTOR_RANGE descriptorRangeForInstancing[1] = {};
	descriptorRangeForInstancing[0].BaseShaderRegister = 0; // から始まる
    descriptorRangeForInstancing[0].NumDescriptors = 1; //
	descriptorRangeForInstancing[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	descriptorRangeForInstancing[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

    /// ===RootParameter作成=== ///
    D3D12_ROOT_PARAMETER rootParameters[3]{};
	// インスタンシング用
    rootParameters[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;    //CBVを使う
    rootParameters[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;    //PixelShaderで使う
    rootParameters[0].Descriptor.ShaderRegister = 0;    //レジスタ番号0とバインド
    rootParameters[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
    rootParameters[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;
    rootParameters[1].DescriptorTable.pDescriptorRanges = descriptorRangeForInstancing;
    rootParameters[1].DescriptorTable.NumDescriptorRanges = _countof(descriptorRangeForInstancing);
    rootParameters[2].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;  //DescriptorTableを使う
    rootParameters[2].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
    rootParameters[2].DescriptorTable.pDescriptorRanges = descriptorRangeForInstancing;
    rootParameters[2].DescriptorTable.NumDescriptorRanges = _countof(descriptorRangeForInstancing);


    ///// ===RootParameter作成=== ///
    //D3D12_ROOT_PARAMETER rootParameters[4]{};
    //rootParameters[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
    //rootParameters[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
    //rootParameters[0].Descriptor.ShaderRegister = 0; // b0

    //rootParameters[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
    //rootParameters[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;
    //rootParameters[1].Descriptor.ShaderRegister = 0; // b0

    ///// ===DescropterTable=== ///
    //rootParameters[2].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
    //rootParameters[2].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
    //rootParameters[2].DescriptorTable.pDescriptorRanges = &descriptorRange;
    //rootParameters[2].DescriptorTable.NumDescriptorRanges = 1;

    ///// ====DirectionalLight=== ///
    //rootParameters[3].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;	   //CBV
    //rootParameters[3].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;    //PixelShader
    //rootParameters[3].Descriptor.ShaderRegister = 1; // b1

    descriptionRootSignature.pParameters = rootParameters;                //ルートパラメータ配列へのポインタ
    descriptionRootSignature.NumParameters = 3;    //配列の長さ

    /// ===Samplerの設定=== ///
    D3D12_STATIC_SAMPLER_DESC staticSamplers[1] = {};
    staticSamplers[0].Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;  //バイリニアフィルタ
    staticSamplers[0].AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;  //0～1の範囲外をリピート
    staticSamplers[0].AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
    staticSamplers[0].AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
    staticSamplers[0].ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;  //比較しない
    staticSamplers[0].MaxLOD = D3D12_FLOAT32_MAX;  //ありったけのMipmapを使う
    staticSamplers[0].ShaderRegister = 0;  //レジスタ番号0を使う
    staticSamplers[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;  //PixelShaderで使う
    descriptionRootSignature.pStaticSamplers = staticSamplers;
    descriptionRootSignature.NumStaticSamplers = _countof(staticSamplers);

    /// ===シリアライズしてバイナリにする=== ///
    Microsoft::WRL::ComPtr<ID3DBlob> signatureBlob = nullptr;
    Microsoft::WRL::ComPtr<ID3DBlob> errorBlob = nullptr;
    HRESULT hr = D3D12SerializeRootSignature(&descriptionRootSignature,
        D3D_ROOT_SIGNATURE_VERSION_1, &signatureBlob, &errorBlob);
    if (FAILED(hr)) {
        throw std::runtime_error(reinterpret_cast<char*>(errorBlob->GetBufferPointer()));
    }

    /// ===バイナリを元に生成=== ///
    hr = dxCore_->GetDevice()->CreateRootSignature(0, signatureBlob->GetBufferPointer(),
        signatureBlob->GetBufferSize(), IID_PPV_ARGS(&rootSignature_));
    if (FAILED(hr)) {
        throw std::runtime_error("ENGINE MESSAGE: Object2d Failed to create root signature");
    }
    Log("ENGINE MESSAGE: Object2d Root signature created successfully :)\n");
}

///--------------------------------------------------------------
///						 グラフィックスパイプラインの作成
void ParticleManager::CreateGraphicsPipeline() {
    /// ===RoorSignatureの作成=== ///
    CreateRootSignature();

    /// ===InputLayoutの設定を行う=== ///
    D3D12_INPUT_ELEMENT_DESC inputElementDescs[3]{};
    //頂点データ
    inputElementDescs[0].SemanticName = "POSITION";
    inputElementDescs[0].SemanticIndex = 0;
    inputElementDescs[0].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
    inputElementDescs[0].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;
    //画像座標データ
    inputElementDescs[1].SemanticName = "TEXCOORD";
    inputElementDescs[1].SemanticIndex = 0;
    inputElementDescs[1].Format = DXGI_FORMAT_R32G32_FLOAT;
    inputElementDescs[1].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;
    //法線データ
    inputElementDescs[2].SemanticName = "NORMAL";
    inputElementDescs[2].SemanticIndex = 0;
    inputElementDescs[2].Format = DXGI_FORMAT_R32G32B32_FLOAT;
    inputElementDescs[2].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;

    D3D12_INPUT_LAYOUT_DESC inputLayoutDesc{};
    inputLayoutDesc.pInputElementDescs = inputElementDescs;
    inputLayoutDesc.NumElements = 3;

    /// ===BlendStateの設定を行う=== ///
    D3D12_BLEND_DESC blendDesc{};
    blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;

    /// ===RasterizerStateの設定を行う=== ///
    D3D12_RASTERIZER_DESC rasterizerDesc{};
    rasterizerDesc.CullMode = D3D12_CULL_MODE_BACK;
    rasterizerDesc.FillMode = D3D12_FILL_MODE_SOLID;

    /// ===Shaderをcompileする=== ///
    Microsoft::WRL::ComPtr<IDxcBlob> vertexShaderBlob = dxCore_->CompileShader(L"resources/shader/Object3D.VS.hlsl", L"vs_6_0");
    if (!vertexShaderBlob) {
        throw std::runtime_error("ENGINE MESSAGE: Sprite Failed to compile vertex shader :(");
    }
    Log("ENGINE MESSAGE: Sprite Vertex shader created successfully :)\n");

    Microsoft::WRL::ComPtr<IDxcBlob> pixelShaderBlob = dxCore_->CompileShader(L"resources/shader/Object3D.PS.hlsl", L"ps_6_0");
    if (!pixelShaderBlob) {
        throw std::runtime_error("ENGINE MESSAGE: Sprite Failed to compile pixel shader :(");
    }
    Log("ENGINE MESSAGE: Sprite Pixel shader created successfully :)\n");

    /// ===PSOを生成する=== ///
    D3D12_GRAPHICS_PIPELINE_STATE_DESC graphicsPipelineStateDesc{};
    graphicsPipelineStateDesc.pRootSignature = rootSignature_.Get();
    graphicsPipelineStateDesc.InputLayout = inputLayoutDesc;
    graphicsPipelineStateDesc.VS = { vertexShaderBlob->GetBufferPointer(), vertexShaderBlob->GetBufferSize() };
    graphicsPipelineStateDesc.PS = { pixelShaderBlob->GetBufferPointer(), pixelShaderBlob->GetBufferSize() };
    graphicsPipelineStateDesc.BlendState = blendDesc;
    graphicsPipelineStateDesc.RasterizerState = rasterizerDesc;
    graphicsPipelineStateDesc.NumRenderTargets = 1;
    graphicsPipelineStateDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
    graphicsPipelineStateDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
    graphicsPipelineStateDesc.SampleDesc.Count = 1;
    graphicsPipelineStateDesc.SampleMask = D3D12_DEFAULT_SAMPLE_MASK;

    //DepthStencilStateの設定を行う
    D3D12_DEPTH_STENCIL_DESC depthStencilDesc{};
    depthStencilDesc.DepthEnable = true;
    depthStencilDesc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
    depthStencilDesc.DepthFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL;
    graphicsPipelineStateDesc.DepthStencilState = depthStencilDesc;
    graphicsPipelineStateDesc.DSVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;

    //実際に生成
    HRESULT hr = dxCore_->GetDevice()->CreateGraphicsPipelineState(&graphicsPipelineStateDesc,
        IID_PPV_ARGS(&graphicsPipelineState_));
    if (FAILED(hr)) {
        throw std::runtime_error("ENGINE MESSAGE: Sprite Failed to create graphics pipeline state :(");
    }
    Log("ENGINE MESSAGE: Sprite Graphics pipeline state created successfully :)\n");
}

///--------------------------------------------------------------
///						 頂点バッファの作成
void ParticleManager::CreateVertexData() {
	//========================================
	// モデルデータの初期化
    modelData_.vertices.push_back(VertexData{ .position = {1.0f, 1.0f, 0.0f, 1.0f}, .texCoord = {0.0f, 0.0f}, .normal = {0.0f, 0.0f, 1.0f} });
    modelData_.vertices.push_back(VertexData{ .position = {-1.0f, 1.0f, 0.0f, 1.0f}, .texCoord = {1.0f, 0.0f}, .normal = {0.0f, 0.0f, 1.0f} });
    modelData_.vertices.push_back(VertexData{ .position = {1.0f, -1.0f, 0.0f, 1.0f}, .texCoord = {0.0f, 1.0f}, .normal = {0.0f, 0.0f, 1.0f} });
    modelData_.vertices.push_back(VertexData{ .position = {1.0f, -1.0f, 0.0f, 1.0f}, .texCoord = {0.0f, 1.0f}, .normal = {0.0f, 0.0f, 1.0f} });
    modelData_.vertices.push_back(VertexData{ .position = {-1.0f, 1.0f, 0.0f, 1.0f}, .texCoord = {1.0f, 0.0f}, .normal = {0.0f, 0.0f, 1.0f} });
    modelData_.vertices.push_back(VertexData{ .position = {-1.0f, -1.0f, 0.0f, 1.0f}, .texCoord = {1.0f, 1.0f}, .normal = {0.0f, 0.0f, 1.0f} });
}

///--------------------------------------------------------------
///                    頂点バッファビューの作成
void ParticleManager::CreateVertexBufferView() {
    //========================================
    // 頂点リソースを作る
    vertexBuffer_ = dxCore_->CreateBufferResource(sizeof(VertexData) * modelData_.vertices.size());
    //========================================
    // 頂点バッファビューを作成する
    vertexBufferView_.BufferLocation = vertexBuffer_->GetGPUVirtualAddress();				//リソースの先頭アドレスから使う
    vertexBufferView_.SizeInBytes = UINT(sizeof(VertexData) * modelData_.vertices.size());	//使用するリソースのサイズは頂点サイズ
    vertexBufferView_.StrideInBytes = sizeof(VertexData);									//1頂点あたりのサイズ
    //========================================
    // 頂点リソースにデータを書き込む
    VertexData* vertexData = nullptr;
    vertexBuffer_->Map(0, nullptr, reinterpret_cast<void**>( &vertexData ));
    std::memcpy(vertexData, modelData_.vertices.data(), sizeof(VertexData) * modelData_.vertices.size());
}
