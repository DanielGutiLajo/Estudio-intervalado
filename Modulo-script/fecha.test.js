const { fechaMayor } = require("./fechas");

describe("Pruebas para la función fechaMayor", () => {
  test("Debe devolver true si la primera fecha es mayor que la segunda", () => {
    expect(fechaMayor("20/03/2025", "19/03/2025")).toBe(true);
  });

  test("Debe devolver false si la primera fecha es menor que la segunda", () => {
    expect(fechaMayor("18/03/2025", "19/03/2025")).toBe(false);
  });

  test("Debe devolver false si las fechas son iguales", () => {
    expect(fechaMayor("19/03/2025", "19/03/2025")).toBe(false);
  });

  test("Debe manejar correctamente fechas con ceros iniciales", () => {
    expect(fechaMayor("01/01/2025", "31/12/2024")).toBe(true);
  });

  test("Debe manejar correctamente fechas en diferentes meses", () => {
    expect(fechaMayor("01/02/2025", "31/01/2025")).toBe(true);
  });

  test("Debe manejar correctamente fechas en diferentes años", () => {
    expect(fechaMayor("01/01/2026", "31/12/2025")).toBe(true);
  });
});
