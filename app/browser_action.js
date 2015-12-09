import _ from 'lodash';
import React from 'react';
import ReactDOM from 'react-dom';

import BackgroundContext from './background_context';

const PauseButton = ({cursor}) => {
  let $capturing = cursor.select('capturing'),
      $requested = cursor.select('requested');
  if (!$requested.get()) { return(<div></div>); }
  return(<button onClick={() => $capturing.apply(capturing => !capturing)}>
    {$capturing.get() ? 'Pause' : 'Resume'}
  </button>);
}

const StartButton = ({cursor}) => {
  let $requested = cursor.select('requested');
  return(<button onClick={() => $requested.apply(requested => !requested)}>
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

if (typeof document !== 'undefined') {
  ReactDOM.render(
    <BackgroundContext>
      <Counter />
      <PauseButton />
      <StartButton />
    </BackgroundContext>,
    document.getElementById('browser-action')
  );
}
