import mysql from 'mysql2'

const connection = mysql.createConnection({
  host: process.env.HOST || 'localhost',
  user: process.env.USER || 'root',
  password: process.env.PASSWORD || '',
  database: process.env.DATABASE || 'control_acceso',
  port: 3306
});

connection.connect((error) => {
  if (error) {
    console.error('Error connecting to MySQL database:', error)
    return
  }
  console.log('Connected to MySQL database.')
});

export default connection
