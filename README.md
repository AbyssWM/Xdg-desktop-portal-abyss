<div align="center">

<img src="https://github.com/AbyssWM.png?size=200" width="140" alt="Abyss logo">

# 🧩 Xdg-desktop-portal-abyss

**xdg-desktop-portal backend for Abyssland.**

[![License](https://img.shields.io/badge/license-BSD--3--Clause-blue?style=flat-square)](LICENSE) [![Fedora COPR](https://img.shields.io/badge/fedora-COPR-51a2da?style=flat-square&logo=fedora&logoColor=white)](https://copr.fedorainfracloud.org/coprs/bb8therobot1717/Abyss/) [![Wayland](https://img.shields.io/badge/wayland-native-ffbc00?style=flat-square&logo=wayland&logoColor=black)](https://wayland.freedesktop.org/) [![C++](https://img.shields.io/badge/C%2B%2B-26-00599c?style=flat-square&logo=cplusplus&logoColor=white)](https://isocpp.org/) [![Stars](https://img.shields.io/github/stars/AbyssWM/Xdg-desktop-portal-abyss?style=flat-square&color=7c3aed)](https://github.com/AbyssWM/Xdg-desktop-portal-abyss/stargazers)

[🌐 Website](https://abysswm.github.io/) · [📖 Wiki](https://abysswm.github.io/wiki/getting-started) · [📦 Install](#-install) · [🌊 Ecosystem](#-the-abyss-ecosystem)

<sub>Part of <a href="https://github.com/AbyssWM"><b>Abyss</b></a> · based on <a href="https://github.com/hyprwm/xdg-desktop-portal-hyprland">xdg-desktop-portal-hyprland</a> by <a href="https://github.com/hyprwm">hyprwm</a></sub>

</div>

---

> [!WARNING]
> **Experimental, pre-1.0.** Packages exist for several distributions, but expect rough edges and breaking changes.

## ✨ About

The desktop portal for Abyssland: screen sharing, screenshots, global shortcuts and more for sandboxed and regular apps.

Xdg-desktop-portal-abyss is a fork of [xdg-desktop-portal-hyprland](https://github.com/hyprwm/xdg-desktop-portal-hyprland). Every Abyss library and tool is forked and renamed, so the whole ecosystem builds and updates as one.

## Desktop integration

The backend uses `org.freedesktop.impl.portal.desktop.abyss`, `abyss.portal`, and the `Abyssland` desktop identity. Abyssland installs `abyssland-portals.conf` with `default=abyss;gtk`. The standard freedesktop portal interfaces and wlroots protocols keep their standard names; Abyssland extensions use `abyssland_*_v1`.

The picker executable is `abyss-share-picker` (the source directory remains `abyssland-share-picker`). Custom picker executables receive arguments directly, including `--allow-token`, and retain the `XDPH_WINDOW_SHARING_LIST` environment contract. Window IDs in that list are transient unsigned Wayland object IDs. Picker output uses `[SELECTION]r/window:ID`, `[SELECTION]/screen:OUTPUT`, or `[SELECTION]/region:OUTPUT@X,Y,W,H`; `r` records restore consent.

Window restore data stores the application ID and title. Matching uses the application ID without case sensitivity and title scoring when several windows match; ambiguous matches open the picker. New tokens contain no process addresses. Existing version 3 addresses are ignored, and version 2 monitor tokens remain supported. Invalid or foreign restore data opens the picker.

## Configuration

The portal currently reads `$XDG_CONFIG_HOME/abyss/xdph.conf`, or
`~/.config/abyss/xdph.conf` when `XDG_CONFIG_HOME` is unset. Start with
[`contrib/config.sample`](contrib/config.sample), which lists the supported
options with their defaults using the current Abyssparse syntax. The planned
compiled C++ configuration system is not yet integrated into this backend.

## Tests

Configure with `-DBUILD_TESTING=ON`, build, then run `ctest --test-dir build --output-on-failure`. The tests cover restore matching, typed D-Bus token round trips, malformed picker data, input-capture boundary validation including extreme coordinates, and the real Qt window button and restore checkbox using the offscreen platform. They do not verify PipeWire frames or restoration across a live compositor restart.

This repository currently has no maintained CI workflow. The inherited SourceHut
manifests targeted a different project and have been retired.

## 📦 Install

<details open>
<summary><b>Fedora 44 / 45</b> — COPR</summary>

```sh
sudo dnf copr enable bb8therobot1717/Abyss
sudo dnf install xdg-desktop-portal-abyss
```
</details>

<details>
<summary><b>Arch Linux</b> — PKGBUILD</summary>

```sh
git clone https://github.com/AbyssWM/arch-builds.git
cd arch-builds/packages/xdg-desktop-portal-abyss && makepkg -si
```
</details>

<details>
<summary><b>Gentoo</b> — overlay</summary>

```sh
eselect repository add abyss git https://github.com/AbyssWM/gentoo-builds.git
emaint sync -r abyss
emerge --ask gui-libs/xdg-desktop-portal-abyss
```
</details>

<details>
<summary><b>Void Linux</b> — xbps-src template</summary>

Copy `srcpkgs/xdg-desktop-portal-abyss` from [void-builds](https://github.com/AbyssWM/void-builds) into your `void-packages` checkout, then:

```sh
./xbps-src pkg xdg-desktop-portal-abyss && sudo xbps-install -R hostdir/binpkgs xdg-desktop-portal-abyss
```
</details>

<details>
<summary><b>Debian · Ubuntu · Linux Mint</b></summary>

Packaging lives in [debian-builds](https://github.com/AbyssWM/debian-builds) and [ubuntu-builds](https://github.com/AbyssWM/ubuntu-builds). A signed APT repository is on the way — see the [install guide](https://abysswm.github.io/wiki/getting-started) for the current status.
</details>

### 🔨 Build from source

Install the Abyss libraries it depends on first (see the [install guide](https://abysswm.github.io/wiki/getting-started)), then:

```sh
meson setup build --prefix=/usr --buildtype=release
meson compile -C build
sudo meson install -C build
```

## 🌊 The Abyss ecosystem

<details>
<summary>All 17 repositories</summary>

| Repository | Role | Based on |
|---|---|---|
| 🪟 [Abyssland](https://github.com/AbyssWM/Abyssland) | compositor | [Hyprland](https://github.com/hyprwm/Hyprland) |
| 📚 [Abyssal](https://github.com/AbyssWM/Abyssal) | library | [aquamarine](https://github.com/hyprwm/aquamarine) |
| 📚 [Abyssutils](https://github.com/AbyssWM/Abyssutils) | library | [hyprutils](https://github.com/hyprwm/hyprutils) |
| 📚 [Abyssparse](https://github.com/AbyssWM/Abyssparse) | library | [hyprlang](https://github.com/hyprwm/hyprlang) |
| 📚 [Abysscursor](https://github.com/AbyssWM/Abysscursor) | library | [hyprcursor](https://github.com/hyprwm/hyprcursor) |
| 📚 [Abyssgraphics](https://github.com/AbyssWM/Abyssgraphics) | library | [hyprgraphics](https://github.com/hyprwm/hyprgraphics) |
| 📚 [Abysswire](https://github.com/AbyssWM/Abysswire) | library | [hyprwire](https://github.com/hyprwm/hyprwire) |
| 🛠️ [Abyss-scanner](https://github.com/AbyssWM/Abyss-scanner) | tool | [hyprwayland-scanner](https://github.com/hyprwm/hyprwayland-scanner) |
| 📜 [Abyssland-protocols](https://github.com/AbyssWM/Abyssland-protocols) | protocols | [hyprland-protocols](https://github.com/hyprwm/hyprland-protocols) |
| 🧩 **Xdg-desktop-portal-abyss** | portal | [xdg-desktop-portal-hyprland](https://github.com/hyprwm/xdg-desktop-portal-hyprland) |
| 📚 [Abysstoolkit](https://github.com/AbyssWM/Abysstoolkit) | library | [hyprtoolkit](https://github.com/hyprwm/hyprtoolkit) |
| 🌙 [Abyss-guiutils](https://github.com/AbyssWM/Abyss-guiutils) | app | [hyprland-guiutils](https://github.com/hyprwm/hyprland-guiutils) |
| 🌙 [AbyssDusk](https://github.com/AbyssWM/AbyssDusk) | app | [hyprsunset](https://github.com/hyprwm/hyprsunset) |
| 🔒 [AbyssLock](https://github.com/AbyssWM/AbyssLock) | app | [hyprlock](https://github.com/hyprwm/hyprlock) |
| 🖼️ [AbyssPaper](https://github.com/AbyssWM/AbyssPaper) | app | [hyprpaper](https://github.com/hyprwm/hyprpaper) |
| 🎨 [AbyssPicker](https://github.com/AbyssWM/AbyssPicker) | app | [hyprpicker](https://github.com/hyprwm/hyprpicker) |
| 🔐 [AbyssPolkit](https://github.com/AbyssWM/AbyssPolkit) | app | [hyprpolkitagent](https://github.com/hyprwm/hyprpolkitagent) |

</details>

## 💜 Credits

Xdg-desktop-portal-abyss exists thanks to **[xdg-desktop-portal-hyprland](https://github.com/hyprwm/xdg-desktop-portal-hyprland)**, created by **vaxerski & contributors** at **[hyprwm](https://github.com/hyprwm)**. All credit for the original work goes to them – please support the original project.

Abyss is an independent project and is **not affiliated with or endorsed by hyprwm**.

## ⚖️ License

This project is licensed under the **BSD 3-Clause License**. The original copyright notice is kept in [`LICENSE`](LICENSE) as required.

## 👥 Contributors

- [bb8TheRobot](https://github.com/bb8TheRobot)
- Claude (Anthropic) – AI pair programmer
