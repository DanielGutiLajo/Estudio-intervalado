const { fechaMayor } = require("./fechas.js");
const miModuloNativo = require("../build/Release/miModuloNativo");
const script = require("../script"); // Importa todo el módulo
const { estudiosAnterioresSeparado } = require("./estado");
function nuevaTarea() {
  let tarea = [
    document.getElementById("nombre").value,
    document.getElementById("anterior").value,
    document.getElementById("siguiente").value,
    document.getElementById("grado").value,
  ];
  let i = 0;
  while (i < estudiosAnterioresSeparado.length && fechaMayor(estudiosAnterioresSeparado[i][2], tarea[2])) i++;
  estudiosAnterioresSeparado.splice(i, 0, tarea);
  const informacion = tarea.join(" ");
  script.eliminarFormulario();
  script.mostrarEstudios();
  miModuloNativo.nuevaTarea(informacion);
}

function editarTarea(indice) {
  let tarea = [
    document.getElementById("nombre").value,
    document.getElementById("anterior").value,
    document.getElementById("siguiente").value,
    document.getElementById("grado").value,
  ];
  if (indice < estudiosAnterioresSeparado.length) {
    estudiosAnterioresSeparado[indice] = tarea;
    const informacion = tarea.join(" ");
    script.eliminarFormulario();
    script.mostrarEstudios();
    miModuloNativo.editarTarea(indice, informacion);
  }
}

function borrarTarea(indice) {
  estudiosAnterioresSeparado.splice(indice, 1);
  script.mostrarEstudios();
  miModuloNativo.borrarTarea(indice);
}

module.exports = { nuevaTarea, editarTarea, borrarTarea };
