# [000] Auto jumping cube

요청 요약:
- 스스로 주기적으로 점프하는 큐브 메시 클래스를 생성.
- 기본 주기는 2초이며, 에디터에서 조정 가능하게 노출.

응답:
- 신규 UE C++ 액터 `AAutoJumpingCube`를 추가했습니다.
  - 파일: `Source/TpsAnimSystem/AutoJumpingCube.h`, `Source/TpsAnimSystem/AutoJumpingCube.cpp`
  - 구성: `UStaticMeshComponent`(엔진 기본 Cube 사용), 자체 타이머로 주기적 점프.
  - 노출 속성: `JumpIntervalSeconds` (기본 2.0초, `EditAnywhere`, `BlueprintReadWrite`).
  - 동작: 타이머마다 점프 시작 → 위로 상승 → 하강 후 착지(부드러운 ease in/out), 점프 중 중복 트리거 방지.

사용 방법(로컬 윈도우 에디터에서):
1) 프로젝트를 빌드합니다(에이전트는 빌드/실행을 시도하지 않습니다).
2) 에디터에서 `Place Actors > All Classes`에 `AutoJumpingCube`를 배치합니다.
3) 디테일 패널에서 `JumpIntervalSeconds`로 주기를 조정합니다(기본 2.0s, 최소 0.05s 권장).

비고:
- 점프 높이/페이즈 시간은 내부 튜닝값으로 고정되어 있으며, 요청 범위에 맞춰 노출은 주기만 제공합니다.
- 추가 파라미터 노출이나 블루프린트 버전이 필요하면 말씀해 주세요.

