# Meyve Bahçesi — approved design

The user approved a C++ desktop rewrite with three modes, original audio, responsive controls and a ready-to-run Windows executable, with cartoon art replacing neon. Their latest instruction explicitly authorizes starting implementation.

## Product

A warm, illustrated orchard arcade game, in Turkish. Cream paper panels, forest-green lettering, a rounded expressive snake, apples, flowers, clouds, shadows and confetti. All illustrations are drawn in code. Original synthesized music and effects require no external media downloads. Menu, mode selection, difficulty, pause, results and settings are keyboard and mouse accessible. F11 toggles fullscreen. Rendering uses a resizable letterboxed 1280×800 canvas.

## Rules

24×18 grid; arrow keys/WASD; two buffered turns, no reverse. Classic has lethal walls and body collisions. Endless wraps walls but body collisions remain lethal. Challenge adds fixed visible hedges. Each fruit awards 10 points; speed increases gradually. A completely filled board wins without attempting to choose food from an empty list. Three difficulty settings. Pause and focus loss stop simulation; resume counts down. Records are separate per mode and difficulty, stored with audio settings in the user's local application-data directory. Read/write failure must not prevent play.

## Architecture and delivery

C++17, raylib 5.5, MSVC x64. A standalone deterministic rules module has no graphics dependencies. A separate audio module synthesizes PCM music and effects in memory. The application owns input, state transitions, interpolation and procedural illustration. Bundle required runtime DLLs beside the EXE if necessary. Keep sources, third-party license, CMake and PowerShell build entry point. The superseded prototype was removed at the user's request after delivery.

## Verification

Rules tests: buffered turns, reversal, wall/body/hedge collisions, wrapping, growth, tail-vacating legality, board completion, fruit placement and difficulty bounds. Verify release compilation, synthesized signal output, portable paths, and actual render screenshots for menu/play/pause/results. Report any untested live UI behavior honestly.
