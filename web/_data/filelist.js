var fs = require('fs');

module.exports = function () {

  var files = new Array()
  const path = "assets"

  fs.readdir(path, function (err, items) {
    for (var i = 0; i < items.length; i++) {
      var file = path + '/' + items[i]
      var size = 0
      fs.stat(file, function (err, stats) {
        size = stats["size"];
      });
      files.push({
        path: file,
        name: items[i],
        size: size
      })
    }
  });
  return files
};