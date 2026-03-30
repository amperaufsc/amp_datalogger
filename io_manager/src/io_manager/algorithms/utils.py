"""
Converte um dicionário "achatado" com chaves pontilhadas em um dicionário aninhado.

Este método transforma chaves no formato "a.b.c" em estruturas de dicionários
aninhados equivalentes, criando automaticamente os níveis intermediários
conforme necessário.

Exemplo:
    Entrada:
        {
            "mux.select_pins.a": 17,
            "mux.select_pins.b": 27,
            "mux.map.000.0": "sensor_1"
        }

    Saída:
        {
            "mux": {
                "select_pins": {
                    "a": 17,
                    "b": 27
                },
                "map": {
                    "000": {
                        "0": "sensor_1"
                    }
                }
            }
        }

@param flat: dicionário com chaves no formato hierárquico pontilhado
@return: dicionário aninhado equivalente
"""

def unflatten_dict(flat: dict) -> dict:
    result = {}
    for key, value in flat.items():
        parts = key.split('.')
        d = result
        for part in parts[:-1]:
            d = d.setdefault(part, {})
        d[parts[-1]] = value
    return result