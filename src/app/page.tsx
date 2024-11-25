'use client'
import { useRef } from 'react';
import styles from '../styles/Home.module.css'

export default function Page() {
  const form = useRef()

  async function submit(e) {
    e.preventDefault();
    const form = e.target;
    const formData = new FormData(form);
    const data = {
      id: null
    }

    formData.forEach((value, key) => {
      data[key] = value
    })

    const resp = await fetch('http://192.168.100.6/register')
    const { id } = await resp.json()
    data.id = id

    fetch('http://192.168.100.15:3000/api/register', {
      method: 'POST',
      body: JSON.stringify(data)
    })
      .then(response => response.json())
      .then(result  => console.log(result))
      .catch(error => console.error('Error:', error))
  }

  return <>
    <div>
      <form onSubmit={submit} ref={form}>
        <div>
          <label htmlFor="name" className={styles.label}>Nombre</label>
          <input type="text" id="name" name="name" />
        </div>
        <div>
          <label htmlFor="apellido_paterno">Apellido Paterno</label>
          <input type="text" id="apellido_paterno" name="apellido_paterno" />
        </div>
        <div>
          <label htmlFor="apellido_materno">Apellido Materno</label>
          <input type="text" id="apellido_materno" name="apellido_materno" />
        </div>
        <div>
          <label htmlFor="codigo_estudiante">Código estudiante</label>
          <input type="text" id="codigo_estudiante" name="codigo_estudiante" />
        </div>
        <div>
          <label htmlFor="telefono">Teléfono</label>
          <input type="text" id="telefono" name="telefono" />
        </div>
        <div>
          <label htmlFor="correo">Correo</label>
          <input type="text" id="correo" name="correo" />
        </div>
        <div>
          <label htmlFor="carrera">Carrera</label>
          <input type="text" id="carrera" name="carrera" />
        </div>
        <div>
          <label htmlFor="grado">Grado</label>
          <input type="text" id="grado" name="grado" />
        </div>
        <div>
          <label htmlFor="grupo">Grupo</label>
          <input type="text" id="grupo" name="grupo" />
        </div>
        <div>
          <label htmlFor="turno">Turno</label>
          <input type="text" id="turno" name="turno" />
        </div>
        <button type="submit">Guardar</button>
      </form>
    </div>
  </>
}