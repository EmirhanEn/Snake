# Continuous snake body rendering

The user's screenshot showed a long snake made of repeated outlined discs. Both the link and joint textures contained local shading and rounded ends; rendering those independently repeated a closed ring at every cell. Draw-order tweaks could not remove this structure.

The renderer now constructs one rounded ribbon along the interpolated head-to-tail path. A seamless external `snake-skin.png` supplies the border, highlights, spots and belly. UV distance advances along the entire path, corners are rounded with short quadratic curves, and the final 38 pixels taper to a point. Discontinuous wrapped sections are split before geometry generation. The existing atlas supplies the expressive head.

Verification:

- Geometry stub first failed four regression assertions. Final geometry suite passes 10 checks for continuous width/UVs, rounded corners, taper, duplicates and wrapped sections.
- A real hidden-window graphics test initially caught the skin texture being reset by the quads-to-triangles transition. Moving texture selection after `rlBegin` fixed the binding. The same test caught alpha loss in the skin's spot painting; opaque authored spot colors fixed it.
- Final CTest: 7/7 groups pass, including actual GPU rendering of a long green body with no gaps or internal dark rings.
- Inspected `artifacts/02-game.png` and `artifacts/06-long-snake.png` from the shipping executable, covering several turns and a 17-cell straight body.
- Replaced the old two PNG caps with one skin PNG; both source and distribution retain independent external media files.
