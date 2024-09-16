#include "EditorAssetLoader.h"
#include "TransientObjectSaverLibrary.h"
#include "AssetNameGenerator.h"
#include "glTFRuntimeFunctionLibrary.h"
#include "RpmActor.h"
#include "RpmNextGen.h"


FEditorAssetLoader::FEditorAssetLoader()
{
	SkeletonToCopy = nullptr;
	OnGlbLoaded.BindRaw(this, &FEditorAssetLoader::HandleGlbLoaded);
	GltfConfig = new FglTFRuntimeConfig();
	GltfConfig->TransformBaseType = EglTFRuntimeTransformBaseType::YForward;
}

FEditorAssetLoader::~FEditorAssetLoader()
{
}

void FEditorAssetLoader::HandleGlbLoaded(const FAsset& Asset, const TArray<unsigned char>& Data)
{
	UglTFRuntimeAsset* GltfAsset = nullptr;
	if (!Data.IsEmpty())
	{
		GltfAsset = UglTFRuntimeFunctionLibrary::glTFLoadAssetFromData(Data, *GltfConfig);
		if (GltfAsset)
		{
			GltfAsset->AddToRoot();
			SaveAsUAsset(GltfAsset, Asset.Id);
			LoadAssetToWorldAsURpmActor(GltfAsset, Asset.Id);
			GltfAsset->RemoveFromRoot();
		}
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

	UTransientObjectSaverLibrary::SaveTransientSkeletalMesh(skeletalMesh, SkeletalMeshAssetPath, SkeletonAssetPath, TEXT(""), NameGenerator->MaterialNameGeneratorDelegate, NameGenerator->TextureNameGeneratorDelegate);

	UE_LOG(LogReadyPlayerMe, Log, TEXT("Character model saved: %s"), *LoadedAssetId);
	return skeletalMesh;
}

void FEditorAssetLoader::LoadBaseModelAsset(const FAsset& Asset)
{
	LoadGlb(Asset, Asset.Id, false);
}

void FEditorAssetLoader::LoadAssetToWorldAsURpmActor(UglTFRuntimeAsset* GltfAsset, FString AssetId)
{
	this->LoadAssetToWorld(AssetId, GltfAsset);
}


void FEditorAssetLoader::LoadAssetToWorld(const FString& AssetId, UglTFRuntimeAsset* GltfAsset)
{
	if (!GEditor)
	{
		UE_LOG(LogReadyPlayerMe, Error, TEXT("GEditor is not available."));
		return;
	}
	
	UWorld* EditorWorld = GEditor->GetEditorWorldContext().World();
	if (!EditorWorld)
	{
		UE_LOG(LogReadyPlayerMe, Error, TEXT("No valid editor world found."));
		return;
	}

	if (GltfAsset)
	{
		FTransform Transform = FTransform::Identity;


		ARpmActor* NewActor = EditorWorld->SpawnActorDeferred<ARpmActor>(ARpmActor::StaticClass(), Transform);

		if (NewActor)
		{
			NewActor->SetFlags(RF_Transient);
			NewActor->Rename(*AssetId);
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
			if (GltfAsset)
			{
				NewActor->LoadGltfAsset(GltfAsset);
			}
			UE_LOG(LogReadyPlayerMe, Log, TEXT("Successfully loaded GLB asset into the editor world"));
			return;
		}
		
		UE_LOG(LogReadyPlayerMe, Error, TEXT("Failed to spawn ARpmActor in the editor world"));
	}
	UE_LOG(LogReadyPlayerMe, Error, TEXT("Failed to load GLB asset from file"));
}
