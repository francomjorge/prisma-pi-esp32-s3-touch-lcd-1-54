# B6 — PRISMA AP

Status: IMPLEMENTED — CI and physical validation pending.

## Scope
- Concurrent Wi-Fi AP + STA mode.
- Default appliance network: `PRISMA-PI`.
- Default AP IPv4 address: `192.168.4.1` using ESP-IDF default AP netif.
- Open AP by default for factory/first-access behavior.
- Runtime API supports WPA2 password configuration (minimum 8 characters).
- Up to 4 AP clients.
- AP client count exposed in the Wi-Fi state contract.
- Existing B5 STA credentials/reconnect behavior retained.

## Service contract
- `prisma_wifi_ap_start(ssid, password)` configures/enables the PRISMA AP.
- Empty password selects open authentication.
- Non-empty password requires at least 8 characters and selects WPA2-PSK.
- `prisma_wifi_ap_stop()` disables PRISMA exposure at the service-contract level.
- `prisma_wifi_get_state()` exposes AP enabled state, SSID, IP and client count together with STA state.

## Acceptance
1. ESP-IDF 5.5.1 build passes.
2. `PRISMA-PI` is visible after boot without requiring an external Wi-Fi network.
3. A phone/notebook can associate to the AP and receive an address.
4. Appliance remains reachable at `192.168.4.1` while AP is enabled.
5. STA can connect/reconnect while the AP remains available.
6. Configuring a valid WPA2 password protects the AP; open mode remains available for factory provisioning.
7. Existing display/touch/audio/IMU/TF behavior is not regressed.

## Deferred to B7
- Web Admin HTTP server.
- Captive/configuration portal UI.
- Admin-user password and web authentication.
- Browser-side Wi-Fi network selection/forget controls.
