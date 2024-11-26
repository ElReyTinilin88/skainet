import conexion from "../../../db/conexion"

export async function POST(
  request: Request
) {
  try {
    // Obtener los datos de la solicitud (body)
    const data = await request.json();

    // Obtener el ID y eliminarlo de los datos para evitar que se inserte en el primer query
    const { id, ...restOfData } = data;

    console.log({ restOfData });

    // Consulta para insertar los datos del alumno
    const query = `
      INSERT INTO \`alumnos\` (\`nombre\`, \`apellido_paterno\`, \`apellido_materno\`, \`codigo_estudiante\`, \`telefono\`, \`correo\`, \`carrera\`, \`grado\`, \`grupo\`, \`turno\`)
      VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?)
    `;
    const values = Object.values(restOfData);

    // Ejecutar la consulta de inserción de datos del alumno
    conexion.query(query, values, (error, result) => {
      if (error) {
        console.error('Error al insertar el alumno:', error);
        return;
      }

      // Obtener el ID del nuevo registro insertado
      console.log(result)
      const { insertId } = result;

      // Consulta para insertar la huella asociada
      const query2 = 'INSERT INTO `huellas` (`id_alumno`, `id_huella`) VALUES (?, ?)';
      const values2 = [insertId, id];

      // Ejecutar la consulta de inserción de la huella
      conexion.query(query2, values2, (error2, result2) => {
        if (error2) {
          console.error('Error al insertar la huella:', error2);
          return;
        }

        // Si todo ha ido bien, podemos enviar una respuesta o hacer algo con el resultado
        console.log('Huella asociada correctamente:', result2);
      });
    });

    return Response.json({ success: true, error: false })

  } catch (error) {
    // Manejo de errores generales
    console.error('Error en la solicitud:', error);
    return Response.json({ success: false, error })
  }
}