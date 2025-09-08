# Death & Respawn (Fall Time) Guide

이 문서는 일정 시간 이상 추락 시 사망 처리하고, UI 버튼으로 시작 위치에서 리스폰하는 구성을 안내합니다. C++ 기능은 `ATpsCharacter`에 구현되어 있으며, UI 애셋(위젯)은 에디터에서 생성/연결합니다.

## 기능 개요 (C++)
- 연속 추락 상태에서 `FallDeathTimeThreshold`(기본 3초)를 초과하면 사망 처리됩니다.
- 사망 시:
  - 이동 비활성화, 입력은 UI 전용 모드로 변경, 마우스 커서 표시
  - `DeathWidgetClass`가 설정되어 있으면 위젯을 화면에 표시
- UI의 리스폰 버튼에서 캐릭터의 `RequestRespawn()`을 호출하면 시작 위치(`InitialSpawnTransform`)로 리스폰합니다.

## 캐릭터 설정 (BP_TpsCharacter)
1) `ATpsCharacter` 기반 블루프린트 열기
2) 기본값 조정
   - Death | Fall
     - `bEnableFallDeath` = true
     - `FallDeathTimeThreshold` = 원하는 초(예: 2.5)
   - Death | UI
     - `DeathWidgetClass` = 아래에서 생성할 `WBP_Death`

## UI 위젯 생성 (WBP_Death)
1) 콘텐츠 브라우저에서 User Widget 생성 → 이름: `WBP_Death`
2) 간단한 텍스트(“You Died”)와 Button(“Respawn”) 배치
3) Graph에서 Button의 OnClicked 바인딩 생성 후 다음 로직 구현
   - Get Owning Player Pawn → Cast to `ATpsCharacter` → Call `RequestRespawn`
4) 버튼에 Keyboard/Gamepad 네비게이션이 가능하도록 Focus를 버튼에 주는 것이 좋습니다
   - Event Construct에서 `Set Keyboard Focus`(Button)

## 작동 방식
- 캐릭터가 추락 상태(MOVE_Falling)이고 Z 속도가 음수일 때만 추락 시간 누적
- 누적이 `FallDeathTimeThreshold` 이상이면 `HandleDeath()` 호출 → 위젯 표시, 입력을 UI Only로 전환
- Respawn 버튼 클릭 시 `RequestRespawn()` → 시작 변환으로 텔레포트, 이동 재활성화, 입력 GameOnly로 복구, 위젯 제거

## 추락 피해(Fall Damage)
- 설정 위치: `BP_TpsCharacter`의 Details
  - Damage | Fall
    - `bEnableFallDamage` (true 시 활성)
    - `MinDamageHeight` (cm): 이 높이 이하 낙하는 무피해
    - `MaxDamageHeight` (cm): 이 높이 이상 낙하는 최대 피해
    - `MaxFallDamage`: 최대 피해량(기본 100)
  - Health
    - `MaxHealth`, `Health`(시작 체력)
- 계산식(선형 스케일)
  - height = max(0, FallStartZ - LandZ)
  - height <= MinDamageHeight → 0
  - height >= MaxDamageHeight → MaxFallDamage
  - 그 사이: MaxFallDamage * (height - MinDamageHeight) / (MaxDamageHeight - MinDamageHeight)
- 적용 타이밍: 착지 시점(Landed). 이외에 특이 케이스에서 안전망으로 틱에서 착지 전환을 감지해 동일 계산을 수행합니다.
- 체력 0 이하가 되면 `HandleDeath()`가 호출되어 사망 처리됩니다.

## 트러블슈팅
- 사망 UI가 보이지 않음
  - `DeathWidgetClass`에 `WBP_Death`가 지정되었는지 확인
  - PlayerController가 캐릭터를 Possess하고 있는지 확인
- 버튼 클릭이 먹히지 않음
  - Death 상태에서 InputMode가 UIOnly로 설정됩니다. 버튼이 포커스를 받는지 확인
  - 여러 UI가 겹친 경우 ZOrder를 높게 설정(`AddToViewport(1000)` 사용)
- 리스폰 후 입력이 안 먹히거나 커서가 남아있음
  - `RequestRespawn()`에서 GameOnly + 커서 off로 복구합니다. 사용자 정의 UI 로직과 충돌이 없는지 확인

## 선택 사항
- GameMode에서 Pawn을 파괴하고 RestartPlayer로 새 Pawn을 스폰하는 방식도 가능합니다.
  - 이 경우 `RequestRespawn()`를 Blueprint에서 override(또는 커스텀 함수)하여 GameMode `RestartPlayer` 호출로 교체하세요.
