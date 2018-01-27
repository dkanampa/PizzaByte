#pragma once
#include "CoreMinimal.h"

/**
* Contains useful functions we use in various places in the code
*/
struct UsefulFunctions
{
public:

	/**
	 * E.g. UsefulFunctions::EnumToString(FString("EDistrictType"), District.Type);
	 */
	template<typename TEnum>
	static FORCEINLINE FString EnumToString(const FString& EnumClass, TEnum Value, bool FullName = false)
	{
		const UEnum* EnumPtr = FindObject<UEnum>(ANY_PACKAGE, *EnumClass, true);
		if (!EnumPtr) return FString("Invalid");

		FString EnumName = EnumPtr->GetNameByValue((int64)Value).ToString();

		if (FullName)
		{
			return EnumName;
		}
		else
		{
			int32 ChopIndex;
			EnumName.FindLastChar(':', ChopIndex);
			return EnumName.RightChop(ChopIndex + 1);
		}
	}

	template <typename EnumType>
	static FORCEINLINE EnumType StringToEnum(const FString& EnumName, const FString& String)
	{
		UEnum* Enum = FindObject<UEnum>(ANY_PACKAGE, *EnumName, true);
		if (!Enum)
		{
			return EnumType(0);
		}
		return (EnumType)Enum->GetValueByName(FName(*String));
	}
};