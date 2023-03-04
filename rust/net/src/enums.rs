use core::fmt::Debug;
use std::str::FromStr;

// Network Mode ////////////////////////////////////////////////////////////////////////////////////

#[derive(Debug, Clone, clap::ValueEnum)] // I don't like that I have to "register" this enum with the CLI library...
pub enum Mode {
  TCP,
  UDP,
}

impl FromStr for Mode {
  type Err = ();

  fn from_str(s: &str) -> Result<Self, Self::Err> {
    match s.to_lowercase().as_str() {
      "tcp" => Ok(Mode::TCP),
      "udp" => Ok(Mode::UDP),
      _ => Err(()),
    }
  }
}
