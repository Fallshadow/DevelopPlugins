#include "FBXParserLibrary.h"

// 全局静态变量：存储复用的 FBX 管理器（防止重复调用创建多个实例）
FbxManager* UFBXParserLibrary::GFBXManagerInstance = nullptr;

FbxManager* UFBXParserLibrary::CreateFBXManager() {
    // 1. 解决重复调用问题：先检查是否已创建，已创建则直接返回
    if (GFBXManagerInstance) {
        return GFBXManagerInstance;
    }

    // 创建 FBX 管理器核心实例
    FbxManager* FBXManager = FbxManager::Create();
    if (!FBXManager) {
        UE_LOG(LogTemp, Error, TEXT("Failed to create FBX Manager!"));
        return nullptr;
    }

    // 2. 检查 FbxIOSettings 创建结果（避免空指针传入 SetIOSettings）
    FbxIOSettings* IOSettings = FbxIOSettings::Create(FBXManager, IOSROOT);
    if (!IOSettings) {
        UE_LOG(LogTemp, Error, TEXT("Failed to create FBX IO Settings!"));
        // IO配置创建失败，需销毁已创建的管理器，避免内存泄漏
        FBXManager->Destroy();
        return nullptr;
    }

    FBXManager->SetIOSettings(IOSettings);

    // 将创建成功的管理器赋值给全局变量，供后续复用
    GFBXManagerInstance = FBXManager;
    return FBXManager;
}

void UFBXParserLibrary::DestroyFBXManager()
{
    if (GFBXManagerInstance) {
        // FBX SDK 父子对象机制：销毁管理器会自动销毁其创建的 IOSettings
        GFBXManagerInstance->Destroy();
        // 重置全局变量，避免野指针
        GFBXManagerInstance = nullptr;
        UE_LOG(LogTemp, Log, TEXT("FBX Manager destroyed successfully!"));
    }
}