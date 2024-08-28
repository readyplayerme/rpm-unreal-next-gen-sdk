#include "UI/SCacheEditorWidget.h"
#include "Widgets/Input/SButton.h"
#include "Widgets/Input/SSlider.h"
#include "Widgets/Input/SEditableTextBox.h"
#include "EditorStyleSet.h"
#include "Widgets/Input/SNumericEntryBox.h"
#include "Widgets/Layout/SScrollBox.h"

void SCacheEditorWidget::Construct(const FArguments& InArgs)
{
    ChildSlot
    [
        SNew(SScrollBox) // Make the entire content scrollable
        + SScrollBox::Slot()
        [
            SNew(SVerticalBox)

            // Title/Label "Get Cache from Remote URL"
            + SVerticalBox::Slot()
            .Padding(5)
            .AutoHeight()
            [
                SNew(STextBlock)
                .Text(FText::FromString("Get Cache from Remote URL"))
                .Font(FCoreStyle::GetDefaultFontStyle("Bold", 24))
            ]

            // Editable text field with label "Cache Url"
            + SVerticalBox::Slot()
            .Padding(5)
            .AutoHeight()
            [
                SNew(SHorizontalBox)
                + SHorizontalBox::Slot()
                .AutoWidth()
                .VAlign(VAlign_Center)
                [
                    SNew(STextBlock)
                    .Text(FText::FromString("Cache URL:"))
                ]
                + SHorizontalBox::Slot()
                .Padding(5, 0, 0, 0)
                .FillWidth(1.0f)
                [
                    SNew(SBox)
                    .HeightOverride(30) // Set text box height
                    [
                        SNew(SEditableTextBox)
                        .Text(FText::FromString("http://"))
                        .OnTextCommitted(this, &SCacheEditorWidget::OnCacheUrlTextCommitted)
                    ]
                ]
            ]

            // Button "Download Remote Cache"
            + SVerticalBox::Slot()
            .Padding(5)
            .AutoHeight()
            [
                SNew(SBox)
                .HeightOverride(40) // Set button height
                [
                    SNew(SButton)
                    .Text(FText::FromString("Download Remote Cache"))
                    .OnClicked(this, &SCacheEditorWidget::OnDownloadRemoteCacheClicked)
                    .HAlign(HAlign_Center)
                    .VAlign(VAlign_Center)
                ]
            ]

            // Integer Slider with label "Items per category"
            + SVerticalBox::Slot()
            .Padding(5)
            .AutoHeight()
            [
                SNew(SHorizontalBox)
                + SHorizontalBox::Slot()
                .AutoWidth()
                .VAlign(VAlign_Center)
                [
                    SNew(STextBlock)
                    .Text(FText::FromString("Items per category:"))
                ]
                + SHorizontalBox::Slot()
                .Padding(5, 0, 0, 0)
                .FillWidth(1.0f)
                [
                    SNew(SNumericEntryBox<int32>)
                    .Value(this, &SCacheEditorWidget::GetItemsPerCategory)
                    .OnValueChanged(this, &SCacheEditorWidget::OnItemsPerCategoryChanged)
                    .AllowSpin(true) // Slider-like behavior
                    .MinValue(1)
                    .MaxValue(100)
                ]
            ]

            // Button "Generate offline cache"
            + SVerticalBox::Slot()
            .Padding(5)
            .AutoHeight()
            [
                SNew(SBox)
                .HeightOverride(40) // Set button height
                [
                    SNew(SButton)
                    .Text(FText::FromString("Generate offline cache"))
                    .OnClicked(this, &SCacheEditorWidget::OnGenerateOfflineCacheClicked)
                    .HAlign(HAlign_Center)
                    .VAlign(VAlign_Center)
                ]
            ]

            // Button "Extract Cache to local folder"
            + SVerticalBox::Slot()
            .Padding(5)
            .AutoHeight()
            [
                SNew(SBox)
                .HeightOverride(40) // Set button height
                [
                    SNew(SButton)
                    .Text(FText::FromString("Extract Cache to local folder"))
                    .OnClicked(this, &SCacheEditorWidget::OnExtractCacheClicked)
                    .HAlign(HAlign_Center)
                    .VAlign(VAlign_Center)
                ]
            ]

            // Button "Open Local Cache Folder"
            + SVerticalBox::Slot()
            .Padding(5)
            .AutoHeight()
            [
                SNew(SBox)
                .HeightOverride(40) // Set button height
                [
                    SNew(SButton)
                    .Text(FText::FromString("Open Local Cache Folder"))
                    .OnClicked(this, &SCacheEditorWidget::OnOpenLocalCacheFolderClicked)
                    .HAlign(HAlign_Center)
                    .VAlign(VAlign_Center)
                ]
            ]
        ]
    ];
}


FReply SCacheEditorWidget::OnGenerateOfflineCacheClicked()
{
    // Handle generating the offline cache
    return FReply::Handled();
}

FReply SCacheEditorWidget::OnExtractCacheClicked()
{
    // Handle extracting the cache
    return FReply::Handled();
}

FReply SCacheEditorWidget::OnOpenLocalCacheFolderClicked()
{
    // Handle opening the local cache folder
    return FReply::Handled();
}

FReply SCacheEditorWidget::OnDownloadRemoteCacheClicked()
{
    // Handle downloading the remote cache
    return FReply::Handled();
}

void SCacheEditorWidget::OnItemsPerCategoryChanged(float NewValue)
{
    ItemsPerCategory = NewValue;
    // Handle slider value change
}

void SCacheEditorWidget::OnCacheUrlChanged(const FText& NewText)
{
    CacheUrl = NewText.ToString();
    // Handle cache URL text change
}
