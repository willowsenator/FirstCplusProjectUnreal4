// Fill out your copyright notice in the Description page of Project Settings.


#include "MainPlayerController.h"

#include "Blueprint/UserWidget.h"

void AMainPlayerController::BeginPlay()
{
    Super::BeginPlay();

    if (HUDOverlayAsset)
    {
        HUDOverlay = CreateWidget<UUserWidget>(this, HUDOverlayAsset);
        
        if (HUDOverlay)
        {
            HUDOverlay->AddToViewport();
            HUDOverlay->SetVisibility(ESlateVisibility::Visible);
        }
    }
    
    if (WEnemyHealthBar)
    {
        EnemyHealthBar = CreateWidget<UUserWidget>(this, WEnemyHealthBar);
        if (EnemyHealthBar)
        {
            EnemyHealthBar->AddToViewport();
            EnemyHealthBar->SetVisibility(ESlateVisibility::Hidden);
            const FVector2D Alignment(0.0f, 0.0f);
            EnemyHealthBar->SetAlignmentInViewport(Alignment);
        }
    }
    
    
}

void AMainPlayerController::DisplayEnemyHealthBar()
{
    if (EnemyHealthBar)
    {
        bEnemyHealthBarVisible = true;
        EnemyHealthBar->SetVisibility(ESlateVisibility::Visible);
    }
}

void AMainPlayerController::RemoveEnemyHealthBar()
{
    if (EnemyHealthBar)
    {
        bEnemyHealthBarVisible = false;
        EnemyHealthBar->SetVisibility(ESlateVisibility::Hidden);
    }
}

void AMainPlayerController::Tick(float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);
    if (EnemyHealthBar)
    {
        FVector2D PositionInViewport;
        ProjectWorldLocationToScreen(EnemyLocation, PositionInViewport);
        PositionInViewport.Y -= 50.f;
        
        const FVector2D SizeInViewport(300.f, 25.f);
        
        EnemyHealthBar->SetPositionInViewport(PositionInViewport, true);
        EnemyHealthBar->SetDesiredSizeInViewport(SizeInViewport);
        
        
    }
}