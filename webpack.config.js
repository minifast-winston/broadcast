var path = require('path');

module.exports = {
  module:{
    noParse: [/aws-sdk/],
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
    manifest: "./manifest.json",

    background: "./background.js",
    background_page: "./background.html",

    browser_action: "./browser_action.js",
    browser_action_page: "./browser_action.html"
  },

  output: {
    filename: "[name].bundle.js",
    chunkFilename: "[id].chunk.js",
    path: path.join(__dirname, 'dist')
  }
}
