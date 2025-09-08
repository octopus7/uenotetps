# [004] Fall damage by height

요청 요약:
- 플레이어가 추락 후 착지 시 낙하 높이에 비례한 피해를 적용.

응답:
- `ATpsCharacter`에 추락 피해와 체력 시스템(경량)을 추가했습니다.
  - 파일: `Source/TpsAnimSystem/TpsCharacter.h`, `Source/TpsAnimSystem/TpsCharacter.cpp`
  - 새 속성
    - Health: `MaxHealth`(100), `Health`(가변)
    - Damage | Fall: `bEnableFallDamage`(기본 true), `MinDamageHeight`(600cm), `MaxDamageHeight`(2000cm), `MaxFallDamage`(100)
    - Damage 계산식: 선형 스케일링 (Min~Max 구간 비례)
  - 동작:
    - 낙하 시작 시 Z 기록 → Landed 시점에 `FallStartZ - LandZ`로 높이를 계산하여 피해 적용
    - 예외적으로 Landed 미호출 상황을 대비해 틱에서도 낙하→비낙하 전환을 감지해 동일 로직 수행
    - 체력이 0 이하가 되면 기존 사망 처리(`HandleDeath`)와 UI 흐름을 그대로 사용
  - Blueprint 이벤트/함수:
    - `ApplyDamage(float)`, `GetHealth()`, `GetHealthNormalized()`
    - `OnHealthChanged(NewHealth, Delta)` (BPImplementableEvent)
- 문서 갱신:
  - `Documents/death-respawn-guide.md`에 "추락 피해(Fall Damage)" 섹션을 추가했습니다.

사용자 후속 단계(에디터):
1) `BP_TpsCharacter`에서 Damage|Fall, Health 값을 프로젝트 기준으로 조정
2) 낙하 테스트로 피해/사망/리스폰 플로우 확인 (`WBP_Death` 필요)

