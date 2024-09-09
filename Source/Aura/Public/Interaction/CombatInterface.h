// Copyright Jordan Jackson

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "CombatInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI, BlueprintType)
class UCombatInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class AURA_API ICombatInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	virtual int32 GetPowerLevel();

	virtual FVector GetWeaponProjectileSocketLocation();

	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
	void SetMotionWarpFacingTarget(const FVector& TargetLocation);
};
