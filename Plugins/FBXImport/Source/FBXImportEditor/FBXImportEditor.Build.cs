using UnrealBuildTool;

public class FBXImportEditor : ModuleRules
{
    public FBXImportEditor(ReadOnlyTargetRules Target) : base(Target)
    {
        // PCH配置：使用显式或共享PCH，提升编译速度
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

        // 公共依赖模块（外部模块可访问，如项目核心模块、其他插件）
        PublicDependencyModuleNames.AddRange(new string[] {
            "Core",               // UE核心模块（基础类型、工具函数）
            "CoreUObject",        // 核心对象模块（UObject、蓝图、结构体）
            "Engine",             // 引擎模块（Actor、Component、StaticMesh）
            "UnrealEd",           // 编辑器模块（导入器、资源管理接口）包含 FBX SDK模块（读取FBX自定义属性、用户数据层）
            "AssetTools",         // 资源工具模块（创建/注册UE资源）
            "MeshDescription",    // 网格描述模块（生成可视化三角形网格）
            "AssetRegistry"       // 资源注册模块（将生成的Actor/网格注册到UE）
        });

        // 私有依赖模块（仅当前插件内部使用，外部不可见）
        PrivateDependencyModuleNames.AddRange(new string[] {
            // "Slate",              // Slate UI模块（若导入器需添加UI交互）
            // "SlateCore",          // Slate核心模块（UI基础组件）
            "EditorFramework"     // 编辑器框架模块（导入器工厂接口依赖）
        });

        // 启用C++17（FBX SDK部分功能需C++17支持，UE5.3+默认兼容）
        CppStandard = CppStandardVersion.Cpp17;

        // // 可选：禁用特定编译器警告（如之前的C4172）
        // if (Target.Platform == UnrealTargetPlatform.Win64)
        // {
        //     PublicCompileOptions.Add("/wd4172"); // 仅Windows平台生效
        // }

        // // 可选：启用编辑器模块编译（确保插件在编辑器模式下正常运行）
        // bBuildEditor = true;
        // bEditorOnly = true; // 标记为仅编辑器插件，打包游戏时不包含
    }
}