// Fill out your copyright notice in the Description page of Project Settings.


#include "CreateQuadLightMesh.h"
#include "CoreMinimal.h"
#include "StaticMesh.h"
#include "StaticMeshBuilder.h"
#include "Engine/StaticMesh.h"
#include "Misc/FileHelper.h"
#include "Serialization/BufferArchive.h"

// Sets default values for this component's properties
UCreateQuadLightMesh::UCreateQuadLightMesh()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}

UStaticMesh* UCreateQuadLightMesh::CreateLightQuadStaticMesh_AlignOrigin(const FString& MeshName, const TArray<FVector>& SrcVertices)
{
	// 1. 创建静态网格资源
	// GetTransientPackage()：临时包（后续会保存到 Content 目录）；
	// RF_Public | RF_Standalone：资源标记（公开、独立）；
	UStaticMesh* LightMesh = NewObject<UStaticMesh>(GetTransientPackage(), FName(*MeshName), RF_Public | RF_Standalone);
	// check(LightMesh)：UE 的断言，确保网格创建成功
	check(LightMesh);

	// 2. 初始化RawMesh
	FStaticMeshSourceModel SourceModel;
	SourceModel.BuildSettings.bRecomputeNormals = false; // 与原代码一致，不自动计算法线
	SourceModel.BuildSettings.bRecomputeTangents = false;
	SourceModel.RawMesh = MakeShared<FRawMesh>();
	LightMesh->AddSourceModel(SourceModel);
	FRawMesh& RawMesh = *SourceModel.RawMesh;

	return nullptr;
}


// Called when the game starts
void UCreateQuadLightMesh::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


// Called every frame
void UCreateQuadLightMesh::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

