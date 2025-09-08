# [005] Overhead HP text facing camera (Z-rotation)

요청 요약:
- 플레이어 머리 위에 현재 HP/전체 HP 텍스트를 표기하고, Z축 회전만으로 항상 카메라를 바라보게 처리.

응답:
- `ATpsCharacter`에 `UTextRenderComponent` 기반 HP 텍스트를 추가했습니다.
  - 파일: `Source/TpsAnimSystem/TpsCharacter.h`, `Source/TpsAnimSystem/TpsCharacter.cpp`
  - 컴포넌트: `HpText` (TextRender)
  - 배치: 캡슐 반높이 + `HpTextOffsetZ`(기본 30cm) 만큼 위에 위치
  - 표시 내용: `Health/MaxHealth` (정수 반올림)
  - 회전: 매 틱마다 플레이어 컨트롤러의 ControlRotation.Yaw + 180°를 사용하여 Z축으로만 회전, 카메라를 향하도록 설정
  - 갱신: `BeginPlay`, `ApplyDamage`, `RequestRespawn` 시점에 텍스트 업데이트

사용자 조정 포인트(에디터):
- `BP_TpsCharacter`에서 `UI|HPText` 카테고리의 `HpTextOffsetZ`로 높이 미세 조정
- `HpText`의 World Size/Color를 필요에 따라 변경 가능 (기본 24pt, White)

비고:
- 필요 시 UWidgetComponent로 교체하여 더 복잡한 UI(아이콘/프로그레스바)를 구현할 수 있습니다. 현 구현은 경량 텍스트 중심입니다.

