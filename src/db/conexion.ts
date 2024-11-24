import mysql from 'mysql2'

const connection = mysql.createConnection({
  host: process.env.HOST,
  user: process.env.USER,
  password: process.env.PASSWORD,
  database: process.env.DATABASE,
  port:  Number(process.env.PORT)
});

connection.connect((error) => {
  if (error) {
    console.error('Error connecting to MySQL database:', error)
    return
  }
  console.log('Connected to MySQL database.')
});

export default connection