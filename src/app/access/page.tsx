'use client'
import { access_api } from "@/actions/access"

export default function Access() {
    async function access() {
        const data = {
            id: null
        }

        // const resp = await fetch('http://192.168.100.6/access')
        // const { id } = await resp.json()
        // data.id = id

        access_api(data)
            .then(resp => console.log(resp))
            .catch(e => console.log(e))
    }

    return <>
        <button onClick={access}>Acceder</button>
    </>
}