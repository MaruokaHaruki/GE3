/*********************************************************************
 * \file   Line.cpp
 * \brief 
 * 
 * \author Harukichimaru
 * \date   January 2025
 * \note   
 *********************************************************************/
#include "Line.h"
#include "LineSetup.h"
#include "DirectXCore.h"

void Line::Initialize(LineSetup* lineSetup) {
    lineSetup_ = lineSetup;
    CreateVertexBuffer();
}

void Line::DrawLine(const Vector3& start, const Vector3& end, const Vector4& color) {
	vertices_.push_back({ start, color });
    vertices_.push_back({ end, color });
}

void Line::Draw() {
    if (vertices_.empty()) return;

    // Update vertex buffer
    void* pData;
    vertexBuffer_->Map(0, nullptr, &pData);
    memcpy(pData, vertices_.data(), sizeof(LineVertex) * vertices_.size());
    vertexBuffer_->Unmap(0, nullptr);

    // Set vertex buffer
    auto commandList = lineSetup_->GetDXManager()->GetCommandList();
    commandList->IASetVertexBuffers(0, 1, &vertexBufferView_);

    // Draw lines
    commandList->DrawInstanced(vertices_.size(), 1, 0, 0);

    // Clear vertices
    vertices_.clear();
}

void Line::CreateVertexBuffer() {
    auto device = lineSetup_->GetDXManager()->GetDevice();
    auto bufferSize = sizeof(LineVertex) * 1000; // Arbitrary size, adjust as needed

    D3D12_HEAP_PROPERTIES heapProps = {};
    heapProps.Type = D3D12_HEAP_TYPE_UPLOAD;

    D3D12_RESOURCE_DESC bufferDesc = {};
    bufferDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
    bufferDesc.Width = bufferSize;
    bufferDesc.Height = 1;
    bufferDesc.DepthOrArraySize = 1;
    bufferDesc.MipLevels = 1;
    bufferDesc.Format = DXGI_FORMAT_UNKNOWN;
    bufferDesc.SampleDesc.Count = 1;
    bufferDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

    device->CreateCommittedResource(
        &heapProps,
        D3D12_HEAP_FLAG_NONE,
        &bufferDesc,
        D3D12_RESOURCE_STATE_GENERIC_READ,
        nullptr,
        IID_PPV_ARGS(&vertexBuffer_)
    );

    vertexBufferView_.BufferLocation = vertexBuffer_->GetGPUVirtualAddress();
    vertexBufferView_.SizeInBytes = bufferSize;
    vertexBufferView_.StrideInBytes = sizeof(LineVertex);
}

