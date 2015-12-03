var paused = true;

setInterval(() => {
  if (!paused) {
    postMessage(Date.now())
  }
}, 500);

onmessage = ({data}) => {
  paused = data;
}
