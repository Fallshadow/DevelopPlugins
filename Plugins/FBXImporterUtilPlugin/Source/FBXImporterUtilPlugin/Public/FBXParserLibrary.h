// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "fbxsdk.h"
#include "FBXParserLibrary.generated.h"

/**
 * 
 */
UCLASS()
class FBXIMPORTERUTILPLUGIN_API UFBXParserLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

private:
	static  FbxManager* CreateFBXManager();
	static  void DestroyFBXManager();
	// 全局静态变量：存储复用的 FBX 管理器（防止重复调用创建多个实例）
	static FbxManager* GFBXManagerInstance;
};
