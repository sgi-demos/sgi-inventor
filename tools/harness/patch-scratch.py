# Prepare scratch/maze.patched.js from scratch/maze.js for the harness.
src = open('scratch/maze.js').read()
old = "var Module = typeof Module != 'undefined' ? Module : {};"
assert old in src
src = src.replace(old, 'var Module = globalThis.HARNESS_MODULE || {};', 1)
src = src.replace('setWindowTitle(', '(function(){})(')
open('scratch/maze.patched.js', 'w').write(src)
print('wrote scratch/maze.patched.js')
