// Copyright Jordan Jackson


#include "AbilitySystem/Data/AttributeInfo.h"

FAuraAttributeInfo UAttributeInfo::GetAttributeInfoByTag(const FGameplayTag& AttributeTag, bool bLogNotFound) const
{
	if (AttributeInformation.Contains(AttributeTag))
	{
		return AttributeInformation.FindRef(AttributeTag);
	}

	if (bLogNotFound)
	{
		UE_LOG(LogTemp, Error, TEXT("Can't find matching AttributeInfo for AttributeTag [%s] on AttributeInfo [%s]"), *AttributeTag.ToString(), *GetNameSafe(this));
	}

	return FAuraAttributeInfo();
}
