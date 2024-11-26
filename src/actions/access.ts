"use server"
import conexion from "../db/conexion"

export async function access_api(data: { id, type }) {
    const query = `
      INSERT INTO \`registros\` (\`id_alumno\`, \`hora\`,\`tipo\`)
      VALUES (?, NOW(),?)
    `
    const values = [data.id,data.type]
    
    // Ejecutar la consulta de inserción de datos del alumno
    conexion.query(query, values, (error, result) => {
        if(error) console.log(error)
    })
    return true
}