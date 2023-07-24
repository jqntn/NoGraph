// Copyright 2023, Julien QUENTIN. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"

class FNoGraphModule : public IModuleInterface
{
public:
  virtual void StartupModule() override;
};