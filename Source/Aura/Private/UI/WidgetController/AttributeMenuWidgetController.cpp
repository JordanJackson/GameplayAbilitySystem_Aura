// Copyright Jordan Jackson


#include "UI/WidgetController/AttributeMenuWidgetController.h"
#include "AbilitySystem/AuraAttributeSet.h"
#include "AbilitySystem/Data/AttributeInfo.h"

void UAttributeMenuWidgetController::BroadcastInitialValues()
{
	const UAuraAttributeSet* AuraAttributeSet = CastChecked<UAuraAttributeSet>(AttributeSet);

	check(AttributeInfo);

	for (auto& Pair : AuraAttributeSet->GameplayTagToGetAttributeMap)
	{
		FAuraAttributeInfo AttrInfo = AttributeInfo->GetAttributeInfoByTag(Pair.Key);
		AttrInfo.AttributeValue = Pair.Value().GetNumericValue(AuraAttributeSet);
		AttributeInfoDelegate.Broadcast(AttrInfo);
	}
}

void UAttributeMenuWidgetController::BindCallbacksToDependencies()
{
	const UAuraAttributeSet* AuraAttributeSet = CastChecked<UAuraAttributeSet>(AttributeSet);

	for (auto& Pair : AuraAttributeSet->GameplayTagToGetAttributeMap)
	{
		AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(Pair.Value()).AddLambda(
			[this, Pair, AuraAttributeSet](const FOnAttributeChangeData& Data)
			{
				FAuraAttributeInfo Info = AttributeInfo->GetAttributeInfoByTag(Pair.Key);
				Info.AttributeValue = Pair.Value().GetNumericValue(AuraAttributeSet);
				AttributeInfoDelegate.Broadcast(Info);
			});
	}
}
