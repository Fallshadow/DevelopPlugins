#include "FBXParserLibrary.h"

// 全局静态变量：存储复用的 FBX 管理器（防止重复调用创建多个实例）
FbxManager* UFBXParserLibrary::GFBXManagerInstance = nullptr;



FbxManager* UFBXParserLibrary::CreateFBXManager() {
    // 1. 解决重复调用问题：先检查是否已创建，已创建则直接返回
    if (GFBXManagerInstance) {
        return GFBXManagerInstance;
    }

    // 创建 FBX 管理器核心实例
    GFBXManagerInstance = FbxManager::Create();
    if (!GFBXManagerInstance) {
        UE_LOG(LogTemp, Error, TEXT("Failed to create FBX Manager!"));
        return nullptr;
    }

    // 2. 检查 FbxIOSettings 创建结果（避免空指针传入 SetIOSettings）
    FbxIOSettings* IOSettings = FbxIOSettings::Create(GFBXManagerInstance, IOSROOT);
    if (!IOSettings) {
        UE_LOG(LogTemp, Error, TEXT("Failed to create FBX IO Settings!"));
        // IO配置创建失败，需销毁已创建的管理器，避免内存泄漏
        GFBXManagerInstance->Destroy();
        return nullptr;
    }

    GFBXManagerInstance->SetIOSettings(IOSettings);

    // 将创建成功的管理器赋值给全局变量，供后续复用
    GFBXManagerInstance = GFBXManagerInstance;
    return GFBXManagerInstance;
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

TArray<FParsedMeshData> UFBXParserLibrary::ParseFBXFile(const FString& FBXFilePath)
{
    TArray<FParsedMeshData> parsedMeshDatas;
    if (!FPaths::FileExists(FBXFilePath)) {
        UE_LOG(LogTemp, Error, TEXT("FBX File not found:%s"), *FBXFilePath);
        return parsedMeshDatas;
    }

    CreateFBXManager();
    if (!GFBXManagerInstance) return parsedMeshDatas;

    FbxImporter* FBXImporter = FbxImporter::Create(GFBXManagerInstance, "CustomFBXImporter");
    // 将 UE 的 FString（TCHAR 编码）转为 UTF8 编码（FBX SDK 要求的编码格式）；
    if (!FBXImporter->Initialize(TCHAR_TO_UTF8(*FBXFilePath), -1, GFBXManagerInstance->GetIOSettings())) {
        UE_LOG(LogTemp, Error, TEXT("FBX Import Failed: %s"), UTF8_TO_TCHAR(FBXImporter->GetStatus().GetErrorString()));
        FBXImporter->Destroy();
        return parsedMeshDatas;
    }

    FbxScene* FBXScene = FbxScene::Create(GFBXManagerInstance, "ImportedFBXScene");
    if (!FBXImporter->Import(FBXScene)) {
        UE_LOG(LogTemp, Error, TEXT("Failed to import FBX into scene"));
        FBXImporter->Destroy();
        FBXScene->Destroy();
        return parsedMeshDatas;
    }

    // 递归遍历所有节点
    auto TraverseFBXNodes = [&](FbxNode* Node, auto&& Traverse) -> void
        {
            if (!Node) return;
            FParsedMeshData MeshData = ParseSingleFBXMeshes(Node);
            if (MeshData.Vertices.Num() > 0) {
                parsedMeshDatas.Add(MeshData);
            }
            for (int32 i = 0; i < Node->GetChildCount(); i++) {
                Traverse(Node->GetChild(i), Traverse);
            }
        };
    TraverseFBXNodes(FBXScene->GetRootNode(), TraverseFBXNodes);

    // 释放资源
    // 按 “创建逆序” 销毁 FBX 对象：导入器 → 场景 → IO 配置 → 管理器；
    FBXImporter->Destroy();
    FBXScene->Destroy();

    // 输出日志：记录解析完成，以及提取到的网格总数；
    UE_LOG(LogTemp, Display, TEXT("FBX Parse Completed! Total meshes: %d"), parsedMeshDatas.Num());

    return parsedMeshDatas;
}

FParsedMeshData UFBXParserLibrary::ParseSingleFBXMeshes(FbxNode* FBXNode) {
    FParsedMeshData ParsedData;
    if (!FBXNode) return ParsedData;

    FbxMesh* FBXMesh = FBXNode->GetMesh();
    if (!FBXMesh) {
        UE_LOG(LogTemp, Warning, TEXT("Node %s is not a mesh"), *FString(FBXNode->GetName()));
        return ParsedData;
    }

    // 网格名称 (FBX SDK 默认 UTF-8/UTF-16，UE FString 有特定编码转换规则)
    ParsedData.MeshName = FString(UTF8_TO_TCHAR(FBXNode->GetName()));

    // 解析顶点（FBX Y轴向上 → UE Z轴向上）
    // 获取 FBX 网格的 “控制点数组”——FBX 中 “控制点” 就是我们常说的 “顶点”，返回的是 FBX 自定义的四维向量数组（X/Y/Z/W，W 一般是 1.0）；
    FbxVector4* FBXVertices = FBXMesh->GetControlPoints();
    int32 VertexCount = FBXMesh->GetControlPointsCount();
    if (VertexCount <= 0) {
        UE_LOG(LogTemp, Warning, TEXT("Mesh %s has no vertices!"), *ParsedData.MeshName);
        return ParsedData;
    }

    // 预分配顶点数组内存（优化性能，避免动态扩容）
    ParsedData.Vertices.Reserve(VertexCount);
    // 循环遍历每个 FBX 顶点，核心是坐标系转换
    // FBX 坐标系：Y 轴向上（X: 右，Y: 上，Z: 前）；
    // UE 坐标系：Z 轴向上（X: 右，Y: 前，Z: 上）；
    // TODO：这里先不改变顺序试试
    for (int32 v = 0; v < VertexCount; v++) {
        FVector UEVertex(
            FBXVertices[v].mData[0],
            /*FBXVertices[v].mData[2],
            FBXVertices[v].mData[1]*/
            FBXVertices[v].mData[1],
            FBXVertices[v].mData[2]
        );
        ParsedData.Vertices.Add(UEVertex);
    }

    // 解析三角面（可选，若不需要索引可删除）
    // 获取 FBX 网格的 “面总数”（FBX 中 Polygon 可以是三角面、四边形等）
    int32 PolygonCount = FBXMesh->GetPolygonCount();
    ParsedData.Triangles.Reserve(PolygonCount * 3);
    for (int32 polyIndex = 0; polyIndex < PolygonCount; polyIndex++) {
        int32 PolygonVertexCount = FBXMesh->GetPolygonSize(polyIndex);
        for (int32 polyVertexIndex = 0; polyVertexIndex < PolygonVertexCount; polyVertexIndex++) {
            ParsedData.Triangles.Add(FBXMesh->GetPolygonVertex(polyIndex, polyVertexIndex));
        }
    }

    UE_LOG(LogTemp, Log, TEXT("Parsed mesh: %s | Vertices: %d"), *ParsedData.MeshName, ParsedData.Vertices.Num());
    return ParsedData;
}