# 언리얼 엔진 포트폴리오
- 엔진 버전 : UE 5.1
- 장르 : 3인칭 싱글플레이 Action RPG
- 영상 링크 : https://youtu.be/t-hS3bEyB8U
- C++ 및 BP 활용


# 요약
1. 캐릭터 구조<br>
<img src="https://user-images.githubusercontent.com/45356392/227791166-aafcac46-6699-4e42-af3c-badfdc6a1705.png"/>

- BaseCharacter : 장착 무기, 액터 컴포넌트(Attribute, Target) 등 플레이어와 적군이 공통적으로 가지고 있는 메소드 및 멤버 구현
<img src="https://user-images.githubusercontent.com/45356392/227845605-8c1b3977-867f-45c4-8178-e7745d084fae.png"/>

<img src="https://user-images.githubusercontent.com/45356392/227845848-05e00c63-7469-4d50-8c17-6752cbb20bb8.png"/>
<img src="https://user-images.githubusercontent.com/45356392/227846044-9689ad84-fb79-4d65-8d4b-28385b265c4a.png"/>
<img src="https://user-images.githubusercontent.com/45356392/227845917-4b1db849-5b3c-4008-9a61-fcb513479817.png"/>

- APlayerCharacter : 액터 컴포넌트(Trail, Camera Shake) 등
<img src="https://user-images.githubusercontent.com/45356392/227848912-14fb3964-c408-49e6-ac82-e26f5acf9bc2.png"/>
<img src="https://user-images.githubusercontent.com/45356392/227849192-298e329b-1169-40de-b95e-0828ff4fbfa3.gif"/>
<img src="https://user-images.githubusercontent.com/45356392/227848577-b5c472ae-b60a-4267-9b76-7f72af5f6645.gif"/>

2. 텔레포트
<img src="https://user-images.githubusercontent.com/45356392/227851113-9d6ccdfc-9f75-49a0-99e8-6ed9e7808e60.png"/>

- Widget Area(Sphere Collision) 접촉 시
<img src="https://user-images.githubusercontent.com/45356392/227850714-54ef9891-5d56-4b0a-a982-2a1b3f3fb779.gif"/>

- Teleport Area(Capsule Collision) 접촉 시
<img src="https://user-images.githubusercontent.com/45356392/227850722-fa121e97-1780-46a9-a0b1-eccaab93dfaf.gif"/>


3. 레벨 로딩
LoadPackageAsync()를 통한 비동기식 레벨 로딩 및 OpenLevel() 시 Options을 통해 레벨 이동 시 각각 세이브 데이터의 Attribute와 Location로 초기화할지 판단
``` cpp
void AProjectZGameModeBase::InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage)
{
	Super::InitGame(MapName, Options, ErrorMessage);
	FString LoadData = UGameplayStatics::ParseOption(Options, "LoadData");
	FString Location = UGameplayStatics::ParseOption(Options, "SetPosition");

	bLoadData = LoadData.ToBool();
	bLocation = Location.ToBool();
}

void AProjectZGameModeBase::AsyncLevelLoad(const FString& LevelDir, const FString& LevelName, const FString& Options)
{
	LoadPackageAsync(LevelDir + LevelName, FLoadPackageAsyncDelegate::CreateLambda([=](
		const FName& PackageName, UPackage* LoadedPackage, EAsyncLoadingResult::Type Result)
			{
				if (Result == EAsyncLoadingResult::Succeeded)
					AsyncLevelLoadFinished(LevelName, Options);
			}
	), 0, PKG_ContainsMap);
}

void AProjectZGameModeBase::AsyncLevelLoadFinished(const FString LevelName, const FString Options)
{
	UGameplayStatics::OpenLevel(GetWorld(), FName(*LevelName), true, Options);
}
```
NewGame<br>
<img src="https://user-images.githubusercontent.com/45356392/227853652-9e8e0fb1-00ab-4c3e-9b3e-4bce640784c0.png"/>
<br>LoadGame<br>
<img src="https://user-images.githubusercontent.com/45356392/227853674-7e7353ce-4523-4ea1-98d8-f2e95048ae5b.png"/>
<br>텔레포트를 통한 이동 시<br>
<img src="https://user-images.githubusercontent.com/45356392/227854152-61d5a68a-4791-4d55-bdb0-a0b5756f5e51.png"/>

# 추후 프로젝트 개발 시 개선사항
1. 코드에 작성된 상수들을 하드코딩이 아닌 데이터 테이블로 처리
2. Money, Chip 등 활용 콘텐츠 기획 방안
3. 적군 AI를 Behavior Tree, State Tree, Smart Object로 구현해야 확장하기 용이
4. 프로젝트 및 클래스의 구조를 먼저 생각 후 액터 컴포넌트로 나누는 등 큰그림을 그려야함
