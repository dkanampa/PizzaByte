#pragma once
#include "FCharsetFlags.generated.h"

/**
 * Special class indicating which character are avaiable
 * Usually mapped to a pizza topping for it's pizza code
 */
USTRUCT(BlueprintType)
struct FCharsetFlags
{
	GENERATED_USTRUCT_BODY();

	FCharsetFlags() {};

	FCharsetFlags(bool _Lowercase, bool _Uppercase, bool _Numbers, bool _Special)
		:Lowercase(_Lowercase), Uppercase(_Uppercase), Numbers(_Numbers), Special(_Special) {};

	// abcdefghijklmnopqrstuvwxyz
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		bool Lowercase = false;

	// ABCDEFGHIJKLMNOPQRSTUVWXYZ
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		bool Uppercase = false;

	// 0123456789
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		bool Numbers = false;

	// !"#$%&'()*+,-./:;<=>?@[\]^_`{|}~
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		bool Special = false;
};