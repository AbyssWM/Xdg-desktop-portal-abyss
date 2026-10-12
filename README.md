<div align="center">

<img src="https://github.com/AbyssWM.png?size=200" width="160" alt="Abyss logo">

# Xdg-desktop-portal-abyss

**xdg-desktop-portal backend for Abyssland.**

Part of [**Abyss**](https://github.com/AbyssWM) · Based on [xdg-desktop-portal-hyprland](https://github.com/hyprwm/xdg-desktop-portal-hyprland) by [hyprwm](https://github.com/hyprwm)

</div>

---

> [!WARNING]
> **Early development.** Abyss is being rebranded and reworked right now. Nothing is ready for daily use yet.

## About

The desktop portal for Abyssland: screen sharing, screenshots, global shortcuts and more for sandboxed and regular apps.

Xdg-desktop-portal-abyss is a fork of [xdg-desktop-portal-hyprland](https://github.com/hyprwm/xdg-desktop-portal-hyprland). Every Abyss library and tool is forked and renamed, so the whole ecosystem builds and updates as one – independent of system packages.

## Desktop integration

The backend uses `org.freedesktop.impl.portal.desktop.abyss`, `abyss.portal`, and the `Abyssland` desktop identity. Abyssland installs `abyssland-portals.conf` with `default=abyss;gtk`. The standard freedesktop portal interfaces and wlroots protocols keep their standard names; Abyssland extensions use `abyssland_*_v1`.

The picker executable is `abyss-share-picker` (the source directory remains `abyssland-share-picker`). Custom picker executables receive arguments directly, including `--allow-token`, and retain the `XDPH_WINDOW_SHARING_LIST` environment contract. Window IDs in that list are transient unsigned Wayland object IDs. Picker output uses `[SELECTION]r/window:ID`, `[SELECTION]/screen:OUTPUT`, or `[SELECTION]/region:OUTPUT@X,Y,W,H`; `r` records restore consent.

Window restore data stores the application ID and title. Matching uses the application ID without case sensitivity and title scoring when several windows match; ambiguous matches open the picker. New tokens contain no process addresses. Existing version 3 addresses are ignored, and version 2 monitor tokens remain supported. Invalid or foreign restore data opens the picker.

## Tests

Configure with `-DBUILD_TESTING=ON`, build, then run `ctest --test-dir build --output-on-failure`. The tests cover restore matching, typed D-Bus token round trips, malformed picker data, and the real Qt window button and restore checkbox using the offscreen platform. They do not verify PipeWire frames or restoration across a live compositor restart.

## The Abyss ecosystem

| Repository | Based on |
|---|---|
| [Abyssland](https://github.com/AbyssWM/Abyssland) | [Hyprland](https://github.com/hyprwm/Hyprland) |
| [Abyssal](https://github.com/AbyssWM/Abyssal) | [aquamarine](https://github.com/hyprwm/aquamarine) |
| [Abyssutils](https://github.com/AbyssWM/Abyssutils) | [hyprutils](https://github.com/hyprwm/hyprutils) |
| [Abyssparse](https://github.com/AbyssWM/Abyssparse) | [hyprlang](https://github.com/hyprwm/hyprlang) |
| [Abysscursor](https://github.com/AbyssWM/Abysscursor) | [hyprcursor](https://github.com/hyprwm/hyprcursor) |
| [Abyssgraphics](https://github.com/AbyssWM/Abyssgraphics) | [hyprgraphics](https://github.com/hyprwm/hyprgraphics) |
| [Abysswire](https://github.com/AbyssWM/Abysswire) | [hyprwire](https://github.com/hyprwm/hyprwire) |
| [Abyss-scanner](https://github.com/AbyssWM/Abyss-scanner) | [hyprwayland-scanner](https://github.com/hyprwm/hyprwayland-scanner) |
| [Abyssland-protocols](https://github.com/AbyssWM/Abyssland-protocols) | [hyprland-protocols](https://github.com/hyprwm/hyprland-protocols) |
| **Xdg-desktop-portal-abyss** | [xdg-desktop-portal-hyprland](https://github.com/hyprwm/xdg-desktop-portal-hyprland) |
| [Abysstoolkit](https://github.com/AbyssWM/Abysstoolkit) | [hyprtoolkit](https://github.com/hyprwm/hyprtoolkit) |
| [Abyss-guiutils](https://github.com/AbyssWM/Abyss-guiutils) | [hyprland-guiutils](https://github.com/hyprwm/hyprland-guiutils) |
| [AbyssDusk](https://github.com/AbyssWM/AbyssDusk) | [hyprsunset](https://github.com/hyprwm/hyprsunset) |
| [AbyssPicker](https://github.com/AbyssWM/AbyssPicker) | [hyprpicker](https://github.com/hyprwm/hyprpicker) |

## Credits

Xdg-desktop-portal-abyss exists thanks to **[xdg-desktop-portal-hyprland](https://github.com/hyprwm/xdg-desktop-portal-hyprland)**, created by **vaxerski & contributors** at **[hyprwm](https://github.com/hyprwm)**. All credit for the original work goes to them – please support the original project.

Abyss is an independent project and is **not affiliated with or endorsed by hyprwm**.

## License

This project is licensed under the **BSD 3-Clause License**. The original copyright notice is kept in [`LICENSE`](LICENSE) as required.

## Contributors

- [bb8TheRobot](https://github.com/bb8TheRobot)
- Claude (Anthropic) – AI pair programmer
