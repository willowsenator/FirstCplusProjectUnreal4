#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "TakeDamageListener.generated.h"

class AActor;
class AController;
class UDamageType;

UCLASS()
class UTakeDamageListener : public UObject
{
	GENERATED_BODY()

public:
	bool bFired = false;

	UFUNCTION()
	void OnDamage(AActor* DamagedActor, float Damage, const UDamageType* DamageType, AController* InstigatedBy, AActor* DamageCauser)
	{
		bFired = true;
	}
};
