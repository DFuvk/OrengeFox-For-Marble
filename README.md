<p align="center">
  <img src="assets/social-preview.png" alt="OrangeFox Recovery untuk Redmi Note 12 Turbo (marble)">
</p>

<p align="center">
  <img src="https://img.shields.io/badge/OrangeFox-Recovery-FF6E1E?style=for-the-badge&logo=android&logoColor=white" alt="OrangeFox Recovery">
  <img src="https://img.shields.io/badge/Device-marble-blue?style=for-the-badge" alt="Device marble">
  <img src="https://img.shields.io/badge/License-Apache%202.0-green?style=for-the-badge" alt="Apache License 2.0">
</p>

# OrangeFox Recovery untuk Redmi Note 12 Turbo (marble)

Repo ini isinya build OrangeFox Recovery (OFRP) buat Redmi Note 12 Turbo, codename **marble**. Device tree-nya nggak saya simpan di sini. Waktu build, GitHub Actions yang sync source `fox_12.1`, clone device tree, build recovery, terus upload hasilnya ke Releases.

Device tree default yang dipakai: [Ctapchuk/android_device_xiaomi_marble-OFRP](https://github.com/Ctapchuk/android_device_xiaomi_marble-OFRP), branch `fox_12.1`. Kalau mau coba tree lain, ganti input `DEVICE_TREE_URL` sama `DEVICE_TREE_BRANCH` pas run workflow.

## Isi repo

- `.github/workflows/OrengeFox.yml` - workflow build-nya
- `.github/dependabot.yml` - update action GitHub tiap minggu
- `assets/social-preview.png` - banner repo

## Fitur

- Tema OrangeFox, ada dark mode
- Flash zip, img, dan OTA
- File manager, terminal, aFTP
- Backup/restore partisi
- Decrypt data, aman buat yang pakai format data
- MTP, ADB sideload, ADB shell

## Build

1. Buka tab **Actions**, pilih workflow **Recovery OFOX Build (fox_12.1) — marble**, lalu klik **Run workflow**.
2. Biarkan default atau ganti input sesuai kebutuhan (device tree, branch, nama maintainer, patch version).
3. Tunggu sekitar 1 jam. Kalau berhasil, zip-nya muncul di **Releases**.

Workflow ini cuma bisa dijalanin owner repo. Kalau build gagal, `build.log` lengkapnya ikut diupload sebagai artifact dan disimpan 7 hari.

## Install

Tiap release isinya satu file zip recovery, namanya kira-kira `OrangeFox-R12.0_<patch>-Unofficial-marble.zip`.

1. Download zip-nya dari **Releases**.
2. Boot ke custom recovery yang sudah jalan (misalnya OrangeFox atau TWRP lama buat marble).
3. Flash zip-nya.
4. Reboot recovery lagi buat mastiin hasilnya.

Release ini nggak nyediain `.img`, jadi zip-nya harus di-flash dari recovery yang sudah ada. Bootloader juga harus sudah unlocked.

## Credits

- [OrangeFox Recovery Project](https://orangefox.tech)
- [Ctapchuk/android_device_xiaomi_marble-OFRP](https://github.com/Ctapchuk/android_device_xiaomi_marble-OFRP) - device tree
- Teman-teman di komunitas TWRP/OFRP yang udah bantu buat marble

## Disclaimer

Flash custom recovery bisa bikin garansi hangus, bootloop, atau brick kalau salah langkah. Resikonya ditanggung sendiri. Saya sebagai pemilik repo ini nggak bertanggung jawab kalau ada kerusakan di perangkat kamu.

## Lisensi

Apache License 2.0, ngikut lisensi upstream OrangeFox/TWRP/AOSP.
