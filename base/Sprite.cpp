///===================================================================///
///							Spriteクラス.cpp 
///===================================================================///
#include "Sprite.h"
#include "SpriteManager.h"
#include <AffineCalc.h>
#include <RendPipeLine.h>

///====================初期化====================///
void Sprite::Initialize(SpriteManager* spriteManager) {
	//引数で受け取ってメンバ変数に記録する
	this->spriteManager_ = spriteManager;

	//頂点バッファの作成
	CreateVertexBuffer();
	//インデックスバッファの作成
	CreateIndexBuffer();
	//マテリアルバッファの作成
	CreateMaterialBuffer();
	//トランスフォーメーションマトリックスバッファの作成
	CreateTransformationMatrixBuffer();
}

///====================更新====================///
//NOTE:引数としてローカル行列とビュー行列を持ってくること
void Sprite::Update(Matrix4x4 viewMatrix) {
	//座標の反映
	transform_.translate = { position_.x,position_.y,0.0f };
	//回転の反映
	transform_.rotate = { 0.0f,0.0f,rotation_ };

	// スプライトの変換行列を作成
	Matrix4x4 worldMatrixSprite = MakeAffineMatrix(transform_.scale, transform_.rotate, transform_.translate);
	transformationMatrixData_->World = worldMatrixSprite;

	// 正射影行列の作成
	Matrix4x4 projectionMatrixSprite = MakeOrthographicMatrix(
		0.0f, 0.0f,
		float(spriteManager_->GetDXManager()->GetWinApp().GetWindowWidth()),
		float(spriteManager_->GetDXManager()->GetWinApp().GetWindowHeight()),
		0.0f, 100.0f);

	// ワールド・ビュー・プロジェクション行列を計算
	Matrix4x4 worldViewProjectionMatrixSprite = Multiply4x4(worldMatrixSprite, Multiply4x4(viewMatrix, projectionMatrixSprite));
	transformationMatrixData_->WVP = worldViewProjectionMatrixSprite;
}

///====================描画====================///
void Sprite::Draw(D3D12_GPU_DESCRIPTOR_HANDLE textureHandle) {
	///2D描画
	//NOTE:Material用のCBuffer(色)とSRV(Texture)は3Dの三角形と同じものを使用。無駄を省け。
	//NOTE:同じものを使用したな？気をつけろ、別々の描画をしたいときは個別のオブジェクトとして宣言し直せ。
	// まず、描画時に使うバッファが有効か確認する
	if (!vertexBuffer_ || !indexBuffer_ || !materialBuffer_ || !transfomationMatrixBuffer_) {
		throw std::runtime_error("One or more buffers are not initialized.");
		return;
	}

	// コマンドリスト取得
	Microsoft::WRL::ComPtr <ID3D12GraphicsCommandList> commandList = spriteManager_->GetDXManager()->GetCommandList();

	// 頂点バッファの設定
	commandList->IASetVertexBuffers(0, 1, &vertexBufferView_);

	// インデックスバッファの設定
	commandList->IASetIndexBuffer(&indexBufferView_);

	// プリミティブのトポロジーを設定（通常のトライアングルリスト）
	commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// Material の設定（定数バッファビューにセット）
	commandList->SetGraphicsRootConstantBufferView(0, materialBuffer_->GetGPUVirtualAddress());

	// TransformationMatrix の設定（定数バッファビューにセット）
	commandList->SetGraphicsRootConstantBufferView(1, transfomationMatrixBuffer_->GetGPUVirtualAddress());

	// テクスチャの設定（SRVハンドルを設定）
	commandList->SetGraphicsRootDescriptorTable(2, textureHandle);

	// 描画コール
	commandList->DrawIndexedInstanced(6, 1, 0, 0, 0);
}


///----------------------------------------------------///
///						プライベート
///----------------------------------------------------///
///====================頂点バッファの作成====================///
void Sprite::CreateVertexBuffer() {
	// 頂点データ用のバッファリソースを作成
	vertexBuffer_ = spriteManager_->GetDXManager()->CreateBufferResource(sizeof(VertexData) * 6);

	if (!vertexBuffer_) {
		throw std::runtime_error("Failed to Create Vertex Buffer");
		return;
	}

	// バッファビューの設定
	vertexBufferView_.BufferLocation = vertexBuffer_->GetGPUVirtualAddress();
	vertexBufferView_.SizeInBytes = sizeof(VertexData) * 6;
	vertexBufferView_.StrideInBytes = sizeof(VertexData);

	// リソースにデータを書き込む
	vertexBuffer_->Map(0, nullptr, reinterpret_cast<void**>( &vertexData_ ));

	// 2つの三角形の頂点データを設定
	vertexData_[0].position = { 0.0f, 360.0f, 0.0f, 1.0f };
	vertexData_[0].texCoord = { 0.0f, 1.0f };
	vertexData_[0].normal = { 0.0f, 0.0f, -1.0f };

	vertexData_[1].position = { 0.0f, 0.0f, 0.0f, 1.0f };
	vertexData_[1].texCoord = { 0.0f, 0.0f };
	vertexData_[1].normal = { 0.0f, 0.0f, -1.0f };

	vertexData_[2].position = { 640.0f, 360.0f, 0.0f, 1.0f };
	vertexData_[2].texCoord = { 1.0f, 1.0f };
	vertexData_[2].normal = { 0.0f, 0.0f, -1.0f };

	vertexData_[3].position = { 0.0f, 0.0f, 0.0f, 1.0f };
	vertexData_[3].texCoord = { 0.0f, 0.0f };
	vertexData_[3].normal = { 0.0f, 0.0f, -1.0f };

	vertexData_[4].position = { 640.0f, 0.0f, 0.0f, 1.0f };
	vertexData_[4].texCoord = { 1.0f, 0.0f };
	vertexData_[4].normal = { 0.0f, 0.0f, -1.0f };

	vertexData_[5].position = { 640.0f, 360.0f, 0.0f, 1.0f };
	vertexData_[5].texCoord = { 1.0f, 1.0f };
	vertexData_[5].normal = { 0.0f, 0.0f, -1.0f };
}

///====================インデックスバッファの作成====================///
void Sprite::CreateIndexBuffer() {
	// インデックスデータ用のバッファリソースを作成
	indexBuffer_ = spriteManager_->GetDXManager()->CreateBufferResource(sizeof(uint32_t) * 6);

	if (!indexBuffer_) {
		throw std::runtime_error("Failed to Create Index Buffer");
		return;
	}

	// バッファビューの設定
	indexBufferView_.BufferLocation = indexBuffer_->GetGPUVirtualAddress();
	indexBufferView_.SizeInBytes = sizeof(uint32_t) * 6;
	indexBufferView_.Format = DXGI_FORMAT_R32_UINT;

	// リソースにデータを書き込む
	indexBuffer_->Map(0, nullptr, reinterpret_cast<void**>( &indexData_ ));

	// インデックスデータを設定
	indexData_[0] = 0;
	indexData_[1] = 1;
	indexData_[2] = 2;
	indexData_[3] = 3;
	indexData_[4] = 4;
	indexData_[5] = 5;
}

///====================マテリアルバッファの作成====================///
void Sprite::CreateMaterialBuffer() {
	// マテリアルデータ用のバッファリソースを作成
	materialBuffer_ = spriteManager_->GetDXManager()->CreateBufferResource(sizeof(Material));
	//書き込むためのアドレス取得
	materialBuffer_->Map(0, nullptr, reinterpret_cast<void**>( &materialData_ ));
	//マテリアルデータ書き込み用変数(赤色を書き込み)
	Material materialSprite = { {1.0f, 1.0f, 1.0f, 1.0f},false };
	//UVトランスフォーム用の単位行列の書き込み
	materialSprite.uvTransform = Identity4x4();
	//マテリアルデータの書き込み
	*materialData_ = materialSprite;
}

///====================トランスフォーメーションマトリックスバッファの作成====================///
void Sprite::CreateTransformationMatrixBuffer() {
	//wvp用のリソースを作る
	transfomationMatrixBuffer_ = spriteManager_->GetDXManager()->CreateBufferResource(sizeof(TransformationMatrix));
	//書き込むためのアドレスを取得
	transfomationMatrixBuffer_->Map(0, nullptr, reinterpret_cast<void**>( &transformationMatrixData_ ));
	//書き込み用変数
	TransformationMatrix transformationMatrix;
	//単位行列の書き込み
	transformationMatrix.WVP = Identity4x4();
	//トランスフォーメーションマトリックスの書き込み
	*transformationMatrixData_ = transformationMatrix;
}
