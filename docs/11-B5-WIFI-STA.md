# B5 — Wi-Fi STA

Status: IMPLEMENTED — CI and physical validation pending.

## Scope
- ESP-IDF Wi-Fi station service.
- Persistent SSID/password in NVS.
- Automatic reconnect to saved network at boot.
- Explicit connect, disconnect and forget operations.
- Runtime state: idle/connecting/connected/failed.
- IPv4 address, RSSI and retry counter exposed to later UI/Web layers.
- Maximum automatic retry count prevents an endless reconnect loop.

## Security / behavior
- Credentials are never written to logs.
- `forget` erases the PRISMA Wi-Fi namespace from NVS.
- B5 does not create an access point; PRISMA AP belongs to B6.
- No hard-coded SSID or password is included in firmware.

## Physical acceptance
1. Boot without saved credentials remains stable and idle.
2. Connect to a valid 2.4 GHz WPA/WPA2 network.
3. DHCP obtains an IPv4 address.
4. State exposes SSID, IP and RSSI.
5. Reboot reconnects using NVS credentials.
6. Disconnect/reconnect works without reboot.
7. Forget removes credentials; reboot does not reconnect.
8. B0-B4 display/touch/audio/IMU/storage regression remains stable.
