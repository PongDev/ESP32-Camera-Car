import express, { Request } from "express";
import expressWs from "express-ws";
import ws from "ws";
import { v4 as uuidv4 } from "uuid";

const app = expressWs(express()).app;

const streamWebSocket: {
  [key: string]: ws;
} = {};

app.ws("/", (ws: ws, req: Request) => {
  ws.on("message", (msg) => {
    const buffer = msg as Uint8Array;
    // console.log(`Received message len: ${(msg as Buffer).length}`);

    switch (buffer.at(0)) {
      case 1:
        for (const key in streamWebSocket) {
          streamWebSocket[key].send(buffer.slice(1));
        }
        // ws.send("Received image");
        break;
      default:
        console.error("Unknown message type");
    }
  });
  ws.send("Connected!");
});

app.ws("/stream", (ws: ws, req: Request) => {
  const uuid = uuidv4();

  streamWebSocket[uuid] = ws;

  ws.on("close", (code: number, reason: Buffer) => {
    delete streamWebSocket[uuid];
  });
});

app.listen(3000);
