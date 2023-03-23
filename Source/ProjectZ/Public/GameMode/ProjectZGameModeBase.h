// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "ProjectZGameModeBase.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTZ_API AProjectZGameModeBase : public AGameModeBase
{
	GENERATED_BODY()
	
public:
	virtual void InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage) override;

	FORCEINLINE bool GetOption() const { return bLoadData; }
	FORCEINLINE bool GetLocation() const { return bLocation; }

private:
	bool bLoadData = false;
	bool bLocation = false;
};
