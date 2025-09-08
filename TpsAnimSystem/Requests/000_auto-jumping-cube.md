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

## Update 1 - 2025-09-08

요청 요약:
- 엔진에 내장된 파란 머티리얼이 있다면 큐브에 적용. 없다면 아무 것도 하지 않기.

응답:
- 액터 생성자에서 다음 엔진 내장 자산 경로를 순서대로 탐색해 적용하도록 수정했습니다. 존재하지 않으면 무시됩니다.
  - `/Engine/EditorMaterials/PersonaBoneUnselectedMaterial.PersonaBoneUnselectedMaterial`
  - `/Engine/EngineDebugMaterials/VertexColorViewMode_BlueOnly.VertexColorViewMode_BlueOnly`
- 어떤 후보도 발견되지 않으면 기본 머티리얼을 유지합니다(무동작).
- 필요 시 특정 머티리얼 경로를 지정하는 프로퍼티를 추가할 수 있습니다.

## Update 2 - 2025-09-08

요청 요약:
- `/Script/Engine.MaterialInstanceConstant'/Engine/TemplateResources/MI_Template_BaseOrange_Metal.MI_Template_BaseOrange_Metal'`을 최우선 머티리얼로 적용.

응답:
- 액터 생성자에서 위 경로의 머티리얼을 최우선으로 로드해 적용하도록 수정했습니다.
- 해당 자산이 없을 경우, 이전과 동일하게 블루 계열 후보들로 폴백하고, 모두 실패 시 아무 것도 하지 않습니다.

## Update 3 - 2025-09-08

요청 요약:
- 큐브에 물리를 적용.

응답:
- 큐브 `UStaticMeshComponent`에 물리 시뮬레이션을 활성화하고 충돌 프로파일을 `PhysicsActor`로 설정했습니다.
  - 시뮬레이션/중력/히트 이벤트 활성화: `SetSimulatePhysics(true)`, `SetEnableGravity(true)`, `SetNotifyRigidBodyCollision(true)`
- 점프 로직(주기 타이머)은 물리 사용 시 상향 임펄스 방식으로 전환했습니다.
  - `AddImpulse(Z=JumpImpulseStrength)` 적용, 착지로 판단되면 다음 임펄스까지 대기
  - 착지 판단: `OnComponentHit`에서 하향 속도 + 상향 노멀 충돌 시 `bIsJumping=false`
- 물리가 비활성인 경우에는 기존 보간(Tick) 기반 점프 로직을 유지합니다.
- 임펄스 세기는 내부 튜닝값(`JumpImpulseStrength=35000.0f`)으로 유지했습니다. 필요 시 노출 가능.

## Update 4 - 2025-09-08

요청 요약:
- 점프 힘을 약화시켜 대략 2m 높이까지만 튀어오르게 조정.

응답:
- 물리 점프 시 임펄스를 상수로 쓰지 않고, 목표 높이 `JumpHeight=200cm`에 맞춰 매번 필요한 초기 속도를 계산해 적용하도록 변경했습니다.
  - 공식: `v0 = sqrt(2 * |g| * h)` (단위: cm/s), `g`는 월드 중력(`GetGravityZ()`), `h=JumpHeight`.
  - `AddImpulse(..., bVelChange=true)`로 질량 무시 델타V 적용 → 어떤 질량이든 비슷한 높이 도달.
- 결과적으로 기본 설정에서 약 2m 점프를 목표로 합니다. 중력이 바뀌면 자동으로 높이가 보정됩니다.
- 필요 시 `JumpHeight`를 노출하여 에디터에서 직접 높이를 조정할 수 있도록 확장 가능합니다.
