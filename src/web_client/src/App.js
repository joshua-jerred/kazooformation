import logo from './logo.svg';
import kazoo from './kazoo.png';
import './App.css';
import React from 'react';
// import "tailwindcss";

const KAZOO_API_URL = "http://localhost:/";

function Message({ text, isPending=false }) {
  return (
    <div className="message" style={{ backgroundColor: isPending ? "grey" : "black", color: "white", padding: "10px", borderRadius: "5px", margin: "5px", fontSize: "16px", textAlign: "left" }}>
      > {text}
    </div>
  );
}


function App() {
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
      console.log(data);
      if (data.hasOwnProperty("messages")) {
        setMessages(data.messages);
      } else {
        console.log("no messages found");
      }

      if (data.hasOwnProperty("messages_to_send")) {
        setPendingSend(data.messages_to_send);
      } else {
        console.log("no pending messages found");
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

    fetch("api/send", {
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
    <div className="App">
      <header className="App-header">
        <div style={{
          alignContent: "left",
          alignItems: "left",
          alignSelf: "left",
          width: "50%",
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
          width: "20em",
          height: "2em",
          // padding: "0px",
          // margin: "0px",
          // border: "0px"
        }}
        onChange={(e) => setNewMessageText(e.target.value)}
        value={newMessageText}
        placeholder='Type your message here...'
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

      </header>
    </div>
  );
}

export default App;
