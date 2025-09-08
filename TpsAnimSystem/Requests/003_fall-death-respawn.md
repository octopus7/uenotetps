# [003] Fall death with UI respawn

요청 요약:
- 일정 시간 이상 추락하면 사망 처리.
- UI에서도 사망을 알리고, ‘Respawn’ 버튼을 누르면 시작 위치에서 다시 스폰.

응답:
- `ATpsCharacter`에 낙하 시간 기반 사망/리스폰 로직을 구현했습니다.
  - 파일: `Source/TpsAnimSystem/TpsCharacter.h`, `Source/TpsAnimSystem/TpsCharacter.cpp`
  - 새 속성/기능:
    - `FallDeathTimeThreshold`(기본 3.0초), `bEnableFallDeath`
    - `bIsDead`, `CurrentFallTime`, `InitialSpawnTransform`
    - `DeathWidgetClass`(UI 위젯 클래스 참조), `RequestRespawn()` (BlueprintCallable)
    - `OnDied()`, `OnRespawned()` (BlueprintImplementableEvent)
  - 동작:
    - MOVE_Falling 상태에서 Z 속도가 음수일 때만 낙하 시간 누적 → 임계 초과 시 사망 처리
    - 사망 시 이동 비활성화, 입력 UIOnly 전환, 마우스 커서 on, 사망 위젯 표시
    - Respawn 버튼(위젯)에서 `RequestRespawn()` 호출 시 시작 위치로 텔레포트, 이동/입력 복구, UI 제거
- UMG 의존성 추가로 Build 스크립트를 갱신했습니다.
  - `Source/TpsAnimSystem/TpsAnimSystem.Build.cs` → `PrivateDependencyModuleNames`에 `UMG`, `Slate`, `SlateCore` 추가
- 사용자(에디터) 작업 가이드 문서를 추가했습니다.
  - `Documents/death-respawn-guide.md`에 UI 위젯 생성과 버튼 바인딩(Respawn) 절차, 트러블슈팅 수록

사용자 후속 단계(에디터):
1) `Documents/death-respawn-guide.md`를 참고해 `WBP_Death`(Respawn 버튼 포함) 생성
2) `BP_TpsCharacter`의 `DeathWidgetClass`에 `WBP_Death` 할당
3) `FallDeathTimeThreshold`(예: 2.5) 등 파라미터 조정 후 플레이로 검증

## Update 1 - 2025-09-08
요청 요약: 문서 디렉터리 `Docs/` → `Documents/`로 변경, 그리고 기존 `002` 번호 충돌 정리
응답: 파일을 `003_fall-death-respawn.md`로 리네임하고, 문서 경로를 `Documents/`로 갱신했습니다.

비고:
- 기본 구현은 동일 Pawn의 위치/상태 리셋 방식입니다. GameMode 재스폰 방식이 필요하면 `RequestRespawn()` 호출 경로를 Blueprint에서 GameMode `RestartPlayer` 호출로 치환하세요.
