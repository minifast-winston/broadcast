import _ from 'lodash';
import React from 'react';
import ReactDOM from 'react-dom';

import BackgroundContext from './background_context';

const PauseButton = ({cursor}) => {
  let $paused = cursor.select('paused');
  return(<button onClick={() => $paused.apply(paused => !paused)}>
    {$paused.get() ? 'Play' : 'Pause'}
  </button>);
}

const StartButton = ({cursor}) => {
  let $started = cursor.select('started');
  return(<button onClick={() => $started.apply(started => !started)}>
    {$started.get() ? 'Stop' : 'Start'}
  </button>);
}

const Counter = ({cursor}) => {
  let $frames = cursor.select('frames'),
      frames = $frames.get() || [],
      frameCount = frames.length,
      fps = _(frames).takeRight(100).chunk(2).map(pair => pair[1] - pair[0]).sum() / 50;
  return (<div>
    Frame: {frameCount}<br/>
    FPS: {Math.floor(fps * 1000)}
  </div>);
}

const CaptureStatus = ({cursor}) => {
  let $loaded = cursor.select('loaded');
  return (<div>
    Capture: {$loaded.get() ? 'loaded' : 'waiting'}
  </div>);
}

if (typeof document !== 'undefined') {
  ReactDOM.render(
    <BackgroundContext>
      <CaptureStatus />
      <Counter />
      <PauseButton />
      <StartButton />
    </BackgroundContext>,
    document.getElementById('browser-action')
  );
}
