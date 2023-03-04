use core::fmt::Debug;
use std::str::FromStr;

// Network Mode ////////////////////////////////////////////////////////////////////////////////////

#[derive(Debug, Clone, clap::ValueEnum)] // I don't like that I have to "register" this enum with the CLI library...
pub enum Mode {
  TCP4,
  UDP4,
  TCP6,
  UDP6,
}

impl FromStr for Mode {
  type Err = ();

  fn from_str(s: &str) -> Result<Self, Self::Err> {
    match s.to_lowercase().as_str() {
      "tcp4" => Ok(Mode::TCP4),
      "udp4" => Ok(Mode::UDP4),
      "tcp6" => Ok(Mode::TCP6),
      "udp6" => Ok(Mode::UDP6),
      _ => Err(()),
    }
  }
}
