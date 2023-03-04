mod enums;

use crate::enums::Mode;
use clap::Parser;
use log::{debug, info};
use std::io::Write;
use std::net::TcpStream;

// CLI /////////////////////////////////////////////////////////////////////////////////////////////

#[derive(Parser, Debug)]
#[command(author,version,about,long_about=None)]
struct Args {
  #[arg(long)]
  host: String,

  #[arg(short, long)]
  port: u16,

  #[arg(long, default_value = "Default message.")]
  msg: String,

  #[arg(value_enum, short, long, default_value_t=Mode::TCP)]
  mode: Mode,
}

// Primary Function ////////////////////////////////////////////////////////////////////////////////

#[derive(Debug)]
pub enum SendError {
  FailedToConnect,
  FailedToWrite,
  NotImplemented,
}

pub fn send(
  host: String,
  port: u16,
  msg: String,
  mode: Mode,
) -> std::result::Result<(), SendError> {
  match mode {
    Mode::TCP => {
      debug!("Trying to connect to {host}:{port}.");
      let stream = TcpStream::connect(format!("{}:{}", host, port));
      match stream {
        Ok(mut conn) => {
          let write_success = conn.write(msg.as_bytes());
          match write_success {
            Ok(_) => Ok(()),
            Err(_) => return Err(SendError::FailedToWrite),
          }
        }
        Err(_) => return Err(SendError::FailedToConnect),
      }
    }
    Mode::UDP => {
      return Err(SendError::NotImplemented);
    }
  }
}

// Main ////////////////////////////////////////////////////////////////////////////////////////////

fn main() -> std::result::Result<(), SendError> {
  env_logger::init();

  info!("Parsing arguments...");
  let args = Args::parse();

  info!("Calling send...");
  send(args.host, args.port, args.msg, args.mode)
}
