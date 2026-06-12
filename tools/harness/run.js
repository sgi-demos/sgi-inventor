// Headless harness: load the Emscripten maze build under Node with a
// recording mock WebGL context, to observe FFP-emulation uniform traffic
// (u_materialDiffuse et al) without a browser.
'use strict';
const fs = require('fs');
const path = require('path');

delete globalThis.fetch;          // force readBinary()/fs path in 3.1.6 loader
delete globalThis.Headers; delete globalThis.Request; delete globalThis.Response;

// ---- recording mock WebGL ----
const log = { uniform4fv: [], materialfv: [], draws: 0, warns: [], calls: {} };
let uniformNames = new Map();

const glConsts = { VERSION:0x1F02, RENDERER:0x1F01, VENDOR:0x1F00,
  MAX_TEXTURE_IMAGE_UNITS:0x8872, MAX_VERTEX_ATTRIBS:0x8869,
  MAX_TEXTURE_SIZE:0xD33, MAX_CUBE_MAP_TEXTURE_SIZE:0x851C,
  MAX_VERTEX_UNIFORM_VECTORS:0x8DFB, MAX_FRAGMENT_UNIFORM_VECTORS:0x8DFD,
  MAX_VARYING_VECTORS:0x8DFC, MAX_COMBINED_TEXTURE_IMAGE_UNITS:0x8B4D,
  MAX_VERTEX_TEXTURE_IMAGE_UNITS:0x8B4C, MAX_RENDERBUFFER_SIZE:0x84E8,
  ALIASED_LINE_WIDTH_RANGE:0x846E, ALIASED_POINT_SIZE_RANGE:0x846D,
  COMPILE_STATUS:0x8B81, LINK_STATUS:0x8B82 };


// real WebGL1 enum values (critical: type enums used in arithmetic)
const GLC = { DEPTH_BUFFER_BIT:0x100, STENCIL_BUFFER_BIT:0x400, COLOR_BUFFER_BIT:0x4000,
 POINTS:0, LINES:1, LINE_LOOP:2, LINE_STRIP:3, TRIANGLES:4, TRIANGLE_STRIP:5, TRIANGLE_FAN:6,
 ZERO:0, ONE:1, SRC_ALPHA:0x302, ONE_MINUS_SRC_ALPHA:0x303,
 FRONT:0x404, BACK:0x405, FRONT_AND_BACK:0x408, CULL_FACE:0xB44, BLEND:0xBE2,
 DEPTH_TEST:0xB71, SCISSOR_TEST:0xC11, POLYGON_OFFSET_FILL:0x8037,
 BYTE:0x1400, UNSIGNED_BYTE:0x1401, SHORT:0x1402, UNSIGNED_SHORT:0x1403,
 INT:0x1404, UNSIGNED_INT:0x1405, FLOAT:0x1406,
 DEPTH_COMPONENT:0x1902, ALPHA:0x1906, RGB:0x1907, RGBA:0x1908, LUMINANCE:0x1909, LUMINANCE_ALPHA:0x190A,
 FRAGMENT_SHADER:0x8B30, VERTEX_SHADER:0x8B31, COMPILE_STATUS:0x8B81, LINK_STATUS:0x8B82,
 ARRAY_BUFFER:0x8892, ELEMENT_ARRAY_BUFFER:0x8893,
 STREAM_DRAW:0x88E0, STATIC_DRAW:0x88E4, DYNAMIC_DRAW:0x88E8,
 TEXTURE_2D:0xDE1, TEXTURE_CUBE_MAP:0x8513, TEXTURE0:0x84C0,
 TEXTURE_MAG_FILTER:0x2800, TEXTURE_MIN_FILTER:0x2801, TEXTURE_WRAP_S:0x2802, TEXTURE_WRAP_T:0x2803,
 NEAREST:0x2600, LINEAR:0x2601, LINEAR_MIPMAP_LINEAR:0x2703, CLAMP_TO_EDGE:0x812F, REPEAT:0x2901,
 FRAMEBUFFER:0x8D40, RENDERBUFFER:0x8D41, FRAMEBUFFER_COMPLETE:0x8CD5,
 COLOR_ATTACHMENT0:0x8CE0, DEPTH_ATTACHMENT:0x8D00, DEPTH_COMPONENT16:0x81A5,
 UNPACK_FLIP_Y_WEBGL:0x9240, UNPACK_PREMULTIPLY_ALPHA_WEBGL:0x9241, UNPACK_ALIGNMENT:0xCF5, PACK_ALIGNMENT:0xD05,
 NO_ERROR:0, NONE:0, LEQUAL:0x203, LESS:0x201, CCW:0x901, CW:0x900,
 VERSION:0x1F02, RENDERER:0x1F01, VENDOR:0x1F00, MAX_VERTEX_ATTRIBS:0x8869 };
let fakeConstNext = 0x20000; const fakeConsts = new Map();

let idCounter = 1;
globalThis.WebGLRenderingContext = class WebGLRenderingContext {};
const mockTarget = {
  canvas: null,
  drawingBufferWidth: 900, drawingBufferHeight: 700,
  getParameter(p) {
    switch (p) {
      case glConsts.VERSION: return 'WebGL 1.0 (mock)';
      case glConsts.RENDERER: return 'mock'; case glConsts.VENDOR: return 'mock';
      case glConsts.ALIASED_LINE_WIDTH_RANGE:
      case glConsts.ALIASED_POINT_SIZE_RANGE: return new Float32Array([1, 64]);
      default: return 4096; // every MAX_* query
    }
  },
  getSupportedExtensions() { return []; },
  getExtension() { return null; },
  getContextAttributes() { return { alpha:true, depth:true, stencil:true, antialias:true }; },
  createShader() { return { id:idCounter++ }; },
  createProgram() { return { id:idCounter++, uniforms:{} }; },
  getShaderParameter(s,p) { return p === glConsts.COMPILE_STATUS ? true : 0; },
  getProgramParameter(pr,p) { return p === glConsts.LINK_STATUS ? true : 0; },
  getShaderInfoLog() { return ''; }, getProgramInfoLog() { return ''; },
  getUniformLocation(pr,name) { const loc={name}; uniformNames.set(loc,name); return loc; },
  getAttribLocation(pr,name) { return idCounter++ % 16; },
  createBuffer() { return { id:idCounter++ }; },
  createTexture() { return { id:idCounter++ }; },
  createFramebuffer() { return { id:idCounter++ }; },
  createRenderbuffer() { return { id:idCounter++ }; },
  checkFramebufferStatus() { return 0x8CD5; /* COMPLETE */ },
  getError() { return 0; },
  uniform4fv(loc,v) {
    const n = uniformNames.get(loc) || '?';
    if (log.uniform4fv.length < 400)
      log.uniform4fv.push([n, Array.from(v).map(x=>+x.toFixed(3))]);
  },
  drawArrays() { log.draws++; }, drawElements() { log.draws++; },
};
Object.assign(mockTarget, GLC);
Object.setPrototypeOf(mockTarget, WebGLRenderingContext.prototype);
const mockGL = new Proxy(mockTarget, {
  get(t, k) {
    if (k in t) return t[k];
    if (typeof k === 'string' && /^[A-Z][A-Z0-9_]*$/.test(k)) {   // unknown enum
      if (!fakeConsts.has(k)) fakeConsts.set(k, fakeConstNext++);
      return fakeConsts.get(k);
    }
    return function(){ log.calls[k] = (log.calls[k]||0)+1; };
  }
});

// ---- minimal DOM ----
const canvas = {
  width: 900, height: 700, clientWidth: 900, clientHeight: 700,
  style: {}, attributes: {},
  getContext(kind) { return /webgl/.test(kind) ? mockGL : null; },
  addEventListener(){}, removeEventListener(){},
  getBoundingClientRect(){ return {left:0, top:0, width:900, height:700}; },
  requestPointerLock(){}, exitPointerLock(){},
};
globalThis.HARNESS_MODULE = {
  canvas,
  arguments: [],
  locateFile: f => path.join(__dirname, 'scratch', f),
  printErr: s => { if (log.warns.length < 60) log.warns.push(String(s)); },
  print: s => { if (log.warns.length < 60) log.warns.push('OUT: ' + String(s)); },
};

// document/window mocks: enough for SDL + Browser in 3.1.6
globalThis.document = {
  getElementById: id => id === 'canvas' ? canvas : null,
  querySelector: () => canvas,
  createElement: () => ({ style:{}, getContext:() => null }),
  addEventListener(){}, removeEventListener(){},
  body: { appendChild(){}, addEventListener(){} },
  fullscreenEnabled: false, URL: 'http://localhost/',
};
globalThis.rafCount = 0;
globalThis.requestAnimationFrame = fn => setTimeout(() => { globalThis.rafCount++; fn(Date.now()); }, 16);
globalThis.screen = { width: 1920, height: 1080 };
Object.defineProperty(globalThis, 'navigator', { value: { userAgent: 'harness', getGamepads: () => [] }, configurable: true });
globalThis.location = { href: 'http://localhost/', pathname: '/', search: '', hash: '' };
globalThis.window = globalThis;     // after all globals above are set
globalThis.addEventListener = () => {}; globalThis.removeEventListener = () => {};
globalThis.matchMedia = () => ({ matches:false, addListener(){}, removeListener(){} });
globalThis.devicePixelRatio = 1;
globalThis.innerWidth = 1920; globalThis.innerHeight = 1080;

// tap GL1 library functions inside the emscripten module
const tapCounts = {}; const matCalls = [];
globalThis.TAP = (name, args) => {
  tapCounts[name] = (tapCounts[name]||0)+1;
  if (name === 'glMaterialfv' && matCalls.length < 30)
    matCalls.push([args[0].toString(16), args[1].toString(16)]);
  if (name === 'flush' && (tapCounts.flush||0) < 6)
    console.log('  flush: vtxCounter=%s stride=%s mode=%s', args[0], args[1], args[2]);
  if (name === 'prepErr') console.log('  PREPARE THREW:\n' + args[0]);
};

// ---- load patched scratch copy of maze.js ----
const scratch = path.join(__dirname, 'scratch', 'maze.patched.js');
require(scratch);

// ---- run ~2.5s of frames, then report ----
setTimeout(() => {
  const mats = log.uniform4fv.filter(([n]) => n === 'u_materialDiffuse');
  const uniq = [...new Set(mats.map(([,v]) => JSON.stringify(v)))];
  console.log('=== RESULT ===');
  console.log('rAF ticks:', globalThis.rafCount, ' draw calls:', log.draws);
  console.log('u_materialDiffuse uploads:', mats.length);
  console.log('distinct diffuse values:', uniq.join('  '));
  const others = [...new Set(log.uniform4fv.map(([n]) => n))];
  console.log('uniform4fv names seen:', others.join(', '));
  const hist = Object.entries(log.calls).sort((a,b)=>b[1]-a[1]).slice(0,12);
  console.log('top GL calls:', hist.map(([k,v])=>k+':'+v).join(' '));
  console.log('GL1 taps:', JSON.stringify(tapCounts));
  console.log('glMaterialfv (face,pname):', JSON.stringify(matCalls.slice(0,12)));
  console.log('--- first warnings ---');
  log.warns.slice(0, 12).forEach(w => console.log(' !', w));
  process.exit(0);
}, 2500);
