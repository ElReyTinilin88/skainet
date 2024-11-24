export async function POST(
    request: Request
  ) {
    const data = {
      resp: 'resp desde api'
    }
    return Response.json({ data })
  }