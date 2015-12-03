import React from 'react';
import ReactDOM from 'react-dom';

import BackgroundContext from './background_context';

const PauseButton = ({cursor}) => {
  let $paused = cursor.select('paused');
  return(<button onClick={() => $paused.apply(paused => !paused)}>
    {$paused.get() ? 'Play' : 'Pause'}
  </button>);
}

const Counter = ({cursor}) => {
  let $frames = cursor.select('frames');
  return (<div>
    Frame: {$frames.get().length}
  </div>);
}

if (typeof document !== 'undefined') {
  ReactDOM.render(
    <BackgroundContext>
      <Counter />
      <PauseButton />
    </BackgroundContext>,
    document.getElementById('browser-action')
  );
}
