import _ from 'lodash';
import React from 'react';
import ReactDOM from 'react-dom';

import BackgroundContext from './background_context';

const PauseButton = ({cursor}) => {
  let $capturing = cursor.select('capturing'),
      $requested = cursor.select('requested');
  if (!$requested.get()) { return(<div></div>); }
  return(<button onClick={() => $capturing.set(!$capturing.get())}>
    {$capturing.get() ? 'Pause' : 'Resume'}
  </button>);
}

const StartButton = ({cursor}) => {
  let $requested = cursor.select('requested');
  return(<button onClick={() => $requested.set(!$requested.get())}>
    {$requested.get() ? 'Stop' : 'Capture'}
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

const Size = ({cursor}) => {
  let bytes = cursor.get('size');
  let magnitude = Math.max(0, Math.floor(Math.log(bytes) / Math.log(1024)));
  let units = ["B", "KiB", "MiB", "GiB", "TiB", "PiB", "EiB"][magnitude];
  let size = bytes / Math.pow(1024, magnitude);
  return (<div>Size: {Math.floor(size * 100) / 100} {units}</div>);
}

if (typeof document !== 'undefined') {
  ReactDOM.render(
    <BackgroundContext>
      <Size />
      <Counter />
      <PauseButton />
      <StartButton />
    </BackgroundContext>,
    document.getElementById('browser-action')
  );
}
