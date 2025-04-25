// import logo from './logo.svg';
import soundwave from './soundwave.svg';
import osi_model from './osi_model.png';
import kazoo from './kazoo.png';
import './App.css';
import React from 'react';
// import "tailwindcss";

// const KAZOO_API_URL = "http://localhost:/";

function Message({ text, isPending=false }) {
  return (
    <div className="message" style={{ 
      backgroundColor: isPending ? "grey" : "black",
      color: "white",
      padding: "10px",
      borderRadius: "5px", 
      margin: "5px", 
      fontSize: "16px", 
      textAlign: "left",
      // width: "100%",
      }}>
      > {text}
    </div>
  );
}

function App() {
  const [sendingAudio, setSendingAudio] = React.useState(false);
  const [displayInfo, setDisplayInfo] = React.useState(false);
  const [newMessageText, setNewMessageText] = React.useState("");
  const [messages, setMessages] = React.useState([
    { text: "PLACEHOLDER" },
  ]);
  const [pendingSend, setPendingSend] = React.useState([
    { text: "PLACEHOLDER" },
  ]);

  const fetchMessages = () => {
    fetch("api/messages").then((response) => {
      if (!response.ok) {
        throw new Error("Network response was not ok");
      }
      return response.json();
    }
    ).then((data) => {
      // console.log(data);
      if (data.hasOwnProperty("messages")) {
        setMessages(data.messages);
      } else {
        console.error("no messages found");
      }
      
      if (data.hasOwnProperty("messages_to_send")) {
        setPendingSend(data.messages_to_send);
      } else {
        console.error("no pending messages found");
      }
      
      if (data.hasOwnProperty("sending_audio")) {
        setSendingAudio(data.sending_audio);
        console.log("sending audio: ", data.sending_audio);
      } else {
        console.error("no sending audio info found");
      }
    }).catch((error) => {
      console.log("failed to get messages: ", error);
    });
  }

  React.useEffect(() => {
    const interval = setInterval(() => {
      fetchMessages();
    }, 1000); // fetch every second

    return () => clearInterval(interval); // cleanup on unmount
  }, []);

  const handleSendMessage = () => {
    if (newMessageText.trim() === "") return; // don't allow empty
    // plain text

    fetch("api/messages", {
      method: "POST",
      headers: {
        "Content-Type": "text/plain",
      },
      body: newMessageText,
    }).then((response) => {
      if (!response.ok) {
        throw new Error("Network response was not ok");
      }
      return response.json();
    }).catch((error) => {
      console.error("Error sending message:", error);
    });

    // setNewMessageText(""); // clear input
  }

  return (
  <div className="App"
    style={{
      width: "100%",
      backgroundColor: "#282c34",
      color: "white",
      minHeight: "100vh",
      display: "flex",
      flexDirection: "column",
      // textAlign: "center",
      // padding: "10px",
      // margin: "10px",
      // height: "100%"
    }}
  >
    <h1
      style={{
        textAlign: "center",
        margin: "0px",
        position: "absolute",
        width: "100%",
        top: "20px",
      }}>
      Kazooformation / Kazoo Connect
    </h1>
    <div 
    style={{
      display: "flex",
      flexDirection: "row",
      alignItems: "center",
      justifyContent: "space-around",
      minHeight: "90vh",
      // height: "100%",
    }}
    >
      {/* The functional bit */}
        <div 
        // className="App-header"
        style={{
          // minHeight: "100vh",
          height: "100%",
          display: "flex",
          flexDirection: "column",
          alignItems: "center",
          justifyContent: "center",
          // font-size: "calc(10px + 2vmin)";
          // color: white;
        }}>
          {/* <h2
          style={{
            fontSize: "1.5em",
          }}
          >Kazoo Connect Chat Client</h2> */}
          <div style={{
            alignContent: "left",
            alignItems: "left",
            alignSelf: "left",
            width: "100%",
          }}>
            {messages.map((message, index) => (
              <Message key={index} text={message.text} />
            ))}
          </div>

          <div style={{
            display: "flex",
            justifyContent: "space-between",
            alignItems: "center",
            // width: "50%",
          }}>
            <input
            style={{
              width: "25em",
              height: "2em",
              // padding: "0px",
              // margin: "0px",
              // border: "0px"
            }}
            onChange={(e) => setNewMessageText(e.target.value)}
            value={newMessageText}
            placeholder='Type your message here...'
            maxLength={40}
            ></input>
            {/* <button
            style={{
              width: "10%",
              padding: "0px",
              margin: "0px",
              marginLeft: "5px",
              // backgroundColor: "black",
              // textColor: "white",
              }}
              onClick={handleSendMessage}
              >send</button> */}
            <img
            src={kazoo}
            style={{
              // width: "100px",
              height: "50px",
              // padding: "0px",
              // margin: "0px",
              // marginLeft: "5px",
              cursor: "pointer",
            }}
            onClick={handleSendMessage}
            alt="send message"
            ></img>
            <img
            src={soundwave}
            style={{
              // width: "100px",
              height: "50px",
              // padding: "0px",
              // margin: "0px",
              // marginLeft: "5px",
              cursor: "pointer",
              color: "red",
              // backgroundColor: "green",
              filter: `invert(1) sepia(50%) ${sendingAudio ? "brightness(1)" : "brightness(0.1)"}`,
              animation: sendingAudio ? "sound-wave-animation infinite 0.5s linear" : "none",
            }}
            onClick={handleSendMessage}
            alt="s"
            ></img>
          </div>

          {pendingSend.length > 0 && (
            <div>
            <h2
            style={{
              color: "white",
              fontSize: "20px",
              textAlign: "left",
              marginTop: "20px"
            }}
            >Pending</h2>
            {pendingSend.map((message, index) => (
              <Message key={index} text={message.text} isPending={true} />
            ))}
          </div>)}
      </div>

      {/* Kazooformation Info */}
      {displayInfo && (
        <div
        style={{
          display: "flex",
          flexDirection: "column",
          // justifyContent: "center",
          alignItems: "center",
          width: "30%",
          // height: "50%",
          // padding: "10px",
          // width: "fit-content",
        }}
        >
        {/* <h2> */}
          {/* Kazooformation / Kazoo Connect */}
        {/* </h2> */}
        <img 
          src={osi_model}
          alt="OSI model with physical layer replaced a kazoo"
          style={{
            width: "80%",
          }}
          />
      </div>
      )}

      <div
        style={{
          position: "absolute",
          bottom: "10px",
          left: "10px",
          display: "flex",
          gap: "10px",
        }}
        >
        {/* Toggle OSI model display */}
        <input
        type="checkbox"
        checked={displayInfo}
        onChange={() => setDisplayInfo(!displayInfo)}
        />
        <button
        onClick={() => {
          fetch("api/messages", {
            method: "DELETE",
          }).then((response) => {
            if (!response.ok) {
              throw new Error("Network response was not ok");
            }
            return response.json();
          }).catch((error) => {
            console.error("Error deleting:", error);
          });
        }}>
          clear
        </button>
      </div>
      </div>
  </div>
  );
}

export default App;
