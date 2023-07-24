// Copyright 2023, Julien QUENTIN. All Rights Reserved.

#include "NoGraph.h"

#include "Framework/Application/SlateApplication.h"
#include "Widgets/Docking/SDockTab.h"
#include "Widgets/Text/STextBlock.h"

#define LOCTEXT_NAMESPACE "FNoGraphModule"

bool
HideGraphInChildren(SWidget& Parent)
{
  auto Result = false;
  Parent.GetChildren()->ForEachWidget([&](SWidget& Child0) {
    if (Child0.GetType() == "SGraphPanel") {
      Child0.GetParentWidget()->GetChildren()->ForEachWidget(
        [&](SWidget& Child1) {
          if (Child1.GetType() == "STextBlock") {
            auto& Text =
              reinterpret_cast<STextBlock*>(&Child1)->GetText().ToString();
            if (Text.Contains("BLUEPRINT") || Text.Contains("INTERFACE") ||
                Text.Contains("MACRO")) {
              Parent.SetVisibility(EVisibility::Hidden);
              Result = true;
              return;
            }
          }
        });
      return;
    }
    if (HideGraphInChildren(Child0) == true) {
      Result = true;
      return;
    }
  });
  return Result;
}

void
HideGraphInTabStack(const TSharedPtr<SDockTab>& ValidTabPtr)
{
  auto DockAreaPtr = ValidTabPtr->GetDockArea();
  if (!DockAreaPtr.IsValid())
    return;
  if (HideGraphInChildren(*reinterpret_cast<SWidget*>(DockAreaPtr.Get())) ==
      true) {
    SDockTab::FOnTabRenamed OnTabRenamed;
    OnTabRenamed.BindLambda(
      [&](TSharedRef<SDockTab> TabRef) { HideGraphInTabStack(TabRef); });
    ValidTabPtr->SetOnTabRenamed(OnTabRenamed);
  }
}

TSharedPtr<SDockTab> NewTabPtr0, NewTabPtr1;

void
FNoGraphModule::StartupModule()
{
  FOnActiveTabChanged::FDelegate OnActiveTabChanged;
  OnActiveTabChanged.BindLambda(
    [&](TSharedPtr<SDockTab> TabPtr0, TSharedPtr<SDockTab> TabPtr1) {
      NewTabPtr0 = TabPtr0;
      NewTabPtr1 = TabPtr1;
    });
  FGlobalTabmanager::Get()->OnActiveTabChanged_Subscribe(OnActiveTabChanged);

  FSlateApplication::Get().OnPreTick().AddLambda([&](float) {
    if (NewTabPtr0.IsValid()) {
      HideGraphInTabStack(NewTabPtr0);
      NewTabPtr0 = nullptr;
    }
    if (NewTabPtr1.IsValid()) {
      HideGraphInTabStack(NewTabPtr1);
      NewTabPtr1 = nullptr;
    }
  });
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FNoGraphModule, NoGraph)