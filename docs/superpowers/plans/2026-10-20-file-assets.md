# File-based cartoon asset migration

The user requested real image and audio files and a sweet cartoon art direction. This continues the approved cartoon C++ game; implement directly in this session.

## Design

Keep the existing game rules and controls. Load authored illustrations, sprite atlas, board and interface skins from `assets/images`; use distributable Balsamiq Sans TTFs from `assets/fonts`; stream music and load effects from `assets/audio/*.wav`. All game-visible assets are copied beside the executable in `dist/assets`. Asset lookup is relative to the executable, independent of the working directory and supports Unicode Windows paths. Missing/corrupt assets produce a readable error instead of silent procedural replacements.

## Tasks

- [x] Write file-resolution/pack-integrity tests, observe failure, implement Unicode-safe asset reading.
- [x] Generate transparent cartoon mascot and sprite atlas plus an opaque quiet backdrop with Imagegen; preserve originals and prompts.
- [x] Add an explicit offline C++ export tool for the original PCM soundtrack/effects, grass board, matching snake body sprites and nine-patch panel files. Export once; shipping game never generates those resources.
- [x] Replace runtime procedural media with texture rendering, external TTFs and file-loaded WAV audio. Keep animation interpolation, particles and text/layout logic in C++.
- [x] Package assets automatically; update user documentation, verify asset contents, build and run complete tests, inspect rendered screens and test audio loading.

## Verification focus

Wrong working directory, missing/corrupt files, atlas cell boundaries, alpha preservation, continuous snake joins, font coverage for Turkish, WAV format/duration, and matching source/distribution asset content. Preserve user preferences and records.

## Verification record

- Initial file-loader stub failed the Unicode binary-reading, executable-directory and missing-pack assertions; implemented reader passed them.
- Final `./build.ps1 -Test`: 5/5 CTest groups passed (file paths, 168 rule checks, original audio, source media pack, distribution media pack).
- Both packs decoded 8 PNGs, 6 stereo WAVs, and 2 TTFs; alpha, atlas cell gutters, audio levels/duration and Turkish glyph checks passed.
- `MeyveBahcesi.exe --capture`: exit 0; inspected menu, play, pause and settings PNGs from actual external assets. Corrected atlas spacing with a targeted Imagegen edit.
- `MeyveBahcesi.exe --verify-media` from a different working directory: exit 0; initialized the actual audio device and loaded external music plus all effects. This verifies loading, not subjective listening quality.
- A copied executable with no assets folder returned exit 1 in capture mode without crashing. Source and distribution hashes matched after packaging.
- No further live keyboard playthrough was performed for this migration; rules and controls are unchanged.
