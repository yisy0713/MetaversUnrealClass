// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/ABGameMode.h"
#include "ABGameMode.h"


AABGameMode::AABGameMode()
{
	//클래스찾기
	//static ConstructorHelpers::FClassFinder<APawn> TrirdPersonClassRef(TEXT("/Game/ThirdPerson/Blueprints/BP_ThirdPersonCharacter.BP_ThirdPersonCharacter_C"));
	////DefaultPawnClass Setting
	//if (TrirdPersonClassRef.Class)
	//{
	//	DefaultPawnClass = TrirdPersonClassRef.Class;
	//}

	static ConstructorHelpers::FClassFinder<APawn> DefaultPawnClassRef(TEXT("/Script/Engine.Blueprint'/Game/Blueprint/BP_ABCharaterPlayer.BP_ABCharaterPlayer_C'"));
	//DefaultPawnClass Setting
	if (DefaultPawnClassRef.Class)
	{
		DefaultPawnClass = DefaultPawnClassRef.Class;
	}
	//PlayerControllerClass Setting
	static ConstructorHelpers::FClassFinder<APlayerController> PlayerControllerClassRef(TEXT("/Script/ArenaBattle.ABPlayerController"));
	if (PlayerControllerClassRef.Class)
	{
		PlayerControllerClass = PlayerControllerClassRef.Class;
	}
	
}
