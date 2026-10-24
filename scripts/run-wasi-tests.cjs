const fs = require('node:fs');
const { WASI } = require('node:wasi');

(async () => {
  const wasi = new WASI({ version: 'preview1', args: ['model-tests'], env: {}, preopens: {} });
  const module = await WebAssembly.compile(fs.readFileSync(process.argv[2]));
  const instance = await WebAssembly.instantiate(module, wasi.getImportObject());
  wasi.start(instance);
})().catch(error => { console.error(error); process.exitCode = 1; });
