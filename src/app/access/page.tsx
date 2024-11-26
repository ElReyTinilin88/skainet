'use client'
import { access_api } from "@/actions/access"
import Swal from "sweetalert2"

export default function Access() {
    async function access(type) {
        const data = {
            id: null,
            type
        }
        try {
            const resp = await fetch('http://192.168.100.6/access')
            const { id } = await resp.json()
            data.id = id

            const saveSuccess = await access_api(data)

            if (saveSuccess)
                Swal.fire({
                    title: "Good job!",
                    text: "You clicked the button!",
                    icon: "success"
                });
        } catch (error) {
            console.log(error)
        }
    }

    return <>
        <button onClick={()=>access('Entrada')}>Acceder</button>
        <button onClick={()=>access('Salida')}>Salir</button>
    </>
}