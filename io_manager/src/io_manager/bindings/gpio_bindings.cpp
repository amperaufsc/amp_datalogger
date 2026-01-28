#include <pybind11/pybind11.h>
#include <pybind11/functional.h>
#include <pybind11/iostream.h>
#include <pybind11/stl.h>

#include "io_manager/interfaces/gpio_output.hpp"
#include "io_manager/interfaces/gpio_input.hpp"
#include "io_manager/interfaces/gpio_interrupt.hpp"
#include "io_manager/interfaces/analog_input.hpp"

#include "io_manager/factory/gpio_factory.hpp"
#include "io_manager/factory/gpio_interrupt_factory.hpp"
#include "io_manager/factory/analog_input_factory.hpp"

namespace py = pybind11;

PYBIND11_MODULE(io_manager_bindings, m) {
  m.doc() = "IO Manager bindings";

  py::enum_<Edge>(m, "Edge")
      .value("RISING", Edge::RISING)
      .value("FALLING", Edge::FALLING)
      .value("BOTH", Edge::BOTH);

  py::class_<GpioOutput>(m, "GpioOutput")
      .def("write", &GpioOutput::write);

  py::class_<GpioInput>(m, "GpioInput")
      .def("read", &GpioInput::read);

  py::class_<GpioInterrupt, std::shared_ptr<GpioInterrupt>>(m, "GpioInterrupt")
      .def("start", &GpioInterrupt::start)
      .def("stop", &GpioInterrupt::stop);

  py::class_<AnalogInput, std::shared_ptr<AnalogInput>>(m, "AnalogInput")
      .def("read", &AnalogInput::read)
      .def("read_channel", &AnalogInput::read_channel);

  m.def(
      "create_gpio_output",
      &create_gpio_output,
      py::arg("bcm_pin"),
      py::arg("initial_value") = false
  );

  m.def(
      "create_gpio_input",
      &create_gpio_input,
      py::arg("bcm_pin")
  );

  m.def(
      "create_gpio_interrupt",
      &create_gpio_interrupt,
      py::arg("bcm_pin"),
      py::arg("edge")
  );

  m.def(
      "create_ads1115",
      &create_ads1115,
      py::arg("i2c_device"),
      py::arg("address"),
      py::arg("channel")
  );
}
