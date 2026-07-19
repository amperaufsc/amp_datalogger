"""
PressureMeasurement: Classe para conversão de tensão em pressão.

Esta classe converte a tensão medida na saída de um sensor de pressão
para o valor correspondente de pressão (bar) utilizando uma calibração
linear obtida experimentalmente.

O cálculo assume que:

* O sensor possui comportamento linear na faixa de operação
* A tensão é fornecida em volts (V)
* A pressão é retornada em bar
* A calibração segue a equação P = a·V + b

A conversão é realizada diretamente a partir dos coeficientes de
calibração do sensor.

Parâmetros:

* a (float): coeficiente angular da calibração (bar/V)
* b (float): coeficiente linear da calibração (bar)
"""

from typing import Optional

class PressureMeasurement:
    def __init__(self, a: float, b: float):
        if not isinstance(a, (int, float)):
            raise TypeError("Parameter 'a' must be a number.")

        if not isinstance(b, (int, float)):
            raise TypeError("Parameter 'b' must be a number.")

        self.a = float(a)
        self.b = float(b)

    def update(self, voltage: float) -> Optional[float]:

        if voltage is None:
            return None

        if not isinstance(voltage, (int, float)):
            return None

        if voltage < 0.0:
            return None

        return self.a * float(voltage) + self.b
