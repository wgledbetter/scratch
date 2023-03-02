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
    return Err(Error::new(
      send(host, port, msg).err().expect("No error?").kind(),
      "wow",
    ));
  }

  return Err(Error::new(
    ErrorKind::Unsupported,
    "You gave me a mode other than send or recv.",
  ));
}
