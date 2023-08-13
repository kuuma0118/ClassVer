#include "ImGuiManager.h"

void ImGuiManager::Initialize(WinApp* winApp, DirectXCommon* directXCommon) {
	directXCommon_ = directXCommon;

	IMGUI_CHECKVERSION();

	ImGui::CreateContext();
	ImGui::StyleColorsDark();

	ImGui_ImplWin32_Init(winApp->GetHwnd());
	ImGui_ImplDX12_Init(directXCommon_->GetDevice(),
		directXCommon_->GetBackBufferCount(),
		directXCommon_->GetRtvDesc().Format,
		directXCommon_->GetSrvHeap(),
		directXCommon_->GetSrvHeap()->GetCPUDescriptorHandleForHeapStart(),
		directXCommon_->GetSrvHeap()->GetGPUDescriptorHandleForHeapStart());
}

void ImGuiManager::Finalize() {
	ImGui_ImplDX12_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();
}

void ImGuiManager::Begin() {
	ImGui_ImplDX12_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	ID3D12DescriptorHeap* descriptorHeaps[] = { directXCommon_->GetSrvHeap() };
	directXCommon_->GetCommandList()->SetDescriptorHeaps(1, descriptorHeaps);
}

void ImGuiManager::End() {
	ImGui::Render();
}

void ImGuiManager::Draw() {
	ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), directXCommon_->GetCommandList());
}