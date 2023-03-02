use std::env::VarError;
use std::io::{Error, ErrorKind};
include!("send.rs");

fn main() -> std::io::Result<()> {
  let mode = std::env::args()
    .nth(1)
    .expect("You must tell me whether to send or recieve.");
  let host = std::env::args()
    .nth(2)
    .expect("You mest tell me the host to send the message to.");
  let port = std::env::args()
    .nth(3)
    .expect("You must tell me which port to use.");

  if mode == "send" {
    let msg = std::env::args()
      .nth(4)
      .expect("You must give me a message to send.");
    let sendSucc = send(host, port, msg).err();
    match sendSucc {
      Some(err) => return Err(Error::new(err.kind(), "Failed to send message.")),
      None => println!("Successfully sent message."),
    }
  }

  return Ok(());
}
