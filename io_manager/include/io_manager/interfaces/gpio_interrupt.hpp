#pragma once

#include <functional>

/**
 * @brief Tipo de borda para interrupção de GPIO.
 */
enum class Edge {
  RISING,
  FALLING,
  BOTH
};

/**
 * @brief Interface genérica para interrupção de GPIO.
 *
 * Implementações devem registrar eventos de borda no kernel Linux
 * e notificar via callback.
 */
class GpioInterrupt {
public:
  using Callback = std::function<void()>;

  virtual ~GpioInterrupt() = default;

  /**
   * @brief Inicia a escuta de interrupções.
   *
   * @param cb Callback chamado quando ocorre a interrupção.
   */
  virtual void start(Callback cb) = 0;

  /**
   * @brief Para a escuta de interrupções.
   */
  virtual void stop() = 0;
};
