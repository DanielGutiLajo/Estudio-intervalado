/*
  Resumen:
  Carga los estudios anteriores y los muestra en la pantalla.
  Y luego los permite editar borrar y crear guardando los cambios 
  en el js y luego en el archivo.
*/

//Funciones importadas
const { fechaMayor } = require("./Modulo-script/fechas.js");
const { nuevaTarea, editarTarea, borrarTarea } = require("./Modulo-script/tareas.js");
const { estudiosAnterioresSeparado } = require("./Modulo-script/estado");
//const miModuloNativo = require(path.join(process.resourcesPath, "miModuloNativo.node"));
const miModuloNativo = require("./build/Release/miModuloNativo");

let fechaFormularioAnterior = new Date();
const opciones = { day: "2-digit", month: "2-digit", year: "numeric" };
const path = require("path");

module.exports = {
  fechaFormularioAnterior,
  mostrarEstudios,
  eliminarFormulario,
};

const { svgStringTick, svgStringLapiz, svgStringBasura } = require("./svgString.js");
let modo = 1;
function cambiarModo(modoNuevo) {
  const array = ["botonTodos", "botonHoy", "botonManana"];
  let boton = document.getElementById(array[modo]);
  boton.disabled = false;

  boton = document.getElementById(array[modoNuevo]);
  boton.disabled = true;

  modo = modoNuevo;
  mostrarEstudios();
}
cargarEstudios();
mostrarEstudios();

function cargarEstudios() {
  const archivos = miModuloNativo.archivosValidos();

  const tituloHtml = document.getElementById("titulo");
  const idArchivo = miModuloNativo.archivoAbrir();

  tituloHtml.innerHTML = archivos[parseInt(idArchivo, 10)];
  const estudiosAnteriores = miModuloNativo.pasarTareas();

  for (let i = 0; i < estudiosAnteriores.length; i++) estudiosAnterioresSeparado[i] = estudiosAnteriores[i].split(" ");
}
function mostrarEstudios() {
  console.log("Mostrando modo", modo);
  const contenedor = document.getElementById("container");
  contenedor.textContent = "";
  const estudiosAnterioresString = `
<div class="lista-borde" style="font-size: x-large; font-weight: 700; margin-left: 20px">Estudios anteriores</div>
`;
  if (estudiosAnterioresSeparado.length > 1) contenedor.insertAdjacentHTML("beforeend", estudiosAnterioresString);
  // Itera sobre el array y crea un div por cada elemento
  const divPadreLista = document.createElement("div");
  divPadreLista.className = "divPadreLista";
  const divTexto = document.createElement("div");
  divPadreLista.appendChild(divTexto);
  divTexto.className = "divTextoLista";
  const arrayDivTexto = [];
  let divBuffer;
  let tipos = ["Nombre", "Fecha anterior", "Fecha siguiente", "Grado"];
  for (let j = 0; j < 4; j++) {
    arrayDivTexto.push(document.createElement("div"));
    divTexto.appendChild(arrayDivTexto[j]);
    divBuffer = document.createElement("div");
    divBuffer.textContent = tipos[j];
    divBuffer.style.fontWeight = "600";
    arrayDivTexto[j].appendChild(divBuffer);
  }
  const divSimbolos = document.createElement("div");
  divPadreLista.appendChild(divSimbolos);
  divBuffer = document.createElement("div");
  divBuffer.style.height = "28.8px";
  divSimbolos.appendChild(divBuffer);

  const hoy = new Date();

  if (modo == 2) {
    hoy.setDate(hoy.getDate() + 1);
    const mananaFormato = hoy.toLocaleDateString("es-ES", opciones);
    let i = 0;
    while (i < estudiosAnterioresSeparado.length && fechaMayor(estudiosAnterioresSeparado[i][2], mananaFormato)) i++;
    for (; i < estudiosAnterioresSeparado.length; i++) escribirTareas(i);
  } else if (modo == 1) {
    const hoyFormato = hoy.toLocaleDateString("es-ES", opciones);
    let i = 0;
    while (i < estudiosAnterioresSeparado.length && fechaMayor(estudiosAnterioresSeparado[i][2], hoyFormato)) i++;
    for (; i < estudiosAnterioresSeparado.length; i++) escribirTareas(i);
  } else if (modo == 0) for (let i = 0; i < estudiosAnterioresSeparado.length; i++) escribirTareas(i);

  function escribirTareas(indice) {
    for (let j = 0; j < 4; j++) {
      divBuffer = document.createElement("div");
      divBuffer.textContent = estudiosAnterioresSeparado[indice][j];
      arrayDivTexto[j].appendChild(divBuffer);
    }
    const divSvg = document.createElement("div");
    Object.assign(divSvg.style, { height: "28.8px", display: "flex", alignItems: "center", gap: "2px" });

    const svgElements = [
      { svg: svgStringTick, clase: "tick", evento: () => abrirFormularioConfirmar(indice) },
      { svg: svgStringLapiz, clase: "lapiz", evento: () => abrirFormularioEditar(indice) },
      { svg: svgStringBasura, clase: "basura", evento: () => borrarTarea(indice) },
    ];

    svgElements.forEach(({ svg, clase, evento }) => {
      divSvg.insertAdjacentHTML("beforeend", svg);
      divSvg.querySelector(`.${clase}`).addEventListener("click", evento);
    });

    divSimbolos.appendChild(divSvg);
    contenedor.appendChild(divPadreLista); // Agrega el div al contenedor
  }
}
function actualizarFechaGrado() {
  const grado = document.getElementById("grado").value;
  let diasAnadir;
  if (grado == 0) diasAnadir = 1;
  else if (grado == 1) diasAnadir = 3;
  else if (grado == 2) diasAnadir = 7;
  else diasAnadir = 7 * 2 ** (grado - 2);

  const nuevaFecha = new Date(fechaFormularioAnterior);
  nuevaFecha.setDate(nuevaFecha.getDate() + diasAnadir);
  let fechaFormateada = nuevaFecha.toLocaleDateString("es-ES", opciones);
  document.getElementById("siguiente").value = fechaFormateada;
}
function abrirFormularioConfirmar(indice) {
  fechaFormularioAnterior = new Date();
  abrirFormulario();
  document.getElementById("grado").value = parseInt(estudiosAnterioresSeparado[indice][3]) + 1;
  actualizarFechaGrado();
  document.getElementById("nombre").value = estudiosAnterioresSeparado[indice][0];
  const nombre = document.getElementById("nombre");
  nombre.removeAttribute("required"); // Elimina el atributo readonly
  nombre.setAttribute("readonly", ""); // Añade el atributo required
  const boton = document.getElementById("botonFormulario");
  boton.addEventListener("click", function () {
    editarTarea(indice); // Esto ejecutará la función solo cuando se haga clic
  });
}
function abrirFormularioEditar(indice) {
  const partes = estudiosAnterioresSeparado[indice][1].split("/");
  const dia = parseInt(partes[0], 10);
  const mes = parseInt(partes[1], 10) - 1; // Mes basado en cero
  const año = parseInt(partes[2], 10);

  fechaFormularioAnterior = new Date(año, mes, dia);
  abrirFormulario();
  document.getElementById("grado").value = estudiosAnterioresSeparado[indice][3];
  actualizarFechaGrado();
  document.getElementById("nombre").value = estudiosAnterioresSeparado[indice][0];
  const siguiente = document.getElementById("siguiente");
  siguiente.removeAttribute("readonly"); // Elimina el atributo readonly
  siguiente.setAttribute("required", ""); // Añade el atributo required
  const anterior = document.getElementById("anterior");
  anterior.removeAttribute("readonly"); // Elimina el atributo readonly
  anterior.setAttribute("required", ""); // Añade el atributo required
  const boton = document.getElementById("botonFormulario");
  boton.addEventListener("click", function () {
    editarTarea(indice); // Esto ejecutará la función solo cuando se haga clic
  });
}
function abrirFormularioAnadir() {
  fechaFormularioAnterior = new Date();
  abrirFormulario();
  const boton = document.getElementById("botonFormulario");
  boton.addEventListener("click", function () {
    console.log("Mostrando modo antes de nueva tarea", modo);
    nuevaTarea(); // Esto ejecutará la función solo cuando se haga clic
  });
}
function abrirFormulario() {
  const template = document.getElementById("formularioTemplate");
  const clon = template.content.cloneNode(true);
  document.body.appendChild(clon);

  document.getElementById("anterior").value = fechaFormularioAnterior.toLocaleDateString("es-ES", opciones);

  actualizarFechaGrado();

  document.getElementById("grado").addEventListener("input", function () {
    if (this.value < 0) this.value = 0; //no permite introducir valores negativos
    actualizarFechaGrado();
  });
}
function eliminarFormulario() {
  console.log("Mostrando modo después de nueva tarea", modo);
  const contenedorFormulario = document.getElementById("Formulario");
  if (contenedorFormulario) contenedorFormulario.remove(); // Eliminar el elemento del DOM
}
