"""
WheelSpeedMeasurement: Classe para cálculo de velocidade angular de uma roda.

Esta classe calcula a velocidade angular (rad/s) de uma roda a partir de
eventos temporais sucessivos, normalmente gerados por um encoder incremental
ou sensor com setores igualmente espaçados por revolução.

O cálculo assume que:
- Cada evento corresponde à passagem por um setor do encoder
- Os setores são uniformemente distribuídos ao longo de uma revolução
- O timestamp é fornecido em segundos

A velocidade angular é calculada com base no intervalo de tempo entre dois
eventos consecutivos.

Parâmetros:
- sectors_per_rev (int): número de setores (ou pulsos) por revolução da roda
"""

import math
from typing import Optional

class WheelSpeedMeasurement:
    def __init__(self, sectors_per_rev: int):
        self.sectors_per_rev = sectors_per_rev
        self._last_timestamp: Optional[float] = None

    def update(self, timestamp: float) -> Optional[float]:
        """
        Atualiza o cálculo com um novo timestamp (em segundos).
        Retorna a velocidade angular (rad/s) ou None se não for possível calcular.
        """
        if self._last_timestamp is None:
            self._last_timestamp = timestamp
            return None

        delta_t = timestamp - self._last_timestamp

        if delta_t <= 1e-6:
            return None

        self._last_timestamp = timestamp

        return (2.0 * math.pi / self.sectors_per_rev) / delta_t
