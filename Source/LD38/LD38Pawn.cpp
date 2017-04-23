// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#include "LD38.h"
#include "LD38Pawn.h"
#include "LD38WheelFront.h"
#include "LD38WheelRear.h"
#include "LD38Hud.h"
#include "Components/SkeletalMeshComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/InputComponent.h"
#include "WheeledVehicleMovementComponent4W.h"
#include "Engine/SkeletalMesh.h"
#include "Engine.h"
#include "LD38GameMode.h"
#include "Checkpoint.h"
#include "HttpModule.h"
#include "IHttpRequest.h"
#include "IHttpResponse.h"
#include "GenericPlatform/GenericPlatformHttp.h"
#include "DefaultValueHelper.h"

// Needed for VR Headset
#if HMD_MODULE_INCLUDED
#include "IHeadMountedDisplay.h"
#endif // HMD_MODULE_INCLUDED

const FName ALD38Pawn::LookUpBinding("LookUp");
const FName ALD38Pawn::LookRightBinding("LookRight");

#define LOCTEXT_NAMESPACE "VehiclePawn"

ALD38Pawn::ALD38Pawn()
{
	// Car mesh
	static ConstructorHelpers::FObjectFinder<USkeletalMesh> CarMesh(TEXT("/Game/Vehicle/Sedan/Sedan_SkelMesh.Sedan_SkelMesh"));
	GetMesh()->SetSkeletalMesh(CarMesh.Object);

	static ConstructorHelpers::FClassFinder<UObject> AnimBPClass(TEXT("/Game/Vehicle/Sedan/Sedan_AnimBP"));
	GetMesh()->SetAnimInstanceClass(AnimBPClass.Class);

	// Simulation
	UWheeledVehicleMovementComponent4W* Vehicle4W = CastChecked<UWheeledVehicleMovementComponent4W>(GetVehicleMovement());

	check(Vehicle4W->WheelSetups.Num() == 4);

	Vehicle4W->WheelSetups[0].WheelClass = ULD38WheelFront::StaticClass();
	Vehicle4W->WheelSetups[0].BoneName = FName("Wheel_Front_Left");
	Vehicle4W->WheelSetups[0].AdditionalOffset = FVector(0.f, -12.f, 0.f);

	Vehicle4W->WheelSetups[1].WheelClass = ULD38WheelFront::StaticClass();
	Vehicle4W->WheelSetups[1].BoneName = FName("Wheel_Front_Right");
	Vehicle4W->WheelSetups[1].AdditionalOffset = FVector(0.f, 12.f, 0.f);

	Vehicle4W->WheelSetups[2].WheelClass = ULD38WheelRear::StaticClass();
	Vehicle4W->WheelSetups[2].BoneName = FName("Wheel_Rear_Left");
	Vehicle4W->WheelSetups[2].AdditionalOffset = FVector(0.f, -12.f, 0.f);

	Vehicle4W->WheelSetups[3].WheelClass = ULD38WheelRear::StaticClass();
	Vehicle4W->WheelSetups[3].BoneName = FName("Wheel_Rear_Right");
	Vehicle4W->WheelSetups[3].AdditionalOffset = FVector(0.f, 12.f, 0.f);

	// Create a spring arm component
	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm0"));
	SpringArm->TargetOffset = FVector(0.f, 0.f, 0.f);
	SpringArm->SetRelativeRotation(FRotator(-15.f, 0.f, 0.f));
	SpringArm->SetupAttachment(RootComponent);
	SpringArm->TargetArmLength = 600.0f;
	//SpringArm->bEnableCameraRotationLag = true;
	//SpringArm->CameraRotationLagSpeed = 7.f;
	SpringArm->bInheritPitch = true;
	SpringArm->bInheritRoll = true;
	SpringArm->bInheritYaw = true;

	SpringArm->SetRelativeLocation(FVector(0, 0, 200.f));

	// Create camera component 
	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera0"));
	Camera->SetupAttachment(SpringArm, USpringArmComponent::SocketName);
	Camera->bUsePawnControlRotation = false;
	Camera->FieldOfView = 90.f;

	// Create In-Car camera component 
	InternalCameraOrigin = FVector(0.0f, -40.0f, 120.0f);

	InternalCameraBase = CreateDefaultSubobject<USceneComponent>(TEXT("InternalCameraBase"));
	InternalCameraBase->SetRelativeLocation(InternalCameraOrigin);
	InternalCameraBase->SetupAttachment(GetMesh());

	InternalCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("InternalCamera"));
	InternalCamera->bUsePawnControlRotation = false;
	InternalCamera->FieldOfView = 90.f;
	InternalCamera->SetupAttachment(InternalCameraBase);

	//Setup TextRenderMaterial
	static ConstructorHelpers::FObjectFinder<UMaterial> TextMaterial(TEXT("Material'/Engine/EngineMaterials/AntiAliasedTextMaterialTranslucent.AntiAliasedTextMaterialTranslucent'"));
	
	UMaterialInterface* Material = TextMaterial.Object;

	// Create text render component for in car speed display
	InCarSpeed = CreateDefaultSubobject<UTextRenderComponent>(TEXT("IncarSpeed"));
	InCarSpeed->SetTextMaterial(Material);
	InCarSpeed->SetRelativeLocation(FVector(70.0f, -75.0f, 99.0f));
	InCarSpeed->SetRelativeRotation(FRotator(18.0f, 180.0f, 0.0f));
	InCarSpeed->SetupAttachment(GetMesh());
	InCarSpeed->SetRelativeScale3D(FVector(1.0f, 0.4f, 0.4f));

	// Create text render component for in car gear display
	InCarGear = CreateDefaultSubobject<UTextRenderComponent>(TEXT("IncarGear"));
	InCarGear->SetTextMaterial(Material);
	InCarGear->SetRelativeLocation(FVector(66.0f, -9.0f, 95.0f));	
	InCarGear->SetRelativeRotation(FRotator(25.0f, 180.0f,0.0f));
	InCarGear->SetRelativeScale3D(FVector(1.0f, 0.4f, 0.4f));
	InCarGear->SetupAttachment(GetMesh());
	
	// Colors for the incar gear display. One for normal one for reverse
	GearDisplayReverseColor = FColor(255, 0, 0, 255);
	GearDisplayColor = FColor(255, 255, 255, 255);

	// Colors for the in-car gear display. One for normal one for reverse
	GearDisplayReverseColor = FColor(255, 0, 0, 255);
	GearDisplayColor = FColor(255, 255, 255, 255);

	bInReverseGear = false;
}

void ALD38Pawn::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	// set up gameplay key bindings
	check(PlayerInputComponent);

	PlayerInputComponent->BindAxis("MoveForward", this, &ALD38Pawn::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &ALD38Pawn::MoveRight);
	PlayerInputComponent->BindAxis("LookUp");
	PlayerInputComponent->BindAxis("LookRight");

	PlayerInputComponent->BindAction("Handbrake", IE_Pressed, this, &ALD38Pawn::OnHandbrakePressed);
	PlayerInputComponent->BindAction("Handbrake", IE_Released, this, &ALD38Pawn::OnHandbrakeReleased);
	PlayerInputComponent->BindAction("SwitchCamera", IE_Pressed, this, &ALD38Pawn::OnToggleCamera);

	PlayerInputComponent->BindAction("Reset", IE_Pressed, this, &ALD38Pawn::ResetToLastCheckpoint);

	PlayerInputComponent->BindAxis("Jump", this, &ALD38Pawn::OnJump);

	PlayerInputComponent->BindAction("ResetVR", IE_Pressed, this, &ALD38Pawn::OnResetVR); 
}

void ALD38Pawn::MoveForward(float Val)
{
	GetVehicleMovementComponent()->SetThrottleInput(Val);
	Throttle = Val;
}

void ALD38Pawn::MoveRight(float Val)
{
	GetVehicleMovementComponent()->SetSteeringInput(Val);
}

void ALD38Pawn::OnHandbrakePressed()
{
	GetVehicleMovementComponent()->SetHandbrakeInput(true);
}

void ALD38Pawn::OnHandbrakeReleased()
{
	GetVehicleMovementComponent()->SetHandbrakeInput(false);
}

void ALD38Pawn::OnToggleCamera()
{
	EnableIncarView(!bInCarCameraActive);
}

void ALD38Pawn::ResetToLastCheckpoint()
{
	if (RaceOver) return;

	AActor* targetCheckpoint = nullptr;

	for (TActorIterator<ACheckpoint> i(GetWorld()); i; ++i)
	{
		if (i->CheckpointNumber != NextCheckpoint)
		{
			targetCheckpoint = *i;
		}
	}

	if (targetCheckpoint)
	{
		SetActorLocation(targetCheckpoint->GetActorLocation() + targetCheckpoint->GetActorLocation().GetSafeNormal() * 300, false, nullptr, ETeleportType::TeleportPhysics);
		SetActorRotation(targetCheckpoint->GetActorLocation().Rotation());
	}
}

void ALD38Pawn::OnHighScoreResponse(FHttpRequestPtr req, FHttpResponsePtr resp, bool success)
{
	FString content = resp->GetContentAsString();

	TArray<FString> lines;

	content.ParseIntoArray(lines, TEXT("\n"));

	for (auto line : lines)
	{
		TArray<FString> parts;
		line.ParseIntoArray(parts, TEXT("\t"));

		CarNames.Add(parts[0]);
		Times.Add(FCString::Atoi(*parts[1]) / 1000.f);
	}

	UE_LOG(LogTemp, Display, TEXT("Got high scores"));
}

void ALD38Pawn::EnableIncarView(const bool bState, const bool bForce)
{
	/*if ((bState != bInCarCameraActive) || ( bForce == true ))
	{
		bInCarCameraActive = bState;
		
		if (bState == true)
		{
			OnResetVR();
			Camera->Deactivate();
			InternalCamera->Activate();
		}
		else
		{
			InternalCamera->Deactivate();
			Camera->Activate();
		}
		
		InCarSpeed->SetVisibility(bInCarCameraActive);
		InCarGear->SetVisibility(bInCarCameraActive);
	}*/
}


void ALD38Pawn::OnHitHandler(AActor * SelfActor, AActor * OtherActor, FVector NormalImpulse, const FHitResult & Hit)
{
	if (Cast<ALD38Pawn>(OtherActor))
	{
		//UE_LOG(LogTemp, Display, TEXT("%s HIT %s (%s)"), *GetName(), *OtherActor->GetName(), *FString::SanitizeFloat(NormalImpulse.Size()));

		UGameplayStatics::PlaySoundAtLocation(GetWorld(), HitSound, (GetActorLocation() + OtherActor->GetActorLocation()) / 2, NormalImpulse.Size() / 300000);
	}
}

void ALD38Pawn::Tick(float Delta)
{
	Super::Tick(Delta);

	// Setup the flag to say we are in reverse gear
	bInReverseGear = GetVehicleMovement()->GetCurrentGear() < 0;
	
	// Update the strings used in the hud (incar and onscreen)
	UpdateHUDStrings();

	// Set the string in the incar hud
	SetupInCarHUD();

	bool bHMDActive = false;
#if HMD_MODULE_INCLUDED
	if ((GEngine->HMDDevice.IsValid() == true) && ((GEngine->HMDDevice->IsHeadTrackingAllowed() == true) || (GEngine->IsStereoscopic3D() == true)))
	{
		bHMDActive = true;
	}
#endif // HMD_MODULE_INCLUDED
	if (bHMDActive == false)
	{
		if ( (InputComponent) && (bInCarCameraActive == true ))
		{
			FRotator HeadRotation = InternalCamera->RelativeRotation;
			HeadRotation.Pitch += InputComponent->GetAxisValue(LookUpBinding);
			HeadRotation.Yaw += InputComponent->GetAxisValue(LookRightBinding);
			InternalCamera->RelativeRotation = HeadRotation;
		}
	}

	//GetMesh()->AddImpulse(FVector(0, 0, -980 * Delta), NAME_None, true);
	GetMesh()->AddForce(FVector(0, 0, 980), NAME_None, true);
	GetMesh()->AddForce(GetMesh()->GetComponentLocation().GetSafeNormal() * -980, NAME_None, true);

	if (auto gameMode = Cast<ALD38GameMode>(GetWorld()->GetAuthGameMode()))
	{
		if (LapsCompleted >= gameMode->Laps && !RaceOver)
		{
			RaceOver = true;

			if (FinalPlace == 0)
			{
				for (TActorIterator<ALD38Pawn> i(GetWorld()); i; ++i)
				{
					if (*i == this) continue;

					FinalPlace = FMath::Max(i->FinalPlace + 1, FinalPlace);
				}
			}

			if (Cast<APlayerController>(GetController()))
			{
				auto req = FHttpModule::Get().CreateRequest();

				int32 duraInMilis = RaceDuration * 1000;

				//FString baseUri = "http://sigma/dyn/lighttpd";
				FString baseUri = "http://quadtree.info";

				req->SetVerb("GET");
				req->SetURL(baseUri + "/ld/ld38/hst.php?insert=1&map=" + FGenericPlatformHttp::UrlEncode(UGameplayStatics::GetCurrentLevelName(GetWorld())) + "&car=" + FString::FromInt(CarId) + "&time1=" + FString::FromInt(duraInMilis));

				req->OnProcessRequestComplete().BindUObject(this, &ALD38Pawn::OnHighScoreResponse);

				req->ProcessRequest();
			}
		}
	}

	if (RaceOver)
	{
		MoveForward(0);
		MoveRight(0);
		Jumping = false;
	}
	else
	{
		RaceDuration += Delta;
	}

	float velocity = 0;

	if (auto pc = Cast<UPrimitiveComponent>(RootComponent)) velocity = pc->GetPhysicsLinearVelocity().Size();
	FVector downForce = GetActorRotation().RotateVector(FVector(0, 0, -1) * velocity * SpoilerPower);

	GetMesh()->AddForce(downForce, NAME_None, true);

	GetMesh()->AddForce(GetActorRotation().RotateVector(FVector(0, 0, 1) * Jumping * JumpPower), NAME_None, true);
	GetMesh()->AddForce(GetActorRotation().RotateVector(FVector(1, 0, 0) * Throttle * ThrustPower), NAME_None, true);

	bool showThrusters = false;

	if (Jumping * JumpPower > 0.01f || Throttle * ThrustPower > 0.01f) showThrusters = true;

	for (auto a : GetComponentsByClass(UParticleSystemComponent::StaticClass()))
	{
		if (a->ComponentHasTag("Thruster"))
		{
			auto psc = Cast<UParticleSystemComponent>(a);

			if (psc)
			{
				//psc->bSuppressSpawning = !showThrusters;
				psc->SetActive(showThrusters);
			}
		}
	}

	if (ThrusterSoundComp) ThrusterSoundComp->SetVolumeMultiplier(showThrusters ? 1 : 0);

	for (auto a : GetComponentsByClass(USpringArmComponent::StaticClass()))
	{
		auto sac = Cast<USpringArmComponent>(a);

		if (sac)
		{
			if (sac->TargetArmLength > 2000)
			{
				sac->SetWorldRotation(this->GetActorLocation().Rotation() + FRotator(180,0,0));
			}
		}
	}

	if (MotorSoundComp)
	{
		MotorSoundComp->SetPitchMultiplier(GetVehicleMovementComponent()->GetEngineRotationSpeed() / 5600 * 1.5f);
	}
}

void ALD38Pawn::BeginPlay()
{
	Super::BeginPlay();

	bool bEnableInCar = false;
#if HMD_MODULE_INCLUDED
	bEnableInCar = UHeadMountedDisplayFunctionLibrary::IsHeadMountedDisplayEnabled();
#endif // HMD_MODULE_INCLUDED
	EnableIncarView(bEnableInCar,true);

	if (MotorSound) MotorSoundComp = UGameplayStatics::PlaySoundAttached(MotorSound, RootComponent);

	if (RocketSound) {
		ThrusterSoundComp = UGameplayStatics::PlaySoundAttached(RocketSound, RootComponent);
		ThrusterSoundComp->SetVolumeMultiplier(0);
	}

	OnActorHit.AddDynamic(this, &ALD38Pawn::OnHitHandler);
	//GetMesh()->OnComponentHit.AddDynamic(this, &ALD38Pawn::OnHitHandler);
}

void ALD38Pawn::OnResetVR()
{
#if HMD_MODULE_INCLUDED
	if (GEngine->HMDDevice.IsValid())
	{
		GEngine->HMDDevice->ResetOrientationAndPosition();
		InternalCamera->SetRelativeLocation(InternalCameraOrigin);
		GetController()->SetControlRotation(FRotator());
	}
#endif // HMD_MODULE_INCLUDED
}

void ALD38Pawn::ReachedCheckpoint(int32 checkpoint)
{
}

void ALD38Pawn::UpdateHUDStrings()
{
	float KPH = FMath::Abs(GetVehicleMovement()->GetForwardSpeed()) * 0.036f;
	int32 KPH_int = FMath::FloorToInt(KPH);

	// Using FText because this is display text that should be localizable
	SpeedDisplayString = FText::Format(LOCTEXT("SpeedFormat", "{0} km/h"), FText::AsNumber(KPH_int));
	
	if (bInReverseGear == true)
	{
		GearDisplayString = FText(LOCTEXT("ReverseGear", "R"));
	}
	else
	{
		int32 Gear = GetVehicleMovement()->GetCurrentGear();
		GearDisplayString = (Gear == 0) ? LOCTEXT("N", "N") : FText::AsNumber(Gear);
	}	
}

void ALD38Pawn::OnJump(float amt)
{
	Jumping = amt;
}

void ALD38Pawn::SetupInCarHUD()
{
	APlayerController* PlayerController = Cast<APlayerController>(GetController());
	if ((PlayerController != nullptr) && (InCarSpeed != nullptr) && (InCarGear != nullptr) )
	{
		// Setup the text render component strings
		InCarSpeed->SetText(SpeedDisplayString);
		InCarGear->SetText(GearDisplayString);
		
		if (bInReverseGear == false)
		{
			InCarGear->SetTextRenderColor(GearDisplayColor);
		}
		else
		{
			InCarGear->SetTextRenderColor(GearDisplayReverseColor);
		}
	}
}

#undef LOCTEXT_NAMESPACE
