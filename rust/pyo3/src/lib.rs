use pyo3::prelude::pymodule;
use pyo3::prelude::PyModule;
use pyo3::prelude::PyResult;
use pyo3::prelude::Python;

#[pymodule]
fn rpy(_py: Python, m: &PyModule) -> PyResult<()> {
  Ok(())
}
