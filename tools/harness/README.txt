Headless Node harness for the Emscripten build.

Runs maze.js under Node with a recording mock WebGL context to observe
the legacy-GL-emulation uniform traffic (u_materialDiffuse etc.) without
a browser. Used to diagnose the grayscale-maze bug (June 2026).

Usage:
  mkdir scratch && cp <build-em>/apps/sdldemos/maze/maze.{js,wasm} scratch/
  python3 patch-scratch.py      # injects Module + no-ops setWindowTitle
  node run.js

Requires the build to be unminified (default at RelWithDebInfo).
