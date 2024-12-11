#include "Particle.h"
#include "Camera.h"
//---------------------------------------
// ファイル読み込み関数
#include <fstream>
#include <sstream>
//---------------------------------------
// 数学関数　
#include <cmath>
#include "MathFunc4x4.h"
#include "Vector3.h"
#include "AffineTransformations.h"
#include "TextureManager.h"
#include "ParticleSetup.h"
#include <numbers>


///=============================================================================
///						
void Particle::Initialize(ParticleSetup* particleSetup) {
	//========================================
	// 引数からSetupを受け取る
	this->particleSetup_ = particleSetup;

	//RandomEngineの初期化
	randomEngine_.seed(std::random_device()( ));

	//========================================
	// 頂点データの作成
	CreateVertexData();
	// 頂点バッファビューの作成
	CreateVertexBufferView();

	//========================================
	// 書き込むためのアドレスを取得
	vertexBuffer_->Map(0, nullptr, reinterpret_cast<void**>( &vertexData_ ));
	//頂点データをリソースにコピー
	std::memcpy(vertexData_, modelData_.vertices.data(), sizeof(VertexData) * modelData_.vertices.size());
}

void Particle::Update() {
	// カメラ情報を取得
	Camera* camera = particleSetup_->GetDefaultCamera();
	Matrix4x4 cameraMatrix = MakeAffineMatrix({ 1.0f,1.0f,1.0f }, camera->GetRotate(), camera->GetTranslate());
	Matrix4x4 viewMatrix = Inverse4x4(cameraMatrix);
	// TODO:後にWindowサイズは変数にする
	Matrix4x4 projectionMatrix = MakePerspectiveFovMatrix(0.45f, float(1280) / float(720), 0.1f, 100.0f);
	Matrix4x4 viewProjectionMatrix = Multiply4x4(viewMatrix, projectionMatrix);

	// カメラ目線の設定
	Matrix4x4 backToFrontMatrix = MakeRotateYMatrix(std::numbers::pi_v<float>);
	Matrix4x4 billboardMatrix{};
	if(isUsedBillboard) {
		billboardMatrix = Multiply4x4(backToFrontMatrix, cameraMatrix);
		// 平行移動成分は無視
		billboardMatrix.m[3][0] = 0.0f;
		billboardMatrix.m[3][1] = 0.0f;
		billboardMatrix.m[3][2] = 0.0f;
	} else {
		billboardMatrix = Identity4x4();
	}

	// スケール調整用の倍率を設定
	constexpr float scaleMultiplier = 0.01f; // 必要に応じて調整

	for(auto& group : particleGroups) {
		Vector2 textureSize = group.second.textureSize;

		for(auto it = group.second.particleList.begin(); it != group.second.particleList.end();) {
			ParticleStr& particle = *it;

			// パーティクルの寿命が尽きた場合は削除
			if(particle.lifeTime <= particle.currentTime) {
				it = group.second.particleList.erase(it);
				continue;
			}

			// スケールをテクスチャサイズに基づいて調整
			particle.transform.scale.x = textureSize.x * scaleMultiplier;
			particle.transform.scale.y = textureSize.y * scaleMultiplier;

			// 位置の更新
			particle.transform.translate = AddVec3(particle.transform.translate, MultiplyVec3(kDeltaTime, particle.velocity));

			// 経過時間を更新
			particle.currentTime += kDeltaTime;

			// ワールド行列の計算
			Matrix4x4 worldMatrix = Multiply4x4(
				billboardMatrix,
				MakeAffineMatrix(particle.transform.scale, particle.transform.rotate, particle.transform.translate));

			// ビュー・プロジェクションを掛け合わせて最終行列を計算
			Matrix4x4 worldviewProjectionMatrix = Multiply4x4(worldMatrix, viewProjectionMatrix);

			// インスタンシングデータの設定
			if(group.second.instanceCount < kNumMaxInstance) {
				group.second.instancingDataPtr[group.second.instanceCount].WVP = worldviewProjectionMatrix;
				group.second.instancingDataPtr[group.second.instanceCount].World = worldMatrix;

				// カラーを設定し、アルファ値を減衰
				group.second.instancingDataPtr[group.second.instanceCount].color = particle.color;
				group.second.instancingDataPtr[group.second.instanceCount].color.w = 1.0f - ( particle.currentTime / particle.lifeTime );
				if(group.second.instancingDataPtr[group.second.instanceCount].color.w < 0.0f) {
					group.second.instancingDataPtr[group.second.instanceCount].color.w = 0.0f;
				}

				++group.second.instanceCount;
			}

			++it;
		}
	}
}

// TODO: ここにメンバ関数を追加していく
///=============================================================================
///						描画
void Particle::Draw() {
	ID3D12GraphicsCommandList* commandList = particleSetup_->GetDXManager()->GetCommandList().Get();
	// ルートシグネチャを設定
	//commandList->SetGraphicsRootSignature(rootSignature_.Get());
	//キャッシュ内に指定されたブレンドモードのPSOが存在するか確認
	//auto it = pipelineStateCache_.find(blendMode_);
	//if (it == pipelineStateCache_.end() || !it->second) {
	//	Logger::Log("PSO for blend mode not found, defaulting to normal blend mode.");
	//	it = pipelineStateCache_.find(kBlendModeNormal);
	//	if (it == pipelineStateCache_.end() || !it->second) {
	//		Logger::Log("Default PSO not found. Aborting draw call.");
	//		return;
	//	}
	//}

	//commandList->SetPipelineState(it->second.Get());
	// プリミティブトポロジ（描画形状）を設定
	//commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	// VBV (Vertex Buffer View)を設定
	commandList->IASetVertexBuffers(0, 1, &vertexBufferView_);

	// 全てのパーティクルグループについて処理を行う
	for(auto& group : particleGroups) {
		if(group.second.instanceCount == 0) continue; // インスタンスが無い場合はスキップ

		Vector2 textureLeftTop = group.second.textureLeftTop;
		Vector2 textureSize = group.second.textureSize;

		for(auto& particle : group.second.particleList) {
			// UV座標の計算
			float uStart = textureLeftTop.x / textureSize.x;
			float uEnd = ( textureLeftTop.x + textureSize.x ) / textureSize.x;
			float vStart = textureLeftTop.y / textureSize.y;
			float vEnd = ( textureLeftTop.y + textureSize.y ) / textureSize.y;

			// 必要であればUV座標を設定する処理を追加
		}

		//マテリアルCBufferの場所を設定
		commandList->SetGraphicsRootConstantBufferView(0, materialBuffer_->GetGPUVirtualAddress());

		// テクスチャのSRVのDescriptorTableを設定
		commandList->SetGraphicsRootDescriptorTable(2, particleSetup_->GetSrvSetup()->GetSRVGPUDescriptorHandle(group.second.srvIndex));

		// インスタンシングデータのSRVのDescriptorTableを設定
		commandList->SetGraphicsRootDescriptorTable(1, particleSetup_->GetSrvSetup()->GetSRVGPUDescriptorHandle(group.second.instancingSrvIndex));

		// Draw Call (インスタンシング描画)
		commandList->DrawInstanced(6, group.second.instanceCount, 0, 0);

		// インスタンスカウントをリセット
		group.second.instanceCount = 0;
	}

}

///=============================================================================
///						エミッター
void Particle::Emit(const std::string name, const Vector3& position, uint32_t count) {
	if(particleGroups.find(name) == particleGroups.end()) {
		// パーティクルグループが存在しない場合はエラーを出力して終了
		assert("Specified particle group does not exist!");

	}

	// 指定されたパーティクルグループが存在する場合、そのグループにパーティクルを追加
	ParticleGroup& group = particleGroups[name];

	// すでにkNumMaxInstanceに達している場合、新しいパーティクルの追加をスキップする
	if(group.particleList.size() >= count) {
		return;
	}

	// 指定された数のパーティクルを生成して追加
	for(uint32_t i = 0; i < count; ++i) {
		/*Particle newParticle = MakeNewParticle(randomEngine, position);
		group.particleList.push_back(newParticle);*/
		group.particleList.push_back(CreateNewParticle(randomEngine_, position));
	}
}

///=============================================================================
///						パーティクルグループ
void Particle::CreateParticleGroup(const std::string& name, const std::string& textureFilePath, uint32_t maxInstanceCount) {
	// 登録済みの名前かチェックして assert
	bool nameExists = false;
	for(auto it = particleGroups.begin(); it != particleGroups.end(); ++it) {
		if(it->second.materialFilePath == name) {
			nameExists = true;
			break;
		}
	}
	if(nameExists) {
		assert(false && "Particle group with this name already exists!");
	}

	// 新たなパーティクルグループを作成
	ParticleGroup newGroup;
	newGroup.materialFilePath = textureFilePath;

	// テクスチャのSRVインデックスを取得して設定
	TextureManager::GetInstance()->LoadTexture(textureFilePath);

	newGroup.srvIndex = TextureManager::GetInstance()->GetTextureIndex(textureFilePath);

	// テクスチャサイズを取得
	const DirectX::TexMetadata& metadata = TextureManager::GetInstance()->GetMetadata(textureFilePath);
	Vector2 textureSize = { static_cast<float>( metadata.width ), static_cast<float>( metadata.height )};
	// カスタムサイズが指定されているかどうか
	//newGroup.textureSize = textureSize;

	// サイズを設定（指定があればそれを使用、なければテクスチャサイズを使用）
	if (customTextureSize.x > 0.0f && customTextureSize.y > 0.0f) {
		newGroup.textureSize = customTextureSize;
	}
	else {
		newGroup.textureSize = textureSize;
	}

	//// テクスチャサイズを設定
	//AdjustTextureSize(newGroup, textureFilePath);

	// インスタンシング用リソースの生成
	//newGroup.instancingResource =
	//	dxCommon_->CreateBufferResource(sizeof(ParticleForGPU) * kNumMaxInstance);
	newGroup.instancingResource =
		particleSetup_->
		GetDXManager()->
		CreateBufferResource(sizeof(ParticleForGPU) * kNumMaxInstance);

	newGroup.instancingResource->Map(0, nullptr, reinterpret_cast<void**>( &newGroup.instancingDataPtr ));
	for(uint32_t index = 0; index < kNumMaxInstance; ++index) {
		newGroup.instancingDataPtr[index].WVP = Identity4x4();
		newGroup.instancingDataPtr[index].World = Identity4x4();
		//newGroup.instancingDataPtr[index].color = Vector4(1.0f, 1.0f, 1.0f, 1.0f);
	}

	// 最大インスタンシング用リソースの生成
	//InstancingMaxResource();

	// インスタンシング用SRVを確保してSRVインデックスを記録
	//newGroup.instancingSrvIndex =　srvManager_->Allocate() + 1;
	newGroup.instancingSrvIndex = particleSetup_->GetSrvSetup()->Allocate() + 1;
	// 作成したSRVをインスタンシング用リソースに設定
	//srvManager_->CreateSRVforStructuredBuffer(newGroup.instancingSrvIndex, newGroup.instancingResource.Get(), kNumMaxInstance, sizeof(ParticleForGPU));
	particleSetup_->GetSrvSetup()->CreateSRVStructuredBuffer(newGroup.instancingSrvIndex, newGroup.instancingResource.Get(), kNumMaxInstance, sizeof(ParticleForGPU));

	// パーティクルグループをリストに追加
	particleGroups.emplace(name, newGroup);

	// マテリアルデータの初期化
	CreateMaterialData();

	// 新しいブレンドモードを設定
	//blendMode_ = blendMode;
	//GraphicsPipelineState(blendMode);  // 再生成
}

///=============================================================================
///						静的メンバ関数
///--------------------------------------------------------------
///						 頂点データの作成
void Particle::CreateVertexData() {
	modelData_.vertices.push_back(VertexData{ .position = {1.0f, 1.0f, 0.0f, 1.0f}, .texCoord = {0.0f, 0.0f}, .normal = {0.0f, 0.0f, 1.0f} });
	modelData_.vertices.push_back(VertexData{ .position = {-1.0f, 1.0f, 0.0f, 1.0f}, .texCoord = {1.0f, 0.0f}, .normal = {0.0f, 0.0f, 1.0f} });
	modelData_.vertices.push_back(VertexData{ .position = {1.0f, -1.0f, 0.0f, 1.0f}, .texCoord = {0.0f, 1.0f}, .normal = {0.0f, 0.0f, 1.0f} });
	modelData_.vertices.push_back(VertexData{ .position = {1.0f, -1.0f, 0.0f, 1.0f}, .texCoord = {0.0f, 1.0f}, .normal = {0.0f, 0.0f, 1.0f} });
	modelData_.vertices.push_back(VertexData{ .position = {-1.0f, 1.0f, 0.0f, 1.0f}, .texCoord = {1.0f, 0.0f}, .normal = {0.0f, 0.0f, 1.0f} });
	modelData_.vertices.push_back(VertexData{ .position = {-1.0f, -1.0f, 0.0f, 1.0f}, .texCoord = {1.0f, 1.0f}, .normal = {0.0f, 0.0f, 1.0f} });
}

///--------------------------------------------------------------
///						 頂点バッファビューの作成
void Particle::CreateVertexBufferView() {
	//========================================
	// 1.頂点バッファの作成
	vertexBuffer_ = particleSetup_->GetDXManager()->CreateBufferResource(sizeof(VertexData) * modelData_.vertices.size());

	//========================================
	// 2.頂点バッファビューの作成
	//リソースの先頭のアドレスから使う
	vertexBufferView_.BufferLocation = vertexBuffer_->GetGPUVirtualAddress();
	//使用するリソースのサイズは頂点のサイズ
	vertexBufferView_.SizeInBytes = UINT(sizeof(VertexData) * modelData_.vertices.size());
	//1頂点あたりのサイズ
	vertexBufferView_.StrideInBytes = sizeof(VertexData);
}

///=============================================================================
///						マテリアルデータの作成
void Particle::CreateMaterialData() {
	// マテリアル用のリソースを作成
	materialBuffer_ = particleSetup_->GetDXManager()->CreateBufferResource(sizeof(Material));

	//書き込むためのアドレスを取得
	materialBuffer_->Map(0, nullptr, reinterpret_cast<void**>( &materialData_ ));
	materialData_->color = Vector4(1.0f, 1.0f, 1.0f, 1.0f);
	//SpriteはLightingしないのfalseを設定する
	materialData_->enableLighting = false;
	materialData_->uvTransform = Identity4x4();
}

///=============================================================================
///						新しいパーティクルを生成
// Particle.cpp
ParticleStr Particle::CreateNewParticle(std::mt19937& randomEngine, const Vector3& position) {
	// カラーと寿命のランダム分布
	std::uniform_real_distribution<float> distColor(colorRange_.min, colorRange_.max);
	std::uniform_real_distribution<float> distTime(lifetimeRange_.min, lifetimeRange_.max);

	// 速度のランダム分布
	std::uniform_real_distribution<float> distSpeed(velocityRange_.min, velocityRange_.max);

	// 新たなパーティクルの生成
	ParticleStr particle;

	particle.transform.scale = { 1.0f, 1.0f, 1.0f };
	particle.transform.rotate = { 0.0f, 0.0f, 0.0f };

	// 初期位置をエミッターの位置に設定
	particle.transform.translate = position;

	// ランダムな方向ベクトルの生成（球面上のランダムな点）
	std::uniform_real_distribution<float> distAngle(0.0f, 1.0f);
	float z = distAngle(randomEngine) * 2.0f - 1.0f; // z ∈ [-1, 1]
	float theta = distAngle(randomEngine) * 2.0f * std::numbers::pi_v<float>; // θ ∈ [0, 2π]
	float r = std::sqrt(1.0f - z * z);
	float x = r * std::cos(theta);
	float y = r * std::sin(theta);

	Vector3 direction = { x, y, z }; // 方向ベクトル

	// 速度を設定
	float speed = distSpeed(randomEngine);

	// 初期速度を設定
	particle.velocity = MultiplyVec3(speed, direction);

	// カラーと寿命を設定
	particle.color = { distColor(randomEngine), distColor(randomEngine), distColor(randomEngine), 1.0f };
	particle.lifeTime = distTime(randomEngine);
	particle.currentTime = 0.0f;

	return particle;
}


