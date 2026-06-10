![SGI Open Inventor Logo](sgi-inventor-logo.png)

## SGI Open Inventor
_SGI Open Inventor source and binaries archive_

This is an archive of Inventor open-source projects:
- [SGI's original version](https://web.archive.org/web/20171010104743/http://oss.sgi.com/cgi-bin/cvsweb.cgi/inventor/) from an archive.org mirror of oss.sgi.com, extracted using [cvsweb-extract](https://github.com/sgi-demos/cvsweb-extract).
- [Aumuell's open-inventor](https://github.com/aumuell/open-inventor), whose aim is to integrate patches applied by various Linux distributions and to apply build fixes for macOS. It is based on an import of SGI's CVS repository at oss.sgi.com.
- [Coin3d's coin](https://github.com/coin3d/coin), a clean room open source reimplementation of the Open Inventor API. Together with bindings for many different GUI tool kits, such as Qt (SoQt and Quarter), X11 (SoXt), and Windows (SoWin) are provided.
- [MeVisLab open-inventor](https://www.mevislab.de/mevislab/features/open-inventor), is based on the original SGI source code that was released to the public in 2000. This implementation builds on Windows, MacOS X, and Linux on both 64- and 32-bit architectures and has been extended with various features.  Inventor source has been extracted from the MeVisLab SDK and archived here.

Eventually I'd like to do an Emscripten port using solely SDL2 and OpenGLES2 for GUI bindings, as there are a few fun demos I'd like to preserve.  Gathering up the raw materials here to do that.
