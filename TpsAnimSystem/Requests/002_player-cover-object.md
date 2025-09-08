# [002] Player Cover Object

요청 요약:
- 플레이어가 엄폐할 수 있는 오브젝트(커버 포인트/객체)와 탐지 컴포넌트 구현
- 디버그 기즈모로 엄폐 가능 지점을 표시하고, 근접 및 각도 조건 충족 시 색상 변화
- 플레이어 클래스에 의존하지 않고, 델리게이트/인터페이스 기반으로 동작 전달

응답:
- 독립적인 코드로 커버 포인트 컴포넌트, 커버 오브젝트 액터, 커버 탐지 컴포넌트를 추가했습니다. 기존 파일은 수정하지 않았습니다.
- 경로: `Independent/CoverSystem-20250908/` (프로젝트에 영향 없이 이동/통합 가능)

구성 개요
- UCoverPointComponent: 한 지점의 엄폐 정보를 보유하고 후보 판정/디버그 렌더링을 담당합니다.
- ACoverObjectActor: 여러 커버 포인트를 담는 액터로, 라인 기반 자동 생성 옵션을 제공합니다.
- UCoverDetectorComponent: 캐릭터/폰에 부착하여 주변 커버 포인트를 스캔, 최적 후보를 산출하고 델리게이트로 알립니다.
- ICoverUser: 선택적 인터페이스. 커버 변경 이벤트를 수신하려는 오너가 구현할 수 있습니다(필수 아님). 델리게이트만으로도 사용 가능.

디버그/기즈모
- 기본 색: 빨강(비후보), 노랑(거리 OK, 각도 미달), 초록(거리+각도 OK), 하늘색(현재 최적 후보 강조)
- UCoverDetectorComponent가 틱마다 후보를 평가하며, UCoverPointComponent의 DrawDebug를 호출해 색상 반영

통합 방법(UE 프로젝트로 옮길 때)
1) 파일 배치
   - 옵션 A: 기존 모듈의 `Source/<ModuleName>/Public|Private/`로 각 헤더/CPP를 이동
   - 옵션 B: 새 플러그인/모듈 `CoverSystem` 생성 후 동일 구조로 배치
2) Build.cs 의존성
   - `PublicDependencyModuleNames`에 `Core`, `CoreUObject`, `Engine` 포함
   - 디버그 드로잉 사용 시 `AIModule` 불필요, `DrawDebugHelpers`는 엔진에 포함됨
3) 에디터에서 배치
   - 레벨에 `ACoverObjectActor` 배치 → `AutoGenerate Line`을 켜고 라인 시작/끝, 간격, Normal/높이 설정으로 포인트 자동 생성 또는 수동으로 컴포넌트 추가
   - 캐릭터/폰 블루프린트에 `UCoverDetectorComponent` 추가 → ScanRadius/각도/거리 임계값 조정, 디버그 체크
4) 이벤트 바인딩(플레이어 수정 없이)
   - 캐릭터/폰 BP에서 `OnBestCoverChanged`, `OnCoverFound`, `OnCoverLost`에 바인딩해 UI 갱신/애니메이션 트리거
   - 입력에서 커버 키를 눌렀을 때 `RequestTakeCover()`를 호출 → 델리게이트 `OnTakeCoverRequested`로 커버 스냅 위치/방향 전달

사용 예(절차)
- 레벨에 ACoverObjectActor를 놓고 자동 생성으로 커버 라인 구성
- 캐릭터 BP에 UCoverDetectorComponent 추가 후 디버그 On
- 플레이 시(또는 PIE) 캐릭터가 커버 라인 근처에서 바라보면 포인트가 초록/하늘색으로 변함
- 커버 키 입력 → Detector의 `RequestTakeCover()` 호출 → 스냅 위치/회전 수신 → 이동/상태 전환은 캐릭터 쪽에서 처리

제약/메모
- 플레이어 전용 구현 의존성 없음(APawn/ACharacter의 일반 속성만 사용). 델리게이트/인터페이스를 통해 동작 위임.
- 윈도우 런타임 의존 실행/검증은 수행하지 않았습니다. 로컬(윈도우)에서 통합 후 컴파일/검증해주세요.
- 입력 스킴 변경은 없으므로 `Documents/enhanced-input-guide.md` 갱신 대상 아님.

생성 파일
- `Independent/CoverSystem-20250908/README.md`
- `Independent/CoverSystem-20250908/Source/CoverSystem/Public/CoverTypes.h`
- `Independent/CoverSystem-20250908/Source/CoverSystem/Public/CoverPointComponent.h`
- `Independent/CoverSystem-20250908/Source/CoverSystem/Private/CoverPointComponent.cpp`
- `Independent/CoverSystem-20250908/Source/CoverSystem/Public/CoverObjectActor.h`
- `Independent/CoverSystem-20250908/Source/CoverSystem/Private/CoverObjectActor.cpp`
- `Independent/CoverSystem-20250908/Source/CoverSystem/Public/CoverDetectorComponent.h`
- `Independent/CoverSystem-20250908/Source/CoverSystem/Private/CoverDetectorComponent.cpp`
- `Independent/CoverSystem-20250908/Source/CoverSystem/Public/CoverUser.h` (선택 인터페이스)
