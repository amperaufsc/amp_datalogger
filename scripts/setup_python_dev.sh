#!/bin/bash

set -e

MINICONDA_DIR="$HOME/miniconda3"
ENV_NAME="amp_datalogger"

if [ ! -d "$MINICONDA_DIR" ]; then
    echo ">> Instalando Miniconda..."

    wget -q https://repo.anaconda.com/miniconda/Miniconda3-latest-Linux-x86_64.sh -O /tmp/miniconda.sh

    bash /tmp/miniconda.sh -b -p "$MINICONDA_DIR"

    rm /tmp/miniconda.sh
else
    echo ">> Miniconda já instalado."
fi

source "$MINICONDA_DIR/etc/profile.d/conda.sh"

conda init bash >/dev/null

conda tos accept --override-channels --channel https://repo.anaconda.com/pkgs/main || true
conda tos accept --override-channels --channel https://repo.anaconda.com/pkgs/r || true

conda config --set auto_activate_base false

if conda env list | awk '{print $1}' | grep -qx "$ENV_NAME"; then
    echo ">> Ambiente $ENV_NAME já existe."
else
    echo ">> Criando ambiente $ENV_NAME..."
    conda create -n "$ENV_NAME" python=3.10 -y
fi

conda activate "$ENV_NAME"

python -m pip install --upgrade pip

pip install \
    numpy \
    scipy \
    pyserial \
    pyyaml \
    ipython \
    rich \
    colorama \
    tqdm \
    black \
    isort \
    flake8 \
    pytest \
    pytest-cov \
    pre-commit \
    lxml

if ! grep -q "### PYTHON AMP_DATALOGGER ENV START ###" ~/.bashrc; then

cat << EOF >> ~/.bashrc

source "$HOME/miniconda3/etc/profile.d/conda.sh"
conda activate $ENV_NAME

EOF

fi

echo
echo "==========================================="
echo " Setup concluído!"
echo "==========================================="
echo
echo "Ambiente ativo: $ENV_NAME"
echo "Feche e abra o terminal."