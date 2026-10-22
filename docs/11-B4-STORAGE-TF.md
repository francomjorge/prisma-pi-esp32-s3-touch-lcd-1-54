# B4 — Storage / TF

Status: IMPLEMENTED — CI and physical validation pending.

## Scope
- Native SDMMC interface using the board BSP TF pin map.
- 4-bit bus: CMD GPIO15, CLK GPIO16, D0 GPIO17, D1 GPIO18, D2 GPIO13, D3 GPIO14.
- FAT filesystem mounted at `/sdcard`.
- Card capacity and free-space reporting.
- Non-destructive read/write self-test using a temporary `.prisma_b4_test` file.
- No automatic formatting if mount fails.

## Safety
B4 never formats the card automatically. Existing user files are not intentionally modified. The self-test creates, verifies and removes only `/sdcard/.prisma_b4_test`.

## Acceptance
1. ESP-IDF 5.5.1 CI build passes.
2. FAT32 TF card mounts at `/sdcard`.
3. Capacity/free-space values are sane.
4. Temporary-file write/read/compare/delete self-test passes.
5. Existing files remain intact.
6. Display, touch, audio and IMU regressions remain functional.
7. 30-minute run shows no watchdog, panic or unexplained reset.

## Deferred
- File browser UI/Web Admin integration.
- Hot-plug policy.
- Recording assets directly to TF.
- Filesystem quotas and media indexing.
