// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/ABWidgetComponent.h"
#include "UI/ABUserWidget.h"

void UABWidgetComponent::InitWidget()
{
	Super::InitWidget();
	
	UABUserWidget* ABUsetWidget = Cast<UABUserWidget>(GetWidget());
	if (ABUsetWidget)
	{
		ABUsetWidget->SetOwinngActor(GetOwner());
	}
}
