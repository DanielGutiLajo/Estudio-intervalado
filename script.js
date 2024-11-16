/*function esperar(ms) {
  console.log("hola");
  return new Promise((resolve) => setTimeout(resolve, ms));
  
}
   type="module" en el html
await esperar(2000); */ // Espera 2 segundos
let estudiosAnterioresSeparado = [];
let fechaFormularioAnterior = new Date();
const opciones = { day: "2-digit", month: "2-digit", year: "numeric" };
const path = require("path");
const miModuloNativo = require(path.join(process.resourcesPath, "miModuloNativo.node"));
//const miModuloNativo = require("./build/Release/miModuloNativo");
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

  //const estudiosAnteriores = ["Análisis", "Álgebra", "Probabilidad", "Mecánica"];
  try {
    console.log("Contenido de estudiosAnteriores:", estudiosAnteriores);
  } catch (error) {
    console.error("Error al imprimir estudiosAnteriores:", error);
  }
  for (let i = 0; i < estudiosAnteriores.length; i++) estudiosAnterioresSeparado[i] = estudiosAnteriores[i].split(" ");
}
function mostrarEstudios() {
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
  tipos = ["Nombre", "Fecha anterior", "Fecha siguiente", "Grado"];
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
    divSvg.style.height = "28.8px";
    divSvg.style.display = "flex";
    divSvg.style.alignItems = "center";
    divSvg.style.gap = "2px";
    const svgStringTick = `
<svg  xmlns="http://www.w3.org/2000/svg"  
width="24"  
height="24"  
viewBox="0 0 24 24"  
fill="none"  
stroke="currentColor"  
stroke-width="2"  
stroke-linecap="round"  
stroke-linejoin="round"  
class="tick">
<path stroke="none" d="M0 0h24v24H0z" fill="none"/><path d="M7 12l5 5l10 -10" /><path d="M2 12l5 5m5 -5l5 -5" /></svg>
`;

    // Inserta el SVG en el div usando insertAdjacentHTML
    divSvg.insertAdjacentHTML("beforeend", svgStringTick);
    const svgTick = divSvg.querySelector(".tick");
    // Añade el evento click dinámicamente con el valor de `i`
    svgTick.addEventListener("click", function () {
      abrirFormularioConfirmar(indice);
    });

    const svgStringLapiz = `
<svg  xmlns="http://www.w3.org/2000/svg"  
width="24"  
height="24"  
viewBox="0 0 24 24"  
fill="none"  
stroke="currentColor"  
stroke-width="2"  
stroke-linecap="round"  
stroke-linejoin="round"  
class="lapiz">
<path stroke="none" d="M0 0h24v24H0z" fill="none"/><path d="M7 7h-1a2 2 0 0 0 -2 2v9a2 2 0 0 0 2 2h9a2 2 0 0 0 2 -2v-1" /><path d="M20.385 6.585a2.1 2.1 0 0 0 -2.97 -2.97l-8.415 8.385v3h3l8.385 -8.415z" />
<path d="M16 5l3 3" /></svg>
`;

    // Inserta el SVG en el div usando insertAdjacentHTML
    divSvg.insertAdjacentHTML("beforeend", svgStringLapiz);
    const svgLapiz = divSvg.querySelector(".lapiz");
    // Añade el evento click dinámicamente con el valor de `i`
    svgLapiz.addEventListener("click", function () {
      abrirFormularioEditar(indice); // Llama a la función `borrarTarea` con el valor correcto de `i`
    });

    const svgStringBasura = `
<svg  
  xmlns="http://www.w3.org/2000/svg"
  width="24"  
  height="24"  
  viewBox="0 0 24 24"  
  fill="currentColor"  
  class="basura">
  <path stroke="none" d="M0 0h24v24H0z" fill="none"/>
  <path d="M20 6a1 1 0 0 1 .117 1.993l-.117 .007h-.081l-.919 11a3 3 0 0 1 -2.824 2.995l-.176 .005h-8c-1.598 0 -2.904 -1.249 -2.992 -2.75l-.005 -.167l-.923 -11.083h-.08a1 1 0 0 1 -.117 -1.993l.117 -.007h16z" />
  <path d="M14 2a2 2 0 0 1 2 2a1 1 0 0 1 -1.993 .117l-.007 -.117h-4l-.007 .117a1 1 0 0 1 -1.993 -.117a2 2 0 0 1 1.85 -1.995l.15 -.005h4z" />
</svg>
`;

    // Inserta el SVG en el div usando insertAdjacentHTML
    divSvg.insertAdjacentHTML("beforeend", svgStringBasura);

    // Selecciona el SVG recién añadido
    const svgBasura = divSvg.querySelector(".basura");
    // Añade el evento click dinámicamente con el valor de `i`
    svgBasura.addEventListener("click", function () {
      borrarTarea(indice); // Llama a la función `borrarTarea` con el valor correcto de `i`
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
function borrarTarea(indice) {
  estudiosAnterioresSeparado.splice(indice, 1);
  const contenedorFormulario = document.getElementById("Formulario");
  if (contenedorFormulario) contenedorFormulario.remove();
  mostrarEstudios();
  miModuloNativo.borrarTarea(indice);
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
  console.log("inicia");
  const partes = estudiosAnterioresSeparado[indice][1].split("/");
  const dia = parseInt(partes[0], 10);
  const mes = parseInt(partes[1], 10) - 1; // Mes basado en cero
  const año = parseInt(partes[2], 10);

  fechaFormularioAnterior = new Date(año, mes, dia);
  console.log(fechaFormularioAnterior);
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
function editarTarea(indice) {
  let tarea = [
    document.getElementById("nombre").value,
    document.getElementById("anterior").value,
    document.getElementById("siguiente").value,
    document.getElementById("grado").value,
  ];
  if (indice < estudiosAnterioresSeparado.length) {
    estudiosAnterioresSeparado[indice] = tarea;
    console.log(estudiosAnterioresSeparado[indice]);
    const informacion = tarea.join(" ");
    eliminarFormulario();
    mostrarEstudios();
    console.log(informacion);
    miModuloNativo.editarTarea(indice, informacion);
  } else console.log("error");
}
function abrirFormularioAnadir() {
  fechaFormularioAnterior = new Date();
  abrirFormulario();
  const boton = document.getElementById("botonFormulario");
  boton.addEventListener("click", function () {
    nuevaTarea(); // Esto ejecutará la función solo cuando se haga clic
  });
}
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
  eliminarFormulario();
  mostrarEstudios();
  miModuloNativo.nuevaTarea(informacion);
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
  const contenedorFormulario = document.getElementById("Formulario");
  if (contenedorFormulario) contenedorFormulario.remove(); // Eliminar el elemento del DOM
}
function fechaMayor(fecha1, fecha2) {
  // Divide la cadena de fecha en partes: día, mes y año
  const partesFecha1 = fecha1.split("/");
  const partesFecha2 = fecha2.split("/");

  // Crea objetos Date usando el formato: año, mes (resta 1 porque los meses en Date comienzan desde 0), día
  const date1 = new Date(partesFecha1[2], partesFecha1[1] - 1, partesFecha1[0]);
  const date2 = new Date(partesFecha2[2], partesFecha2[1] - 1, partesFecha2[0]);

  // Compara las fechas
  if (date1 > date2) {
    return true;
  } else if (date1 < date2) {
    return false;
  } else {
    return false;
  }
}
