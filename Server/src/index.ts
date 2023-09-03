import express, { Request } from "express";
import expressWs from "express-ws";
import ws from "ws";
import fs from "fs";

const app = expressWs(express()).app;

app.ws("/", (ws: ws, req: Request) => {
  ws.on("message", (msg) => {
    const buffer = msg as Uint8Array;
    console.log(`Received message len: ${(msg as Buffer).length}`);

    switch (buffer.at(0)) {
      case 1:
        // fs.writeFileSync(`tmp/${Date.now()}.jpg`, buffer.slice(1) as Buffer);
        ws.send("Received image");
        break;
      default:
        console.log("Unknown message type");
    }
  });
  ws.send("Connected!");
});

app.listen(3000);
