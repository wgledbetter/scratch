use std::io::Write;
use std::net::TcpStream;

fn send(host: String, port: String, msg: String) -> std::io::Result<usize> {
  let mut stream = TcpStream::connect(format!("{}:{}", host, port))?;
  return stream.write(msg.as_bytes());
}
