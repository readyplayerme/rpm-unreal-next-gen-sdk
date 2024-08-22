#include "EditorAssetLoader.h"

#include "AssetToolsModule.h"
#include "glTFRuntimeAssetActor.h"
#include "glTFRuntimeFunctionLibrary.h"
#include "RpmActor.h"
#include "TransientObjectSaverLibrary.h"
#include "UEditorAssetNamer.h"
#include "Animation/SkeletalMeshActor.h"
#include "AssetRegistry/AssetRegistryModule.h"
#include "Factories/PhysicsAssetFactory.h"

FEditorAssetLoader::FEditorAssetLoader()
{
}

FEditorAssetLoader::~FEditorAssetLoader()
{
}

void FEditorAssetLoader::OnAssetDownloadComplete(FString FilePath, UglTFRuntimeAsset* gltfAsset, bool bWasSuccessful,
                                                 FString LoadedAssetId)
{
	if (bWasSuccessful)
	{
		//LoadGltfAssetToWorld(gltfAsset);
		gltfAsset->AddToRoot();
		SaveAsUAsset(gltfAsset, LoadedAssetId);
		gltfAsset->RemoveFromRoot();
	}
}

void FEditorAssetLoader::SaveAsUAsset(UglTFRuntimeAsset* gltfAsset, FString LoadedAssetId)
{
	const FglTFRuntimeSkeletonConfig SkeletonConfig = FglTFRuntimeSkeletonConfig();
	USkeleton* Skeleton = gltfAsset->LoadSkeleton(0, SkeletonConfig);

	FglTFRuntimeSkeletalMeshConfig meshConfig = FglTFRuntimeSkeletalMeshConfig();
	meshConfig.Skeleton = Skeleton;

	USkeletalMesh* skeletalMesh = gltfAsset->LoadSkeletalMeshRecursive(TEXT(""), {}, meshConfig);
	skeletalMesh->AddToRoot();
	skeletalMesh->SetSkeleton(Skeleton);
	Skeleton->SetPreviewMesh(skeletalMesh);

	const FString CoreAssetPath = TEXT("/Game/ReadyPlayerMe/") + LoadedAssetId + "/";
	const FString SkeletonAssetPath = CoreAssetPath + LoadedAssetId + TEXT("_Skeleton");
	const FString SkeletalMeshAssetPath = CoreAssetPath + LoadedAssetId + TEXT("_SkeletalMesh");

	const auto NameGenerator = NewObject<UUEditorAssetNamer>();
	NameGenerator->SetPath(CoreAssetPath);

	FTransientObjectSaverMaterialNameGenerator MaterialNameGeneratorDelegate;
	MaterialNameGeneratorDelegate.BindUFunction(NameGenerator, FName("GenerateMaterialName"));
	FTransientObjectSaverTextureNameGenerator TextureNameGeneratorDelegate;
	TextureNameGeneratorDelegate.BindUFunction(NameGenerator, FName("GenerateTextureName"));

	UTransientObjectSaverLibrary::SaveTransientSkeletalMesh(skeletalMesh, SkeletalMeshAssetPath,
	                                                        SkeletonAssetPath,
	                                                        TEXT(""),
	                                                        MaterialNameGeneratorDelegate,
	                                                        TextureNameGeneratorDelegate);

	UE_LOG(LogTemp, Log, TEXT("Character model saved: %s"), *LoadedAssetId);
}

void FEditorAssetLoader::LoadGLBFromURLWithId(const FString& URL, FString LoadedAssetId)
{
	OnAssetDownloaded.BindLambda(
		[LoadedAssetId, this](FString FilePath, UglTFRuntimeAsset* gltfAsset,
		                      bool bWasSuccessful)
		{
			if (!gltfAsset)
			{
				UE_LOG(LogTemp, Log, TEXT("No gltf asset"));
				return;
			}
			OnAssetDownloadComplete(FilePath, gltfAsset, bWasSuccessful, LoadedAssetId);
		});
	LoadGLBFromURL(URL);
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
		ARpmActor* NewActor = EditorWorld->SpawnActorDeferred<ARpmActor>(ARpmActor::StaticClass(), Transform);
		if (NewActor)
		{
			NewActor->SetFlags(RF_Transient);

			if (SkeletonToCopy)
			{
				NewActor->SkeletalMeshConfig.SkeletonConfig.CopyRotationsFrom = SkeletonToCopy;
			}
			NewActor->FinishSpawning(Transform);
			NewActor->DispatchBeginPlay();
			GEditor->SelectNone(true, true, true);
			GEditor->SelectActor(NewActor, true, true, false, true);

			// Register the actor in the editor world and update the editor
			GEditor->SelectActor(NewActor, true, true);
			GEditor->EditorUpdateComponents();

			NewActor->LoadglTFAsset(gltfAsset);
			UE_LOG(LogTemp, Log, TEXT("Successfully loaded GLB asset into the editor world"));
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("Failed to spawn AglTFRuntimeAssetActor in the editor world"));
		}
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to load GLB asset from file"));
	}
}
