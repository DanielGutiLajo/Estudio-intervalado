/**
 * Compara dos fechas en formato dd/mm/yyyy.
 * @param {string} fecha1 - La primera fecha.
 * @param {string} fecha2 - La segunda fecha.
 * @returns {boolean} - True si fecha1 es mayor que fecha2, false en caso contrario.
 */
function fechaMayor(fecha1, fecha2) {
  // Divide la cadena de fecha en partes: día, mes y año
  const partesFecha1 = fecha1.split("/");
  const partesFecha2 = fecha2.split("/");

  // Crea objetos Date usando el formato: año, mes (resta 1 porque los meses en Date comienzan desde 0), día
  const date1 = new Date(partesFecha1[2], partesFecha1[1] - 1, partesFecha1[0]);
  const date2 = new Date(partesFecha2[2], partesFecha2[1] - 1, partesFecha2[0]);

  // Compara las fechas
    return date1 > date2;

}

// Exportar funciones
module.exports = {
  fechaMayor,
};
