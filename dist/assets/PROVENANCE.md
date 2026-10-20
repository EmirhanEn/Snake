# Kaynaklar ve üretim kaydı

## Görseller

`mascot.png`, `sprites.png`, `backdrop.png`: OpenAI'nin yerleşik Imagegen aracıyla bu proje için üretilen yeni illüstrasyonlar. `sprites.png` için ikinci bir Imagegen düzenlemesiyle hücre aralıkları düzeltildi. Aşağıdaki istemler kullanıldı. Görseller harici bir oyundan veya stok paketinden alınmadı.

Diğer 4 PNG: `tools/export_assets.cpp` içindeki çevrimdışı çizim aracıyla bu proje için hazırlanıp PNG olarak kaydedildi. Bunlar oyun sırasında yeniden üretilmez. `snake-skin.png`, hücre başına tekrarlanan yuvarlak parçaların yerine kesintisiz gövde malzemesidir.

## Ses

6 WAV: `src/composer.cpp` içindeki özgün nota, ritim ve ses tasarımı, `tools/export_assets.cpp` ile PCM WAV dosyalarına aktarıldı. Müzik 112 BPM, 16 ölçü, stereo 22050 Hz / 16 bit biçimindedir. Oyun bu dosyaları yükler; üretici kod yalnızca geliştirme aracı ve test hedeflerinde bulunur.

## Yazı tipleri

Balsamiq Sans, Balsamiq Sans Project Authors. SIL Open Font License 1.1; tam metin `fonts/OFL.txt`.

Resmi dağıtım: [Google Fonts — Balsamiq Sans](https://github.com/google/fonts/tree/main/ofl/balsamiqsans). Fontlar değiştirilmeden paketlenmiştir.

## Imagegen istemleri

### Maskot

Use case: illustration-story. Asset type: transparent PNG hero illustration for a cute cartoon Snake desktop game called Orchard. Draw ONE adorable friendly lime-green snake, full body visible, curled into a soft S with head raised on the RIGHT, facing the viewer. Big expressive ivory eyes with dark forest-green pupils, tiny rosy peach cheeks, sweet smile, pale buttery belly and a few tiny subtle spots. Small red apple near tail and a little daisy. Visually polished children's picture-book cartoon with clean soft forest-green outlines, gentle hand-painted gouache shading, matte colors, rounded forms, no pixel art, no 3D plastic, no neon. Palette warm cream, sage, lime green, peach, muted coral. Composition: centered, all contents inside a square with 12% clear padding. Character fills 78% width and 74% height. Isolated on genuine fully TRANSPARENT background; no scene rectangle, no text, no logo, no watermark. Preserve transparent space around snake. Deliver a beautiful production game illustration, easy to read at 400px.

### Karakter sayfası

Use case: stylized-concept. Asset type: production 2D game SPRITE ATLAS, square canvas, exactly FOUR columns and FOUR rows of equal square cells (16 cells total). Genuine transparent background throughout, absolutely no grid lines, no labels, no text. Each sprite is centered in its own cell with generous padding, no bleed between cells. A coherent adorable orchard snake game style, hand-painted gouache cartoon, rounded shapes, soft dark forest-green outlines, matte pastel sage/lime, warm cream, muted coral, peach. Precise layout in reading order: Row1: (1) a cute round lime-green snake HEAD facing RIGHT, two big ivory eyes, rosy cheeks, smiling; (2) same head facing RIGHT with eyes closed blinking; (3) same head facing RIGHT excited open-mouth munching; (4) same head facing RIGHT dizzy eyes, sweet harmless cartoon loss. Row2: (1) horizontal lime-green snake BODY segment with pale highlight, (2) quarter-turn lime-green BODY corner bending from LEFT to UP, (3) tapering horizontal lime-green TAIL with tip pointing LEFT, (4) ripe red APPLE with stem and small leaf. Row3: (1) compact rounded lush green HEDGE bush from directly above, (2) small cream DAISY with yellow center, (3) small peach-pink FLOWER with yellow center, (4) soft sage LEAF pointing right. Row4: (1) small fluffy cream CLOUD, (2) friendly warm golden SUN with little rays, (3) rounded golden five-point STAR, (4) a tiny tuft of GRASS. Single clean isolated sprite per cell, entire object inside cell, no bases or scene backgrounds or cast ground shadows. Heads and body must share the same lime green shading and line weight. Cute, sweet, premium illustrated game art. NOT a screenshot or UI mockup, only the 4x4 sprite sheet. All 16 cells equal and precisely aligned.

### Arka plan

Use case: illustration-story. Asset type: widescreen background illustration for a desktop cartoon orchard game UI, landscape 16:10 composition. Warm ivory cream lightly textured paper fills the entire canvas. Broad CENTER and LEFT areas are almost empty light cream for overlaid text and game board, with NO objects or contrast in the central 85%. Only along the far outer corners: a few softly painted sage leaves, tiny daisies, delicate grass tufts, small peach flowers. Very pale sunny garden shapes and faint watercolor cloud wisps at the extreme top right and bottom left; low contrast subtle texture. Sweet cozy children's picture-book gouache style, rounded clean shapes, premium illustration, gentle lime/sage/cream/pale apricot palette. This is a quiet UI BACKDROP, not a full scene; absolutely no characters, no snakes, no apples, no text, no letters, no frames, no panels, no logo. Flat readable unobstructed ivory interior. Full opaque background.

### Karakter sayfası aralık düzeltmesi

Edit this sprite atlas only to improve production grid alignment and spacing. Preserve the exact same adorable watercolor cartoon art, colors, characters, 16 object identities and reading order. Recompose as a perfectly regular 4-column by 4-row square atlas of equal square cells. Every single sprite must be completely INSIDE its own cell with at least 12% transparent padding on EVERY side. Especially shrink the sun so ALL rays fit INSIDE row 4 column 2; its rays must not rise into row 3. Keep clouds entirely within row 4 column 1, and the hedge within row 3 column 1. Reduce all object scales consistently as needed to guarantee clear empty gutters. Transparent background, absolutely no labels, grids, borders, text or extra elements. Do not repaint the style or change any expressions. This is a precision spacing correction for 16 separate sprites used via a 4x4 source rectangle.
