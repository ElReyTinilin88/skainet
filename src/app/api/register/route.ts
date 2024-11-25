import conexion from "../../../db/conexion"

export async function POST(
  request: Request
) {
  const data = await request.json()

  console.log({data})

  const query = 'INSERT INTO `control_acceso`.`alumnos` (`id`, `nombre`, `apellido_paterno`, `apellido_materno`, `codigo_estudiante`, `telefono`, `correo`, `carrera`, `grado`, `grupo`, `turno`) VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)';
  const values = Object.values(data);

  conexion.query(query, values, (error, result, fields) => {
    console.log({ result, error, fields })
  })

  return Response.json({ success: true })
}