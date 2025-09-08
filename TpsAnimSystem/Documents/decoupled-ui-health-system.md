# 플레이어 HP 비결합 UI 갱신 전략

목표: 플레이어/Pawn 클래스에 직접 바인딩하지 않고 UI(예: HP)를 갱신합니다. 생산자(게임플레이 상태)와 소비자(UI)를 이벤트/뷰모델/메시지 버스로 느슨하게 연결합니다.

## 권장 옵션(상황별 선택)
- GAS 사용 시: AbilitySystemComponent/AttributeSet의 속성 변경 델리게이트 + 경량 바인더(Subsystem)로 UI에 전달
- 블루프린트 중심/경량: HealthComponent + 델리게이트 + LocalPlayerSubsystem 바인더
- 생산자/소비자 다수: Gameplay Message Subsystem 채널(발행/구독)
- UMG 성능/테스트: UMG MVVM(ViewModel + FieldNotify) 푸시 바인딩

## 옵션 1: UMG MVVM(ViewModel) + Subsystem 바인더
- 데이터 모델: Health 전용 ViewModel(UObject)을 두고 Current/Max 등을 FieldNotify와 함께 노출합니다.
- 소유/수명: `ULocalPlayerSubsystem`(또는 `UGameInstanceSubsystem`)이 ViewModel 인스턴스를 생성·보관합니다.
- 공급자: 플레이어의 HealthComponent(또는 GAS)가 체력 변경 시 ViewModel 값을 갱신(푸시)합니다.
- 소비자(UI): 위젯은 MVVM 바인딩으로 ViewModel만 주입받아 사용하며, 플레이어 직접 참조가 없습니다.
- 장점: 낮은 결합도, 이벤트/푸시 기반(틱 바인딩 없음), 포제션/리스폰에도 견고, 모킹/테스트 용이.

## 옵션 2: Gameplay Message Subsystem(메시지 버스)
- 채널/페이로드: `FHealthChangedMessage { New, Max, Instigator… }` 와 같은 채널 키/페이로드를 정의합니다.
- 발행: HealthComponent가 체력 변경 시 해당 채널로 메시지를 발행합니다.
- 구독: UI 또는 UI 바인더가 채널을 구독하고 수신 시 뷰 상태를 갱신합니다.
- 장점: 다수 생산자/소비자 시 유리, 시스템 간 결합 최소화, 향후 UI/분석 추가가 용이.

## 옵션 3: HealthComponent + 델리게이트 + LocalPlayerSubsystem 바인더
- 생산자: `UHealthComponent`가 멀티캐스트 `OnHealthChanged` 델리게이트를 제공합니다.
- 중간자: `ULocalPlayerSubsystem`이 현재 Pawn의 HealthComponent에 대한 구독/해제를 관리(포제션 변화 대응)합니다.
- 소비자(UI): 바인더가 노출하는 읽기 전용 상태(Current/Max) 또는 UI용 델리게이트에 바인딩해 표시합니다.
- 장점: 구현 단순, 의존성 최소. MVVM/메시지 버스 대비 범용성은 낮습니다.

## 수명/네트워크 고려사항
- 포제션/리스폰: 바인더(Subsystem)가 Pawn 변경을 감지하면 이전 구독을 해제하고 새 Pawn/Component로 재구독합니다.
- 복제: 서버 권위 체력 변경 → OnRep/ASC 델리게이트/메시지 발행 → 클라이언트 UI가 수신 후 갱신합니다.
- 성능: UMG “직접/틱 바인딩”은 지양하고, FieldNotify/델리게이트/메시지 기반의 푸시 갱신을 권장합니다.
- 안전성: 약한 참조 및 유효성 체크로 UI 파괴 시 안전하게 해제되도록 하고, 구독/해제 순서를 일관되게 유지합니다.

## 선택 가이드
- 이미 GAS 사용: GAS 델리게이트 + Subsystem 바인더 또는 메시지 버스.
- 블루프린트 중심/간단: 옵션 3(델리게이트 + Subsystem 바인더).
- 장기 유지보수/테스트: 옵션 1(MVVM) 또는 메시지 버스 패턴.

## 비고
- 본 문서는 개념 가이드이며, 구현 코드는 포함하지 않습니다.
- 선택하신 옵션 기준으로 클래스 역할, 에셋 연결, 배선 절차를 별도 문서로 정리해 드릴 수 있습니다.
