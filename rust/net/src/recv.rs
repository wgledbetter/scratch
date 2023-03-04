mod enums;

use crate::enums::Mode;
use clap::Parser;
use log::{debug, info};
use std::io::Read;
use std::net::TcpListener;

// CLI /////////////////////////////////////////////////////////////////////////////////////////////

#[derive(Parser, Debug)]
#[command(author,version,about,long_about=None)]
struct Args {
  #[arg(long)]
  host: String,

  #[arg(short, long)]
  port: u16,

  #[arg(value_enum, short, long, default_value_t=Mode::TCP)]
  mode: Mode,
}

// Primary Function ////////////////////////////////////////////////////////////////////////////////

#[derive(Debug)]
pub enum RecvError {
  FailedToBind,
  FailedToAccept,
  FailedToRead,
  FailedUTF8Conversion,
  NotImplemented,
}

pub fn recv(host: String, port: u16, mode: Mode) -> std::result::Result<Vec<u8>, RecvError> {
  match mode {
    Mode::TCP => {
      debug!("Trying to bind to {host}:{port}.");
      let listener_result = TcpListener::bind(format!("{}:{}", host, port));
      match listener_result {
        Ok(listener) => {
          debug!("Waiting to accept connections...");
          let accept_result = listener.accept();
          match accept_result {
            Ok((mut stream, addr)) => {
              debug!("Trying to read from tcp stream...");
              let mut buf = [0u8; 1024];
              let read_result = stream.read(&mut buf);
              match read_result {
                Ok(_) => Ok(buf.to_vec()),
                Err(_) => Err(RecvError::FailedToRead),
              }
            }
            Err(_) => return Err(RecvError::FailedToAccept),
          }
        }
        Err(_) => return Err(RecvError::FailedToBind),
      }
    }
    Mode::UDP => {
      return Err(RecvError::NotImplemented);
    }
  }
}

// Main ////////////////////////////////////////////////////////////////////////////////////////////

fn main() -> std::result::Result<(), RecvError> {
  env_logger::init();

  info!("Parsing arguments...");
  let args = Args::parse();

  let recvResult = recv(args.host, args.port, args.mode);
  match recvResult {
    Ok(msg) => {
      let string = match std::str::from_utf8(&msg) {
        Ok(m) => m,
        Err(_) => return Err(RecvError::FailedUTF8Conversion),
      };
      println!("Recieved message {}", string);
      Ok(())
    }
    Err(e) => Err(e),
  }
}
