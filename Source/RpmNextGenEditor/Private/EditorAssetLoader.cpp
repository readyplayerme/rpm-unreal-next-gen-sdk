#include "EditorAssetLoader.h"

#include "glTFRuntimeAssetActor.h"
#include "glTFRuntimeFunctionLibrary.h"
#include "TransientObjectSaverLibrary.h"
#include "Animation/SkeletalMeshActor.h"

FEditorAssetLoader::FEditorAssetLoader()
{
	OnAssetDownloaded.BindRaw(this, &FEditorAssetLoader::OnAssetDownloadComplete);
}

FEditorAssetLoader::~FEditorAssetLoader()
{
}

void FEditorAssetLoader::OnAssetDownloadComplete(FString FilePath, UglTFRuntimeAsset* gltfAsset, bool bWasSuccessful)
{
	if(bWasSuccessful)
	{
		LoadGltfAssetToWorld(gltfAsset);
		SaveAsUAsset(gltfAsset, FilePath);
	}
}


void FEditorAssetLoader::LoadGltfAssetToWorld(UglTFRuntimeAsset* gltfAsset)
{
	if (!GEditor) 
	{
		UE_LOG(LogTemp, Error, TEXT("GEditor is not available."));
		return;
	}

	// Get the editor world context
	UWorld* EditorWorld = GEditor->GetEditorWorldContext().World();
	if (!EditorWorld)
	{
		UE_LOG(LogTemp, Error, TEXT("No valid editor world found."));
		return;
	}

	if (gltfAsset)
	{
		FTransform Transform = FTransform::Identity;
		// Spawn an actor of type AglTFRuntimeAssetActor in the editor world
		AglTFRuntimeAssetActor* NewActor = EditorWorld->SpawnActorDeferred<AglTFRuntimeAssetActor>(AglTFRuntimeAssetActor::StaticClass(), Transform);
		if (NewActor)
		{
			NewActor->SetFlags(RF_Transient);
			NewActor->Asset = gltfAsset;
			if(SkeletonToCopy)
			{
				NewActor->SkeletalMeshConfig.SkeletonConfig.CopyRotationsFrom = SkeletonToCopy;
			}
			NewActor->bAllowSkeletalAnimations = false;
			NewActor->bAllowNodeAnimations = false;
			NewActor->StaticMeshConfig.bGenerateStaticMeshDescription = true;
			NewActor->FinishSpawning(Transform);
			NewActor->DispatchBeginPlay();
			GEditor->SelectNone(true, true, true);
			GEditor->SelectActor(NewActor, true, true, false, true);

			// Register the actor in the editor world and update the editor
			GEditor->SelectActor(NewActor, true, true);
			GEditor->EditorUpdateComponents();

			UE_LOG(LogTemp, Log, TEXT("Successfully loaded GLB asset into the editor world"));
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("Failed to spawn AglTFRuntimeAssetActor in the editor world"));
		}
 //return;


	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to load GLB asset from file"));
	}
}

void FEditorAssetLoader::SaveAsUAsset(UglTFRuntimeAsset* gltfAsset, FString Path)
{
	FglTFRuntimeSkeletonConfig skeletonConfig = FglTFRuntimeSkeletonConfig();
	if(SkeletonToCopy)
	{
		skeletonConfig.CopyRotationsFrom = SkeletonToCopy;
	}
	USkeleton* Skeleton = gltfAsset->LoadSkeleton(0, skeletonConfig);
	UTransientObjectSaverLibrary::SaveTransientSkeleton(Skeleton,TEXT("/Game/ReadyPlayerMe/TestSkeleton"));
	// FglTFRuntimeSkeletalMeshConfig meshConfig = FglTFRuntimeSkeletalMeshConfig();
	// USkeletalMesh* skeletalMesh = gltfAsset->LoadSkeletalMeshRecursive("", {}, meshConfig);
	// const FTransientObjectSaverMaterialNameGenerator& MaterialNameGenerator = FTransientObjectSaverMaterialNameGenerator();
	// const FTransientObjectSaverTextureNameGenerator& TextureNameGenerator = FTransientObjectSaverTextureNameGenerator(); 
	// UTransientObjectSaverLibrary::SaveTransientSkeletalMesh(skeletalMesh, TEXT("/Game/ReadyPlayerMe/TestSkeletalMesh"), TEXT("/Game/ReadyPlayerMe/TestSkeleton"), TEXT("/Game/ReadyPlayerMe/TestPhysicsAsset"), MaterialNameGenerator, TextureNameGenerator);
}
