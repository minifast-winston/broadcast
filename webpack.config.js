var path = require('path');

module.exports = {
  module: {
    loaders: [
      {
        test: /\.js?$/,
        exclude: /node_modules/,
        loader: 'babel',
        query: {
          cacheDirectory: true,
          presets: ['react', 'es2015'],
          plugins: ['transform-runtime']
        }
      },
      {
        test: /\.(html|json)$/,
        loader: "file?name=[name].[ext]",
      }
    ]
  },

  context: path.join(__dirname, 'app'),
  resolve: {
    root: [path.join(__dirname, 'app')],
    extensions: ["", ".js"]
  },

  entry: {
    background: "./background.js",
    browser_action: "./browser_action.js",
    ticker: "./ticker.js",
    html: "./browser_action.html",
    manifest: "./manifest.json"
  },

  output: {
    filename: "[name].bundle.js",
    chunkFilename: "[id].chunk.js",
    path: path.join(__dirname, 'dist')
  }
}
