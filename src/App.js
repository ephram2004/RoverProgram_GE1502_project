import React, { Component } from 'react';
import './App.css';
import ToggleButton from 'react-toggle-button';

//
let status = require('./images/logo/notconnected.png');
let statustext = "not connected";
let statusstyle = {
    textformat: {
        color: '#f40029',
    }
};
let run = require('./images/logo/notrunning.png')
let runtext = "Rover Stopped";
let runstyle = {
  textformat: {
      color: '#f40029',
  }
};
let starttext = "Start";
let startstyle = {
  texformat: {
    color: '#000000',
  }
};

let connection = 1;

if (connection === 0) {
  status = require('./images/logo/notconnected.png');
  statustext = "Not Connected";
  statusstyle = {
    textformat: {
        color: '#f40029',
        right: '20px',
    }
  };
} 
else if (connection === 1) {
  status = require('./images/logo/connected.png');
  statustext = "Connected";
  statusstyle = {
    textformat: {
        color: '#0fa828',
    }
  };
}

var instructions = [0, 0, 0, 0, 0];
var units = [0,0];

let instructionString=instructions.toString();

let data = ["length", "width", "holes", "seeds", "rows"];
let measure = ["lengthmeasure", "widthmeasure"];

function getUnits () {
  for (let i=0; i<=1; i++) {
      let input = document.getElementById(measure[i]);
      var unitsvalue = parseInt(input.options[input.selectedIndex].value);
      units[i] = unitsvalue;
  }
  console.log(units);
  return units;
}

function getdata () {
    for(let i=0; i<=1; i++) {
        let input = parseInt(document.getElementById(data[i]).value);
        switch (units[i]) {
          case 2:
            instructions[i] = input * 0.3048;
            break;
          case 3:
            instructions[i] = input * 0.9144;
            break;
          default:
            instructions[i] = input;
        }
    }
    for(let i=2; i<=4; i++) {
        var select = document.getElementById(data[i]);
        var value = parseInt(select.options[select.selectedIndex].value);
        instructions[i] = value;
    }
    console.log(instructions);
    instructionString = instructions.toString();
    console.log(instructionString);
}

const handleClick = () => {
    getUnits();
    getdata();
} 

const resetClickhandle = () => {
    reset();
}

function reset () {
    for(let i=0; i<=4; i++) {
        document.getElementById(data[i]).value = '';
    }
}

//

class App extends Component {

  constructor(props) {
    super(props);
    this.state = { instructionString: ''};
    this.state = { ledOn: false};
  }

  setuserInstructions(state) {
    this.setState({ instructionString: state !== ''})
  }

  setLedState(state) {
    this.setState({ ledOn: state !== '0' })
  }

  componentDidMount() {
    fetch('/inputText')
      .then(response => response.text())
      .then(state => this.setuserInstructions(state));

    fetch('/led')
      .then(response => response.text())
      .then(state => this.setLedState(state));
  }

  handleStateChange(ledOn) {
    fetch('/led', { method: 'PUT', body: ledOn ? '0' : '1'})
      .then(response => response.text())
      .then(state => this.setLedState(state));
    
    fetch('/inputText', { method: 'PUT', body: instructionString})
      .then(response => response.text())

    if(ledOn === true) {
      run = require('./images/logo/notrunning.png');
      runtext = "Rover Stopped";
      runstyle = {
        textformat: {
          color: '#f40029',
        }
      };
      starttext = "Start";
      startstyle = {
        texformat: {
          color: '#000000',
        }
      };

    }
    else if (ledOn === false) {
      run = require('./images/logo/running.png');
      runtext = "Rover Running";
      runstyle = {
        textformat: {
          color: '#0fa828',
        }
      };
      starttext = "Started";
      startstyle = {
        texformat: {
          color: '#0fa828',
        }
      };
    }
  }

  render() {
    return (
      <body>
          <div className="titleblock"> 
              <h1>Interface For Rover</h1>
          </div>
          <div className="spacetop"></div>
          <div className="userinput">
              <div className="dashboard">
                  <div className="status" id="connection">
                    <img src={status} alt="rover connection status"></img>
                    <h5 className="statustext" style={statusstyle.textformat}>{statustext}</h5>
                  </div>
                  <div className="status" id="run">
                    <img className="runstatus" src={run} alt="rover operation status"></img>
                    <h5 className="runtext" style={runstyle.textformat}>{runtext}</h5>
                  </div>
                  <div className="batteryindicator">
                      <div className="bigcircle">
                          <h2 className="battval">100%</h2>
                      </div>
                  </div>
                  <div className="buttonContainer play">
                      <div className="center">
                          <h1 className="steps">1.</h1>
                          <div className="btn" onClick={handleClick}>
                              <div className="upload">Upload</div>
                          </div>
                          <h1 className="stepSecond">2.</h1>
                          <h2 className="secondstepstat" style={startstyle.texformat}>{starttext}</h2>
                          <div className="start">
                            <ToggleButton
                              value={this.state.ledOn}
                              onToggle={value => this.handleStateChange(value)}
                            />
                          </div>
                      </div>
                  </div>
                  <div className="buttonContainer reload">
                      <div className="reloadBtn">
                          <span className="icon" onClick={resetClickhandle}>&#8635;</span>
                      </div>
                  </div>
              </div>
              <div className="inputs">
                  <div className="spacebottom1">
                      <div className="lw">
                          <h3>Length:</h3>
                          <input type="text" className="text" id="length"/>
                          <select className="measure" id="lengthmeasure">
                                  <option value="1">m</option>
                                  <option value="2">ft</option>
                                  <option value="3">yd</option>
                          </select>
                      </div>
                  </div>
                  <div className="spacebottom1">
                      <div className="lw">
                          <h3>Width:</h3>
                          <input type="text" className="text" id="width"/>
                          <select className="measure" id="widthmeasure">
                                  <option value="1">m</option>
                                  <option value="2">ft</option>
                                  <option value="3">yd</option>
                          </select>
                      </div>
                  </div>
                  <div className="container">
                      <div className="spacebottom">
                          <div className="long">
                              <h3># of Holes:</h3>
                              <select className="dropdown" id="holes">
                                  <option value="1">1</option>
                                  <option value="2">2</option>
                                  <option value="3">3</option>
                                  <option value="4">4</option>
                                  <option value="5">5</option>
                                  <option value="6">6</option>
                                  <option value="7">7</option>
                                  <option value="8">8</option>
                                  <option value="9">9</option>
                                  <option value="10">10</option>
                              </select>
                          </div>
                      </div>
                      <div className="spacebottom">
                          <div className="long">
                              <h3># of Seeds:</h3>
                              <select className="dropdown" id="seeds">
                                  <option value="1">1</option>
                                  <option value="2">2</option>
                                  <option value="3">3</option>
                                  <option value="4">4</option>
                                  <option value="5">5</option>
                                  <option value="6">6</option>
                                  <option value="7">7</option>
                                  <option value="8">8</option>
                                  <option value="9">9</option>
                                  <option value="10">10</option>
                              </select>
                          </div>
                      </div>
                      <div className="spacebottom">
                          <div className="long">
                              <h3># of Rows:</h3>
                              <select className="dropdown" id="rows">
                                  <option value="1">1</option>
                                  <option value="2">2</option>
                                  <option value="3">3</option>
                                  <option value="4">4</option>
                                  <option value="5">5</option>
                                  <option value="6">6</option>
                                  <option value="7">7</option>
                                  <option value="8">8</option>
                                  <option value="9">9</option>
                                  <option value="10">10</option>
                              </select>
                          </div>
                      </div>
                  </div>                   
              </div>
          </div>
        </body>
    );
  }
}

export default App;
