#pragma once

#include "CoreMinimal.h"
#include "Widgets/SCompoundWidget.h"

// Start menu: a title and two buttons, Start and Quit.
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
