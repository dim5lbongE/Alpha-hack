# Alpha Hack 2.2.1 Beta — 개발 중

Geometry Dash **2.2091** / Geode **v6.0.0-alpha.1** / 기존 모드 ID `dim5l.practice_sync` 유지.

이 폴더는 **수정한 소스코드**입니다. Windows DLL 및 Android SO를 새로 빌드하지 않았습니다. 따라서 2.2.1의 설치 가능한 `.geode` 파일이 아니며, 사용자가 2.2.0 `.geode`를 실행하면 기존 2.2.0 기능만 작동합니다.

## 이번 변경

- **Speedhack**: cocos 스케줄러와 디렉터 시간 값을 이중 변경하던 코드를 제거했습니다. 대신 활성 플레이 중 `GJBaseGameLayer::update`에 0.10~5.00× 시간 배율을 한 번만 적용합니다. 메뉴·다른 장면 시간은 건드리지 않습니다.
- **TPS Bypass (실험)**: Gameplay 메뉴에 토글과 TPS 목표 입력을 추가했습니다. 120~1000 설정을 받아 `GJBaseGameLayer::getModifiedDelta`의 잔여 시간을 관리하면서 물리 스텝을 분할하는 방식을 적용했습니다. 240 TPS는 게임 기본 상태를 유지합니다.
- **FPS Bypass**: Display 메뉴로 이동했습니다. FPS는 렌더링 속도, TPS는 게임 물리 업데이트 속도입니다.
- **수정된 기록 보호**: 비기본 TPS 사용도 기존 Alpha Hack 치트 표시 및 게임 테스트 모드에 포함시킵니다. 다른 모드의 기록 보호는 보증하지 않습니다.
- **기존 기능 유지**: 연습 음악, Noclip, HUD, 로컬 잠금 해제, 음악 파일 교체, VSync, Smooth Fix 등.

## 사용 방법 (빌드 후)

PC는 **Tab**으로 모드를 열고, Android는 게임 안의 **Alpha** 버튼을 누르세요.
Gameplay에서 **Speedhack**을 켜고 슬라이더를 조정합니다. TPS Bypass를 켠 다음 TPS 목표를 120~1000 범위로 입력하고 **Apply TPS**를 누릅니다.

**주의:** TPS 240이 아닌 설정은 게임 물리 판정을 변경합니다. 다른 TPS/Physics Bypass, Click Between Frames 계열 모드와 동시에 사용하면 충돌하거나 테스트 결과가 달라질 수 있습니다. 기록 제출 전에는 해당 규정을 확인하세요.

## 검증 및 배포 상태

- `tests/core_test.cpp`: 물리 시간 누적·잔여 시간·제한값 테스트 **통과** (216 FPS/TPS/Speed 조합)
- Geode SDK의 C++ 헤더를 통한 **네이티브 컴파일 미실시** (이 환경에 Geode SDK 없음)
- Windows·Android 실제 게임 실행 및 입력 타이밍 확인 **미실시**
- `.geode` 배포 **보류** — 설치 가능한 최신 바이너리가 없는 상태

재빌드 지침은 `BUILD.md`, 검증 체크리스트는 `VALIDATION.txt`를 참고하세요.
