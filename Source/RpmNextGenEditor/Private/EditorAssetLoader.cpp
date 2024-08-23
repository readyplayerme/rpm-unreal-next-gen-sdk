#include "EditorAssetLoader.h"

#include "glTFRuntimeFunctionLibrary.h"
#include "RpmActor.h"
#include "TransientObjectSaverLibrary.h"
#include "AssetNameGenerator.h"
#include "glTFRuntimeAssetActor.h"

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
		gltfAsset->AddToRoot();
		auto SkeletalMesh = SaveAsUAsset(gltfAsset, LoadedAssetId);
		LoadAssetToWorldAsURpmActor(SkeletalMesh);
		gltfAsset->RemoveFromRoot();
	}
}

USkeletalMesh* FEditorAssetLoader::SaveAsUAsset(UglTFRuntimeAsset* GltfAsset, const FString& LoadedAssetId) const
{
	const FglTFRuntimeSkeletonConfig SkeletonConfig = FglTFRuntimeSkeletonConfig();
	USkeleton* Skeleton = GltfAsset->LoadSkeleton(0, SkeletonConfig);

	FglTFRuntimeSkeletalMeshConfig meshConfig = FglTFRuntimeSkeletalMeshConfig();
	meshConfig.Skeleton = Skeleton;

	USkeletalMesh* skeletalMesh = GltfAsset->LoadSkeletalMeshRecursive(TEXT(""), {}, meshConfig);
	skeletalMesh->SetSkeleton(Skeleton);
	Skeleton->SetPreviewMesh(skeletalMesh);

	const FString CoreAssetPath = FString::Printf(TEXT("/Game/ReadyPlayerMe/%s/"), *LoadedAssetId);
	const FString SkeletonAssetPath = FString::Printf(TEXT("%s%s_Skeleton"), *CoreAssetPath, *LoadedAssetId);
	const FString SkeletalMeshAssetPath = FString::Printf(TEXT("%s%s_SkeletalMesh"), *CoreAssetPath, *LoadedAssetId);

	const auto NameGenerator = NewObject<UAssetNameGenerator>();
	NameGenerator->SetPath(CoreAssetPath);

	UTransientObjectSaverLibrary::SaveTransientSkeletalMesh(skeletalMesh, SkeletalMeshAssetPath,
	                                                        SkeletonAssetPath,
	                                                        TEXT(""),
	                                                        NameGenerator->MaterialNameGeneratorDelegate,
	                                                        NameGenerator->TextureNameGeneratorDelegate);

	UE_LOG(LogTemp, Log, TEXT("Character model saved: %s"), *LoadedAssetId);
	return skeletalMesh;
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

void FEditorAssetLoader::LoadAssetToWorldAsURpmActor(UglTFRuntimeAsset* gltfAsset)
{
	this->LoadAssetToWorld(nullptr, gltfAsset);
}

void FEditorAssetLoader::LoadAssetToWorldAsURpmActor(USkeletalMesh* SkeletalMesh)
{
	this->LoadAssetToWorld(SkeletalMesh, nullptr);
}

void FEditorAssetLoader::LoadAssetToWorld(USkeletalMesh* SkeletalMesh, UglTFRuntimeAsset* gltfAsset)
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

	if (SkeletalMesh)
	{
		FTransform Transform = FTransform::Identity;


		ARpmActor* NewActor = EditorWorld->SpawnActorDeferred<ARpmActor>(ARpmActor::StaticClass(), Transform);

		if (NewActor)
		{
			NewActor->SetFlags(RF_Transient);

			NewActor->FinishSpawning(Transform);
			NewActor->DispatchBeginPlay();
			if (SkeletalMesh)
			{
				NewActor->HandleSkeletalMeshLoaded(SkeletalMesh);
			}

			GEditor->SelectNone(true, true, true);
			GEditor->SelectActor(NewActor, true, true, false, true);

			// Register the actor in the editor world and update the editor
			GEditor->SelectActor(NewActor, true, true);
			GEditor->EditorUpdateComponents();
			if (gltfAsset)
			{
				NewActor->LoadglTFAsset(gltfAsset);
			}
			UE_LOG(LogTemp, Log, TEXT("Successfully loaded GLB asset into the editor world"));
			return;
		}
		
		UE_LOG(LogTemp, Error, TEXT("Failed to spawn AglTFRuntimeAssetActor in the editor world"));
	}
	UE_LOG(LogTemp, Error, TEXT("Failed to load GLB asset from file"));
}
