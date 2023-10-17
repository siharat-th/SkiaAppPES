import { JS5 } from './JS5.js'
import { JS5App } from './JS5Singleton.js'


let canvas = CanvasKitInit({
  locateFile: (file) => './bin/' + file,
}).then((CK) => {
  window.CanvasKit = CK;
});

var baseUrl = './sql/';
var config = {
  locateFile: filename => `${baseUrl}/${filename}`
} 
let sql = initSqlJs(config).then(function (SQL) {
  window.SQL = SQL;
});

Promise.all([canvas, sql]).then(() => {
  let js5 = new JS5('jsCanvas', 1);
  let app = new JS5App()
  window.js5app = app;
  js5.init(app);
});

