The slotcar scenes request Adobe Utopia (Font nodes name Utopia-Regular,
Utopia-Italic, Utopia-Bold) and libFL opens these files by that exact
name.

These files are DejaVu Serif TrueType fonts (free license, see
https://dejavu-fonts.github.io) standing in for Utopia: the genuine
freely-licensed Utopia release (Adobe's donation to the TeX Users
Group) is Type1-only, and the FreeType 2.6.0 inside Emscripten's
USE_FREETYPE port fails to map most glyphs through a Type1 unicode
charmap (native FreeType 2.13 handles it fine). A faithful TTF/OTF
conversion of real Utopia (e.g. via fontforge, or the Heuristica
fork) is a welcome upgrade.
