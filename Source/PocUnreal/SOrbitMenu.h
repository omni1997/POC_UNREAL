#pragma once

#include "CoreMinimal.h"
#include "Widgets/SCompoundWidget.h"

// Menu d'accueil : un titre et deux boutons, Start et Quitter.
class SOrbitMenu : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SOrbitMenu) {}
		SLATE_EVENT(FSimpleDelegate, OnStart)
		SLATE_EVENT(FSimpleDelegate, OnQuit)
	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs);

private:
	TSharedRef<SWidget> MakeButton(const FText& Label, FSimpleDelegate OnClicked);
};
