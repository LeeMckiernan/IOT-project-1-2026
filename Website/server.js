const express = require("express");
const XLSX = require("xlsx");
const WebSocket = require("ws");
const chokidar = require("chokidar");
const path = require("path");

const app = express();
const PORT = 3000;

app.use(express.static("public"));

const server = app.listen(PORT, () => {
  console.log(`Server running on http://localhost:${PORT}`);
});

// WebSocket server
const wss = new WebSocket.Server({ server });

// Function to read Excel
function readExcel() {
  const workbook = XLSX.readFile("data.xlsx");
  const sheet = workbook.Sheets[workbook.SheetNames[0]];
  return XLSX.utils.sheet_to_json(sheet);
}

// Send data to all clients
function broadcastData() {
  const data = readExcel();
  const json = JSON.stringify(data);

  wss.clients.forEach(client => {
    if (client.readyState === WebSocket.OPEN) {
      client.send(json);
    }
  });
}

// Watch Excel file for changes
chokidar.watch("data.xlsx").on("change", () => {
  console.log("Excel file updated!");
  broadcastData();
});

// Send data when client connects
wss.on("connection", ws => {
  ws.send(JSON.stringify(readExcel()));
});