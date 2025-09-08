# Decoupled Actor Interaction in Unreal Engine

직접 타입 캐스팅을 피하고 느슨한 결합을 위한 권장 설계를 정리합니다. 호출자는 구현 타입에 의존하지 않고 계약/메시지/태그에 의존합니다.

## 핵심 옵션
- 인터페이스(권장 기본): `UInterface`/BP Interface로 상호작용 계약 정의. 호출 측은 `Implements<...>()` 확인 후 `I...::Execute_*` 호출만 수행. 구현은 액터 본체 대신 전용 컴포넌트가 담당하면 재사용성↑.
- 메시지 라우터(UE5): `UGameplayMessageSubsystem`으로 태그 채널 기반 Pub/Sub. 발행자는 채널+페이로드만 브로드캐스트, 구독자는 채널만 구독. 다대다/모듈 간 통신에 적합.
- GAS: Gameplay Ability System으로 상호작용을 능력/이벤트/태그로 모델링. 트리거는 `GameplayEvent`, 효과는 `GameplayEffect`, 피드백은 `GameplayCue`, 게이팅은 `GameplayTags`.
- 델리게이트+서브시스템: 생산자가 `Multicast Delegate` 노출, 소비자는 `UWorldSubsystem/UGameInstanceSubsystem` 등에서 등록·해제 관리. 수명/참조(TWeakObjectPtr) 관리가 명확할 때 적합.
- 컴포넌트+태그 기반: 기능별 컴포넌트(예: Damageable/Interactable) + `GameplayTags`로 능력/상태 노출. 호출자는 타입이 아니라 “능력 보유 여부”와 인터페이스에만 의존.

## 언제 무엇을 선택할까
- 1:1/간단한 상호작용(문 열기, 아이템 줍기): 인터페이스 기반이 가장 단순·안전.
- 다수 시스템 간 브로드캐스트(퀘스트, UI, 오디오 트리거): Gameplay Message Subsystem.
- 전투/버프/쿨다운/예측·복제 필요: GAS.
- 레벨 전역 서비스/레지스트리(예: 인터랙터 목록, 타겟팅 서비스): Subsystem(+델리게이트).

## 전형적인 상호작용 구조(권장)
- 캐릭터의 `InteractionComponent`가 라인트레이스 후 히트된 액터가 인터페이스 구현 여부만 검사.
- 상호작용 호출은 `IInteractable::Execute_TryInteract(Request)` 형태(Instigator, 컨텍스트를 구조체로 전달).
- 상호작용 성공/실패/상태 변화는 필요 시 `UGameplayMessageSubsystem`으로 채널 브로드캐스트(예: `Interaction.Completed`).
- 네트워크: 입력은 클라→서버 RPC로 검증, 서버가 성공 시 서버/클라 각각에서 메시지 또는 GAS 트리거. 복제는 결정 상태(태그/액션)만, 파생값은 로컬 계산.

## 장단점 요약
- 인터페이스: 단순, 안전, 컴파일 의존성 최소. 캐스팅 제거. 직접 참조는 필요.
- 메시지 라우터: 발행자·구독자 완전 분리, 다수 모듈 확장 용이. 네트워크 전송/복제는 별도 설계 필요.
- GAS: 규칙/복제/예측 포함한 표준 프레임워크. 초기 구조·학습 비용 큼.
- 델리게이트+서브시스템: 고성능·명확한 계약. 바인딩/해제와 수명 관리 주의.

## 실무 팁
- “상호작용 가능”은 인터페이스로, “조건/게이팅”은 `GameplayTags`로 표현해 잠금/쿨다운/상태 전이를 분리.
- 구현은 가급적 액터 본체가 아닌 전용 컴포넌트로 분리해 재사용성↑.
- 이벤트 팬아웃이 필요하면 인터페이스 호출 이후 메시지로 UI/사운드/퀘스트 등 파생 시스템에 통지.
- 델리게이트 바인딩은 Subsystem이 소유하고 약한 참조 사용, 레벨 전환 시 일괄 해제.
