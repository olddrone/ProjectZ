// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "EnemyController.generated.h"

class UBlackboardComponent;
class UBehaviorTreeComponent;
/**
 * 
 */
UCLASS()
class PROJECTZ_API AEnemyController : public AAIController
{
	GENERATED_BODY()

public:
	AEnemyController();

	virtual void OnPossess(APawn* InPawn) override;

	FORCEINLINE UBlackboardComponent* GetBlackboardComponent() const { return BlackboardComponent; }

protected:

private:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI Behavior", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UBlackboardComponent> BlackboardComponent;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI Behavior", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UBehaviorTreeComponent> BehaviorTreeComponent;
};
