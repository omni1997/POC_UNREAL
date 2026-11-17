#include "SOrbitMenu.h"
#include "Styling/CoreStyle.h"
#include "Widgets/Input/SButton.h"
#include "Widgets/Layout/SBorder.h"
#include "Widgets/Layout/SBox.h"
#include "Widgets/SBoxPanel.h"
#include "Widgets/Text/STextBlock.h"

void SOrbitMenu::Construct(const FArguments& InArgs)
{
	ChildSlot
	.HAlign(HAlign_Center)
	.VAlign(VAlign_Center)
	[
		SNew(SBorder)
		.BorderImage(FCoreStyle::Get().GetBrush("WhiteBrush"))
		.BorderBackgroundColor(FLinearColor(0.02f, 0.02f, 0.06f, 0.85f))
		.Padding(FMargin(48.f, 32.f))
		[
			SNew(SVerticalBox)
			+ SVerticalBox::Slot()
			.AutoHeight()
			.HAlign(HAlign_Center)
			.Padding(0.f, 0.f, 0.f, 28.f)
			[
				SNew(STextBlock)
				.Text(FText::FromString(TEXT("Cube en orbite")))
				.Font(FCoreStyle::GetDefaultFontStyle("Bold", 32))
				.ColorAndOpacity(FLinearColor::White)
			]
			+ SVerticalBox::Slot()
			.AutoHeight()
			.Padding(0.f, 6.f)
			[
				MakeButton(FText::FromString(TEXT("Start")), InArgs._OnStart)
			]
			+ SVerticalBox::Slot()
			.AutoHeight()
			.Padding(0.f, 6.f)
			[
				MakeButton(FText::FromString(TEXT("Quitter")), InArgs._OnQuit)
			]
		]
	];
}

TSharedRef<SWidget> SOrbitMenu::MakeButton(const FText& Label, FSimpleDelegate OnClicked)
{
	return SNew(SBox)
		.WidthOverride(260.f)
		.HeightOverride(56.f)
		[
			SNew(SButton)
			// Pas de focus clavier : sinon Espace "cliquerait" le bouton sélectionné.
			.IsFocusable(false)
			.HAlign(HAlign_Center)
			.VAlign(VAlign_Center)
			.OnClicked_Lambda([OnClicked]()
			{
				OnClicked.ExecuteIfBound();
				return FReply::Handled();
			})
			[
				SNew(STextBlock)
				.Text(Label)
				.Font(FCoreStyle::GetDefaultFontStyle("Regular", 20))
			]
		];
}
