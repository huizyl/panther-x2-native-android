# Licensing and provenance

This repository contains changes to multiple upstream projects. There is no single replacement license covering all upstream code.

- `projects/kernel-6.1/`: Linux kernel / DTS changes retain their existing notices and GPL-2.0 / applicable dual-license terms. Base commit is recorded in `source-snapshot.json`; upstream is the Radxa/Rockchip Android kernel named in the fixed manifests.
- Android platform, build, HAL and Java/C++ changes retain their original per-file Apache-2.0, BSD or other upstream terms. Software Gatekeeper files retain AOSP copyright and Apache-2.0 notices. Check the base project's LICENSE/NOTICE before redistribution.
- The new Python helper scripts and documentation are Apache-2.0. License texts are in `licenses/`; source files and patches retain their original headers.
- The manifests record upstream locations and exact revisions. Vendor override records explain inaccessible original inputs; they are part of the build provenance, not a claim that all vendor commits remain available forever.

No firmware binaries, bootloaders, application APKs, signing private keys, original equipment NVM, or user data are distributed here. A future binary release must separately satisfy corresponding-source and third-party firmware/vendor/WebView license obligations. Publishing this source snapshot is not such a binary release.
