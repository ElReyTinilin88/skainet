"use server"
import conexion from "../db/conexion"

export async function access_api(data: {}) {
    console.log(data)
    return data
}