// Fill out your copyright notice in the Description page of Project Settings.


#include "SRpmDeveloperLoginWidget.h"
#include "Auth/DevAuthTokenCache.h"
#include "EditorCache.h"
#include "HttpModule.h"
#include "IImageWrapper.h"
#include "SlateOptMacros.h"
#include "Api/Assets/Models/AssetListRequest.h"
#include "Api/Assets/Models/AssetListResponse.h"
#include "Auth/DeveloperAccountApi.h"
#include "Auth/DeveloperTokenAuthStrategy.h"
#include "Interfaces/IHttpResponse.h"
#include "Widgets/Input/SEditableTextBox.h"
#include "IImageWrapperModule.h"
#include "Auth/DeveloperAuthApi.h"
#include "Auth/DeveloperLoginRequest.h"
#include "Settings/RpmDeveloperSettings.h"

BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION

void SRpmDeveloperLoginWidget::Construct(const FArguments& InArgs)
{
	FDeveloperAuth AuthData = DevAuthTokenCache::GetAuthData();
	DevAuthTokenCache::SetAuthData(AuthData);

	bIsLoggedIn = AuthData.IsValid();
	Settings = GetDefault<URpmDeveloperSettings>();
	UserName = AuthData.Name;
	ChildSlot
	[
		SNew(SVerticalBox)
		+ SVerticalBox::Slot()
		.Padding(10)
		.AutoHeight()
		[
			SNew(STextBlock)
			.Text(FText::FromString("Sign in with your Ready Player Me Studio account"))
			.Visibility(this, &SRpmDeveloperLoginWidget::GetLoginViewVisibility)
		]
		+ SVerticalBox::Slot()
		.Padding(10)
		.AutoHeight()
		[
			SNew(STextBlock)
			.Text(FText::FromString("Email:"))
			.Visibility(this, &SRpmDeveloperLoginWidget::GetLoginViewVisibility)
		]
		+ SVerticalBox::Slot()
		.Padding(10)
		.AutoHeight()
		[
			SAssignNew(EmailTextBox, SEditableTextBox)
			.Visibility(this, &SRpmDeveloperLoginWidget::GetLoginViewVisibility)
		]
		+ SVerticalBox::Slot()
		.Padding(10)
		.AutoHeight()
		[
			SNew(STextBlock)
			.Text(FText::FromString("Password:"))
			.Visibility(this, &SRpmDeveloperLoginWidget::GetLoginViewVisibility)
		]
		+ SVerticalBox::Slot()
		.Padding(10)
		.AutoHeight()
		[
			SAssignNew(PasswordTextBox, SEditableTextBox)
			.IsPassword(true)
			.Visibility(this, &SRpmDeveloperLoginWidget::GetLoginViewVisibility)
		]
		+ SVerticalBox::Slot()
		.Padding(10)
		.AutoHeight()
		[
			SNew(SButton)
			.Text(FText::FromString("Login"))
			.OnClicked(this, &SRpmDeveloperLoginWidget::OnLoginClicked)
			.Visibility(this, &SRpmDeveloperLoginWidget::GetLoginViewVisibility)
		]
		+ SVerticalBox::Slot()
		.Padding(10)
		.AutoHeight()
		[
			SNew(SButton)
			.Text(FText::FromString("Use Demo Account"))
			.OnClicked(this, &SRpmDeveloperLoginWidget::OnUseDemoAccountClicked)
			.Visibility(this, &SRpmDeveloperLoginWidget::GetLoginViewVisibility)
		]
		+ SVerticalBox::Slot()
		.Padding(10)
		.AutoHeight()
		[
			SNew(SHorizontalBox)
			+ SHorizontalBox::Slot()
			.FillWidth(1.0)
			[
				SNew(STextBlock)
				.Text(this, &SRpmDeveloperLoginWidget::GetWelcomeText)
				.Visibility(this, &SRpmDeveloperLoginWidget::GetLoggedInViewVisibility)
			]
			+ SHorizontalBox::Slot()
			.AutoWidth()
			.HAlign(HAlign_Right)
			[
				SNew(SButton)
				.Text(FText::FromString("Logout"))
				.OnClicked(this, &SRpmDeveloperLoginWidget::OnLogoutClicked)
				.Visibility(this, &SRpmDeveloperLoginWidget::GetLoggedInViewVisibility)
			]
		]
		+ SVerticalBox::Slot()
		.Padding(10)
		.AutoHeight()
		[
			SNew(STextBlock)
			.Text(FText::FromString("Project Settings"))
			.Font(FSlateFontInfo(FPaths::EngineContentDir() / TEXT("Slate/Fonts/Roboto-Regular.ttf"), 16))
			.Visibility(this, &SRpmDeveloperLoginWidget::GetLoggedInViewVisibility)
		]
		+ SVerticalBox::Slot()
		.Padding(10)
		.AutoHeight()
		[
			SNew(STextBlock)
			.Text(FText::FromString("Select the Ready Player Me application to link to project"))
			.Visibility(this, &SRpmDeveloperLoginWidget::GetLoggedInViewVisibility)
		]
		
		+ SVerticalBox::Slot()
		.Padding(10)
		.AutoHeight()
		[
			SNew(SComboBox<TSharedPtr<FString>>)
			.OptionsSource(&ComboBoxItems)
			.OnSelectionChanged(this, &SRpmDeveloperLoginWidget::OnComboBoxSelectionChanged)
			.OnGenerateWidget_Lambda([](TSharedPtr<FString> Item) {
				return SNew(STextBlock).Text(FText::FromString(*Item));
			})
			[
				SNew(STextBlock)
				.Text(this, &SRpmDeveloperLoginWidget::GetSelectedComboBoxItemText)
			]
			.Visibility(this, &SRpmDeveloperLoginWidget::GetLoggedInViewVisibility)
		]
		+ SVerticalBox::Slot()
		.Padding(10)
		.AutoHeight()
		[
			SNew(STextBlock)
			.Text(FText::FromString("Character Styles"))
			.Font(FSlateFontInfo(FPaths::EngineContentDir() / TEXT("Slate/Fonts/Roboto-Regular.ttf"), 16))
			.Visibility(this, &SRpmDeveloperLoginWidget::GetLoggedInViewVisibility)
		]
		+ SVerticalBox::Slot()
		.Padding(10)
		.AutoHeight()
		[
			SNew(STextBlock)
			.Text(FText::FromString("Here you can import your character styles from Studio"))
			.Visibility(this, &SRpmDeveloperLoginWidget::GetLoggedInViewVisibility)
		] 
		+ SVerticalBox::Slot()
		.AutoHeight()
		[
		
			SAssignNew(ContentBox, SVerticalBox)
			.Visibility(this, &SRpmDeveloperLoginWidget::GetLoggedInViewVisibility)
		]	
	];
	
	EmailTextBox->SetText(FText::FromString(EditorCache::GetString(CacheKeyEmail)));
	Initialize();
}

void SRpmDeveloperLoginWidget::Initialize()
{
	if(bIsInitialized) return;

	if(!DeveloperAuthApi.IsValid())
	{
		DeveloperAuthApi = MakeUnique<FDeveloperAuthApi>();
		DeveloperAuthApi->OnLoginResponse.BindRaw(this, &SRpmDeveloperLoginWidget::HandleLoginResponse);
	}
	
	if (!AssetApi.IsValid())
	{
		AssetApi = MakeUnique<FAssetApi>();
		AssetApi->OnListAssetsResponse.BindRaw(this, &SRpmDeveloperLoginWidget::HandleBaseModelListResponse);

	}
	if(!DeveloperAccountApi.IsValid())
	{
		DeveloperTokenAuthStrategy* AuthStrategy = new DeveloperTokenAuthStrategy();
		DeveloperAccountApi = MakeUnique<FDeveloperAccountApi>(AuthStrategy);

		DeveloperAccountApi->SetAuthenticationStrategy(AuthStrategy);
		DeveloperAccountApi->OnOrganizationResponse.BindRaw(this, &SRpmDeveloperLoginWidget::HandleOrganizationListResponse);
		DeveloperAccountApi->OnApplicationListResponse.BindRaw(this, &SRpmDeveloperLoginWidget::HandleApplicationListResponse);
	}
	bIsInitialized = true;
	if(bIsLoggedIn)
	{
		GetOrgList();
	}
}	

void SRpmDeveloperLoginWidget::AddCharacterStyle(const FAsset& StyleAsset)
{
	TSharedPtr<SImage> ImageWidget;
	const FVector2D ImageSize(100.0f, 100.0f);

	ContentBox->AddSlot()
	   .AutoHeight()
	   .Padding(5)
	   [
		   SNew(SHorizontalBox)
		   + SHorizontalBox::Slot()
		   .AutoWidth()
		   .Padding(5)
		   [
			   SNew(SVerticalBox)
			   + SVerticalBox::Slot()
			   .AutoHeight()
			   .HAlign(HAlign_Left)
			   [
				   SAssignNew(ImageWidget, SImage).// The image will be set later after downloading
				   DesiredSizeOverride(ImageSize) 			   	
			   ]
			   + SVerticalBox::Slot()
			   .AutoHeight()
			   .Padding(5, 5)
			   [
			   		SNew(SBox)
				   .WidthOverride(100.0f) // Match button width to image width
				   [
					   SNew(SButton)
					   .Text(FText::FromString("Load Style"))
					   .OnClicked_Lambda([this, StyleAsset]() -> FReply
					   {
						   OnLoadStyleClicked(StyleAsset.Id);
						   return FReply::Handled();
					   })
				   ]
			   ]
		   ]
		   + SHorizontalBox::Slot()
		   .AutoWidth()
		   .VAlign(VAlign_Top)
		   .Padding(10, 10, 0, 0) // Padding from the left side of the Image & Button stack
		   [
			   SNew(STextBlock)
			   .Text(FText::FromString(FString::Printf(TEXT("ID: %s"), *StyleAsset.Id)))
		   ]
	   ];

	DownloadImage(StyleAsset.IconUrl, [ImageWidget](UTexture2D* Texture)
	{
	  if (ImageWidget.IsValid())
	  {
		  SetImageFromTexture(Texture, ImageWidget);
	  }
	});
}

void SRpmDeveloperLoginWidget::DownloadImage(const FString& Url, TFunction<void(UTexture2D*)> Callback)
{
	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> HttpRequest = FHttpModule::Get().CreateRequest();
	HttpRequest->OnProcessRequestComplete().BindLambda([Callback, this](FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
	{
		if (bWasSuccessful && Response.IsValid() && Response->GetContentLength() > 0)
		{
			// Create a texture from the image data
			UTexture2D* Texture = CreateTextureFromImageData(Response->GetContent());
			if (Texture)
			{
				Callback(Texture);
			}
		}
	});

	HttpRequest->SetURL(Url);
	HttpRequest->SetVerb(TEXT("GET"));
	HttpRequest->ProcessRequest();
}

void SRpmDeveloperLoginWidget::SetImageFromTexture(UTexture2D* Texture, const TSharedPtr<SImage>& ImageWidget)
{
	const FVector2D DesiredSize(100.0f, 100.0f);
	if (Texture)
	{
		FSlateBrush* Brush = new FSlateBrush();
		Brush->SetResourceObject(Texture);
		Brush->ImageSize = DesiredSize;
		ImageWidget->SetImage(Brush);
	}
}

UTexture2D* SRpmDeveloperLoginWidget::CreateTextureFromImageData(const TArray<uint8>& ImageData)
{
	IImageWrapperModule& ImageWrapperModule = FModuleManager::LoadModuleChecked<IImageWrapperModule>(FName("ImageWrapper"));
	EImageFormat ImageFormat = ImageWrapperModule.DetectImageFormat(ImageData.GetData(), ImageData.Num());

	if (ImageFormat != EImageFormat::Invalid)
	{
		TSharedPtr<IImageWrapper> ImageWrapper = ImageWrapperModule.CreateImageWrapper(ImageFormat);
		if (ImageWrapper.IsValid() && ImageWrapper->SetCompressed(ImageData.GetData(), ImageData.Num()))
		{
			TArray<uint8> UncompressedBGRA ;
			if (ImageWrapper->GetRaw(ERGBFormat::BGRA, 8, UncompressedBGRA))
			{
				UTexture2D* Texture = UTexture2D::CreateTransient(ImageWrapper->GetWidth(), ImageWrapper->GetHeight());
				if (Texture)
				{
					void* TextureData = Texture->GetPlatformData()->Mips[0].BulkData.Lock(LOCK_READ_WRITE);
					FMemory::Memcpy(TextureData, UncompressedBGRA.GetData(), UncompressedBGRA.Num());
					Texture->GetPlatformData()->Mips[0].BulkData.Unlock();

					Texture->UpdateResource();
					return Texture;
				}
				UE_LOG(LogTemp, Error, TEXT("Failed to create texture: Texture is null."));
			}
			else
			{
				UE_LOG(LogTemp, Error, TEXT("Failed to get raw image data."));
			}
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("Failed to set compressed data or ImageWrapper is invalid."));
		}
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Invalid image format detected."));
	}

	return nullptr;
}


void SRpmDeveloperLoginWidget::OnLoadStyleClicked(const FString& StyleId)
{
	AssetLoader = FEditorAssetLoader();
	const FString Url = FString::Printf(TEXT("%s"), *CharacterStyleAssets[StyleId].GlbUrl);
	AssetLoader.LoadGLBFromURL(Url);
}

EVisibility SRpmDeveloperLoginWidget::GetLoginViewVisibility() const
{
	return bIsLoggedIn ? EVisibility::Collapsed : EVisibility::Visible;
}

EVisibility SRpmDeveloperLoginWidget::GetLoggedInViewVisibility() const
{
	return bIsLoggedIn ? EVisibility::Visible : EVisibility::Collapsed;
}

FText SRpmDeveloperLoginWidget::GetWelcomeText() const
{
	return FText::Format(FText::FromString("Welcome {0}"), FText::FromString(UserName));
}

FReply SRpmDeveloperLoginWidget::OnLoginClicked()
{
	FString Email = EmailTextBox->GetText().ToString();
	FString Password = PasswordTextBox->GetText().ToString();
	EditorCache::SetString(CacheKeyEmail, Email);
	Email = Email.TrimStartAndEnd();
	Password = Password.TrimStartAndEnd();
	
	FDeveloperLoginRequest LoginRequest = FDeveloperLoginRequest(Email, Password);
	DeveloperAuthApi->LoginWithEmail(LoginRequest);
	return FReply::Handled();
}

void SRpmDeveloperLoginWidget::GetOrgList()
{
	FOrganizationListRequest OrgRequest;
	DeveloperAccountApi->ListOrganizationsAsync(OrgRequest);
}

void SRpmDeveloperLoginWidget::HandleLoginResponse(const FDeveloperLoginResponse& Response, bool bWasSuccessful)
{
	if(bWasSuccessful)
	{
		UserName = Response.Data.Name;
		FDeveloperAuth AuthData = FDeveloperAuth(Response.Data, false);
		DevAuthTokenCache::SetAuthData(AuthData);
		SetLoggedInState(true);
		GetOrgList();
		return;
	}
	UE_LOG(LogTemp, Error, TEXT("Login request failed"),);
	DevAuthTokenCache::ClearAuthData();
}

void SRpmDeveloperLoginWidget::HandleOrganizationListResponse(const FOrganizationListResponse& Response,
	bool bWasSuccessful)
{

	if (bWasSuccessful)
	{			
		if(Response.Data.Num() == 0)
		{
			UE_LOG(LogTemp, Error, TEXT("No organizations found"));
			return;
		}
			
		FApplicationListRequest Request;
		Request.Params.Add("organizationId", Response.Data[0].Id);
		DeveloperAccountApi->ListApplicationsAsync(Request);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to list organizations"));
	}
}


void SRpmDeveloperLoginWidget::HandleApplicationListResponse(const FApplicationListResponse& Response,
	bool bWasSuccessful)
{
	if (bWasSuccessful)
	{
		TArray<FString> Items;
		for (const FApplication& App : Response.Data)
		{
			Items.Add(App.Name);
		}
		PopulateComboBoxItems(Items);

	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to list applications"));
	}
	LoadBaseModelList();
}


void SRpmDeveloperLoginWidget::PopulateComboBoxItems(const TArray<FString>& Items)
{
	ComboBoxItems.Empty();
	for (const FString& Item : Items)
	{
		ComboBoxItems.Add(MakeShared<FString>(Item));
	}
	SelectedComboBoxItem = ComboBoxItems.Num() > 0 ? ComboBoxItems[0] : nullptr;
}

FText SRpmDeveloperLoginWidget::GetSelectedComboBoxItemText() const
{
	return SelectedComboBoxItem.IsValid() ? FText::FromString(*SelectedComboBoxItem) : FText::FromString("Select an option");
}



void SRpmDeveloperLoginWidget::OnComboBoxSelectionChanged(TSharedPtr<FString> NewValue, ESelectInfo::Type SelectInfo)
{
	SelectedComboBoxItem = NewValue;
}

FReply SRpmDeveloperLoginWidget::OnUseDemoAccountClicked()
{	
	UserName = "DemoUser";
	//TODO: Implement demo account login
	UE_LOG(LogTemp, Error, TEXT("Demo account login not implemented"));
	return FReply::Handled();
}

FReply SRpmDeveloperLoginWidget::OnLogoutClicked()
{
	DevAuthTokenCache::ClearAuthData();
	SetLoggedInState(false);
	return FReply::Handled();
}

void SRpmDeveloperLoginWidget::LoadBaseModelList()
{
	FAssetListRequest Request = FAssetListRequest();
	FAssetListQueryParams Params = FAssetListQueryParams();
	Params.ApplicationId = Settings->ApplicationId;
	Params.Type = "baseModel";
	Request.Params = Params;
	AssetApi->SetAuthenticationStrategy(new DeveloperTokenAuthStrategy());
	AssetApi->ListAssetsAsync(Request);
}

void SRpmDeveloperLoginWidget::HandleBaseModelListResponse(const FAssetListResponse& Response, bool bWasSuccessful)
{
	CharacterStyleAssets.Empty();
	for (FAsset Asset : Response.Data)
	{
		CharacterStyleAssets.Add(Asset.Id, Asset);
		AddCharacterStyle(Asset);
	}
}


void SRpmDeveloperLoginWidget::SetLoggedInState(const bool IsLoggedIn)
{
	this->bIsLoggedIn = IsLoggedIn;

	// Force the UI to refresh
	Invalidate(EInvalidateWidget::Layout);
}

END_SLATE_FUNCTION_BUILD_OPTIMIZATION
