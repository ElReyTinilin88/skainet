export async function GET(
  request: Request
) {
  const data = {
    resp: 'resp desde api'
  }
  return Response.json({ data })
}