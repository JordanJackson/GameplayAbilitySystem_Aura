// Copyright Jordan Jackson

#pragma once

#include "CoreMinimal.h"
#include "Character/AuraCharacterBase.h"
#include "Interaction/HighlightInterface.h"
#include "AuraEnemy.generated.h"

/**
 * 
 */
UCLASS()
class AURA_API AAuraEnemy : public AAuraCharacterBase, public IHighlightInterface
{
	GENERATED_BODY()

public:
	AAuraEnemy();

	// Highlight Interface
	virtual void Highlight() override;
	virtual void ClearHighlight() override;
	
protected:
	virtual void BeginPlay() override;
};
