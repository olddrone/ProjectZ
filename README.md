언리얼 엔진 포트폴리오
- 엔진 버전 : UE 5.1
- 장르 : 3인칭 싱글플레이 Action RPG
- 영상 링크 : https://youtu.be/t-hS3bEyB8U
- C++ 및 BP 활용


요약
1. 캐릭터 구조
<src="https://user-images.githubusercontent.com/45356392/227791166-aafcac46-6699-4e42-af3c-badfdc6a1705.png"/>

BaseCharacter : 장착 무기, 액터 컴포넌트(Attribute, Target) 등 플레이어와 적군이 공통적으로 가지고 있는 메소드 및 멤버 구현
<src="https://user-images.githubusercontent.com/45356392/227791212-e0c2cf14-9ddd-4412-9deb-6cdfe2ebd1fb.png"/>

<src="https://user-images.githubusercontent.com/45356392/227791213-11bd56cd-8055-4f59-a958-ef4fb7b97f2c.png"/>

<src="https://user-images.githubusercontent.com/45356392/227791215-fdd809e6-281b-4138-b7d8-b7a62e3a6228.png"/>


APlayerCharacter : 액터 컴포넌트(Trail, Camera Shake) 등
<img width="80%" src=""/>
<img width="80%" src=""/>
<img width="80%" src=""/>

2. 텔레포트
<img width="80%" src=""/>

Widget Area(Sphere Collision) 접촉 
<img width="80%" src=""/>

Teleport Area(Capsule Collision) 접촉 시
<img width="80%" src=""/>

3. 레벨 로딩

추후 프로젝트 개발 시 개선사항
- 상수들을 데이터 테이블로 처리해야 하는데 하드 코딩된 부분
- Money, Chip에 대한 사용처
- 적군 AI를 BehaviorTree 혹은 UE5에서 제공하는 StateTree 혹은 Smart Objects로구현했으면 하는 아쉬움
- 최대한 액터 컴포넌트 단위로 작성했어야 하는 아쉬움
