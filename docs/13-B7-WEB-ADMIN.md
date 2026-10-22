# B7 — Web Admin

Status: IMPLEMENTED BASELINE — CI and physical validation pending.

## Scope
- Embedded HTTP server using ESP-IDF `esp_http_server`.
- Web Admin starts after B6 network initialization.
- Root endpoint `/` provides the appliance Web Admin entry point.
- Server is available from the PRISMA AP and from STA when the appliance has a LAN address.
- Service lifecycle exposes explicit start/stop calls.

## Baseline acceptance
1. ESP-IDF 5.5.1 build passes.
2. Join `PRISMA-PI` and open `http://192.168.4.1/`.
3. Browser receives the B7 Web Admin response.
4. Web server remains running while local appliance UI remains responsive.
5. STA mode can expose the same HTTP service on its DHCP address.
6. B0-B6 regressions remain stable.

## Next B7 increments
The B7 server is deliberately introduced first as a minimal compileable/runtime baseline. Subsequent B7 increments add the full administration UI and APIs already defined by the project specification: system/status dashboard, Wi-Fi configuration/forget, PRISMA AP configuration, admin password, TF file browser/metadata/preview operations, diagnostics and safe action endpoints.

## Not in B7
Realtime transport belongs to B8. ESP-NOW/discovery belongs to B9. BLE belongs to B10. Power/shutdown finalization belongs to B11.
