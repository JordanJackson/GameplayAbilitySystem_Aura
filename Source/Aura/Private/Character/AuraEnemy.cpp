// Copyright Jordan Jackson

#include "Character/AuraEnemy.h"
#include "Aura/Aura.h"
#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "AbilitySystem/AuraAttributeSet.h"
#include "AbilitySystem/AuraAbilitySystemLibrary.h"
#include "AI/AuraAIController.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/WidgetComponent.h"
#include "UI/Widget/AuraUserWidget.h"

AAuraEnemy::AAuraEnemy()
{
	GetMesh()->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);

	AbilitySystemComponent = CreateDefaultSubobject<UAuraAbilitySystemComponent>("AbilitySystemComponent");
	AbilitySystemComponent->SetIsReplicated(true);
	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Minimal);

	bUseControllerRotationPitch = false;
	bUseControllerRotationRoll = false;
	bUseControllerRotationYaw = false;

	GetCharacterMovement()->bUseControllerDesiredRotation = true;

	AttributeSet = CreateDefaultSubobject<UAuraAttributeSet>("AuraAttributeSet");

	StatusBarWidgetComponent = CreateDefaultSubobject<UWidgetComponent>("StatusBar");
	StatusBarWidgetComponent->SetupAttachment(GetRootComponent());
}

void AAuraEnemy::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	if (!HasAuthority())
	{
		return;
	}

	AuraAIController = Cast<AAuraAIController>(NewController);
	AuraAIController->GetBlackboardComponent()->InitializeBlackboard(*BehaviorTree->BlackboardAsset);
	AuraAIController->RunBehaviorTree(BehaviorTree);
	AuraAIController->GetBlackboardComponent()->SetValueAsBool(FName("IsInHitReact"), false);

	const bool bIsRangedClass = CharacterClass == ECharacterClass::Elementalist || CharacterClass == ECharacterClass::Ranger;
	AuraAIController->GetBlackboardComponent()->SetValueAsBool(FName("IsRangedClass"), bIsRangedClass);
}

void AAuraEnemy::BeginPlay()
{
	Super::BeginPlay();

	GetCharacterMovement()->MaxWalkSpeed = bHitReactActive ? 0.0f : BaseWalkSpeed;

	InitAbilityActorInfo();

	if (HasAuthority())
	{
		UAuraAbilitySystemLibrary::GiveStartupAbilities(this, AbilitySystemComponent, CharacterClass);
	}

	if (UAuraUserWidget* AuraUserWidget = Cast<UAuraUserWidget>(StatusBarWidgetComponent->GetUserWidgetObject()))
	{
		AuraUserWidget->SetWidgetController(this);
	}

	UAuraAttributeSet* AuraAS = CastChecked<UAuraAttributeSet>(AttributeSet);

	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AuraAS->GetHealthAttribute()).AddLambda(
		[this](const FOnAttributeChangeData& Data)
		{
			OnHealthChanged.Broadcast(Data.NewValue);
		});

	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AuraAS->GetMaxHealthAttribute()).AddLambda(
		[this](const FOnAttributeChangeData& Data)
		{
			OnMaxHealthChanged.Broadcast(Data.NewValue);
		});

	const FGameplayTag HitReactTag = FGameplayTag::RequestGameplayTag(FName("Status.HitReact"));
	AbilitySystemComponent->RegisterGameplayTagEvent(HitReactTag, EGameplayTagEventType::NewOrRemoved).AddUObject(
		this, &ThisClass::HitReactTagChanged
	);

	OnHealthChanged.Broadcast(AuraAS->GetHealth());
	OnMaxHealthChanged.Broadcast(AuraAS->GetMaxHealth());
}

void AAuraEnemy::InitAbilityActorInfo()
{
	AbilitySystemComponent->InitAbilityActorInfo(this, this);
	CastChecked<UAuraAbilitySystemComponent>(AbilitySystemComponent)->AbilityActorInfoSet();

	if (HasAuthority())
	{
		InitializeDefaultAttributes();
	}
}

void AAuraEnemy::InitializeDefaultAttributes() const
{
	UAuraAbilitySystemLibrary::InitializeDefaultAttributes(this, AbilitySystemComponent, CharacterClass, Level);
}

void AAuraEnemy::Highlight()
{
	GetMesh()->SetRenderCustomDepth(true);
	GetMesh()->SetCustomDepthStencilValue(CUSTOM_DEPTH_RED);

	if (Weapon)
	{
		Weapon->SetRenderCustomDepth(true);
		Weapon->SetCustomDepthStencilValue(CUSTOM_DEPTH_RED);
	}
}

void AAuraEnemy::ClearHighlight()
{
	GetMesh()->SetRenderCustomDepth(false);

	if (Weapon)
	{
		Weapon->SetRenderCustomDepth(false);
	}
}

int32 AAuraEnemy::GetPowerLevel() const
{
	return Level;
}

void AAuraEnemy::Die()
{
	SetLifeSpan(LifespanAfterDeath);

	Super::Die();
}

AActor* AAuraEnemy::GetCombatTarget_Implementation() const
{
	return CombatTarget;
}

void AAuraEnemy::SetCombatTarget_Implementation(AActor* InCombatTarget)
{
	CombatTarget = InCombatTarget;
}

void AAuraEnemy::MulticastHandleDeath_Implementation()
{
	Super::MulticastHandleDeath_Implementation();

	StatusBarWidgetComponent->DetachFromComponent(FDetachmentTransformRules(EDetachmentRule::KeepWorld, true));
}

void AAuraEnemy::HitReactTagChanged(const FGameplayTag CallbackTag, int32 NewCount)
{
	bHitReactActive = NewCount > 0;
	
	GetCharacterMovement()->MaxWalkSpeed = bHitReactActive ? 0.0f : BaseWalkSpeed;
	if (AuraAIController)
	{
		AuraAIController->GetBlackboardComponent()->SetValueAsBool(FName("IsInHitReact"), bHitReactActive);
	}
}
