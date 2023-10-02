import express, { Request } from "express";
import expressWs from "express-ws";
import ws from "ws";
import { v4 as uuidv4 } from "uuid";
import path from "path";

const config = {
  port: 3000,
};

const app = expressWs(express()).app;

const cameraWebSocket: {
  [key: string]: {
    ws: ws;
    controllers: {
      [key: string]: ws;
    };
  };
} = {};

app.get("/", (req, res) => {
  res.sendFile(path.join(__dirname, "../index.html"));
});

app.get("/getCameraIDList", (req, res) => {
  res.send(JSON.stringify(Object.keys(cameraWebSocket)));
});

app.ws("/", (ws: ws, req: Request) => {
  let clientName: string;

  ws.on("message", (msg) => {
    const buffer = msg as Uint8Array;

    switch (buffer.at(0)) {
      case 0:
        clientName = buffer.slice(1).toString();
        if (clientName in cameraWebSocket) {
          cameraWebSocket[clientName] = {
            ws: ws,
            controllers: cameraWebSocket[clientName].controllers,
          };
        } else {
          cameraWebSocket[clientName] = {
            ws: ws,
            controllers: {},
          };
        }
        console.log(`Client set name to ${clientName}`);
        break;
      case 1:
        for (const controllerWebsocket of Object.values(
          cameraWebSocket[clientName].controllers
        )) {
          controllerWebsocket.send(buffer.slice(1));
        }
        break;
      default:
        console.error("Unknown message type");
    }
  });

  ws.on("close", (code: number, reason: Buffer) => {
    console.log(`Client ${clientName} disconnected`);
  });
  console.log("Client connected");
  ws.send(String.fromCharCode(0) + "Connected!");
});

app.ws("/controller/:targetCameraName", (ws: ws, req: Request) => {
  let targetCameraName: string = req.params.targetCameraName;

  if (!(targetCameraName in cameraWebSocket)) {
    ws.send(String.fromCharCode(0) + "Camera not found");
    ws.close();
    console.log(
      `Target Camera Name ${targetCameraName} not found`
    );
    return;
  }

  const uuid = uuidv4();

  cameraWebSocket[targetCameraName].controllers[uuid] = ws;

  ws.on("close", (code: number, reason: Buffer) => {
    console.log(
      `Controller Client ${uuid} disconnected from ${targetCameraName}`
    );
    delete cameraWebSocket[targetCameraName].controllers[uuid];
  });

  ws.on("message", (msg) => {
    const buffer = msg as Uint8Array;
    cameraWebSocket[targetCameraName].ws.send(buffer);
  });
  console.log(`Controller Client ${uuid} connected to ${targetCameraName}`);
});

console.log("Listening on port " + config.port);
app.listen(config.port);
