const { app, BrowserWindow } = require("electron");



function createWindow() {
  const win = new BrowserWindow({
    width: 800,
    height: 600,
    webPreferences: {
      contextIsolation: false, // Desactiva el aislamiento de contexto (menos seguro)
      nodeIntegration: true, // Permite el acceso directo a Node.js en el renderizador
    },
  });
  win.webContents.openDevTools(); // Abre las herramientas de desarrollo
  win.loadFile("index.html");
}

app.whenReady().then(createWindow);
app.on("window-all-closed", () => {
  if (process.platform !== "darwin") app.quit();
});
app.on("activate", () => {
  if (BrowserWindow.getAllWindows().length === 0) createWindow();
});

