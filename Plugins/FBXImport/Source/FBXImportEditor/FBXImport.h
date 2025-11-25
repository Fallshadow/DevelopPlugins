#pragma once
#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"

// 插件模块类（用于初始化和卸载插件）
class FBXImportModule : public IModuleInterface
{
public:
    // 模块加载时调用（插件启动）
    virtual void StartupModule() override;

    // 模块卸载时调用（插件关闭）
    virtual void ShutdownModule() override;

    // 静态方法：获取模块实例（便于其他代码调用）
    static inline FBXImporterModule& Get()
    {
        return FModuleManager::LoadModuleChecked<FBXImporterModule>("FBXImport");
    }

    // 静态方法：判断模块是否已加载
    static inline bool IsAvailable()
    {
        return FModuleManager::Get().IsModuleLoaded("FBXImport");
    }
};